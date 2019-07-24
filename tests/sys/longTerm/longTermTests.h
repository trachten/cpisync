//
// Created by Zifan Wang on 2019-07-17.
//

#ifndef CPISYNCLIB_LONGTERMTEST_H
#define CPISYNCLIB_LONGTERMTEST_H

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

class LongTermTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(LongTermTest);
    CPPUNIT_TEST(CPISyncLongTerm);
    CPPUNIT_TEST(IBLTSyncLongTerm);
    CPPUNIT_TEST_SUITE_END();

  public:
    LongTermTest();
    ~LongTermTest() override;
    void setUp() override;
    void tearDown() override;

    /**
	 * CPI type syncs have an error upper limit specified on creation. This test makes sure that the observed error is lower
	 * than that specified upper bound for each CPI type sync (Regular, Prob and Inter CPI)
 	 */
    static void CPISyncLongTerm();
    static void IBLTSyncLongTerm();
};

#endif