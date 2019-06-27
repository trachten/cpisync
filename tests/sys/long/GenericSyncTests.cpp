/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/23/2018.
//

#include <Communicants/CommSocket.h>
#include <Syncs/CPISync.h>
#include <Syncs/ProbCPISync.h>
#include <Syncs/InterCPISync.h>
#include <Communicants/CommString.h>
#include <cstdlib>
#include "GenericSyncTests.h"
#include "Syncs/GenSync.h"
#include "Communicants/CommDummy.h"
#include "Aux/Auxiliary.h"
#include "TestAuxiliary.h"

using namespace std::chrono;
// constants

CPPUNIT_TEST_SUITE_REGISTRATION(GenSyncTest);

GenSyncTest::GenSyncTest() = default;
GenSyncTest::~GenSyncTest() = default;

void GenSyncTest::setUp(){
    // constant seed so that randomness is the same across runs
    const int SEED = 619;
    srand(SEED);
}
void GenSyncTest::tearDown(){}

// tests

void GenSyncTest::testAddRemoveElems() {

    vector<DataObject *> objectsPtr;

    // create one object that is convertible to a DataObject
    ZZ *last = new ZZ(randZZ());
	DataObject* newDO;
    // create elts-1 random DataObjects (the last element will be `last`)
    for (unsigned long ii = 0; ii < ELTS - 1; ii++) {
        objectsPtr.push_back(new DataObject(randZZ()));
    }

    // create all configurations of GenSyncs (created using both constructors)
    auto combos = standardCombos();
    auto file = fileCombos();
    combos.insert(combos.end(), file.begin(), file.end());

    for (auto genSync:combos) {
        multiset<string> objectsStr;

        for (auto dop : objectsPtr) {
            genSync.addElem(dop);
            // store a multiset of the expected dataset's string representation
            objectsStr.insert(dop->print());
        }

        // add the ZZ, thus testing the generic addElem function
        newDO = genSync.addElem(last);
        objectsStr.insert(newDO->print());

        // create a multiset containing the string representation of objects stored in GenSync
        multiset<string> res;
        for (auto dop : genSync.dumpElements()) {
            res.insert(dop);
        }

        CPPUNIT_ASSERT(multisetDiff(res, objectsStr).empty());

        for(auto elem : objectsPtr) {
            genSync.delElem(elem);
        }
        //Remove the data that was added with the template
        genSync.delElem(newDO);
        CPPUNIT_ASSERT(genSync.dumpElements().empty());
    }

    //Mem clean-up
    delete newDO;
    delete last;
    for(auto & ptr : objectsPtr){
        delete ptr;
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
    auto toAdd = make_shared<CPISync>(mBar, eltSizeSq, err);
    genSync.addSyncAgt(toAdd);
    genSyncOther.addSyncAgt(make_shared<CPISync>(mBar, eltSizeSq, err));
    CPPUNIT_ASSERT_EQUAL((CPISync*)(*genSync.getSyncAgt(0)).get(), toAdd.get());

    // syncing with the newly added commsocket and probcpisync should succeed
	//(oneWay = false, probSync = false)
	CPPUNIT_ASSERT(syncTest(genSync, genSyncOther, false, false));

    // test deleting a communicant by index and by pointer
    genSync.delComm(cs);
    CPPUNIT_ASSERT_EQUAL(0, genSync.numComm());
    genSyncOther.delComm(0);
    CPPUNIT_ASSERT_EQUAL(0, genSyncOther.numComm());

    // test deleting a syncmethod

    // create an InterCPISync to add and push to the beginning of the SyncMethod vector in `genSync`
    auto secondSync = make_shared<InterCPISync>(mBar, eltSizeSq, err, numParts);
    genSync.addSyncAgt(secondSync, 0);

    // removes the first SyncMethod. if delSyncAgt is successful, the SyncMethod at idx #0 should be `firstSync`
    genSync.delSyncAgt(0);
    auto newFirst = dynamic_cast<CPISync*>((*genSync.getSyncAgt(0)).get());
    CPPUNIT_ASSERT(newFirst != nullptr);
    CPPUNIT_ASSERT_EQUAL(newFirst->getName(), toAdd->getName());
}

void GenSyncTest::testGetName() {
    GenSync genSync({}, {});

    // check that string returned by getName actually contains some form of information about the GenSync object
    CPPUNIT_ASSERT(!genSync.getName().empty());
}

void GenSyncTest::testCounters() {
    // create the first CommSocket here so that we can check GenSync counters against Communicant counters for accuracy
    auto cs = make_shared<CommSocket>(port);

    // initialize GenSync objects
    GenSync genSync({cs}, {make_shared<CPISync>(mBar, eltSizeSq, err)});
    GenSync genSyncOther({make_shared<CommSocket>(port)}, {make_shared<CPISync>(mBar, eltSizeSq, err)});

    // get an upper bound of the time since the last sync to test against `res`
    auto before = std::chrono::high_resolution_clock::now();
	//(oneWay = false, probSync = false)
	CPPUNIT_ASSERT(syncTest(genSyncOther, genSync, false, false));
    auto after = std::chrono::high_resolution_clock::now();
    double res = genSync.getSyncTime(0);

    // check that Communicant counters == the respective GenSync counters
    CPPUNIT_ASSERT_EQUAL(cs->getXmitBytes(), genSync.getXmitBytes(0));
    CPPUNIT_ASSERT_EQUAL(cs->getRecvBytes(), genSync.getRecvBytes(0));

    // `res` should be positive and less than the time spent in syncTest
    CPPUNIT_ASSERT(res <= duration_cast<microseconds>(after - before).count() && res >= 0);
}

void GenSyncTest::testPort() {
    GenSync genSync({make_shared<CommSocket>(port)}, {make_shared<CPISync>(eltSizeSq, mBar, err)});
    CPPUNIT_ASSERT_EQUAL(port, (const unsigned int) genSync.getPort(0));

    GenSync genSyncOther({make_shared<CommString>(iostr)}, {make_shared<CPISync>(eltSizeSq, mBar, err)});
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
	for (int ii = 0; ii < twoWayClient.size(); ii++) {
		//(oneWay = false, probSync = false)
		CPPUNIT_ASSERT(syncTest(twoWayClient.at(ii), twoWayServer.at(ii), false, false));
	}
}

void GenSyncTest::testOneWaySync() {
    vector<GenSync> oneWayClient = oneWayCombos();
    vector<GenSync> oneWayServer = oneWayCombos();

    // sync every GenSync configuration with itself
    for(int ii = 0; ii < oneWayClient.size(); ii++) {
    	//(oneWay = true, probSync = false)
		CPPUNIT_ASSERT(syncTest(oneWayClient.at(ii), oneWayServer.at(ii), true, false));
	}
}

void GenSyncTest::testTwoWayProbSync() {
	vector<GenSync> twoWayProbClient = twoWayProbCombos();
	vector<GenSync> twoWayProbServer = twoWayProbCombos();

	// sync every GenSync configuration with itself
	for (int ii = 0; ii < twoWayProbClient.size(); ii++) {
		//(oneWay = false, probSync = true)
		CPPUNIT_ASSERT(syncTest(twoWayProbClient.at(ii), twoWayProbServer.at(ii), false, true));
	}
}

void GenSyncTest::testOneWayProbSync() {
    vector<GenSync> oneWayProbClient = oneWayProbCombos();
    vector<GenSync> oneWayProbServer = oneWayProbCombos();

    // sync every GenSync configuration with itself
    for(int ii = 0; ii < oneWayProbClient.size(); ii++) {
		//(oneWay = true, probSync = true)
		CPPUNIT_ASSERT(syncTest(oneWayProbClient.at(ii), oneWayProbServer.at(ii), true, true));
    }
}
