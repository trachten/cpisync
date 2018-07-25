//
// Created by eliez on 7/23/2018.
//

#include <CommSocket.h>
#include <CPISync.h>
#include <ProbCPISync.h>
#include <CPISync_HalfRound.h>
#include <InterCPISync.h>
#include <CommString.h>
#include "ForkHandle.h"
#include "GenSyncTest.h"
#include "GenSync.h"

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
 * returns all possible gensync configurations
 * @param eltSize The size, in bits, of elements in CPISync-based sync protocols.
 * @param mbar The max amount of differences between GenSyncs in CPISync-based sync protocols.
 */
vector<GenSync*> genSyncCombos(size_t eltSize, int mBar, string iostr="", string host="localhost", int port=8001, double err=0.0001, int numParts=3) {
    vector<GenSync*> ret;

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

            ret.push_back(new GenSync(builder.build()));
        }
    }
    return ret;
}

vector<GenSync*> firstGenSyncConstructorCombos(size_t eltSize, int mBar, string iostr="", string host="localhost", int port=8001, double err=0.0001, int numParts=3) {
    vector<GenSync*> ret;

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
                    communicants = {new CommString(iostr, true)}; //TODO: b64 should be a constant
                    break;
                case GenSync::SyncComm::socket:
                    communicants = {new CommSocket(port, host)};
                    break;
                default:
                    CPPUNIT_FAIL("Unreachable");
                    break;
            }

            ret.push_back(new GenSync(communicants, methods));
        }
    }
    return ret;
}

void syncTest(GenSync GenSyncClient, GenSync GenSyncServer, int times) {
    // TODO: implement this for different protocols and communicants
    for(int jj = 0; jj < times; jj++) {
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

void GenSyncTest::testSyncWithCPI() {
    // TODO: implement this for different protocols and communicants
    for(int jj = 0; jj < TIMES; jj++) {
        // setup DataObjects
        const unsigned char SIMILAR = rand(); // amt of elems common to both GenSyncs
        const unsigned char CLIENT_MINUS_SERVER = rand(); // amt of elems unique to client
        const unsigned char SERVER_MINUS_CLIENT = rand(); // amt of elems unique to server

        vector<DataObject *> objectsPtr;

        for (unsigned long ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER - 1; ii++) { // -1 so that one object can be templated
            objectsPtr.push_back(new DataObject(randZZ()));
        }
        ZZ* last = new ZZ(randZZ());
        objectsPtr.push_back(new DataObject(*last));

 //  vector<GenSync> syncs = genSyncCombos(sizeof(randZZ())*CHAR_BIT, CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT);
//    for(GenSync GenSyncClient : syncs) {
//        for(GenSync GenSyncServer : syncs) {


                // Build server
                GenSync GenSyncServer = GenSync::Builder().
                        setProtocol(GenSync::SyncProtocol::CPISync).
                        setMbar(CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT).
                        setBits(sizeof(randZZ())*CHAR_BIT).
                        setComm(GenSync::SyncComm::socket).
                        build();
                // ... add data objects unique to the server
                for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + SERVER_MINUS_CLIENT; iter++) {
                    GenSyncServer.addElem(*iter);
                }

                // Build client
                GenSync GenSyncClient = GenSync::Builder().
                        setProtocol(GenSync::SyncProtocol::CPISync).
                        setMbar(CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT).
                        setBits(sizeof(randZZ())*CHAR_BIT).
                        setComm(GenSync::SyncComm::socket).
                        build();
                // ... add data objects unique to the client
                for (auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT; iter != objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; iter++) {
                    GenSyncClient.addElem(*iter);
                }

                // add common data objects to both
                for(auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; iter != objectsPtr.end() - 1; iter++) { // minus 1 so that the templated element can be tested
                    GenSyncClient.addElem(*iter);
                    GenSyncServer.addElem(*iter);
                }
                GenSyncClient.addElem(last);
                GenSyncServer.addElem(last);

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
        //}
    //}
}
//
//void syncCommStrings(GenSync GenSyncClient, GenSync GenSyncServer) {
//    GenSyncClient.startSync(0);
//    GenSyncClient.m
//}

void GenSyncTest::testSyncWithSockets() {
    for(int ii = 0; ii < TIMES; ii++) {
        // setup DataObjects
        const unsigned char SIMILAR = rand(); // amt of elems common to both GenSyncs
        const unsigned char CLIENT_MINUS_SERVER = rand(); // amt of elems unique to client
        const unsigned char SERVER_MINUS_CLIENT = rand(); // amt of elems unique to server

        vector<DataObject *> objectsPtr;

        for (unsigned long ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER -
                                        1; ii++) { // -1 so that one object can be templated
            objectsPtr.push_back(new DataObject(randZZ()));
        }
        ZZ *last = new ZZ(randZZ());
        objectsPtr.push_back(new DataObject(*last));

        for (auto prot = GenSync::SyncProtocol::BEGIN;
                prot != GenSync::SyncProtocol::END;
                ++prot) {

            // Build server
            GenSync GenSyncServer = GenSync::Builder().
                    setProtocol(prot).
                    setMbar(CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT).
                    setBits(sizeof(randZZ()) * CHAR_BIT).
                    setComm(GenSync::SyncComm::socket).
                    build();
            // ... add data objects unique to the server
            for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + SERVER_MINUS_CLIENT; iter++) {
                GenSyncServer.addElem(*iter);
            }

            // Build client
            GenSync GenSyncClient = GenSync::Builder().
                    setProtocol(prot).
                    setMbar(CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT).
                    setBits(sizeof(randZZ()) * CHAR_BIT).
                    setComm(GenSync::SyncComm::socket).
                    build();
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
            GenSyncServer.addElem(last);

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
}

void GenSyncTest::testSyncWithString() {
    //todo
}

void GenSyncTest::testAddRemoveElemsNoFile() {
//    // TODO: change this to include delElem tests when delElem is implemented
//    const unsigned char ELTS = rand(); // amt of elems in the data structure
//
//    vector<DataObject *> objectsPtr;
//
//    for (unsigned long ii = 0; ii < ELTS; ii++) { // -1 so that one datum can be added with the templated addElem function
//        objectsPtr.push_back(new DataObject(randZZ()));
//    }
//
//    ZZ *last = new ZZ(randZZ());
//    auto combos = genSyncCombos(sizeof(randZZ()) * CHAR_BIT, ELTS);
//    cout << combos.size();
//    for (GenSync genSync : combos) {
//        // Test add and remove data from GenSync
//        multiset<string> objectsStr;
//        for (auto dop : objectsPtr) {
//            genSync.addElem(dop);
//            objectsStr.insert(dop->print());
//        }
//        genSync.addElem(last);
//        objectsStr.insert((new DataObject(*last))->print()); // get the expected print value for the non-dataobject datum
//
//        multiset<string> res;
//        for (auto dop : genSync.dumpElements()) {
//            res.insert(dop->print());
//        }
//        CPPUNIT_ASSERT(multisetDiff(res, objectsStr).empty()); // test that dumpElements, and implicitly the addElem functions, worked
//
//        // Tests for other adds
//    }
}

void GenSyncTest::testGetName() {
    // no actual spec for what getName should do - this is implementation dependent.
    GenSync genSync({}, {});
    CPPUNIT_ASSERT_EQUAL(string("I am a GenSync object"), genSync.getName());
}

void GenSyncTest::testBuilder() {
    // generate all possible configurations of builder
    size_t eltSize = sizeof(int) * CHAR_BIT;
    int diffs = 64;
    string iostr = randString();
    stringstream host;
    host << randByte() << "." << randByte() << "." << randByte() << "." << randByte();
    unsigned int port = randByte();
    double err = randDouble();
    auto numParts = 3;
    vector<GenSync*> configs = genSyncCombos(eltSize, diffs, iostr, host.str(), port, err, numParts);
    vector<GenSync*> firstConstructorConfigs = firstGenSyncConstructorCombos(eltSize, diffs, iostr, host.str(), port, err, numParts);

    CPPUNIT_ASSERT_EQUAL(configs.size(), firstConstructorConfigs.size());

    // test that each GenSync can successfully sync with an identical GenSync created by the first constructor
    for(int ii = 0; ii < configs.size(); ii++) {
        // basic tests
        CPPUNIT_ASSERT_EQUAL(configs.at(ii)->getSyncAgt(0).operator*()->getName(), firstConstructorConfigs.at(ii)->getSyncAgt(0).operator*()->getName());
        CPPUNIT_ASSERT_EQUAL(configs.at(ii)->numComm(), firstConstructorConfigs.at(ii)->numComm()); // todo: add a test that the communicants are the same and that there is only 1 syncagent
    }
}

void GenSyncTest::testSyncWithFirstConstructor() {
    // generate all possible configurations of builder
    size_t eltSize = sizeof(randZZ()) * CHAR_BIT;
    int diffs = 1000;
    string iostr = "\1\1";
    string host = "localhost";
    unsigned int port = 8001;
    double err = randDouble();
    auto numParts = 3;
    vector<GenSync*> firstConstructorConfigs = firstGenSyncConstructorCombos(eltSize, diffs, iostr, host, port, err, numParts);
    vector<GenSync*> firstConstructorConfigsOther = firstGenSyncConstructorCombos(eltSize, diffs, iostr, host, port, err, numParts);

    // sync every GenSync with itself
    for(int ii=0; ii < firstConstructorConfigs.size(); ii++) {
        GenSync genSync = *firstConstructorConfigs.at(ii);
        GenSync genSyncOther = *firstConstructorConfigsOther.at(ii);
        syncTest(genSync, genSyncOther, TIMES); // second case breaks!!!!!!

    }

}