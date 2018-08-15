/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.h
 * Author: kaets
 *
 * Created on June 27, 2018, 9:49 AM
 */

#ifndef FULLSYNCTEST_H
#define FULLSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>

class FullSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(FullSyncTest);
    
    CPPUNIT_TEST(justSyncTest);
    CPPUNIT_TEST(testAddDelElem);
    CPPUNIT_TEST(testGetStrings);
            
    CPPUNIT_TEST_SUITE_END();
public:
    FullSyncTest();

    ~FullSyncTest() override;
    void setUp() override;
    void tearDown() override;

    // Test reconciliation
    void justSyncTest();

    // Test adding and deleting elements
    void testAddDelElem();

    // Test that printElem() and getName() return some nonempty string
    void testGetStrings();
private:

};

#endif /* FULLSYNCTEST_H */

