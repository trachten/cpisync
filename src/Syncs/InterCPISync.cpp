/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   InterCPI.cpp
 * Author: Ari Trachtenberg
 * 
 * Created on November 30, 2011, 10:46 PM
 */

#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Communicants/Communicant.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/CPISync.h>
#include <CPISync/Syncs/InterCPISync.h>
#include <NTL/RR.h>

InterCPISync::InterCPISync(long m_bar, long bits, int epsilon, int partition,bool Hashes /* = false*/)
: maxDiff(m_bar), bitNum(bits), pFactor(partition), hashes(Hashes),
	probEps(conv<int>(ceil(-log10((RR_ONE - pow(RR_ONE - pow(RR_TWO,(RR) -epsilon),RR_ONE/ (RR_ONE+ pow(RR_TWO,(RR) bits) *
	(RR) partition / (RR) m_bar * (RR) ceil(bits*log(2)/log(partition))))))/log10(RR_TWO)))){

	/**
	 * ProbEps: Derivation
	 * Probability of failure for each CPISync node = 2^-Epsilon
	 * 1-(1-P[error per node])^maxNodes <= 2^-Epsilon //Probability of 1 or more errors occuring:
	 * max nodes for InterCPISync = 1 + (symDifs*partition/m_bar)*ceil(bits* log_p(2))
	 * Then solve for P[error per node] to find the epsilon_0(Epsilon for each CPISync node) needed
	 * for each CPISync in order to bound the total error by the given epsilon
	 */

	Logger::gLog(Logger::METHOD,"Entering InterCPISync::InterCPISync");
	// setup ZZ_p field size
	redundant_k = to_long(CeilToZZ(to_RR(probEps) / bitNum)); //

	if (redundant_k <= 0) redundant_k = 1; //k at least 1

	DATA_MAX = power(ZZ_TWO, bitNum); // maximum data element for the multiset
	ZZ fieldSize = NextPrime(DATA_MAX + maxDiff + redundant_k);
	ZZ_p::init(fieldSize);

	treeNode = nullptr;
	useExisting=false;
	SyncID = SYNC_TYPE::Interactive_CPISync; // the synchronization type
}

InterCPISync::~InterCPISync() {
	// clean out the CPISync tree
	_deleteTree(treeNode);
}


bool InterCPISync::delElem(shared_ptr<DataObject> datum) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::delElem");
    if(!SyncMethod::delElem(datum)) return false; // run the parent's version first

    Logger::gLog(Logger::METHOD_DETAILS, ". (InterCPISync) removing item " + datum->print());

    //If empty do nothing
    if(treeNode == nullptr){
        Logger::error("No elements are present in this sync object");
        return false;
    }
    //If not empty delete the element from each relevant branch in the tree
    else {
		bool success = _delElem(datum,treeNode,ZZ_ZERO,DATA_MAX);
		//If the parent node contains an empty set then clear
		if(treeNode->getDatum()->getNumElem() == 0) {
			treeNode = nullptr;
		}
		return success;
	}
}

bool InterCPISync::addElem(shared_ptr<DataObject> newDatum) {
	/* recursively add an element to all appropriate nodes until:
	* 1.  We reach a leaf node with <m_bar entries.
	* 2.  We reach a node with range < m_bar
	* Assuming there are no hash collisions, we should always end at one
	* of these two options.  Otherwise, we report an error by returning false.
	*/

	Logger::gLog(Logger::METHOD,"Entering InterCPISync::AddElem");
	if (!SyncMethod::addElem(newDatum)) return false; //Run parents version

	addElemHashID = rep(_hash(newDatum)); // compute the hash of the item for use in the recursive addElem

	Logger::gLog(Logger::METHOD_DETAILS, ". (InterCPISync) adding item " + newDatum->print() + " with representation = " + toStr(addElemHashID)); // log the action

	if(treeNode == nullptr)
		treeNode = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k,hashes), pFactor);

	CPISync *curr = treeNode->getDatum();
	return curr->addElem(newDatum);
	//  return addElem(newDatum, treeNode, NULL, ZZ_ZERO, DATA_MAX); // use the recursive helper method
}

bool InterCPISync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>>& selfMinusOther, list<shared_ptr<DataObject>>& otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering InterCPISync::SyncClient");
    // 0. Set up communicants
    if(!useExisting) {
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);
    }

    mySyncStats.timerStart(SyncStats::COMM_TIME);
    // ... check that the other side is doing the same synchronization
    SendSyncParam(commSync);
    mySyncStats.timerEnd(SyncStats::COMM_TIME);

    // 1. Do the sync
    pTree *parentNode = treeNode;//Create a copy of the root node - Just to make sure that it is not deleted
    commSync->hardResetCommCounters(); //Because each CPISync will reset the communicant stats need to reset and use the "total" fields
    bool result = SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf) // also call the parent to establish bookkeeping variables
                  && _SyncClient(commSync, selfMinusOther, otherMinusSelf, parentNode, ZZ_ZERO, DATA_MAX);//Call the modified Sync with data Ranges

    if (result) { // Sync succeeded
        Logger::gLog(Logger::METHOD, string("Interactive sync succeeded.\n")
                                     + "   self - other =  " + printListOfSharedPtrs(selfMinusOther) + "\n"
                                     + "   other - self =  " + printListOfSharedPtrs(otherMinusSelf) + "\n"
                                     + "\n");
    }
    else
        Logger::gLog(Logger::METHOD, "Synchronization failed.  Please increase bit size of elements or reduce partition factor.");

    // Close communicants
    if(!useExisting) commSync->commClose();

    //Record Stats


    return result;
}

void InterCPISync::SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::SendSyncParam");
    // take care of parent sync method
    SyncMethod::SendSyncParam(commSync);

    commSync->commSend(enumToByte(SyncID));
    commSync->commSend(maxDiff);
    commSync->commSend(bitNum);
    commSync->commSend(probEps);
    commSync->commSend(pFactor);

    if (commSync->commRecv_byte() == SYNC_FAIL_FLAG) throw SyncFailureException("Sync parameters do not match.");

    Logger::gLog(Logger::COMM, "Sync parameters match");
}


void InterCPISync::RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::RecvSyncParam");
    // take care of parent sync method
    SyncMethod::RecvSyncParam(commSync);

    byte theSyncID = commSync->commRecv_byte();
    long mbarClient = commSync->commRecv_long();
    long bitsClient = commSync->commRecv_long();
    int epsilonClient = commSync->commRecv_int();
    long pFactorClient = commSync->commRecv_long();

    if (theSyncID != enumToByte(SyncID) || mbarClient != maxDiff || bitsClient != bitNum || epsilonClient != probEps || pFactor != pFactorClient) {
        // report a failure to establish sync parameters
        commSync->commSend(SYNC_FAIL_FLAG);
        Logger::gLog(Logger::COMM, "Sync parameters differ from client to server: Client has (" +
                                   toStr(mbarClient) + "," + toStr(bitsClient) + "," + toStr(epsilonClient) + "," + toStr(pFactorClient) +
                                   ").  Server has (" + toStr(maxDiff) + "," + toStr(bitNum) + "," + toStr(probEps) + "," + toStr(pFactor) + ").");
        throw SyncFailureException("Sync parameters do not match.");
    }
    commSync->commSend(SYNC_OK_FLAG);
    Logger::gLog(Logger::COMM, "Sync parameters match");
}

bool InterCPISync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>>& selfMinusOther, list<shared_ptr<DataObject>>& otherMinusSelf) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncServer");

    bool result = SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);
    // 0. Set up communicants
    if(!useExisting) {
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);
    }

    mySyncStats.timerStart(SyncStats::COMM_TIME);
    // ... verify sync parameters
    RecvSyncParam(commSync);
    mySyncStats.timerEnd(SyncStats::COMM_TIME);

    // 1. Do the sync
    pTree * parentNode = treeNode;
    commSync->hardResetCommCounters(); //Because each CPISync will reset the communicant stats need to reset and use the "total" fields
    result &= _SyncServer(commSync, selfMinusOther, otherMinusSelf, parentNode, ZZ_ZERO, DATA_MAX);
    if (result) { // Sync succeeded
        Logger::gLog(Logger::METHOD, string("Interactive sync succeeded.\n")
                                     + "   self - other =  " + printListOfSharedPtrs(selfMinusOther) + "\n"
                                     + "   other - self =  " + printListOfSharedPtrs(otherMinusSelf) + "\n"
                                     + "\n");
    } else
        Logger::gLog(Logger::METHOD, "Synchronization failed.  Please increase bit size of elements or reduce partition factor.");



    // 2. Close communicants
    if(!useExisting) commSync->commClose();

    return result;
}

//Private
void InterCPISync::_deleteTree(pTree *treeNode){
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::deleteTree");
    if (treeNode == nullptr)
        return; // i.e. nothing to do
    else {
        for (int ii = 0; ii < pFactor; ii++) {
            _deleteTree(treeNode->child[ii]);
        }
        delete treeNode;
    }
}

ZZ_p InterCPISync::_hash(shared_ptr<DataObject>datum) const {
    ZZ num = datum->to_ZZ(); // convert the datum to a ZZ
    return to_ZZ_p(num % DATA_MAX); // reduce to bit_num bits and make into a ZZ_p
}

bool InterCPISync::_createTreeNode(pTree *&treeNode, pTree *parent, const ZZ &begRange, const ZZ &endRange) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::createTreeNode");
    treeNode = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k, hashes),pFactor);

    CPISync *curr = treeNode->getDatum(); // the current node

    if (parent != nullptr) {
        // add all appropriate parent info
        CPISync *par = parent->getDatum(); // the parent node

        for (auto elem = par->beginElements(); elem != par->endElements(); elem++) {
            const ZZ elemZZ = rep(_hash(*elem));
            if ((elemZZ >= begRange) && (elemZZ < endRange) && (!curr->addElem(*elem))) //if element is in range and add fails
                return false;
        }
    }
    return true;
}

bool InterCPISync::_addElem(shared_ptr<DataObject>newDatum, pTree *&treeNode, pTree *parent, const ZZ &begRange,
                            const ZZ &endRange) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::addElem");
    Logger::gLog(Logger::METHOD_DETAILS, ".  (InterCPISync) adding in range " + toStr(begRange) + " - " + toStr(endRange));
    CPISync *curr;

    // if the current node is empty, create it
    if (treeNode == nullptr) {
        _createTreeNode(treeNode, parent, begRange, endRange); // create a new tree node here
        curr = treeNode->getDatum();

        if (parent == nullptr) // I'm at the root of the tree ... nothing was propagated, so add the new datum
            if (!curr->addElem(newDatum))
                return false;
    }
    else { // treeNode was not null ... just add the newDatum
        curr = treeNode->getDatum();
        if (!curr->addElem(newDatum)) return false; //Add to current node
    }

    // do we need to add to its children as well?
    if (curr->getNumElem() > 0 && endRange - begRange > 1) { // keep dividing until we have no elements or we reach a cell with only one value in its range
        // find the correct child
        ZZ step = (endRange - begRange) / pFactor;

        if (step == 0) { // minimum step size is 1 - this will leave some partitions unused
            step=1;
        }

        long childPos = to_long((addElemHashID - begRange) / step);
        if (childPos >= pFactor) childPos = pFactor - 1; // lump all final elements into the last child

        ZZ newBegin = begRange + childPos * step; // beginning point of the range of the appropriate child
        ZZ newEnd = (childPos == pFactor - 1) ?
                    endRange : // last elements lumped into the last child
                    min(newBegin + step, endRange); // don't overrun the end range of the parent node

        // recursively add newDatum to the appropriate child
        if (!_addElem(newDatum, treeNode->child[childPos], treeNode, newBegin, newEnd)) return false;

        // create nodes for all NULL children
        for (long ii = 0; ii < pFactor - 1; ii++)
            if (treeNode->child[ii] == nullptr)
                _createTreeNode(treeNode->child[ii], treeNode, begRange + ii * step, begRange + (ii + 1) * step);

        if (treeNode->child[pFactor - 1] == nullptr) // last case is special
            _createTreeNode(treeNode->child[pFactor - 1], treeNode, begRange + (pFactor - 1) * step, endRange);
    }
    return true;
}

bool InterCPISync::_delElem(shared_ptr<DataObject> datum, pTree * &node, const ZZ &begRange, const ZZ &endRange) {
	// Compute the hash of the element to find out which children to search if it is present in the parent
	addElemHashID = rep(_hash(datum));

	//If you are in the parent node and delete fails the element is not in your tree
	if(node == treeNode){
		//Only print logger details when you first enter (When node == parentNode)
		Logger::gLog(Logger::METHOD,"Entering recursive InterCPISync::delElem");
		Logger::gLog(Logger::METHOD_DETAILS, ". (InterCPISync) removing item recursively" + datum->print());
		if(!node->getDatum()->delElem(datum)) return false;
	}
	//If you've deleted the last element from the branch and you are not in the parent node you have succeeded
	if (node->getDatum()->getNumElem() == 0) {
		return true;
	}

	ZZ step = (endRange - begRange) / pFactor; //Calculate the size of the bin of one node

	if (step == 0) { // minimum step size is 1 - this will leave some partitions unused
		step = 1;
	}

	long childPos = to_long((addElemHashID - begRange) / step);
	if (childPos >= pFactor) childPos = pFactor - 1; // lump all final elements into the last child

	ZZ newBegin = begRange + childPos * step; // beginning point of the range of the appropriate child
	ZZ newEnd = (childPos == pFactor - 1) ?
				endRange : // last elements lumped into the last child
				min(newBegin + step, endRange); // don't overrun the end range of the parent node

	//Recurse if the child pointer has elements in it
	if (node->child[childPos] != nullptr && node->child[childPos]->getDatum()->getNumElem() != 0) {
		return _delElem(datum, node->child[childPos], newBegin, newEnd);
	}
	else return true;
}





// Recursive helper function for SyncClient

bool InterCPISync::_SyncClient(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther,
							   list<shared_ptr<DataObject>> &otherMinusSelf, pTree *&treeNode) {
	Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncClient");
		try {
			// Declare whether we have an empty node or not
			int response; // the other's declaration
			if (treeNode == nullptr) {// Case 0:  I've got nothing; the other must have something
				Logger::gLog(Logger::METHOD_DETAILS, "My node is empty");
				commSync->commSend(SYNC_NO_INFO);
				response = commSync->commRecv_byte(); // get the other Communicant's initial declaration

				if (response != SYNC_NO_INFO) // it is not the case that both nodes are empty
					CPISync::receiveAllElem(commSync, otherMinusSelf);
					return true;
			} else
				commSync->commSend(SYNC_SOME_INFO); // I have some elements

			// Otherwise, I have something in this node
			CPISync *node = treeNode->getDatum(); // the current node
			response = commSync->commRecv_byte(); // get the other Communicants initial declaration

			Logger::gLog(Logger::METHOD_DETAILS, "My node has " + toStr(node->getNumElem()) + " elements.");
			Logger::gLog(Logger::COMM, " ... data is " + node->printElem());
			if (response == SYNC_NO_INFO) {// Case 1:  I have something; the other has nothing
				node->sendAllElem(commSync, selfMinusOther); // send all I've got
				return true;
			} else { // Case 2: We both have something
				// synchronize the current node
				node->SyncClient(commSync, selfMinusOther, otherMinusSelf); // attempt synchroniztion
				if (commSync->commRecv_byte() == SYNC_FAIL_FLAG) { // i.e. the sync is reported by the Server to have failed; recurse
					Logger::gLog(Logger::METHOD_DETAILS, " > dividing into children");
					// synchronize the children, in order
					for (int ii = 0; ii < pFactor; ii++) {
						Logger::gLog(Logger::METHOD_DETAILS, "  CHILD: " + toStr(ii));
						_SyncClient(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii]);
					}
					Logger::gLog(Logger::METHOD_DETAILS, "< returning from division");
				}
			return true;
			}
		} catch (const SyncFailureException& s) {
			Logger::gLog(Logger::METHOD_DETAILS, s.what());
			commSync->commClose();
			throw (s);
		}
}


// Recursive helper function for SyncServer
bool InterCPISync::_SyncServer(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther,
							   list<shared_ptr<DataObject>> &otherMinusSelf, pTree *&treeNode) {

	Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncServer");
	// Declare whether we have an empty node or not
	int response; // the other's declaration

	if (treeNode == nullptr || treeNode->getDatum()->getNumElem() == 0) {// Case 0:  I've got nothing; the other must have something
		Logger::gLog(Logger::METHOD_DETAILS, "My node is empty");
		commSync->commSend(SYNC_NO_INFO);
		response = commSync->commRecv_byte(); // get the other Communicants initial declaration

		if (response != SYNC_NO_INFO) // it is not the case that both nodes are empty
			CPISync::receiveAllElem(commSync, otherMinusSelf);
			return true;
	} else
		commSync->commSend(SYNC_SOME_INFO); // I have some elements

	// Otherwise, I have something in this node
	CPISync *node = treeNode->getDatum(); // the current node

	response = commSync->commRecv_byte(); // get the other Communicants initial declaration

	Logger::gLog(Logger::METHOD_DETAILS, "My node has " + toStr(node->getNumElem()) + " elements.");
	Logger::gLog(Logger::COMM, " ... data is " + node->printElem());
	if (response == SYNC_NO_INFO) {// Case 1:  I have something; the other has nothing
		node->sendAllElem(commSync, selfMinusOther); // send all I've got
		return true;
	} else { // Case 2: We both have something
		// synchronize the current node
		if (!node->SyncServer(commSync, selfMinusOther, otherMinusSelf)) { // sync failure - go on with children
			commSync->commSend(SYNC_FAIL_FLAG); // notify the client to try the children

		Logger::gLog(Logger::METHOD_DETAILS, " > dividing into children");
		// synchronize the children, in order
		for (int ii = 0; ii < pFactor; ii++) {
			Logger::gLog(Logger::METHOD_DETAILS, "  CHILD: " + toStr(ii));
			_SyncServer(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii]);
		}
		Logger::gLog(Logger::METHOD_DETAILS, "< returning from division");

		} else
			commSync->commSend(SYNC_OK_FLAG); // notify the client that sync succeeded ... do not recurse

		return true; // should always return true
	}
}

bool InterCPISync::_SyncServer(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther,
							   list<shared_ptr<DataObject>> &otherMinusSelf, pTree *treeNode, const ZZ &begRange,
							   const ZZ &endRange) {

	//Establish initial Handshakes - Check If I have nothing or If Client has nothing
	int response;

	if(treeNode == nullptr || treeNode->getDatum()->getNumElem() == 0){
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(SYNC_NO_INFO);
		response = commSync->commRecv_byte();
		if(response!=SYNC_NO_INFO)
			CPISync::receiveAllElem(commSync, otherMinusSelf);

        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());

        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        return true;
	}
	else {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(SYNC_SOME_INFO);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        response = commSync->commRecv_byte();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        CPISync *node = treeNode->getDatum();

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (response == SYNC_NO_INFO) {
            node->sendAllElem(commSync, selfMinusOther); // send all I've got
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            return true;
        } else {
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            //Attempt Sync on current node
            if (!node->SyncServer(commSync, selfMinusOther,
                                  otherMinusSelf)) { // sync failure - create Children and go try to sync

                // Accumulate stats from each CPISync in InterCPISyncs mySyncStats object
                mySyncStats.increment(SyncStats::XMIT, node->mySyncStats.getStat(SyncStats::XMIT));
                mySyncStats.increment(SyncStats::RECV, node->mySyncStats.getStat(SyncStats::RECV));
                mySyncStats.increment(SyncStats::COMM_TIME, node->mySyncStats.getStat(SyncStats::COMM_TIME));
                mySyncStats.increment(SyncStats::IDLE_TIME, node->mySyncStats.getStat(SyncStats::IDLE_TIME));
                mySyncStats.increment(SyncStats::COMP_TIME, node->mySyncStats.getStat(SyncStats::COMP_TIME));


                mySyncStats.timerStart(SyncStats::COMM_TIME);
                commSync->commSend(SYNC_FAIL_FLAG);
                mySyncStats.timerEnd(SyncStats::COMM_TIME);

                mySyncStats.timerStart(SyncStats::COMP_TIME);
                auto *tempTree = new pTree(
                        new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k, hashes), pFactor);
                createChildren(treeNode, tempTree, begRange, endRange);//Create child Nodes;
                treeNode = tempTree;                    //Update the current parent node(parent node only used for referencing the child nodes)
                ZZ step = (endRange - begRange) / pFactor;
                mySyncStats.timerEnd(SyncStats::COMP_TIME);
                //if(step ==0) step = 1;
                for (int ii = 0; ii < pFactor - 1; ii++) {
                    _SyncServer(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii], begRange + (ii * step),
                                begRange + (ii + 1) * step);
                }//Last child needs to handle odd pFactors
                _SyncServer(commSync, selfMinusOther, otherMinusSelf, treeNode->child[pFactor - 1],
                            begRange + ((pFactor - 1) * step), endRange);
            } else {
                mySyncStats.timerStart(SyncStats::COMM_TIME);
                commSync->commSend(SYNC_OK_FLAG);
                mySyncStats.timerEnd(SyncStats::COMM_TIME);
            }
            return true;
        }
    }
}

void InterCPISync::createChildren(pTree * parentNode, pTree * tempTree, const ZZ& begRange, const ZZ& endRange){

	ZZ step = (endRange - begRange)/pFactor;//Get the step size of the node to establish bin sizes
	if(step ==0) step = 1;                  //Set minimum step size to 1 to avoid divide errors
	int pos;
	CPISync * nodes[pFactor];
	if(endRange != begRange){
		for(int ii=0;ii<pFactor;ii++)
		{
			tempTree->child[ii] =  new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k,hashes),pFactor);//Create child nodes for parent
			nodes[ii] = tempTree->child[ii]->getDatum();//Create references for the child nodes(used for insertion)
		}	
		CPISync * parent = parentNode->getDatum();//Get the parent node

		for(auto elem = parent->beginElements();elem!=parent->endElements();elem++){    //Iterate through all parent information
			ZZ elemZZ = rep(_hash(*elem));
			pos = pFactor-1;
			for(int jj=0;jj<pFactor-1;jj++){
				if((elemZZ >= (begRange +(jj*step))) && (elemZZ < (begRange + (jj+1)*step))){
					pos = jj;
					break;
				}
			}
			nodes[pos]->addElem(*elem);//Add to appropriate child
		}
	}
}

bool InterCPISync::_SyncClient(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther,
							   list<shared_ptr<DataObject>> &otherMinusSelf, pTree *treeNode, const ZZ &begRange,
							   const ZZ &endRange)
{
	try{
	    //Initial Handshakes - Check if I have nothing or server has nothing
		int response;
		if(treeNode == nullptr)
		{
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(SYNC_NO_INFO);
			response = commSync->commRecv_byte();
            if (response != SYNC_NO_INFO) // it is not the case that both nodes are empty
				CPISync::receiveAllElem(commSync, otherMinusSelf);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            return true;
		} else
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(SYNC_SOME_INFO); // I have some elements
            response = commSync->commRecv_byte(); // get the other Communicants initial declaration

            CPISync *node = treeNode->getDatum(); // the current node

            Logger::gLog(Logger::METHOD_DETAILS, "My node has " + toStr(node->getNumElem()) + " elements.");
            Logger::gLog(Logger::COMM, " ... data is " + node->printElem());
            if (response == SYNC_NO_INFO) {// Case 1:  I have something; the other has nothing
                node->sendAllElem(commSync, selfMinusOther); // send all I've got
                mySyncStats.timerEnd(SyncStats::COMM_TIME);
                return true;
            } else { // Case 2: We both have something
                // synchronize the current node
                mySyncStats.timerEnd(SyncStats::COMM_TIME);
                node->SyncClient(commSync, selfMinusOther, otherMinusSelf); // attempt synchroniztion

                // Accumulate stats from each CPISync in InterCPISyncs mySyncStats object
                mySyncStats.increment(SyncStats::XMIT,node->mySyncStats.getStat(SyncStats::XMIT));
                mySyncStats.increment(SyncStats::RECV,node->mySyncStats.getStat(SyncStats::RECV));
                mySyncStats.increment(SyncStats::COMM_TIME,node->mySyncStats.getStat(SyncStats::COMM_TIME));
                mySyncStats.increment(SyncStats::IDLE_TIME,node->mySyncStats.getStat(SyncStats::IDLE_TIME));
                mySyncStats.increment(SyncStats::COMP_TIME,node->mySyncStats.getStat(SyncStats::COMP_TIME));

                if (commSync->commRecv_byte() == SYNC_FAIL_FLAG)
                { // i.e. the sync is reported by the Server to have failed; recurse
                    mySyncStats.timerStart(SyncStats::COMP_TIME);
                    auto *tempTree = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k,hashes),pFactor);
                    createChildren(treeNode, tempTree, begRange, endRange);//Create child Nodes;
                    treeNode = tempTree;				    //Update the current parent node(temp parent only children are used)
                    ZZ step = (endRange - begRange)/pFactor;
                    mySyncStats.timerEnd(SyncStats::COMP_TIME);

                    //if(step ==0) step = 1;
                    for(int ii=0;ii<pFactor-1;ii++)
                    {
                        _SyncClient(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii], begRange + (ii * step),
                                    begRange + (ii + 1) * step);
                    }//Last Child needs to handle odd pFactors
                    _SyncClient(commSync, selfMinusOther, otherMinusSelf, treeNode->child[pFactor - 1],
                                begRange + ((pFactor - 1) * step), endRange);
                }
                return true;
            }
	} catch (const SyncFailureException& s) {
		Logger::gLog(Logger::METHOD_DETAILS, s.what());
		commSync->commClose();
		throw (s);
	}
}