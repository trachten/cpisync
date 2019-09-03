//
// Created by Sean Brandenburg on 2019-08-28.
//

#include "DiffEstimatorTest.h"
#include <CPISync/Syncs/GenSync.h>
#include "TestAuxiliary.h"


CPPUNIT_TEST_SUITE_REGISTRATION(DiffEstimatorTest);

const int DIF_EST_ERROR = 10;
const int SMALL_SIMILAR = 10;
const int SMALL_DIFS = 5;
const int SIMILAR = 100;
const int DIFS = 50;
const int LARGE_SIMILAR = 10000;
const int LARGE_DIFS = 5000;

DiffEstimatorTest::DiffEstimatorTest() = default;
DiffEstimatorTest::~DiffEstimatorTest() = default;

void DiffEstimatorTest::setUp() {
	const int SEED = 823;
	srand(SEED);
}

void DiffEstimatorTest::tearDown() {
}

void DiffEstimatorTest::MinWiseSketchesErrorTestSmallDifs() {

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR). //This is actually the number of hashes to use in MinWiseSketches right now. Will be changed. With 100 hashes error should be 1/sqrt(100) = 10%
			build();


	multiset<string> myMultiset;
	addElements(false, SMALL_SIMILAR, SMALL_DIFS, SMALL_DIFS, GenSyncServer,GenSyncClient,myMultiset);
	int estDif = DifEstForkTest(GenSyncClient, GenSyncServer, 0, 0, 0);
	CPPUNIT_ASSERT(true);

	//TODO: A check that estDif is within the expected error margins should go here
}

void DiffEstimatorTest::MinWiseSketchesErrorTest() {
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR). //This is actually the number of hashes to use in MinWiseSketches right now. Will be changed. With 100 hashes error should be 1/sqrt(100) = 10%
			build();

	multiset<string> myMultiset;
	addElements(false, SIMILAR, DIFS, DIFS, GenSyncServer,GenSyncClient,myMultiset);
	int estDif = DifEstForkTest(GenSyncClient, GenSyncServer, 0, 0, 0);
	CPPUNIT_ASSERT(true);

	//TODO: A check that estDif is within the expected error margins should go here
}

void DiffEstimatorTest::MinWiseSketchesErrorTestLargeDifs() {
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::MinWiseSketches).
			setDifEstErr(DIF_EST_ERROR). //This is actually the number of hashes to use in MinWiseSketches right now. Will be changed. With 100 hashes error should be 1/sqrt(100) = 10%
			build();


	multiset<string> myMultiset;
	addElements(false, LARGE_SIMILAR, LARGE_DIFS, LARGE_DIFS, GenSyncServer,GenSyncClient,myMultiset);
	int estDif = DifEstForkTest(GenSyncClient, GenSyncServer, 0, 0, 0);
	CPPUNIT_ASSERT(true);

	//TODO: A check that estDif is within the expected error margins should go here
}

void DiffEstimatorTest::WrappedBloomFilterErrorTest(){
	const int rounds = 100; 
	const int outerRounds = 1;
	const int DifEstError = 2; // difEstError = -log2(errorProb)
	double sucProb = 1 - (double)1/pow(2,DifEstError); // convert difesterror to probability of error 

	for(int jj =0; jj < outerRounds; jj ++){
		int similar = 100 * pow(5, jj);
		int difs = 50 * pow(5, jj); // for each round similar and difs increase by 5 times

		long match = 0;

		GenSync GenSyncServer = GenSync::Builder().
				setProtocol(GenSync::SyncProtocol::CPISync).
				setComm(GenSync::SyncComm::socket).
				setBits(eltSize * 8). // Bytes to bits
				setErr(err).
				setDiffProtocol(GenSync::DiffProtocol::WrappedBloomFilter).
				setDifEstErr(DifEstError).
				build();

		GenSync GenSyncClient = GenSync::Builder().
				setProtocol(GenSync::SyncProtocol::CPISync).
				setComm(GenSync::SyncComm::socket).
				setBits(eltSize * 8). // Bytes to bits
				setErr(err).
				setDiffProtocol(GenSync::DiffProtocol::WrappedBloomFilter).
				setDifEstErr(DifEstError). // DifEstError = - log2(errorProb)
				build();

		for(int ii =0; ii <rounds; ii++){

			
			multiset<string> myMultiset;
			addElements(false, similar, difs, difs, GenSyncServer,GenSyncClient,myMultiset);
			int estDif = DifEstForkTest(GenSyncClient, GenSyncServer, 0, 0, 0);

			// check if the result is the upper bound
			if(estDif >= difs)
				match ++;

			// free allocated memory
			GenSyncClient.clearData();
			GenSyncServer.clearData();
			myMultiset.clear();
		}
		// check success times no less than expected times
		CPPUNIT_ASSERT(rounds * sucProb <= match);
	}
}

void DiffEstimatorTest::StrataEstimatorErrorTest(){
	const int DifEstError = 10;
	const int similar = 1000;
	const int difs = 500;
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::StrataEst).
			setDifEstErr(DifEstError).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::CPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSize * 8). // Bytes to bits
			setErr(err).
			setDiffProtocol(GenSync::DiffProtocol::StrataEst).
			setDifEstErr(DifEstError). // TODO: DifEstError has not been used anywhere yet
			build();


	multiset<string> myMultiset;
	addElements(false, similar, difs, difs, GenSyncServer,GenSyncClient,myMultiset);
	int estDif = DifEstForkTest(GenSyncClient, GenSyncServer, 0, 0, 0);
	CPPUNIT_ASSERT(true);
	// A check that estDif is within the expected error margins should go here
	// error function needed
}

