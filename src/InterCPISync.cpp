/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   IncreCPI.cpp
 * Author: arit
 * 
 * Created on November 30, 2011, 10:46 PM
 */

#include "Auxiliary.h"
#include "DataObject.h"
#include "CPISync.h"
#include "InterCPISync.h"
#include "CPISync_HalfRound.h"
#include "CPISync_HalfRound_Hashed.h"
#include "Communicant.h"
#include "Exceptions.h"
#include "Logger.h"
InterCPISync::InterCPISync(long m_bar, long bits, int epsilon, int partition)
: maxDiff(m_bar), bitNum(bits), probEps(epsilon + bits), pFactor(partition) {
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::InterCPISync");
  // setup ZZ_p field size
  redundant_k = to_long(CeilToZZ(to_RR(probEps) / bitNum)); //
  if (redundant_k <= 0) //k at least 1
    redundant_k = 1;

  DATA_MAX = power(ZZ_TWO, bitNum);
  ZZ fieldSize = NextPrime(DATA_MAX + maxDiff + redundant_k);
  ZZ_p::init(fieldSize);

  DATA_MAX = power(ZZ_TWO, bitNum); // maximum data element for the multiset
  treeNode = NULL;
  useExisting=false;
  SyncID = SYNCTYPE_Interactive_CPISync; // the synchronization type
}

InterCPISync::~InterCPISync() {
  // clean out the CPISync tree
  deleteTree(treeNode);
}

void InterCPISync::deleteTree(pTree *treeNode) {
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::deleteTree");
    if (treeNode == NULL)
    return; // i.e. nothing to do
  else {
    for (int ii = 0; ii < pFactor; ii++) {
      deleteTree(treeNode->child[ii]);
    }
    delete treeNode;
  }
}

bool InterCPISync::delElem(DataObject* datum) {
  SyncMethod::delElem(datum); // run the parent's version first
  throw new UnimplementedMethodException("InterCPISync delete element");
}

bool InterCPISync::addElem(DataObject* newDatum) {
  /* recursively add an element to all appropriate nodes until:
   * 1.  We reach a leaf node with <m_bar entries.
   * 2.  We reach a node with range < m_bar
   * Assuming there are no hash collisions, we should always end at one
   * of these two options.  Otherwise, we report an error by returning false.
   */
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::AddElem");  
  if (!SyncMethod::addElem(newDatum)) // run the parent's version first
    return false;

  addElemHashID = rep(hash(newDatum)); // compute the hash of the item for use in the recursive addElem

  Logger::gLog(Logger::METHOD_DETAILS, ". (InterCPISync) adding item " + newDatum->print() + " with representation = " + toStr(addElemHashID)); // log the action

  if(treeNode == NULL)
  {
	 treeNode = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k), pFactor);
  }	  
  CPISync * curr = treeNode->getDatum();
  return curr->addElem(newDatum);	
//  return addElem(newDatum, treeNode, NULL, ZZ_ZERO, DATA_MAX); // use the recursive helper method
}

ZZ_p InterCPISync::hash(DataObject* datum) const {
  ZZ num = datum->to_ZZ(); // convert the datum to a ZZ

  return to_ZZ_p(num % DATA_MAX); // reduce to bit_num bits and make into a ZZ_p
}

bool InterCPISync::createTreeNode(pTree * &treeNode, pTree * parent, const ZZ &begRange, const ZZ &endRange) {
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::createTreeNode");
    treeNode = new pTree(
          new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k),
          pFactor);

  CPISync *curr = treeNode->getDatum(); // the current node

  if (parent != NULL) {
    // add all appropriate parent info
    CPISync *par = parent->getDatum(); // the parent node
    vector<DataObject *>::const_iterator elem = par->beginElements();
    for (; elem != par->endElements(); elem++) {
      ZZ elemZZ = rep(hash(*elem));
      if (elemZZ >= begRange && elemZZ < endRange) // element is in the range
        if (!curr->addElem(*elem))
          return false;
    }
  }
  return true;
}

bool InterCPISync::addElem(DataObject* newDatum, pTree * &treeNode, pTree * parent, const ZZ &begRange, const ZZ &endRange) {
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::addElem");
    CPISync *curr;

  Logger::gLog(Logger::METHOD_DETAILS, ".  (InterCPISync) adding in range " + toStr(begRange) + " - " + toStr(endRange));

  // if the current node is empty, create it
  if (treeNode == NULL) {
    createTreeNode(treeNode, parent, begRange, endRange); // create a new tree node here
    curr = treeNode->getDatum();

    if (parent == NULL) // I'm at the root of the tree ... nothing was propagated, so add the new datum
      if (!curr->addElem(newDatum))
        return false;
  } else { // treeNode was not null ... just add the newDatum
    // add to this node
    curr = treeNode->getDatum(); // the current node
    if (!curr->addElem(newDatum))
      return false;
  }

  // do we need to add to its children as well?
  if (curr->getNumElem() > 0 && endRange - begRange > 1) { // keep dividing until we have no elements or we reach a cell with only one value in its range
    // find the correct child
    ZZ step = (endRange - begRange) / pFactor;

    if (step == 0) { // minimum step size is 1 - this will leave some partitions unused
      step=1;
    }
    long childPos = to_long((addElemHashID - begRange) / step);
    if (childPos >= pFactor)
      childPos = pFactor - 1; // lump all final elements into the last child

    ZZ newBegin = begRange + childPos * step; // beginning point of the range of the appropriate child
    ZZ newEnd = (childPos == pFactor - 1) ?
            endRange : // last elements lumped into the last child
            min(newBegin + step, endRange); // don't overrun the end range of the parent node

    // recursively add newDatum to the appropriate child
    if (!addElem(newDatum, treeNode->child[childPos], treeNode, newBegin, newEnd))
      return false;
    // create nodes for all NULL children
    for (long ii = 0; ii < pFactor - 1; ii++)
      if (treeNode->child[ii] == NULL)
        createTreeNode(treeNode->child[ii], treeNode, begRange + ii * step, begRange + (ii + 1) * step);
    if (treeNode->child[pFactor - 1] == NULL) // last case is special
      createTreeNode(treeNode->child[pFactor - 1], treeNode, begRange + (pFactor - 1) * step, endRange);
  }

  return true;
}

void InterCPISync::SendSyncParam(Communicant* commSync, bool oneWay /* = false */) {
  Logger::gLog(Logger::METHOD,"Entering InterCPISync::SendSyncParam");
    // take care of parent sync method
  SyncMethod::SendSyncParam(commSync);

  commSync->commSend(SyncID);
  commSync->commSend(maxDiff);
  commSync->commSend(bitNum);
  commSync->commSend(probEps);
  commSync->commSend(pFactor);
  if (commSync->commRecv_byte() == SYNC_FAIL_FLAG)
    throw SyncFailureException("Sync parameters do not match.");
  Logger::gLog(Logger::COMM, "Sync parameters match");
}

bool InterCPISync::SyncClient(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering InterCPISync::SyncClient");
  // 0. Set up communicants
    if(!useExisting)
  commSync->commConnect();
  // ... check that the other side is doing the same synchronization
  SendSyncParam(commSync);


  // 1. Do the sync
  //Original Code
/*
  bool result = SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf) // also call the parent to establish bookkeeping variables
          && SyncClient(commSync, selfMinusOther, otherMinusSelf, treeNode);*/
  pTree *parentNode = treeNode;//Create a copy of the root node - Just to make sure that it is not deleted
  bool result = SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf) // also call the parent to establish bookkeeping variables
          && SyncClient(commSync, selfMinusOther, otherMinusSelf, parentNode,ZZ_ZERO, DATA_MAX);//Call the modified Sync with data Ranges 
  if (result) { // Sync succeeded
    Logger::gLog(Logger::METHOD, string("Interactive sync succeeded.\n")
            + "   self - other =  " + printListOfPtrs(selfMinusOther) + "\n"
            + "   other - self =  " + printListOfPtrs(otherMinusSelf) + "\n"
            + "\n");
  } else
    Logger::gLog(Logger::METHOD, "Synchronization failed.  Please increase bit size of elements or reduce partition factor.");

  // Close communicants
  if(!useExisting)
  commSync->commClose();

  return result;
}

// Recursive helper function for SyncClient

bool InterCPISync::SyncClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncClient");
  try {
    // Declare whether we have an empty node or not
    int response; // the other's declaration
    if (treeNode == NULL) {// Case 0:  I've got nothing; the other must have something
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
          SyncClient(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii]);
        }
        Logger::gLog(Logger::METHOD_DETAILS, "< returning from division");
      }
      return true;
    }
  } catch (SyncFailureException s) {
    Logger::gLog(Logger::METHOD_DETAILS, s.what());
    commSync->commClose();
    throw (s);
  }
}

void InterCPISync::RecvSyncParam(Communicant* commSync, bool oneWay /* = false */) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::RecvSyncParam");
  // take care of parent sync method
  SyncMethod::RecvSyncParam(commSync);

  byte theSyncID = commSync->commRecv_byte();
  long mbarClient = commSync->commRecv_long();
  long bitsClient = commSync->commRecv_long();
  int epsilonClient = commSync->commRecv_int();
  long pFactorClient = commSync->commRecv_long();

  if (theSyncID != SyncID ||
          mbarClient != maxDiff ||
          bitsClient != bitNum ||
          epsilonClient != probEps ||
          pFactor != pFactorClient) {
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

bool InterCPISync::SyncServer(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncServer");
  // 0. Set up communicants
    if(!useExisting)
  commSync->commListen();

  // ... verify sync parameters
  RecvSyncParam(commSync);

  // 1. Do the sync
//Original Sync
/*  bool result = SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf) // also call the parent to establish bookkeeping variables
          && SyncServer(commSync, selfMinusOther, otherMinusSelf, treeNode);*/
 pTree * parentNode = treeNode;
 bool result = SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf) // also call the parent to establish bookkeeping variables
          && SyncServer(commSync, selfMinusOther, otherMinusSelf, parentNode,ZZ_ZERO, DATA_MAX);
  if (result) { // Sync succeeded
    Logger::gLog(Logger::METHOD, string("Interactive sync succeeded.\n")
            + "   self - other =  " + printListOfPtrs(selfMinusOther) + "\n"
            + "   other - self =  " + printListOfPtrs(otherMinusSelf) + "\n"
            + "\n");
  } else
    Logger::gLog(Logger::METHOD, "Synchronization failed.  Please increase bit size of elements or reduce partition factor.");



  // 2. Close communicants
 if(!useExisting)
  commSync->commClose();

  return result;
}

// Recursive helper function for SyncServer

bool InterCPISync::SyncServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode) {
    Logger::gLog(Logger::METHOD,"Entering InterCPISync::SyncServer");
  // Declare whether we have an empty node or not
   // printf("SyncLevel = [%d]\n",syncLevel);
  int response; // the other's declaration
  if (treeNode == NULL || treeNode->getDatum()->getNumElem() == 0) {// Case 0:  I've got nothing; the other must have something
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
        SyncServer(commSync, selfMinusOther, otherMinusSelf, treeNode->child[ii]);
      }
      Logger::gLog(Logger::METHOD_DETAILS, "< returning from division");

    } else {
      commSync->commSend(SYNC_OK_FLAG); // notify the client that sync succeeded ... do not recurse
    }

    return true; // should always return true
  }
}
bool InterCPISync::SyncServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *parentNode,const ZZ begRange, const ZZ endRange)
{
        //Establish initial Handshakes - Check If I have nothing or If Client has nothing
	int response;
	if(parentNode == NULL || parentNode->getDatum()->getNumElem() == 0){
		commSync->commSend(SYNC_NO_INFO);
		response = commSync->commRecv_byte();
		if(response!=SYNC_NO_INFO)
			CPISync::receiveAllElem(commSync, otherMinusSelf);
		return true;
	}
	else
		commSync->commSend(SYNC_SOME_INFO);

	CPISync * node = parentNode->getDatum();
	response = commSync->commRecv_byte();
	if(response == SYNC_NO_INFO)
	{
		node->sendAllElem(commSync, selfMinusOther); // send all I've got
		return true;
	}
	else
	{
                //Attempt Sync on current node
		if (!node->SyncServer(commSync, selfMinusOther, otherMinusSelf)) { // sync failure - create Children and go try to syn
			commSync->commSend(SYNC_FAIL_FLAG);

			pTree *tempTree = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k),pFactor);
			createChildren(parentNode, tempTree, begRange, endRange);//Create child Nodes;
			if(parentNode != treeNode) delete parentNode;				     //Delete the previous parent node
			parentNode = tempTree;				    //Update the current parent node(parent node only used for referencing the child nodes)
			ZZ step = (endRange - begRange)/pFactor;
			//if(step ==0) step = 1;
			for(int ii=0;ii<pFactor-1;ii++)
			{
				SyncServer(commSync, selfMinusOther, otherMinusSelf, parentNode->child[ii], begRange+(ii*step), begRange + (ii + 1)*step);
			}//Last child needs to handle odd pFactors
			SyncServer(commSync, selfMinusOther, otherMinusSelf, parentNode->child[pFactor-1], begRange+((pFactor-1)*step), endRange);
		}
		else
		{
			commSync->commSend(SYNC_OK_FLAG);
		}
		return true;
	}
} 
void InterCPISync::createChildren(pTree * parentNode, pTree * tempTree, const ZZ begRange, const ZZ endRange)
{
	
	ZZ step = (endRange - begRange)/pFactor;//Get the step size of the node to establish bin sizes
	if(step ==0) step = 1;                  //Set minimum step size to 1 to avoid divide errors
	int pos;
	CPISync * nodes[pFactor];
	if(endRange != begRange){
		for(int ii=0;ii<pFactor;ii++)
		{
			tempTree->child[ii] =  new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k),pFactor);//Create child nodes for parent
			nodes[ii] = tempTree->child[ii]->getDatum();//Create references for the child nodes(used for insertion)
		}	
		CPISync * parent = parentNode->getDatum();//Get the parent node
		vector<DataObject *>::const_iterator elem = parent->beginElements();
		for(;elem!=parent->endElements();elem++){    //Iterate through all parent information
			ZZ elemZZ = rep(hash(*elem));
                        pos = pFactor-1;
                        for(int jj=0;jj<pFactor-1;jj++)
                        {
                            if((elemZZ >= (begRange +(jj*step))) && (elemZZ < (begRange + (jj+1)*step)))
                            {
                                pos = jj;
                                break;
                            }
                        }
                        
                        /*ZZ dival;
			div(dival,elemZZ,begRange+step);     //Calculate the bin 
                        conv(pos,dival);// Which child node to add data from parent
                        if(pos >= pFactor) pos = pFactor-1; // For odd pFactors put the edge data into the last bin
			*/
                        //cout<<"Elem = "<<toStr(elemZZ)<<" Begin Range = "<<toStr(begRange)<<" End Range = "<<toStr(endRange)<<" Step = "<<toStr(step)<<" ZZ Pos = "<<toStr(dival)<<"Int Pos = "<<pos<<endl;
                        //cout<<"Elem = "<<toStr(elemZZ)<<" Begin Range = "<<toStr(begRange)<<" End Range = "<<toStr(endRange)<<" Step = "<<toStr(step)<<" Int Pos = "<<pos<<endl;
			nodes[pos]->addElem(*elem);//Add to appropriate child
		}
	}
}
bool InterCPISync::SyncClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *parentNode,const ZZ begRange, const ZZ endRange) 
{
	try{
                //Initial Handshakes - Check if I have nothing or server has nothing
		int response;
		if(parentNode == NULL)
		{
			commSync->commSend(SYNC_NO_INFO);
			response = commSync->commRecv_byte();
			if (response != SYNC_NO_INFO) // it is not the case that both nodes are empty
				CPISync::receiveAllElem(commSync, otherMinusSelf);
			return true;
		} else
			commSync->commSend(SYNC_SOME_INFO); // I have some elements
		
		CPISync *node = parentNode->getDatum(); // the current node
		response = commSync->commRecv_byte(); // get the other Communicants initial declaration

		Logger::gLog(Logger::METHOD_DETAILS, "My node has " + toStr(node->getNumElem()) + " elements.");
		Logger::gLog(Logger::COMM, " ... data is " + node->printElem());
		if (response == SYNC_NO_INFO) {// Case 1:  I have something; the other has nothing
			node->sendAllElem(commSync, selfMinusOther); // send all I've got
			return true;
		} else { // Case 2: We both have something
			// synchronize the current node
			node->SyncClient(commSync, selfMinusOther, otherMinusSelf); // attempt synchroniztion
			if (commSync->commRecv_byte() == SYNC_FAIL_FLAG) 
			{ // i.e. the sync is reported by the Server to have failed; recurse
				pTree *tempTree = new pTree(new CPISync_ExistingConnection(maxDiff, bitNum, probEps, redundant_k),pFactor);
				createChildren(parentNode, tempTree, begRange, endRange);//Create child Nodes;
				if(parentNode != treeNode) delete parentNode;	    //Delete the previous parent node
				parentNode = tempTree;				    //Update the current parent node(temp parent only children are used)
				ZZ step = (endRange - begRange)/pFactor;
				//if(step ==0) step = 1;
				for(int ii=0;ii<pFactor-1;ii++)
				{
					SyncClient(commSync, selfMinusOther, otherMinusSelf, parentNode->child[ii], begRange+(ii*step), begRange + (ii + 1)*step);
				}//Last Child needs to handle odd pFactors
				SyncClient(commSync, selfMinusOther, otherMinusSelf, parentNode->child[pFactor-1], begRange+((pFactor-1)*step), endRange);
			}
			return true;
		}
	} catch (SyncFailureException s) {
		Logger::gLog(Logger::METHOD_DETAILS, s.what());
		commSync->commClose();
		throw (s);
	}
}
