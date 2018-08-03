/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   CommunicantTest.h
 * Author: kaets
 *
 * Created on May 24, 2018, 10:08:52 AM
 */

#ifndef COMMUNICANTTEST_H
#define COMMUNICANTTEST_H

#include <cppunit/extensions/HelperMacros.h>

class CommunicantTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(CommunicantTest);
    
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST(testBytesAndResetCommCounters);
    CPPUNIT_TEST(testEstablishModAndCommZZ_p);
    CPPUNIT_TEST(testCommUstringBytes);
    CPPUNIT_TEST(testCommString);
    CPPUNIT_TEST(testCommLong);
    CPPUNIT_TEST(testCommUstringNoBytes);
    CPPUNIT_TEST(testCommDataObject);
    CPPUNIT_TEST(testCommDataObjectPriority);
    CPPUNIT_TEST(testCommDataObjectList);
    CPPUNIT_TEST(testCommDouble);
    CPPUNIT_TEST(testCommByte);
    CPPUNIT_TEST(testCommInt);
    CPPUNIT_TEST(testCommVec_ZZ_p);
    CPPUNIT_TEST(testCommZZ);
    CPPUNIT_TEST(testCommZZNoArgs);
    
    CPPUNIT_TEST_SUITE_END();

public:
    CommunicantTest();
    virtual ~CommunicantTest();
    void setUp();
    void tearDown();

private:
    // constants
    const int TIMES = 100; // Amount of times to run each test
    const int LOWER_BOUND = 0; // Lower bound on the length of random strings
    const int UPPER_BOUND = 50; // Upper bound on the length of random strings
    const double DELTA = 0.00001; // error tolerance comparing doubles

    // Tests argument-free construction.
    void testConstruct();
    
    // Tests bytes-communicated-counter getters and resetCommCounters.
    void testBytesAndResetCommCounters();
    
    // Tests establishMod functions, commSend for ZZ_p, and commRecv for ZZ_p.
    void testEstablishModAndCommZZ_p();
    
    // Tests commSend and Recv for ustring, passing the size of the ustring in bytes as an argument.
    void testCommUstringBytes();
    
    // Tests commSend and Recv for string
    void testCommString();
    
    // Tests commSend and Recv for long int
    void testCommLong();
    
    // Tests commSend and Recv for ustring without passing size as an argument
    void testCommUstringNoBytes();
    
    // Tests commSend and Recv for normal DataObject
    void testCommDataObject();
    
    // Tests commSend and Recv for DataObject with priority
    void testCommDataObjectPriority();
    
    // Tests commSend and Recv for a list of DataObject
    void testCommDataObjectList();
    
    // Tests commSend and Recv for double
    void testCommDouble();
    
    // Tests commSend and Recv for byte
    void testCommByte();
    
    // Tests commSend and Recv for int
    void testCommInt();
    
    // Tests commSend and Recv for vec_ZZ_p. Requires establishModSend and Recv
    void testCommVec_ZZ_p();
    
    // Tests commSend and Recv for ZZ, passing the size of the ZZ as an argument
    void testCommZZ();
    
    // Tests commSend and Recv for ZZ without passing any arguments
    void testCommZZNoArgs();

    

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CommunicantTest, CommunicantTest );


#endif /* COMMUNICANTTEST_H */

