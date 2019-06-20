/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.h
 * Author: Eliezer Pearl
 *
 * Created on June 27, 2018, 9:49 AM
 */

#ifndef FULLSYNCTEST_H
#define FULLSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>

class FullSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(FullSyncTest);
    
    CPPUNIT_TEST(FullSyncReconcileTest);
    CPPUNIT_TEST(FullMultiSyncReconcileTest);
    CPPUNIT_TEST(testAddDelElem);
    CPPUNIT_TEST(testGetStrings);
            
    CPPUNIT_TEST_SUITE_END();
public:
    FullSyncTest();

    ~FullSyncTest() override;
    void setUp() override;
    void tearDown() override;

	/**
 	* Test full reconciliation with FullSync protocol (All elements are exchanged and the sets are updated to be the Union of the two sets
 	*/
    static void FullSyncReconcileTest();

    static void FullMultiSyncReconcileTest();
	/**
 	* Test adding and deleting elements
 	*/
    static void testAddDelElem();

	/**
 	* Test that printElem() and getName() return some nonempty string
 	*/
    static void testGetStrings();

};

#endif /* FULLSYNCTEST_H */

