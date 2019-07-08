//
// Created by Sean Brandenburg on 2019-07-02.
//

#include "BenchmarkTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BenchmarkTest);

BenchmarkTest::BenchmarkTest() = default;

BenchmarkTest::~BenchmarkTest() = default;

void BenchmarkTest::setUp() {
	const int SEED = 233;
	srand(SEED);
}

void BenchmarkTest::tearDown() {

}

void BenchmarkTest::CPISyncErrorBenchmark() {

	const int testRuns = 10; // Number of times to sync
	const int SIMILAR = 32; //Number of elements in common between the server and client
	const int DIFS = 8; // Number of elements unique to the server AND number of elements unique to client (Sym Difs = DIFS *2)
	const int failExpected = ceil(testRuns * pow(2,-err)); //Amount of failures should be less than P[error] * number of runs
	int failCount = 0; //Keeps track of how many synchronizations are reported as failures for comparison to theoretical value

	//Vector containing GenSyncs of types CPISync, ProbCPISync, InterCPISync
	vector<GenSync> CPISyncClient = twoWayCombos(2*DIFS);
	vector<GenSync> CPISyncServer = twoWayCombos(2*DIFS);


	//Itterate through each type of sync (CPISync, ProbCPISync, InterCPISync) exclude FullSync becuase it does not have a theoretical probability of failure
	for(int ii = 0; ii < CPISyncClient.size() - 1; ii++) {
		//Test that less than (failExpected) tests fail in (testRuns) tests for sets
		for (int jj = 0; jj < testRuns; jj++) {
			//Having some issues with having 2 different genSync's with multiset enabled at the same time.
			//TODO: Re-enable when multiset issue is fixed
			bool success = benchmarkSync(CPISyncClient.at(ii), CPISyncServer.at(ii), SIMILAR, DIFS, DIFS, false, false/* (ii % 2 == 1) */); //Multiset is set to true if ii is odd
			if (!success) {
				failCount++;
			}
		}
		//If more test failed than the calculated failExpected, then CPISync's error may not be properly bounded
		CPPUNIT_ASSERT(failCount < failExpected);
	}
}
void BenchmarkTest::TimedSyncThreshold(){
	const int MAX_TIME = 10; // Test completion threshold (If a test does not complete in this amount of seconds, stop the test and record the size of the previous sync)
	string syncStats;
	string syncStatsMax;
	double difs; //Number of elements unique to the server and unique to the client (also the number of elements that are shared between the two sets)

	//CPISyncs
	vector<GenSync> CPISyncClient;
	vector<GenSync> CPISyncServer;
	for(int ii = 0; ii < twoWayCombos(1).size(); ii++) {
		difs = 1;
		while(1) {
			//Double the ammount of differences until the sync can not complete within MAX_TIME
			difs *= 2;
			//Mbar*2 because there are difs differences in the server and another difs in the client
			CPISyncClient = twoWayCombos(difs*2);
			CPISyncServer = twoWayCombos(difs*2);

			CPPUNIT_ASSERT(benchmarkSync(CPISyncClient[ii],CPISyncServer[ii],difs,difs,difs,false,false));
			syncStatsMax = CPISyncServer[ii].printStats(0,0);
			if(CPISyncServer[ii].getSyncTime(0) > MAX_TIME) break;
			syncStats = CPISyncServer[ii].printStats(0,0);
		}

		//Report Stats
		cout << endl << "Maximum number of set differences (Multiples of 2) able to synchronize in under " << MAX_TIME << " second(s): " << floor(difs) << endl;
		cout << syncStats << endl;

		cout << "Stats for the first sync that took longer than " << MAX_TIME << " second(s)" << endl;
		cout << syncStatsMax;
	}

	//IBLT Sync tests
	difs = 1;
	while(1){
		difs *= 2;
		vector<GenSync> IBLTGenClient = twoWayProbCombos(difs*3);
		vector<GenSync> IBLTGenServer= twoWayProbCombos(difs*3);

		CPPUNIT_ASSERT(benchmarkSync(IBLTGenClient[0],IBLTGenServer[0],difs,difs,difs,true,false));

		syncStatsMax = IBLTGenServer[0].printStats(0,0);
		if(IBLTGenServer[0].getSyncTime(0) > MAX_TIME)break;
		syncStats = IBLTGenServer[0].printStats(0,0);
	}

	//Report Stats
	cout << endl << "Maximum number of set differences (Multiples of 2) able to synchronize in under " << MAX_TIME << " second(s): " << floor(difs) << endl;
	cout << syncStats << endl;
	cout << "Stats for the first sync that took longer than " << MAX_TIME << " second(s)" << endl;
	cout << syncStatsMax;
}