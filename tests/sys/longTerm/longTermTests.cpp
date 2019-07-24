//
// Created by Zifan Wang on 2019-07-23.
//

#include "longTermTests.h"

CPPUNIT_TEST_SUITE_REGISTRATION(LongTermTest);

LongTermTest::LongTermTest() = default;
LongTermTest::~LongTermTest() = default;

void LongTermTest::setUp()
{
    const int SEED = 221;
    srand(SEED);
}

void LongTermTest::tearDown() {}

void LongTermTest::CPISyncLongTerm()
{

    const int testRounds = 10;   // # of test rounds
    const int dif = 4;           // Initial difference size for both A/B and B/A
    const int similiar = 32;     // size of A^B
    const int difPerRound = 3;   // # of elems to be added to client during each round
    const bool multiSet = false; // true iff it's testing on multiset
    bool success = true;         // true iff test succeeds in the end
    bool details = false;        // true iff to show inner progress during test

    // CPISyncs
    vector<GenSync> CPISyncClient = twoWayCombos(dif * 2);
    vector<GenSync> CPISyncServer = twoWayCombos(dif * 2);

    // cout << "[longTermTest] Starting with A-B = B-A = " << dif << endl;

    for (int ii = 0; ii < /*CPISyncClient.size()*/ 1; ii++)
    {
        success &= longTermSync(CPISyncClient[ii], CPISyncServer[ii], similiar, dif, dif, false, false, false, false, difPerRound, details, testRounds);
    }

    CPPUNIT_ASSERT(success);
}

void LongTermTest::IBLTSyncLongTerm()
{
    const int testRounds = 10;   // # of test rounds
    const int dif = 4;           // Initial difference size for both A/B and B/A
    const int similiar = 32;     // size of A^B
    const int difPerRound = 3;   // # of elems to be added to client during each round
    const bool multiSet = false; // true iff it's testing on multiset
    bool success = true;         // true iff test succeeds in the end
    bool details = true;         // true iff to show inner progress during test

    // IBLTSyncs
    // Only works when mbar values are way bigger than difference
    // Problems here
    vector<GenSync> IBLTSyncClient = twoWayProbCombos(dif * 20);
    vector<GenSync> IBLTSyncServer = twoWayProbCombos(dif * 20);

    for (int ii = 0; ii < IBLTSyncClient.size(); ii++)
    {
        success &= longTermSync(IBLTSyncClient[ii], IBLTSyncServer[ii], similiar, dif, dif, true, false, false, false, difPerRound, details, testRounds);
    }

    CPPUNIT_ASSERT(success);
}
