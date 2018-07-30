/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <iostream>
#include <fstream>

#include "GenSync.h"
#include "Exceptions.h"
#include "Communicant.h"
#include "DataObject.h"
#include "SyncMethod.h"
#include "Logger.h"
#include "Auxiliary.h"
#include "CPISync.h"
#include "CommSocket.h"
#include "CommString.h"
#include "ProbCPISync.h"
#include "InterCPISync.h"
#include "CPISync_HalfRound.h"

/**
 * Construct a default GenSync object - communicants and objects will have to be added later
 */
GenSync::GenSync() {
}

/**
 * Construct a specific GenSync object
 */
GenSync::GenSync(const vector<Communicant*> &cVec, const vector<SyncMethod*> &mVec, const list<DataObject*> &data) {
    myCommVec = cVec;
    mySyncVec = mVec;
    outFile = NULL; // no output file is being used

    // add each datum one by one
    list<DataObject*>::const_iterator itData = data.begin();
    for (; itData != data.end(); itData++)
        addElem(*itData);
}

GenSync::GenSync(const vector<Communicant*> &cVec, const vector<SyncMethod*> &mVec, string fileName) {
    myCommVec = cVec;
    mySyncVec = mVec;
    outFile = NULL; // add elements without writing to the file at first
    Logger::gLog(Logger::METHOD, "Entering GenSync::GenSync");
    // read data from a file
    Logger::gLog(Logger::METHOD, "Utilizing file: " + fileName);
    ifstream inFile(fileName.c_str());
    string str;
    for (getline(inFile, str); inFile.good(); getline(inFile, str)) {
        addElem(new DataObject(str)); // add this datum to our list
        Logger::gLog(Logger::METHOD_DETAILS, "... added set element " + str);
    }
    inFile.close();

    // register the file to which new data should be appended
    outFile = shared_ptr<ofstream>(new ofstream(fileName.c_str(), ios::app));

}

// destruct a gensync object

GenSync::~GenSync() {
    // clear out memory
    int a = myData.size();
    myData.clear();

    //    vector<SyncMethod*>::iterator itAgt = mySyncVec.begin();
    //    for (; itAgt != mySyncVec.end(); itAgt++) {
    //        delete *itAgt;
    //    }
    mySyncVec.clear();

    //    vector<Communicant*>::iterator itComm = myCommVec.begin();
    //    for (; itComm != myCommVec.end(); itComm++) {
    //        delete *itComm;
    //    }
    myCommVec.clear();

    // close and free the output file
    if (outFile != NULL) {
        outFile->close();
        outFile.reset();
    }

}


// listen, receive data and conduct synchronization

bool GenSync::listenSync(int method_num) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::listenSync");
    // find the right syncAgent	
    vector<SyncMethod*>::iterator syncAgent = mySyncVec.begin();
    advance(syncAgent, method_num);

    bool syncSuccess = true; // true if all syncs so far were successful

    // ask each communicant to listen, one by one
    vector<Communicant*>::iterator itComm;
    list<DataObject*> selfMinusOther, otherMinusSelf;
    for (itComm = myCommVec.begin(); itComm != myCommVec.end(); ++itComm) {
        // initialize variables
        selfMinusOther.clear();
        otherMinusSelf.clear();

        try {
            syncSuccess &= (*syncAgent)->SyncServer(*itComm, selfMinusOther, otherMinusSelf);
        } catch (SyncFailureException s) {
            Logger::error_and_quit(s.what());
            return false;
        }

        // add any items that were found in the reconciliation
        list<DataObject*>::iterator itDO;
        for (itDO = otherMinusSelf.begin(); itDO != otherMinusSelf.end(); itDO++)
            addElem(*itDO);
    }

    return syncSuccess;
}

// request connection, send data and get the result
bool GenSync::startSync(int method_num) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::startSync");
    // find the right syncAgent	
    vector<SyncMethod*>::iterator syncAgentIt = mySyncVec.begin();
    advance(syncAgentIt, method_num);

    bool syncSuccess = true; // true if all syncs so far were successful
    vector<Communicant*>::iterator itComm;
    list<DataObject*> selfMinusOther, otherMinusSelf;

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
        } catch (SyncFailureException s) {
            Logger::error_and_quit(s.what());
            return false;
        }

        // add any items that were found in the reconciliation
        list<DataObject*>::iterator itDO;
        for (itDO = otherMinusSelf.begin(); itDO != otherMinusSelf.end(); itDO++)
            addElem(*itDO);
    }

    Logger::gLog(Logger::METHOD, "Sync succeeded:  " + toStr(syncSuccess));
    return syncSuccess;

}

// add element

void GenSync::addElem(DataObject* newDatum) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::addElem");
    // store locally
    myData.push_back(newDatum);

    // update synch methods' metadata
    vector<SyncMethod*>::iterator itAgt;
    for (itAgt = mySyncVec.begin(); itAgt != mySyncVec.end(); ++itAgt) {
        if (!(*itAgt)->addElem(newDatum))
            Logger::error_and_quit("Could not add item " + newDatum->to_string() + ".  Please considering increasing the number of bits per set element.");
    }

    // update file
    if (outFile != NULL)
        (*outFile) << newDatum->to_string() << endl;
}

// delete element

void GenSync::delElem(DataObject* newDatum) {
    throw new UnimplementedMethodException("GenSync::delElem");
}



// insert a communicant in the vector at the index position

void GenSync::addComm(Communicant* newComm, int index) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::addComm");
    vector<Communicant*>::iterator itComm;

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
    vector<Communicant*>::iterator itComm;

    itComm = myCommVec.begin();
    if (index == 1) {
        myCommVec.erase(itComm);
    } else {
        advance(itComm, index - 1);
        myCommVec.erase(itComm);
    }

}

void GenSync::delComm(Communicant* oldComm) {
    myCommVec.erase(std::remove(myCommVec.begin(), myCommVec.end(), oldComm), myCommVec.end());
}

int GenSync::numComm() {
    return myCommVec.size();
}

// insert a syncmethod in the vector at the index position

void GenSync::addSyncAgt(SyncMethod* newAgt, int index) {
    Logger::gLog(Logger::METHOD, "Entering GenSync::addSyncAgt");
    // create and populate the new agent
    list<DataObject*>::iterator itData;
    for (itData = myData.begin(); itData != myData.end(); itData++)
        if (!newAgt->addElem(*itData))
            Logger::error_and_quit("Was not able to add an item to the next syncagent.");

    // add the agent to the sync agents vector
    mySyncVec.push_back(newAgt);
}


// delete a syncmethod from the vector at the index position

void GenSync::delSyncAgt(int index) {
    mySyncVec.erase(getSyncAgt(index));
}

vector<SyncMethod*>::iterator GenSync::getSyncAgt(int index) {
    vector<SyncMethod*>::iterator itAgt;

    itAgt = mySyncVec.begin();
    advance(itAgt, index);

    return itAgt;
}

const list<DataObject *> GenSync::dumpElements() {
    return myData;
}

const long GenSync::getXmitBytes(int commIndex) const {
    return myCommVec[commIndex]->getXmitBytes();
}

const long GenSync::getRecvBytes(int commIndex) const {
    return myCommVec[commIndex]->getRecvBytes();
}

const double GenSync::getSyncTime(int commIndex) const {
    Communicant* comm = myCommVec[commIndex];

    // true iff there has been a sync (since sync resets comm counters)
    if(comm->Communicant::getTotalTime() != comm->Communicant::getResetTime()) {
        return (double) (clock() - comm->Communicant::getResetTime()) / CLOCKS_PER_SEC;
    } else {
        return comm->Communicant::getTotalTime();
    }

}

int GenSync::getPort(int commIndex) {
    // null iff comm isn't a CommSocket
    if (auto cs = dynamic_cast<CommSocket*>(myCommVec[commIndex])) {
        return cs->getPort();
    } else {
        return -1;
    }

}

// Builder methods

GenSync GenSync::Builder::build() {
    // variables of possible use
    vector<Communicant*> theComms;
    vector<SyncMethod*> theMeths;

    // check pre-conditions
    if (proto == SyncProtocol::UNDEFINED)
        throw invalid_argument("The synchronization protocol has not been defined.");
    if (comm == SyncComm::UNDEFINED)
        throw invalid_argument("No communication protocol defined.");

    // setup
    switch (comm) {
        case SyncComm::socket:
            myComm = new CommSocket(port, host);
            Logger::gLog(Logger::METHOD, "Connecting to host " + host + " on port " + toStr(port));
            break;
        case SyncComm::string:
            myComm = new CommString(ioStr, base64);
            Logger::gLog(Logger::METHOD, "Connecting to " + toStr(base64 ? "base64" : "") + " string " + ioStr);
            break;
        default:
            throw invalid_argument("I don't know how to set up communication through the provided requested mode.");
    }
    theComms.push_back(myComm);

    invalid_argument noMbar("Must define <mbar> explicitly for this sync.");
    switch (proto) {
        case SyncProtocol::CPISync:
            if (mbar == Builder::UNDEFINED)
                throw noMbar;
            myMeth = new ProbCPISync(mbar, bits, errorProb);
            break;
        case SyncProtocol::InteractiveCPISync:
            if (mbar == Builder::UNDEFINED)
                throw noMbar;
            myMeth = new InterCPISync(mbar, bits, errorProb, numParts);
            break;
        case SyncProtocol::OneWayCPISync:
            if (mbar == Builder::UNDEFINED)
                throw noMbar;
            myMeth = new CPISync_HalfRound(mbar, bits, errorProb);
            break;
        default:
            throw invalid_argument("I don't know how to synchronize with this protocol.");
    }
    theMeths.push_back(myMeth);

    return GenSync(theComms, theMeths);
}

// static consts

const string GenSync::Builder::DFT_HOST = "localhost";
const string GenSync::Builder::DFT_IO = "";
const int GenSync::Builder::DFT_ERROR = 8;