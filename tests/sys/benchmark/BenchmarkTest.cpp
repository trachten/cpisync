//
// Created by Sean Brandenburg on 2019-07-02.
//

#include "BenchmarkTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BenchmarkTest);

BenchmarkTest::BenchmarkTest() = default;

BenchmarkTest::~BenchmarkTest() = default;

void BenchmarkTest::setUp()
{
	const int SEED = 233;
	srand(SEED);
}

void BenchmarkTest::tearDown()
{
}

void BenchmarkTest::CPISyncErrorBenchmark()
{

	const int testRuns = 10;								// Number of times to sync
	const int SIMILAR = 32;									//Number of elements in common between the server and client
	const int DIFS = 8;										// Number of elements unique to the server AND number of elements unique to client (Sym Difs = DIFS *2)
	const int failExpected = ceil(testRuns * pow(2, -err)); //Amount of failures should be less than P[error] * number of runs
	int failCount = 0;										//Keeps track of how many synchronizations are reported as failures for comparison to theoretical value

	//Vector containing GenSyncs of types CPISync, ProbCPISync, InterCPISync
	vector<GenSync> CPISyncClient = twoWayCombos(2 * DIFS);
	vector<GenSync> CPISyncServer = twoWayCombos(2 * DIFS);

	//Itterate through each type of sync (CPISync, ProbCPISync, InterCPISync) exclude FullSync because it does not have a theoretical probability of failure
	for (int ii = 0; ii < CPISyncClient.size() - 1; ii++)
	{
		//Test that less than (failExpected) tests fail in (testRuns) tests for sets
		for (int jj = 0; jj < testRuns; jj++)
		{
			bool success = benchmarkSync(CPISyncClient.at(ii), CPISyncServer.at(ii), SIMILAR, DIFS, DIFS, false, false);
			if (!success)
			{
				failCount++;
			}
		}
		//If more test failed than the calculated failExpected, then CPISync's error may not be properly bounded
		CPPUNIT_ASSERT(failCount < failExpected);
	}
}
void BenchmarkTest::TimedSyncThreshold()
{
	const int MAX_TIME = 10; // Test completion threshold (If a test does not complete in this amount of seconds, stop the test and record the size of the previous sync)
	string syncStats;
	string syncStatsMax;
	int difs; //Number of elements unique to the server and unique to the client (also the number of elements that are shared between the two sets)

	//CPISyncs
	vector<GenSync> CPISyncClient;
	vector<GenSync> CPISyncServer;
	for (int ii = 0; ii < twoWayCombos(1).size(); ii++)
	{
		difs = 1;
		while (1)
		{
			//Double the ammount of differences until the sync can not complete within MAX_TIME
			difs *= 2;
			//Mbar*2 because there are difs differences in the server and another difs in the client
			CPISyncClient = twoWayCombos(difs * 2);
			CPISyncServer = twoWayCombos(difs * 2);

			CPPUNIT_ASSERT(benchmarkSync(CPISyncClient[ii], CPISyncServer[ii], difs, difs, difs, false, false));
			syncStatsMax = CPISyncServer[ii].printStats(0);
			if (CPISyncServer[ii].getSyncTime(0) > MAX_TIME)
				break;
			syncStats = CPISyncServer[ii].printStats(0);
		}

		//Report Stats
		cout << endl
			 << "Maximum number of set differences (Multiples of 2) able to synchronize in under " << MAX_TIME << " second(s): " << difs << " difs" << endl;
		cout << syncStats << endl;
		cout << endl
			 << "Maximum number of set differences (Multiples of 2) NOT able to synchronize in under " << MAX_TIME << " second(s): " << difs * 2 << " difs" << endl;
		cout << syncStatsMax;
	}

	//IBLT Sync tests
	difs = 1;
	while (1)
	{
		difs *= 2;
		vector<GenSync> IBLTGenClient = twoWayProbCombos(difs * 3);
		vector<GenSync> IBLTGenServer = twoWayProbCombos(difs * 3);

		CPPUNIT_ASSERT(benchmarkSync(IBLTGenClient[0], IBLTGenServer[0], difs, difs, difs, true, false));

		syncStatsMax = IBLTGenServer[0].printStats(0);
		if (IBLTGenServer[0].getSyncTime(0) > MAX_TIME)
			break;
		syncStats = IBLTGenServer[0].printStats(0);
	}

	//Report Stats
	cout << endl
		 << "Maximum number of set differences (Multiples of 2) able to synchronize in under " << MAX_TIME << " second(s): " << difs << " difs" << endl;
	cout << syncStats << endl;
	cout << endl
		 << "Maximum number of set differences (Multiples of 2) NOT able to synchronize in under " << MAX_TIME << " second(s): " << difs * 2 << " difs" << endl;
	cout << syncStatsMax;
}

void BenchmarkTest::BitThresholdTest()
{

	const int MAX_BYTES = 10000; // Runs two-way syncs with twice as many expected elements each time until more than MAX_BYTES need to be sent to reconcille
	string syncStats;
	string syncStatsMax;
	int difs; //Number of elements unique to the server and unique to the client (also the number of elements that are shared between the two sets)

	//CPISyncs
	vector<GenSync> CPISyncClient;
	vector<GenSync> CPISyncServer;
	for (int ii = 0; ii < twoWayCombos(1).size(); ii++)
	{
		difs = 1;
		while (1)
		{
			//Double the ammount of differences until the sync can not complete without sending more than MAX_BYTEs
			difs *= 2;
			//Mbar*2 because there are difs differences in the server and another difs in the client
			CPISyncClient = twoWayCombos(difs * 2);
			CPISyncServer = twoWayCombos(difs * 2);

			CPPUNIT_ASSERT(benchmarkSync(CPISyncClient[ii], CPISyncServer[ii], difs, difs, difs, false, false));
			syncStatsMax = CPISyncServer[ii].printStats(0);
			if (CPISyncServer[0].getXmitBytes(0) > MAX_BYTES)
				break;
			syncStats = CPISyncServer[ii].printStats(0);
		}

		//Report Stats
		cout << endl
			 << "Maximum number of set differences (Multiples of 2) able to synchronize while sending less than " << MAX_BYTES << " bytes: " << difs << " difs" << endl;
		cout << syncStats << endl;
		cout << "Stats for the first sync that took more than " << MAX_BYTES << " bytes to complete: " << difs * 2 << " difs" << endl;
		cout << syncStatsMax;
	}

	//IBLT Sync tests
	difs = 1;
	while (1)
	{
		difs *= 2;
		vector<GenSync> IBLTGenClient = twoWayProbCombos(difs * 3);
		vector<GenSync> IBLTGenServer = twoWayProbCombos(difs * 3);

		CPPUNIT_ASSERT(benchmarkSync(IBLTGenClient[0], IBLTGenServer[0], difs, difs, difs, true, false));

		syncStatsMax = IBLTGenServer[0].printStats(0);
		if (IBLTGenServer[0].getXmitBytes(0) > MAX_BYTES)
			break;
		syncStats = IBLTGenServer[0].printStats(0);
	}

	//Report Stats
	cout << endl
		 << "Maximum number of set differences (Multiples of 2) able to synchronize while sending less than " << MAX_BYTES << " bytes: " << difs << " difs" << endl;
	cout << syncStats << endl;
	cout << "Stats for the first sync that took more than " << MAX_BYTES << " bytes to complete: " << difs * 2 << " difs" << endl;
	cout << syncStatsMax;
}

void BenchmarkTest::CPISyncLongTerm()
{

	const int testRounds = 10;   // # of test rounds
	const int dif = 4;			 // Initial difference size for both A/B and B/A
	const int similiar = 32;	 // size of A^B
	const int difPerRound = 3;   // # of elems to be added to client during each round
	const bool multiSet = false; // true iff it's testing on multiset
	bool success = true;		 // true iff test succeeds in the end
	bool details = false;		 // true iff to show inner progress during test

	// CPISyncs
	vector<GenSync> CPISyncClient = twoWayCombos(dif * 2);
	vector<GenSync> CPISyncServer = twoWayCombos(dif * 2);

	for (int ii = 0; ii < CPISyncClient.size(); ii++)
	{
		success &= longTermSync(CPISyncClient[ii], CPISyncServer[ii], similiar, dif, dif, false, false, false, false, difPerRound, details, testRounds);
	}

	CPPUNIT_ASSERT(success);
}

void BenchmarkTest::IBLTSyncLongTerm()
{
	const int testRounds = 10;   // # of test rounds
	const int dif = 4;			 // Initial difference size for both A/B and B/A
	const int similiar = 32;	 // size of A^B
	const int difPerRound = 3;   // # of elems to be added to client during each round
	const bool multiSet = false; // true iff it's testing on multiset
	bool success = true;		 // true iff test succeeds in the end
	bool details = false;		 // true iff to show inner progress during test

	// # ExpElems should be the total expected # of set after reconciliation, but not m_bar which are used for twoWayCombos funciton
	vector<GenSync> IBLTSyncClient = twoWayProbCombos(difPerRound * testRounds + dif + similiar);
	vector<GenSync> IBLTSyncServer = twoWayProbCombos(difPerRound * testRounds + dif + similiar);

	for (int ii = 0; ii < IBLTSyncClient.size(); ii++)
	{
		success &= longTermSync(IBLTSyncClient[ii], IBLTSyncServer[ii], similiar, dif, dif, true, false, false, false, difPerRound, details, testRounds);
	}

	CPPUNIT_ASSERT(success);
}

void BenchmarkTest::IBLTSyncErrBenchMark()
{
	const int testRuns = 40;															   // Number of times to sync
	const int SIMILAR = 32;																   // Number of elements in common between the server and client
	const int DIFS = 8;																	   // Number of elements unique to the server AND number of elements unique to client (Sym Difs = DIFS *2)
	const int failExpected = testRuns * (1 - pow(1 - exp(-N_HASH * 3 / 2), N_HASH)); // Amount of failures should be less than P[error] * number of runss,
																						   // see https://arxiv.org/pdf/1101.2245.pdf for how this error being calculated.
	int failCount = 0;																	   // Keeps track of how many synchronizations are reported as failures for comparison to theoretical value

	// Vector containing by now only IBLTSync
	vector<GenSync> IBLTSyncServer = twoWayProbCombos(2 * DIFS + SIMILAR);
	vector<GenSync> IBLTSyncClient = twoWayProbCombos(2 * DIFS + SIMILAR);

	for (int ii = 0; ii < IBLTSyncClient.size() - 1; ii++)
	{
		// Test that less than (failExpected) tests fail in (testRuns) tests for sets
		for (int jj = 0; jj < testRuns; jj++)
		{
			bool success = benchmarkSync(IBLTSyncClient.at(ii), IBLTSyncServer.at(ii), SIMILAR, DIFS, DIFS, true, false);
			if (!success)
			{
				failCount++;
			}
		}
		//If more test failed than the calculated failExpected, then IBLTSync's error may not be properly bounded
		CPPUNIT_ASSERT(failCount < failExpected);
	}
}
