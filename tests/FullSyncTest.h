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
    
    CPPUNIT_TEST(testAll);
            
    CPPUNIT_TEST_SUITE_END();
public:
    FullSyncTest();
    virtual ~FullSyncTest();
    void setUp();
    void tearDown();
    void testAll();
private:

};

#endif /* FULLSYNCTEST_H */

