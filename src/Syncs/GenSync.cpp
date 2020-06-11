/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <iostream>
#include <fstream>

#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/CPISync.h>
#include <CPISync/Communicants/CommSocket.h>
#include <CPISync/Communicants/CommString.h>
#include <CPISync/Syncs/ProbCPISync.h>
#include <CPISync/Syncs/InterCPISync.h>
#include <CPISync/Syncs/FullSync.h>
#include <CPISync/Syncs/IBLTSync.h>
#include <CPISync/Syncs/IBLTSync_HalfRound.h>
#include <CPISync/Syncs/CPISync_HalfRound.h>
#include <CPISync/Syncs/IBLTSetOfSets.h>
#include <CPISync/Syncs/CuckooSync.h>

using namespace std::chrono;

/**
 * Construct a default GenSync object - communicants and objects will have to be added later
 */
GenSync::GenSync() = default;

/**
 * Construct a specific GenSync object
 */
GenSync::GenSync(
        const vector<shared_ptr<Communicant>> &cVec,
        const vector<shared_ptr<SyncMethod>> &mVec,
        void (*postProcessing)(list<shared_ptr<DataObject>>,list<shared_ptr<DataObject>>,void (GenSync::*add)(shared_ptr<DataObject>),bool (GenSync::*del)(shared_ptr<DataObject>), GenSync *pGenSync),
        const list<shared_ptr<DataObject>> &data)
{
    myCommVec = cVec;
    mySyncVec = mVec;
    outFile = nullptr; // no output file is being used
    _PostProcessing = postProcessing;

    // add each datum one by one
    auto itData = data.begin();
    for (; itData != data.end(); itData++)
        addElem(*itData);
}

GenSync::GenSync(const vector<shared_ptr<Communicant>> &cVec, const vector<shared_ptr<SyncMethod>> &mVec, const string& fileName) {
    myCommVec = cVec;
    mySyncVec = mVec;
    outFile = nullptr; // add elements without writing to the file at first
    Logger::gLog(Logger::METHOD, "Entering GenSync::GenSync");
    // read data from a file
    Logger::gLog(Logger::METHOD, "Utilizing file: " + fileName);
    ifstream inFile(fileName.c_str());
    string str;
    for (getline(inFile, str); inFile.good(); getline(inFile, str)) {
        addElem(make_shared<DataObject>(str)); // add this datum to our list
        Logger::gLog(Logger::METHOD_DETAILS, "... added set element " + str);
    }
    inFile.close();

    // register the file to which new data should be appended
    outFile = std::make_shared<ofstream>(fileName.c_str(), ios::app);

}

// destruct a gensync object

GenSync::~GenSync() {
    // clear out memory
    myData.clear();

    //    vector<shared_ptr<SyncMethod>>::iterator itAgt = mySyncVec.begin();
    //    for (; itAgt != mySyncVec.end(); itAgt++) {
    //        delete *itAgt;
    //    }
    mySyncVec.clear();

    //    vector<shared_ptr<Communicant>>::iterator itComm = myCommVec.begin();
    //    for (; itComm != myCommVec.end(); itComm++) {
    //        delete *itComm;
    //    }
    myCommVec.clear();

    // close and free the output file
    if (outFile != nullptr) {
        outFile->close();
        outFile.reset();
    }

}

//DATA MANIPULATION

// add element
void GenSync::addElem(shared_ptr<DataObject> newDatum) {
	Logger::gLog(Logger::METHOD, "Entering GenSync::addElem");
	// store locally
	myData.push_back(newDatum);

	// update sync methods' metadata
	vector<shared_ptr<SyncMethod>>::iterator itAgt;
	for (itAgt = mySyncVec.begin(); itAgt != mySyncVec.end(); ++itAgt) {
		if (!(*itAgt)->addElem(newDatum))
			Logger::error_and_quit("Could not add item " + newDatum->to_string() + ".  Please considering increasing the number of bits per set element.");
	}

	// update file
	if (outFile != nullptr)
		(*outFile) << newDatum->to_string() << endl;
}

// delete element
bool GenSync::delElem(shared_ptr<DataObject> delPtr) {
	Logger::gLog(Logger::METHOD, "Entering GenSync::delElem");
	if (!myData.empty()) {
		//Iterate through mySyncVec and call that sync's delElem method
		for (const auto& itAgt : mySyncVec) {
			if (!itAgt->delElem(delPtr)) {
                Logger::error("Error deleting item. SyncVec delete failed ");
				return false;
			}
		}

		//Remove data from GenSync object meta-data and report success of delete
		unsigned long before = myData.size();
		myData.remove(delPtr); //Does not have a return value so check size difference
		return myData.size() < before;
	}
	else{
		Logger::error("genSync is empty");
		return false;
	}
}

//Call delete elements on all data
bool GenSync::clearData(){
	bool success = true;

	auto itr = myData.begin();
	while (itr != myData.end()) {
		success &= delElem(*itr++);
	}

	return success && myData.empty();
}

const list<string> GenSync::dumpElements() {
	list<string> dump;
	for(const auto& itr : myData){
		dump.push_back(itr->print());
	}
	return dump;
}

// COMMUNICANT MANIPULATION

// insert a communicant in the vector at the index position
void GenSync::addComm(const shared_ptr<Communicant>& newComm, int index) {
	Logger::gLog(Logger::METHOD, "Entering GenSync::addComm");
	vector<shared_ptr<Communicant>>::iterator itComm;

	itComm = myCommVec.begin();
	if (index == 0) {
		myCommVec.push_back(newComm);
	} else {
		advance(itComm, index - 1);
		myCommVec.insert(itComm, newComm);
	}
}

/**
 *  delete a communicant from the vector at the index position
 * */
void GenSync::delComm(int index) {
	vector<shared_ptr<Communicant>>::iterator itComm;

	itComm = myCommVec.begin();
	if (index == 0) {
		myCommVec.erase(itComm);
	} else {
		advance(itComm, index - 1);
		myCommVec.erase(itComm);
	}

}

void GenSync::delComm(const shared_ptr<Communicant>& oldComm) {
	myCommVec.erase(std::remove(myCommVec.begin(), myCommVec.end(), oldComm), myCommVec.end());
}

int GenSync::numComm() {
	return narrow_cast<int>(myCommVec.size());
}

// insert a syncmethod in the vector at the index position
void GenSync::addSyncAgt(const shared_ptr<SyncMethod>& newAgt, int index) {
	Logger::gLog(Logger::METHOD, "Entering GenSync::addSyncAgt");
	// create and populate the new agent
	list<shared_ptr<DataObject>>::iterator itData;
	for (itData = myData.begin(); itData != myData.end(); itData++)
		if (!newAgt->addElem(*itData))
			Logger::error_and_quit("Was not able to add an item to the next syncagent.");

	// add the agent to the sync agents vector
	auto idxIter = mySyncVec.begin();
	advance(idxIter, index);
	mySyncVec.insert(idxIter, newAgt);
}


// delete a syncmethod from the vector at the index position

void GenSync::delSyncAgt(int index) {
	mySyncVec.erase(getSyncAgt(index));
}

vector<shared_ptr<SyncMethod>>::iterator GenSync::getSyncAgt(int index) {
	vector<shared_ptr<SyncMethod>>::iterator itAgt;

	itAgt = mySyncVec.begin();
	advance(itAgt, index);

	return itAgt;
}

// SYNCHRONIZATION METHODS

// listen, receive data and conduct synchronization
bool GenSync::serverSyncBegin(int sync_num) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::serverSyncBegin");
    // find the right syncAgent	
    auto syncAgent = mySyncVec.begin();
    advance(syncAgent, sync_num);

    bool syncSuccess = true; // true if all syncs so far were successful

    // ask each communicant to listen, one by one
    vector<shared_ptr<Communicant>>::iterator itComm;
    list<shared_ptr<DataObject>> selfMinusOther, otherMinusSelf;
    for (itComm = myCommVec.begin(); itComm != myCommVec.end(); ++itComm) {
        // initialize variables
        selfMinusOther.clear();
        otherMinusSelf.clear();

        try {
            syncSuccess &= (*syncAgent)->SyncServer(*itComm, selfMinusOther, otherMinusSelf);
        } catch (SyncFailureException& s) {
            Logger::error_and_quit(s.what());
            return false;
        }

        // post process and add any items that were found in the reconciliation
        _PostProcessing(otherMinusSelf, myData, &GenSync::addElem, &GenSync::delElem, this);
    }

    return syncSuccess;
}

// request connection, send data and get the result
bool GenSync::clientSyncBegin(int sync_num) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::clientSyncBegin");
    // find the right syncAgent	
    auto syncAgentIt = mySyncVec.begin();
    advance(syncAgentIt, sync_num);

    bool syncSuccess = true; // true if all syncs so far were successful
    vector<shared_ptr<Communicant>>::iterator itComm;
    list<shared_ptr<DataObject>> selfMinusOther, otherMinusSelf;

    for (itComm = myCommVec.begin(); itComm != myCommVec.end(); ++itComm) {
        // initialize variables
        selfMinusOther.clear();
        otherMinusSelf.clear();

        // do the sync
        try {
            if (!(*syncAgentIt)->SyncClient(*itComm, selfMinusOther, otherMinusSelf)) {
                Logger::gLog(Logger::METHOD, "Sync to " + (*itComm)->getName() + " failed!");
                syncSuccess = false;
            }
        } catch (SyncFailureException& s) {
            Logger::error_and_quit(s.what());
            return false;
        }

        // add any items that were found in the reconciliation
        _PostProcessing(otherMinusSelf, myData, &GenSync::addElem, &GenSync::delElem, this);
    }

    Logger::gLog(Logger::METHOD, "Sync succeeded:  " + toStr(syncSuccess));
    return syncSuccess;

}

const unsigned long GenSync::getXmitBytes(int syncIndex) const {
    return narrow_cast<unsigned long>(mySyncVec[syncIndex]->mySyncStats.getStat(SyncMethod::SyncStats::XMIT));
}

const unsigned long GenSync::getRecvBytes(int syncIndex) const {
    return narrow_cast<unsigned long>(mySyncVec[syncIndex]->mySyncStats.getStat(SyncMethod::SyncStats::RECV));
}

const double GenSync::getCommTime(int syncIndex) const {
    return mySyncVec[syncIndex]->mySyncStats.getStat(SyncMethod::SyncStats::COMM_TIME);
}

const double GenSync::getIdleTime(int syncIndex) const {
    return mySyncVec[syncIndex]->mySyncStats.getStat(SyncMethod::SyncStats::IDLE_TIME);
}

const double GenSync::getCompTime(int syncIndex) const {
    return mySyncVec[syncIndex]->mySyncStats.getStat(SyncMethod::SyncStats::COMP_TIME);
}

const double GenSync::getTotalTime(int syncIndex) const {
    return mySyncVec[syncIndex]->mySyncStats.totalTime();
}


string GenSync::printStats(int syncIndex) const{
	stringstream returnStream;

	returnStream << "Stats for " << mySyncVec[syncIndex]->getName() << endl;
	returnStream << "Bytes Transmitted: " << getXmitBytes(syncIndex) << endl;
	returnStream << "Bytes Received: " << getRecvBytes(syncIndex) << endl;
	returnStream << "Communication Time(s): " << getCommTime(syncIndex) << endl;
    returnStream << "Idle Time(s): " << getIdleTime(syncIndex) << endl;
    returnStream << "Computation Time(s): " <<  getCompTime(syncIndex) << endl;

	return returnStream.str();
}

int GenSync::getPort(int commIndex) {
    // null iff comm isn't a CommSocket
    if (auto cs = dynamic_cast<CommSocket*>(myCommVec[commIndex].get())) {
        return cs->getPort();
    } else {
        return -1;
    }

}

// Builder methods

GenSync GenSync::Builder::build() {
    // variables of possible use
    vector<shared_ptr<Communicant>> theComms;
    vector<shared_ptr<SyncMethod>> theMeths;

    // check pre-conditions
    if (proto == SyncProtocol::UNDEFINED)
        throw invalid_argument("The synchronization protocol has not been defined.");
    if (comm == SyncComm::UNDEFINED)
        throw invalid_argument("No communication protocol defined.");

    // setup
    switch (comm) {
        case SyncComm::socket:
            myComm = make_shared<CommSocket>(port, host);
            Logger::gLog(Logger::METHOD, "Connecting to host " + host + " on port " + toStr(port));
            break;
        case SyncComm::string:
            myComm = make_shared<CommString>(ioStr, base64);
            Logger::gLog(Logger::METHOD, "Connecting to " + toStr(base64 ? "base64" : "") + " string " + ioStr);
            break;
        default:
            throw invalid_argument("I don't know how to set up communication through the provided requested mode.");
    }
    theComms.push_back(myComm);

    const invalid_argument noMbar("Must define <mbar> explicitly for this sync.");

    // set default post process function pointer
    _postProcess = SyncMethod::postProcessing_SET;
    
    switch (proto)
    {
        case SyncProtocol::CPISync:
            if (mbar.isNullQ())
                throw noMbar;
            myMeth = make_shared<CPISync>(mbar, bits, errorProb, 0, hashes);
            break;
        case SyncProtocol::ProbCPISync:
            if (mbar.isNullQ())
                throw noMbar;
            myMeth = make_shared<ProbCPISync>(mbar, bits, errorProb, hashes);
            break;
        case SyncProtocol::InteractiveCPISync:
            if (mbar.isNullQ())
                throw noMbar;
            myMeth = make_shared<InterCPISync>(mbar, bits, errorProb, numParts, hashes);
            break;
        case SyncProtocol::OneWayCPISync:
            if (mbar.isNullQ())
                throw noMbar;
            myMeth = make_shared<CPISync_HalfRound>(mbar, bits, errorProb);
            break;
        case SyncProtocol::FullSync:
            myMeth = make_shared<FullSync>();
            break;
        case SyncProtocol::IBLTSync:
            myMeth = make_shared<IBLTSync>(numExpElem, bits);
            break;
        case SyncProtocol::OneWayIBLTSync:
            myMeth = make_shared<IBLTSync_HalfRound>(numExpElem, bits);
            break;
        case SyncProtocol::IBLTSetOfSets:
            myMeth = make_shared<IBLTSetOfSets>(numExpElem, numElemChldSet, bits);
            _postProcess = IBLTSetOfSets::postProcessing_IBLTSetOfSets;
            break;
        case SyncProtocol::CuckooSync:
            myMeth = make_shared<CuckooSync>(fngprtSize, bucketSize, filterSize, maxKicks);
            break;
        default:
            throw invalid_argument("I don't know how to synchronize with this protocol.");
    }
    theMeths.push_back(myMeth);

    if (fileName.isNullQ()) // is data to be drawn from a file?
        return GenSync(theComms, theMeths, _postProcess);
    else
        return GenSync(theComms, theMeths, fileName);
}

// static consts

const string GenSync::Builder::DFT_HOST = "localhost";
const string GenSync::Builder::DFT_IO;
const int GenSync::Builder::DFT_ERROR = 8;
const auto GenSync::Builder::DFT_MBAR = NOT_SET<long>();