//
// Created by eliez on 8/6/2018.
//

#include "ProbCPISync.h"
#include "CPISync_HalfRound.h"
#include "InterCPISync.h"
#include "CommString.h"
#include "CommSocket.h"
#include "ForkHandle.h"
#include "Auxiliary.h"
#include "GenSync.h"
#include "FullSync.h"

#ifndef CPISYNCLIB_TESTAUXILIARY_H
#define CPISYNCLIB_TESTAUXILIARY_H

// constants
const int NUM_TESTS = 1; // Times to run oneWay and twoWay sync tests

const size_t eltSize = sizeof(randZZ()) * CHAR_BIT; // size of elements stored in sync tests
const int mBar = UCHAR_MAX*2; // max differences between client and server in sync tests
const string iostr = ""; // initial string used to construct CommString
const bool b64 = true; // whether CommString should communicate in b64
const string host = "localhost"; // host for CommSocket
const unsigned int port = 8031; // port for CommSocket
const int err = 8; // negative log of acceptable error probability for probabilistic syncs
const int numParts = 3; // partitions per level for divide-and-conquer syncs

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
                            setBits(eltSize).
                            setMbar(mBar);
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    builder.
                            setBits(eltSize).
                            setMbar(mBar).
                            setNumPartitions(numParts);
                    break;
                case GenSync::SyncProtocol::FullSync:
                    break; // nothing needs to be done for a fullsync
                default:
                    CPPUNIT_FAIL("A sync method is not being tested for!");
                    break;
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
                    CPPUNIT_FAIL("A communicant is not being tested for!");
                    break;
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
                    methods = {make_shared<ProbCPISync>(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSize, err, numParts)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                default:
                    CPPUNIT_FAIL("A sync method is not being tested for!");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    communicants = {make_shared<CommString>(iostr, b64)};
                    break;
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    CPPUNIT_FAIL("A communicant is not being tested for!");
                    break;
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
                case GenSync::SyncProtocol::CPISync:
                    methods = {make_shared<ProbCPISync>(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSize, err)};
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSize, err, numParts)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                default:
                    CPPUNIT_FAIL("A sync method is not being tested for!");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                case GenSync::SyncComm::string:
                    continue; // not a 1way sync
                default:
                    CPPUNIT_FAIL("A communicant is not being tested for!");
                    break;
            }
            ret.push_back(GenSync(communicants, methods));
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
                    methods = {make_shared<ProbCPISync>(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    continue; // not a 2way sync
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSize, err, numParts)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                default:
                    CPPUNIT_FAIL("A sync method is not being tested for!");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                case GenSync::SyncComm::string:
                    continue; // not a 2way sync
                default:
                    CPPUNIT_FAIL("A communicant is not being tested for!");
                    break;
            }

            ret.push_back(GenSync(communicants, methods));
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
inline void _syncTest(bool oneWay, GenSync GenSyncServer, GenSync GenSyncClient) {
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
            if(!oneWay) {
                // reconcile client with server
                forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);

                // check reasonable statistics
                CPPUNIT_ASSERT(resultClient.bytes > 0);
                CPPUNIT_ASSERT(resultClient.success);

                // convert reconciled elements into string representation
                multiset<string> resClient;
                for (auto dop : GenSyncClient.dumpElements()) {
                    resClient.insert(dop->print());
                }

                // check that expected and resultant reconciled sets match up in both size and contents
                CPPUNIT_ASSERT_EQUAL(reconciled.size(), resClient.size());
                CPPUNIT_ASSERT(multisetDiff(reconciled, resClient).empty());
            }
            exit(0);
        } else if (pID < 0) {
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            // wait for child process to complete
            waitpid(pID, &chld_state, my_opt);

            // reconcile server with client
            forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);

            // check reasonable statistics
            CPPUNIT_ASSERT(resultServer.bytes > 0);
            CPPUNIT_ASSERT(resultServer.success);

            // convert reconciled elements into string representation
            multiset<string> resServer;
            for (auto dop : GenSyncServer.dumpElements()) {
                resServer.insert(dop->print());
            }

            // check that expected and resultant reconciled sets match up in both size and contents
            CPPUNIT_ASSERT_EQUAL(reconciled.size(), resServer.size());
            CPPUNIT_ASSERT(multisetDiff(reconciled, resServer).empty());
        }
    }
}


/**
 * One way synctest
 * @param GenSyncClient
 * @param GenSyncServer
 */
inline void syncTestOneWay(GenSync GenSyncClient, GenSync GenSyncServer) {
    _syncTest(true, GenSyncClient, GenSyncServer);
}

/**
 * Two way synctest
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 */
inline void syncTest(GenSync GenSyncServer, GenSync GenSyncClient) {
    _syncTest(false, GenSyncServer, GenSyncClient);
}

#endif //CPISYNCLIB_TESTAUXILIARY_H
