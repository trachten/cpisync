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


// constants

const int TIMES = 1; // Times to run oneWay and twoWay sync tests

const size_t eltSize = sizeof(randZZ()) * CHAR_BIT; // size of elements stored in sync tests
const int mBar = UCHAR_MAX*2; // max differences between client and server in sync tests
const string iostr = ""; // initial string used to construct CommString
const bool b64 = true; // whether CommString should communicate in b64
const string host = "localhost"; // host for CommSocket
const unsigned int port = 8021; // port for CommSocket
const int err = 8; // negative log of acceptable error probability for probabilistic syncs
const int numParts = 3; // partitions per level for divide-and-conquer syncs

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

/**
 * Get the temp directory of the system (POSIX).
 * In C++17, this can be replaced with std::filesystem::temp_directory_path.
 * @return path to temp directory
 */
 string tempDir() {
     // possible environment variables containing path to temp directory
     const char* opts[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

     // return the first defined env var in opts
     for(const char* ss : opts) {
         // true iff ss is an env var
         if(const char* path = getenv(ss)) {
             return string(path);
         }
     }
     return "/tmp"; // default temp directory if no env var is found
 }

/**
 * returns all possible gensync configurations constructed using GenSync::Builder
 */
vector<GenSync> builderCombos() {
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

vector<GenSync> constructorCombos(bool useFile) {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<Communicant*> communicants;
            vector<SyncMethod*> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {new ProbCPISync(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {new CPISync_HalfRound(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {new InterCPISync(mBar, eltSize, err, numParts)};
                    break;
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    communicants = {new CommString(iostr, b64)};
                    break;
                case GenSync::SyncComm::socket:
                    communicants = {new CommSocket(port, host)};
                    break;
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }
            ret.emplace_back(useFile ? GenSync(communicants, methods, tempDir() + "/gensynctest/" + toStr(rand())) : GenSync(communicants, methods));
        }
    }
    return ret;
}

/**
 * returns all possible gensync configurations constructed using the standard way of creating GenSync objects (without a file)
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 * @param iostr The string initializing string-based communicants.
 * @param host The host connected to by socket-based communicants.
 * @param port The port connected to by socket-based communicants.
 * @param err The negative log of the failure probability for probability-based syncs.
 * @param numParts Amount of partitions per level for partition-based syncs.
 */
vector<GenSync> standardCombos() {
    return constructorCombos(false);
}

/**
 * returns all possible gensync configurations constructed by creating GenSync objects using a file to store contents
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 * @param iostr The string initializing string-based communicants.
 * @param host The host connected to by socket-based communicants.
 * @param port The port connected to by socket-based communicants.
 * @param err The negative log of the failure probability for probability-based syncs.
 * @param numParts Amount of partitions per level for partition-based syncs.
 */
vector<GenSync> fileCombos() {
    return constructorCombos(true);
}

/**
 * Runs tests assuring that two GenSync objects successfully sync via two-way communication
 */

void _syncTest(bool oneWay, GenSync GenSyncClient, GenSync GenSyncServer) {
    // TODO: implement this for different protocols and communicants
    for(int jj = 0; jj < TIMES; jj++) {
        // setup DataObjects
        const unsigned char SIMILAR = rand(); // amt of elems common to both GenSyncs
        const unsigned char CLIENT_MINUS_SERVER = rand(); // amt of elems unique to client
        const unsigned char SERVER_MINUS_CLIENT = rand(); // amt of elems unique to server

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

        GenSyncClient.addElem(last);
        GenSyncServer.addElem(last); // ensure that adding a object that fits the generic type T works

        // create the expected reconciled multiset
        multiset<string> reconciled;
        for (auto dop : objectsPtr) {
            reconciled.insert(dop->print());
        }

        // create two processes to test successful reconciliation
        int err = 1;
        int chld_state;
        int my_opt = 0;
        pid_t pID = fork();
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);
            if(!oneWay) {
                forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
                // check reasonable statistics
                CPPUNIT_ASSERT(resultClient.bytes > 0);
                CPPUNIT_ASSERT(resultClient.success);

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
            waitpid(pID, &chld_state, my_opt);
            forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
            // check reasonable statistics
            CPPUNIT_ASSERT(resultServer.bytes > 0);
            CPPUNIT_ASSERT(resultServer.success);

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


void syncTestOneWay(GenSync GenSyncClient, GenSync GenSyncServer) {
    _syncTest(true, GenSyncClient, GenSyncServer);
}

/**
 * returns gensync configurations that communicate two ways constructed using the standard way of creating GenSync objects (without a file)
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 * @param host The host connected to by socket-based communicants.
 * @param port The port connected to by socket-based communicants.
 * @param err The negative log of the failure probability for probability-based syncs.
 * @param numParts Amount of partitions per level for partition-based syncs.
 */
vector<GenSync> twoWayCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<Communicant*> communicants;
            vector<SyncMethod*> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {new ProbCPISync(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    continue; // not a 2way sync
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {new InterCPISync(mBar, eltSize, err, numParts)};
                    break;
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {new CommSocket(port, host)};
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

/**
 * returns gensync configurations that communicate one-way constructed using the standard way of creating GenSync objects (without a file)
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 * @param iostr The string initializing string-based communicants.
 * @param host The host connected to by socket-based communicants.
 * @param port The port connected to by socket-based communicants.
 * @param err The negative log of the failure probability for probability-based syncs.
 * @param numParts Amount of partitions per level for partition-based syncs.
 */
vector<GenSync> oneWayCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<Communicant*> communicants;
            vector<SyncMethod*> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {new ProbCPISync(mBar, eltSize, err)};
                    break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {new CPISync_HalfRound(mBar, eltSize, err)};
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {new InterCPISync(mBar, eltSize, err, numParts)};
                    break;
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {new CommSocket(port, host)};
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

void syncTest(GenSync GenSyncClient, GenSync GenSyncServer) {
    _syncTest(false, GenSyncClient, GenSyncServer);
}

// tests

void GenSyncTest::testAddRemoveElems() {
    const unsigned char ELTS = 64; // amt of elems in the data structure

    vector<DataObject *> objectsPtr;

    for (unsigned long ii = 0; ii < ELTS; ii++) { // -1 so that one datum can be added with the templated addElem function
        objectsPtr.push_back(new DataObject(randZZ()));
    }

    ZZ *last = new ZZ(randZZ());

    auto combos = standardCombos(); // also do other combos for this test
    auto file = fileCombos();
    combos.insert(combos.end(), file.begin(), file.end());
    for (GenSync genSync : combos) {
        // Test add and remove data from GenSync
        multiset<string> objectsStr;
        for (auto dop : objectsPtr) {
            genSync.addElem(dop);
            objectsStr.insert(dop->print());
        }
        genSync.addElem(last);
        objectsStr.insert((new DataObject(*last))->print()); // get the expected print value for the non-dataobject datum

        multiset<string> res;
        for (auto dop : genSync.dumpElements()) {
            res.insert(dop->print());
        }
        CPPUNIT_ASSERT(multisetDiff(res, objectsStr).empty()); // test that dumpElements, and implicitly the addElem functions, worked

        // TODO: uncomment when GenSync::delElem is implemented
//        for(auto dop : objectsPtr) {
//            genSync.delElem(dop);
//        }
//        CPPUNIT_ASSERT(genSync.dumpElements().empty());
    }

}

void GenSyncTest::testAddSyncMethodAndComm() {
    GenSync genSync({}, {});
    GenSync genSyncOther({}, {});

    int before = genSync.numComm();
    genSync.addComm(new CommSocket(port, host));
    genSyncOther.addComm(new CommSocket(port, host));
    int after = genSync.numComm();
    CPPUNIT_ASSERT_EQUAL(before + 1, after);

    ProbCPISync toAdd(mBar, eltSize, err);
    genSync.addSyncAgt(&toAdd);
    genSyncOther.addSyncAgt(new ProbCPISync(mBar, eltSize, err));
    CPPUNIT_ASSERT_EQUAL((ProbCPISync*) *genSync.getSyncAgt(0), &toAdd);

    // should succeed.
    syncTest(genSync, genSyncOther);
}

void GenSyncTest::testGetName() {
    // no actual spec for what getName should do - this is implementation dependent.
    GenSync genSync({}, {});
    CPPUNIT_ASSERT_EQUAL(string("I am a GenSync object"), genSync.getName());
}

void GenSyncTest::testCounters() {
    // create the first CommSocket here so that we can check GenSync counters against Communicant counters
    CommSocket cs(port);

    // initialize GenSync objects
    GenSync genSync({&cs}, {new ProbCPISync(mBar, eltSize, err)});
    GenSync genSyncOther({new CommSocket(port)}, {new ProbCPISync(mBar, eltSize, err)});

    CPPUNIT_ASSERT_EQUAL(cs.getTotalTime(), (clock_t) genSync.getSyncTime(0)); // since no sync yet, getSyncTime should be time since communicant creation

    // perform the sync-tests on both GenSync objects. this will result in bytes being transmitted and recieved
    clock_t before = clock();
    genSyncOther.numComm();
    syncTest(genSyncOther, genSync);
    clock_t after = clock();
    double res = genSyncOther.getSyncTime(0);
    clock_t afterGetSync = clock();

    // check that Communicant counters == the respective GenSync counters
    CPPUNIT_ASSERT_EQUAL(cs.getXmitBytes(), genSync.getXmitBytes(0));
    CPPUNIT_ASSERT_EQUAL(cs.getRecvBytes(), genSync.getRecvBytes(0));

    // res should be >= the time spent in syncTest, but <= the clock() called right after it
    // CPPUNIT_ASSERT(res >= (after - before) && res <= afterGetSync); //TODO: uncomment when GenSync::getSyncTime() memleak is resolved
    CPPUNIT_ASSERT_EQUAL(port, (const unsigned int) genSync.getPort(0));

    genSync.delComm(&cs);
    CPPUNIT_ASSERT_EQUAL(0, genSync.numComm());
    genSyncOther.delComm(0);
    CPPUNIT_ASSERT_EQUAL(0, genSyncOther.numComm());

    auto secondSync = new ProbCPISync(mBar, eltSize, err);
    genSync.addSyncAgt(secondSync); // by default, pushed to back of the sync vector. 2 syncs now.
    genSync.delSyncAgt(0); // removes the first sync. the only sync should be secondSync
    CPPUNIT_ASSERT_EQUAL(secondSync->getName(), (*genSync.getSyncAgt(0))->getName()); // ensure that the first and only sync is now secondSync

}

void GenSyncTest::testBuilder() {
    // generate two vectors, one of builder-generated GenSyncs, the other of standard-generated GenSyncs
    vector<GenSync> builderConstructor = builderCombos();
    vector<GenSync> standardConstructor = standardCombos();

    // ensure that there are the same amount of configurations regardless of the constructor called
    CPPUNIT_ASSERT_EQUAL(builderConstructor.size(), standardConstructor.size());

    // test that each GenSync has the same internal params as an identical GenSync created by the other constructor
    for(int ii = 0; ii < builderConstructor.size(); ii++) {
        // basic tests
        CPPUNIT_ASSERT_EQUAL((*builderConstructor.at(ii).getSyncAgt(0))->getName(), (*standardConstructor.at(ii).getSyncAgt(0))->getName());
        CPPUNIT_ASSERT_EQUAL(builderConstructor.at(ii).numComm(), standardConstructor.at(ii).numComm());
    } // better test - sync them. although that is tough
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