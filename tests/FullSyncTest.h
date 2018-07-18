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
    
    CPPUNIT_TEST(basicTest);
    CPPUNIT_TEST(thoroughTest);
            
    CPPUNIT_TEST_SUITE_END();
public:
    FullSyncTest();
    virtual ~FullSyncTest();
    void setUp();
    void tearDown();
    
    /**
     * Runs a simple test where server and client are initialized with fixed sets that differ in exactly one element.
     */
    void basicTest();
    void thoroughTest();
private:

};

#endif /* FULLSYNCTEST_H */

