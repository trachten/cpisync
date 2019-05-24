/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/10/2018.
//

#ifndef CPISYNCLIB_IBLTSYNCTEST_H
#define CPISYNCLIB_IBLTSYNCTEST_H


#include <cppunit/extensions/HelperMacros.h>

class IBLTSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(IBLTSyncTest);

        CPPUNIT_TEST(justSyncTest);
        CPPUNIT_TEST(testAddDelElem);
        CPPUNIT_TEST(testGetStrings);
        CPPUNIT_TEST(failConditionTest);

    CPPUNIT_TEST_SUITE_END();
public:
    IBLTSyncTest();

    ~IBLTSyncTest() override;
    void setUp() override;
    void tearDown() override;

    // Test reconciliation
    void justSyncTest();

    // Test adding and deleting elements
    void testAddDelElem();

    // Test that printElem() and getName() return some nonempty string
    void testGetStrings();

	// Test that the fail conditions for IBLTSync are functioning correctly
	void failConditionTest();
};

#endif //CPISYNCLIB_IBLTSYNCTEST_H
