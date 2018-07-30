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

const int TIMES = 1; // times to test syncing

// consts for reasonable values
const size_t eltSize = sizeof(randZZ()) * CHAR_BIT;
const int mBar = UCHAR_MAX*2;
const string iostr = "";
const string host = "localhost";
const unsigned int port = 8001;
const int err = 8;
const int numParts = 3;
const bool b64 = true;

CPPUNIT_TEST_SUITE_REGISTRATION(GenSyncTest);
GenSyncTest::GenSyncTest(){}
GenSyncTest::~GenSyncTest(){}
void GenSyncTest::setUp(){
    const int SEED = 617;
    srand(SEED);
}
void GenSyncTest::tearDown(){}

/**
 * An awkward helper for iterating enums.
 * @param curr The current enum value
 * @return the next enum value
 */
template <typename T>
T &operator++(T& curr) {
    curr = (T)(((int) (curr) + 1));
    return curr;
}

/**
 * Get the temp directory of the system (POSIX).
 * In C++17, this can be replaced with std::filesystem::temp_directory_path
 * @return path to temp directory
 */
 string tempDir() {
     // possible environment variables that store the temp directory
     const char* opts[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

     // return the first defined env var in opts
     for(const char* ss : opts) {
         if(const char* loc = getenv(ss))
             return string(loc);
     }
     return "/tmp"; // default temp directory if no env var is found
 }

/**
 * returns all possible gensync configurations constructed using the builder way of creating GenSync objects
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 * @param iostr The string initializing string-based communicants.
 * @param host The host connected to by socket-based communicants.
 * @param port The port connected to by socket-based communicants.
 * @param err The negative log of the failure probability for probability-based syncs.
 * @param numParts Amount of partitions per level for partition-based syncs.
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

            GenSync test = GenSync(communicants, methods, tempDir() + "/gensynctest/" + toStr(rand()));
            GenSync other = GenSync(communicants, methods);
            ret.emplace_back(useFile ? test : other);
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
                    communicants = {new CommString(iostr, b64)};
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
 * Runs tests assuring that two GenSync objects successfully sync via two-way communication
 */
void syncTest(GenSync GenSyncClient, GenSync GenSyncServer) {
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

void GenSyncTest::testAddRemoveElems() {
    const unsigned char ELTS = 64; // amt of elems in the data structure

    vector<DataObject *> objectsPtr;

    for (unsigned long ii = 0; ii < ELTS; ii++) { // -1 so that one datum can be added with the templated addElem function
        objectsPtr.push_back(new DataObject(randZZ()));
    }

    ZZ *last = new ZZ(randZZ());

    int bits = sizeof(randZZ()) * CHAR_BIT;
    int mbar = ELTS;

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

    int port = 8001;
    int before = genSync.numComm();
    genSync.addComm(new CommSocket(port));
    genSyncOther.addComm(new CommSocket(port));
    int after = genSync.numComm();
    CPPUNIT_ASSERT_EQUAL(before + 1, after);

    size_t eltSize = UCHAR_MAX*2;
    int mBar = sizeof(randZZ()) * CHAR_BIT;
    int err = 8;

    CPISync toAdd(eltSize, mBar, err);
    genSync.addSyncAgt(&toAdd);
    genSyncOther.addSyncAgt(new CPISync(eltSize, mBar, err));
    CPPUNIT_ASSERT_EQUAL((CPISync*) *genSync.getSyncAgt(0), &toAdd);

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
    CommSocket* cs = new CommSocket(port);

    // initialize GenSync objects
    GenSync genSync({cs}, {new CPISync(mBar, eltSize, err)});
    GenSync genSyncOther({new CommSocket(port)}, {new CPISync(mBar, eltSize, err)});

    // perform the sync-tests on both GenSync objects. this will result in bytes being transmitted and recieved
    syncTest(genSync, genSyncOther);

    // check that Communicant counters == the respective GenSync counters
    CPPUNIT_ASSERT_EQUAL(cs->getXmitBytes(), genSync.getXmitBytes(0));
    CPPUNIT_ASSERT_EQUAL(cs->getRecvBytes(), genSync.getRecvBytes(0));
}

void GenSyncTest::testBuilder() {

    // random parameters... we aren't actually syncing GenSyncs, so values don't need to be actual
    vector<GenSync> builderConstructor = builderCombos();
    vector<GenSync> standardConstructor = standardCombos();

    // ensure that there are the same amount of configurations regardless of the constructor called
    CPPUNIT_ASSERT_EQUAL(builderConstructor.size(), standardConstructor.size());

    // test that each GenSync has the same internal params as an identical GenSync created by the other constructor
    for(int ii = 0; ii < builderConstructor.size(); ii++) {
        // basic tests
        CPPUNIT_ASSERT_EQUAL((*builderConstructor.at(ii).getSyncAgt(0))->getName(), (*standardConstructor.at(ii).getSyncAgt(0))->getName());
        CPPUNIT_ASSERT_EQUAL(builderConstructor.at(ii).numComm(), standardConstructor.at(ii).numComm()); // todo: add a test that the communicants are the same and that there is only 1 syncagent
    } // better test - sync them.
}

void GenSyncTest::testTwoWaySync() {
    vector<GenSync> genSyncsClient = twoWayCombos();
    vector<GenSync> genSyncsServer = twoWayCombos();

    // sync every GenSync configuration with itself
    for(int ii = 0; ii < genSyncsClient.size(); ii++) {
        syncTest(genSyncsClient.at(ii), genSyncsServer.at(ii));
    }
}

void GenSyncTest::testOneWaySync() {
    CPPUNIT_ASSERT_MESSAGE("Unimplemented", true);
    // TODO: implement a test that syncs two gensync objects using a one-way sync (halfround cpi and commstring for instance)
}