//
// Created by eliez on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTEST_H
#define CPISYNCLIB_GENSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
class GenSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(GenSyncTest);
    //CPPUNIT_TEST(testSyncWithCPI);
    //CPPUNIT_TEST(testSyncWithSockets);
    //CPPUNIT_TEST(testSyncWithString);
    //CPPUNIT_TEST(testAddRemoveElemsNoFile);
    //CPPUNIT_TEST(testGetName);
    //CPPUNIT_TEST(testBuilder);
    CPPUNIT_TEST(testSyncWithFirstConstructor);
    CPPUNIT_TEST_SUITE_END();
public:
    GenSyncTest();
    virtual ~GenSyncTest();
    void setUp();
    void tearDown();
    void testSyncWithCPI();
    void testSyncWithSockets();
    void testSyncWithString();
    void testAddRemoveElemsNoFile();
    void testGetName();
    void testBuilder();
    void testSyncWithFirstConstructor();

    const int TIMES = 1; // times to run randomized test
};


#endif //CPISYNCLIB_GENSYNCTEST_H
