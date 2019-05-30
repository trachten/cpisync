/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/6/2018.
//

#include <Syncs/IBLTSync.h>
#include <Syncs/IBLTSync_HalfRound.h>
#include "Syncs/ProbCPISync.h"
#include "Syncs/CPISync_HalfRound.h"
#include "Syncs/InterCPISync.h"
#include "Communicants/CommString.h"
#include "Communicants/CommSocket.h"
#include "Aux/Auxiliary.h"
#include "Syncs/GenSync.h"
#include "Syncs/FullSync.h"
#include "Aux/ForkHandle.h"

#ifndef CPISYNCLIB_GENERIC_SYNC_TESTS_H
#define CPISYNCLIB_GENERIC_SYNC_TESTS_H

// constants
const int NUM_TESTS = 1; // Times to run oneWay and twoWay sync tests

const size_t eltSizeSq = (size_t) pow(sizeof(randZZ()), 2); // size^2 of elements stored in sync tests
const size_t eltSize = sizeof(randZZ()); // size of elements stored in sync tests
const int mBar = UCHAR_MAX*2; // max differences between client and server in sync tests
const string iostr; // initial string used to construct CommString
const bool b64 = true; // whether CommString should communicate in b64
const string host = "localhost"; // host for CommSocket
const unsigned int port = 8001; // port for CommSocket
const int err = 8; // negative log of acceptable error probability for probabilistic syncs
const int numParts = 3; // partitions per level for divide-and-conquer syncs
const int numExpElem = UCHAR_MAX*2; // max elements in an IBLT for IBLT syncs

// helpers

/**
 * returns all possible gensync configurations constructed using GenSync::Builder
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> builderCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            GenSync::Builder builder = GenSync::Builder().
                    setProtocol(prot).
                    setComm(comm);

            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                case GenSync::SyncProtocol::OneWayCPISync:
                    builder.
                            setBits(eltSizeSq).
                            setMbar(mBar);
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    builder.
                            setBits(eltSizeSq).
                            setMbar(mBar).
                            setNumPartitions(numParts);
                    break;
                case GenSync::SyncProtocol::FullSync:
                    break; // nothing needs to be done for a fullsync
                case GenSync::SyncProtocol::IBLTSync:
                    builder.
                            setBits(eltSize).
                            setNumExpectedElements(numExpElem);
                    break;
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    builder.
                            setBits(eltSize).
                            setNumExpectedElements(numExpElem);
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    builder.
                            setIoStr(iostr);
                    break;
                case GenSync::SyncComm::socket:
                    builder.
                            setHost(host).
                            setPort(port);
                    break;
                default:
                    continue;
            }

            ret.emplace_back(GenSync(builder.build()));
        }
    }
    return ret;
}

/**
 * return a vector of GenSyncs constructed using either of the two GenSync constructors
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 * @param useFile if true, GenSyncs constructed using file-based constructor. otherwise, constructed using other constructor
 */
inline vector<GenSync> constructorCombos(bool useFile) {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {make_shared<ProbCPISync>(mBar, eltSizeSq, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSizeSq, err)};
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSizeSq, err, numParts)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                case GenSync::SyncProtocol::IBLTSync:
                    methods = {make_shared<IBLTSync>(numExpElem, eltSize)};
                    break;
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    methods = {make_shared<IBLTSync_HalfRound>(numExpElem, eltSize)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    communicants = {make_shared<CommString>(iostr, b64)};
                    break;
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            // call constructor depending on useFile
            ret.emplace_back(useFile ? GenSync(communicants, methods, temporaryDir() + "/gensynctest/" + toStr(rand())) : GenSync(communicants, methods));
        }
    }
    return ret;
}

/**
 * returns gensync configurations that communicate one-way constructed using the standard way of creating GenSync objects (without a file)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> oneWayCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSizeSq, err)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }
            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * returns gensync configurations that communicate two ways constructed using the standard way of creating GenSync objects (without a file)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> twoWayCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {make_shared<ProbCPISync>(mBar, eltSizeSq, err)};
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSizeSq, err, numParts)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * returns gensync configurations that communicate two ways using syncs with probability of only partially succeeding
 * constructed using the standard way of creating GenSync objects (without a file)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> twoWayProbCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::IBLTSync:
                    methods = {make_shared<IBLTSync>(numExpElem, eltSize)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * returns gensync configurations that communicate one way using syncs with probability of only partially succeeding
 * constructed using the standard way of creating GenSync objects (without a file)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> oneWayProbCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    methods = {make_shared<IBLTSync_HalfRound>(numExpElem, eltSize)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * returns all possible gensync configurations constructed using the standard way of creating GenSync objects (without a file)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> standardCombos() {
    return constructorCombos(false);
}

/**
 * returns all possible gensync configurations constructed by creating GenSync objects using a file to store contents
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> fileCombos() {
    return constructorCombos(true);
}

/**
 * Runs tests assuring that two GenSync objects successfully sync via two-way communication
 * @param oneWay true iff the sync will be one way (only server is reconciled)
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 */
inline void _syncTest(GenSync GenSyncServer, GenSync GenSyncClient, bool oneWay=false, bool probSync=false) {
    for(int jj = 0; jj < NUM_TESTS; jj++) {
        // setup DataObjects
        const unsigned char SIMILAR = randByte(); // amt of elems common to both GenSyncs
        const unsigned char CLIENT_MINUS_SERVER = randByte(); // amt of elems unique to client
        const unsigned char SERVER_MINUS_CLIENT = randByte(); // amt of elems unique to server

        vector<DataObject *> objectsPtr;

        for (unsigned long ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER -
                                        1; ii++) {
            objectsPtr.push_back(new DataObject(randZZ()));
        }
        ZZ *last = new ZZ(randZZ()); // last datum represented by a ZZ so that the templated addElem can be tested
        objectsPtr.push_back(new DataObject(*last));

        // ... add data objects unique to the server
        for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + SERVER_MINUS_CLIENT; iter++) {
            GenSyncServer.addElem(*iter);
        }

        // ... add data objects unique to the client
        for (auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT;
             iter != objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; iter++) {
            GenSyncClient.addElem(*iter);
        }

        // add common data objects to both
        for (auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
             iter != objectsPtr.end() - 1; iter++) { // minus 1 so that the templated element can be tested
            GenSyncClient.addElem(*iter);
            GenSyncServer.addElem(*iter);
        }

        // ensure that adding a object that fits the generic type T works
        GenSyncClient.addElem(last);
        GenSyncServer.addElem(last);

        // create the expected reconciled multiset
        multiset<string> reconciled;
        for (auto dop : objectsPtr) {
            reconciled.insert(dop->print());
        }

        // create two processes to test successful reconciliation. the parent process tests the server; the child, the client.
        int err = 1;
        int chld_state;
        int my_opt = 0;
        pid_t pID = fork();
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);

            // in oneWay mode, we only care about the results from the server-side sync
            if(!oneWay) {
                // reconcile client with server
                forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);

                // check reasonable statistics
                CPPUNIT_ASSERT(resultClient.success);
                CPPUNIT_ASSERT(resultClient.bytes > 0);

                // convert reconciled elements into string representation
                multiset<string> resClient;
                for (auto dop : GenSyncClient.dumpElements()) {
                    resClient.insert(dop->print());
                }

                if(!probSync) {
                    // check that expected and resultant reconciled sets match up in both size and contents
                    CPPUNIT_ASSERT_EQUAL(reconciled.size(), resClient.size());
                    CPPUNIT_ASSERT(multisetDiff(reconciled, resClient).empty());
                } else {
                    // True iff the reconciled set contains at least one more element than it did before reconciliation
                    CPPUNIT_ASSERT(resClient.size() > SIMILAR + CLIENT_MINUS_SERVER);

                    // True iff the elements added during reconciliation were elements that the client was lacking that the server had
                    CPPUNIT_ASSERT(multisetDiff(reconciled, resClient).size() < CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT);
                }
            }
            exit(0);
        } else if (pID < 0) {
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            // wait for child process to complete
            waitpid(pID, &chld_state, my_opt);

            // reconcile server with client
            // TODO: replace with forkHandleServer once bugs are fixed w/ it
            forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);

            // check reasonable statistics
            CPPUNIT_ASSERT(resultServer.success);
            CPPUNIT_ASSERT(resultServer.bytes > 0);

            // convert reconciled elements into string representation
            multiset<string> resServer;
            for (auto dop : GenSyncServer.dumpElements()) {
                resServer.insert(dop->print());
            }

            if(!probSync) {
                // check that expected and resultant reconciled sets match up in both size and contents
                CPPUNIT_ASSERT_EQUAL(reconciled.size(), resServer.size());
                CPPUNIT_ASSERT(multisetDiff(reconciled, resServer).empty());
            } else {
                // True iff the reconciled set contains at least one more element than it did before reconciliation
                CPPUNIT_ASSERT(resServer.size() > SIMILAR + SERVER_MINUS_CLIENT);

                // True iff the elements added during reconciliation were elements that the server was lacking that the client had
                CPPUNIT_ASSERT(multisetDiff(reconciled, resServer).size() < CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT);
            }
        }
    }
}


/**
 * One way synctest
 * @param GenSyncClient
 * @param GenSyncServer
 */
inline void syncTestOneWay(const GenSync &GenSyncClient, const GenSync &GenSyncServer) {
    _syncTest(GenSyncClient, GenSyncServer, true);
}

/**
 * Two way synctest
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 */
inline void syncTest(const GenSync &GenSyncServer, const GenSync &GenSyncClient) {
    _syncTest(GenSyncServer, GenSyncClient, false);
}

/**
 * One way probabilistic synctest
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 */
inline void syncTestOneWayProb(const GenSync &GenSyncClient, const GenSync &GenSyncServer) {
    _syncTest(GenSyncClient, GenSyncServer, true, true);
}

/**
 * Two way probabilistic synctest
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 */
inline void syncTestProb(const GenSync &GenSyncClient, const GenSync &GenSyncServer) {
    _syncTest(GenSyncClient, GenSyncServer, false, true);
}

inline string socketSendRecieve(CommSocket* clientPtr,CommSocket* serverPtr,string sendString){
  clientPtr->commSend(sendString.c_str(),sendString.length());
  return serverPtr->commRecv(sendString.length());
}

#endif //CPISYNCLIB_GENERIC_SYNC_TESTS_H
