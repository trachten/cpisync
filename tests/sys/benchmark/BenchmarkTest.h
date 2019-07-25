//
// Created by Sean Brandenburg on 2019-07-02.
//

#ifndef CPISYNCLIB_BENCHMARKTEST_H
#define CPISYNCLIB_BENCHMARKTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <CPISync/Syncs/IBLTSync.h>
#include <CPISync/Syncs/IBLTSync_HalfRound.h>
#include <CPISync/Syncs/ProbCPISync.h>
#include <CPISync/Syncs/CPISync_HalfRound.h>
#include <CPISync/Syncs/InterCPISync.h>
#include <CPISync/Communicants/CommString.h>
#include <CPISync/Communicants/CommSocket.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/FullSync.h>
#include <CPISync/Aux/ForkHandle.h>
#include "TestAuxiliary.h"

class BenchmarkTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(BenchmarkTest);

	CPPUNIT_TEST(CPISyncLongTerm);
	CPPUNIT_TEST(IBLTSyncLongTerm);
	CPPUNIT_TEST(CPISyncErrorBenchmark);
	CPPUNIT_TEST(IBLTSyncErrBenchMark);
	CPPUNIT_TEST(TimedSyncThreshold);
	CPPUNIT_TEST(BitThresholdTest);

	CPPUNIT_TEST_SUITE_END();

  public:
	BenchmarkTest();
	~BenchmarkTest() override;
	void setUp() override;
	void tearDown() override;

	/**
	 * CPI type syncs have an error upper limit specified on creation. This test makes sure that the observed error is lower
	 * than that specified upper bound for each CPI type sync (Regular, Prob and Inter CPI)
 	 */
	static void CPISyncErrorBenchmark();

	/**
	 * Runs each two-way sync until it can no longer complete in under MAX_TIME = 10 seconds. Start at 4 symmetric differences and doubles
	 * until the the test no longer passes. Print out stats for the largest sync that passed as well as the smallest sync that
	 * did not pass
	 */
	static void TimedSyncThreshold();

	/**
	 * Run each two-way sync, doubling the number of differences until the sync can no longer comoplete with under MAX_BYTES
	 * transmitted. Prints out stats for the largest sync that was able to complete with under MAX_BYTES sent and the smallest
	 * sync that was not able to complete
	 */
	static void BitThresholdTest();

	/**
	 * A sync that mimics a client making itterative changes and periodically syncing those changes to a server to simulate
	 * an actual use case
	 */
	// static void ServerSyncItterative();

	/*
	 * Long term tests that performs continous rounds syncs on client and server, with each time a const number of elements added to client
	 * and do another round of reconciliation. 
	 */
	static void CPISyncLongTerm();

	static void IBLTSyncLongTerm();

	static void IBLTSyncErrBenchMark();
};

#endif //CPISYNCLIB_BENCHMARKTEST_H
