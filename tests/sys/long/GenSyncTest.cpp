//
// Created by eliez on 7/23/2018.
//

#include <CommSocket.h>
#include <CPISync.h>
#include <ProbCPISync.h>
#include <CPISync_HalfRound.h>
#include <InterCPISync.h>
#include <CommString.h>
#include <cstdlib>
#include "ForkHandle.h"
#include "GenSyncTest.h"
#include "GenSync.h"
#include "CommDummy.h"
#include "Auxiliary.h"


// constants

CPPUNIT_TEST_SUITE_REGISTRATION(GenSyncTest);

GenSyncTest::GenSyncTest() = default;
GenSyncTest::~GenSyncTest() = default;

void GenSyncTest::setUp(){
    // constant seed so that randomness is the same across runs
    const int SEED = 617;
    srand(SEED);
}
void GenSyncTest::tearDown(){}

// helpers

vector<GenSync> GenSyncTest::builderCombos() {
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
                default:
                    CPPUNIT_FAIL("Unreachable");
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
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            ret.emplace_back(GenSync(builder.build()));
        }
    }
    return ret;
}

vector<GenSync> GenSyncTest::constructorCombos(bool useFile) {
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
                default:
                    CPPUNIT_FAIL("Unreachable");
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
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            // call constructor depending on useFile
            ret.emplace_back(useFile ? GenSync(communicants, methods, temporaryDir() + "/gensynctest/" + toStr(rand())) : GenSync(communicants, methods));
        }
    }
    return ret;
}

vector<GenSync> GenSyncTest::oneWayCombos() {
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
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                case GenSync::SyncComm::string:
                    continue; // not a 1way sync
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }
            ret.push_back(GenSync(communicants, methods));
        }
    }
    return ret;
}

vector<GenSync> GenSyncTest::twoWayCombos() {
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
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                case GenSync::SyncComm::string:
                    continue; // not a 2way sync
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            ret.push_back(GenSync(communicants, methods));
        }
    }
    return ret;
}

vector<GenSync> GenSyncTest::standardCombos() {
    return constructorCombos(false);
}

vector<GenSync> GenSyncTest::fileCombos() {
    return constructorCombos(true);
}

void GenSyncTest::_syncTest(bool oneWay, GenSync GenSyncServer, GenSync GenSyncClient) {
    for(int jj = 0; jj < TIMES; jj++) {
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

void GenSyncTest::syncTestOneWay(GenSync GenSyncClient, GenSync GenSyncServer) {
    _syncTest(true, GenSyncClient, GenSyncServer);
}

void GenSyncTest::syncTest(GenSync GenSyncServer, GenSync GenSyncClient) {
    _syncTest(false, GenSyncServer, GenSyncClient);
}

// tests

void GenSyncTest::testAddRemoveElems() {
    const unsigned char ELTS = 64; // amt of elems to be put in the data structure
    vector<DataObject *> objectsPtr;

    // create one object that is convertible to a DataObject
    ZZ *last = new ZZ(randZZ());

    // create elts-1 random DataObjects (the last element will be `last`)
    for (unsigned long ii = 0; ii < ELTS - 1; ii++) {
        objectsPtr.push_back(new DataObject(randZZ()));
    }

    // create all configurations of GenSyncs (created using both constructors)
    auto combos = standardCombos();
    auto file = fileCombos();
    combos.insert(combos.end(), file.begin(), file.end());

    for (GenSync genSync : combos) {
        multiset<string> objectsStr;

        for (auto dop : objectsPtr) {
            genSync.addElem(dop);

            // store a multiset of the expected dataset's string representation
            objectsStr.insert(dop->print());
        }

        // add the ZZ, thus testing the generic addElem function
        genSync.addElem(last);
        objectsStr.insert((new DataObject(*last))->print());

        // create a multiset containing the string representation of objects stored in GenSync
        multiset<string> res;
        for (auto dop : genSync.dumpElements()) {
            res.insert(dop->print());
        }

        CPPUNIT_ASSERT(multisetDiff(res, objectsStr).empty());

        // TODO: uncomment when GenSync::delElem is implemented
//        for(auto dop : objectsPtr) {
//            genSync.delElem(dop);
//        }
//        CPPUNIT_ASSERT(genSync.dumpElements().empty());
    }

}

void GenSyncTest::testAddRemoveSyncMethodAndComm() {
    // create two empty GenSyncs
    GenSync genSync({}, {});
    GenSync genSyncOther({}, {});

    // add a CommSocket and test that numComm increases by one as a result
    int before = genSync.numComm();
    auto cs = make_shared<CommSocket>(port, host);
    genSync.addComm(cs);
    genSyncOther.addComm(make_shared<CommSocket>(port, host));
    int after = genSync.numComm();
    CPPUNIT_ASSERT_EQUAL(before + 1, after);

    // add a ProbCPISync and test that getSyncAgt correctly reports the sync agent at index #0
    auto toAdd = make_shared<ProbCPISync>(mBar, eltSize, err);
    genSync.addSyncAgt(toAdd);
    genSyncOther.addSyncAgt(make_shared<ProbCPISync>(mBar, eltSize, err));
    CPPUNIT_ASSERT_EQUAL((ProbCPISync*)(*genSync.getSyncAgt(0)).get(), toAdd.get());

    // syncing with the newly added commsocket and probcpisync should succeed
    syncTest(genSync, genSyncOther);

    // test deleting a communicant by index and by pointer
    genSync.delComm(cs);
    CPPUNIT_ASSERT_EQUAL(0, genSync.numComm());
    genSyncOther.delComm(0);
    CPPUNIT_ASSERT_EQUAL(0, genSyncOther.numComm());

    // test deleting a syncmethod

    // create an InterCPISync to add and push to the end of the SyncMethod vector in `genSync`
    auto secondSync = make_shared<InterCPISync>(mBar, eltSize, err, numParts);
    genSync.addSyncAgt(secondSync);

    // removes the first SyncMethod. if delSyncAgt is successful, the SyncMethod at idx #0 should be `secondSync`
    genSync.delSyncAgt(0);
    auto newFirst = dynamic_cast<InterCPISync*>((*genSync.getSyncAgt(0)).get());
    CPPUNIT_ASSERT(newFirst != NULL && newFirst->getName() == secondSync->getName());
}

void GenSyncTest::testGetName() {
    GenSync genSync({}, {});

    // check that string returned by getName actually contains some form of information about the GenSync object
    CPPUNIT_ASSERT(genSync.getName() != string());
}

void GenSyncTest::testCounters() {
    // create the first CommSocket here so that we can check GenSync counters against Communicant counters for accuracy
    auto cs = make_shared<CommSocket>(port);

    // initialize GenSync objects
    GenSync genSync({cs}, {make_shared<ProbCPISync>(mBar, eltSize, err)});
    GenSync genSyncOther({make_shared<CommSocket>(port)}, {make_shared<ProbCPISync>(mBar, eltSize, err)});

    // since no sync has happened yet, getSyncTime should report the time since the Communicant at idx #0's creation
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double) cs->getTotalTime() / CLOCKS_PER_SEC, genSync.getSyncTime(0), 1.0);

    // perform the sync-tests on both GenSync objects. this will result in bytes being transmitted and received.

    // get an upper bound of the time since the last sync to test against `res`
    double before = (double) clock() / CLOCKS_PER_SEC;
    syncTest(genSyncOther, genSync);
    double after = (double) clock() / CLOCKS_PER_SEC;
    double res = genSyncOther.getSyncTime(0);

    // check that Communicant counters == the respective GenSync counters
    CPPUNIT_ASSERT_EQUAL(cs->getXmitBytes(), genSync.getXmitBytes(0));
    CPPUNIT_ASSERT_EQUAL(cs->getRecvBytes(), genSync.getRecvBytes(0));

    // `res` should be positive and less than the time spent in syncTest
    CPPUNIT_ASSERT(res <= after - before && res >= 0); // fix
}

void GenSyncTest::testPort() {
    GenSync genSync({make_shared<CommSocket>(port)}, {make_shared<ProbCPISync>(eltSize, mBar, err)});
    CPPUNIT_ASSERT_EQUAL(port, (const unsigned int) genSync.getPort(0));

    GenSync genSyncOther({make_shared<CommString>(iostr)}, {make_shared<ProbCPISync>(eltSize, mBar, err)});
    CPPUNIT_ASSERT_EQUAL(-1, genSyncOther.getPort(0));
}

void GenSyncTest::testBuilder() {
    // generate two vectors, one of builder-generated GenSyncs, the other of standard-generated GenSyncs
    vector<GenSync> builderConstructor = builderCombos();
    vector<GenSync> standardConstructor = standardCombos();

    // ensure that there are the same amount of configurations regardless of the constructor called
    CPPUNIT_ASSERT_EQUAL(builderConstructor.size(), standardConstructor.size());

    // test that each GenSync has the same internal params as an identical GenSync created by the other constructor
    for(int ii = 0; ii < builderConstructor.size(); ii++) {
        // basic equality tests - could also synctest identical builders with standards, but right now certain configs require one- or two-way syncs
        CPPUNIT_ASSERT_EQUAL((*builderConstructor.at(ii).getSyncAgt(0))->getName(), (*standardConstructor.at(ii).getSyncAgt(0))->getName());
        CPPUNIT_ASSERT_EQUAL(builderConstructor.at(ii).numComm(), standardConstructor.at(ii).numComm());
    }
}

void GenSyncTest::testTwoWaySync() {
    vector<GenSync> twoWayClient = twoWayCombos();
    vector<GenSync> twoWayServer = twoWayCombos();

    // sync every GenSync configuration with itself
    for(int ii = 0; ii < twoWayClient.size(); ii++) {
        syncTest(twoWayClient.at(ii), twoWayServer.at(ii));
    }
}

void GenSyncTest::testOneWaySync() {
    vector<GenSync> oneWayClient = oneWayCombos();
    vector<GenSync> oneWayServer = oneWayCombos();

    // sync every GenSync configuration with itself
    for(int ii = 0; ii < oneWayClient.size(); ii++) {
        syncTestOneWay(oneWayClient.at(ii), oneWayServer.at(ii));
    }
}
