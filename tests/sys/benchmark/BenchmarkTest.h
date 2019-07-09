//
// Created by Sean Brandenburg on 2019-07-02.
//

#ifndef CPISYNCLIB_BENCHMARKTEST_H
#define CPISYNCLIB_BENCHMARKTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <Syncs/IBLTSync.h>
#include <Syncs/IBLTSync_HalfRound.h>
#include "Syncs/ProbCPISync.h"
#include "Syncs/CPISync_HalfRound.h"
#include "Syncs/InterCPISync.h"
#include "Communicants/CommString.h"
#include "Communicants/CommSocket.h"
#include "Aux/Auxiliary.h"
#include "Syncs/GenSync.h"
#include "Syncs/FullSync.h"
#include "Aux/ForkHandle.h"
#include "TestAuxiliary.h"

class BenchmarkTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(BenchmarkTest);
	CPPUNIT_TEST(CPISyncErrorBenchmark);
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
	void TimedSyncThreshold();

	/**
	 * Run each two-way sync, doubling the number of differences until the sync can no longer comoplete with under MAX_BYTES
	 * transmitted. Prints out stats for the largest sync that was able to complete with under MAX_BYTES sent and the smallest
	 * sync that was not able to complete
	 */
	void BitThresholdTest();
};

#endif //CPISYNCLIB_BENCHMARKTEST_H
