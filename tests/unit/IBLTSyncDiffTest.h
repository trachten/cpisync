//
// Created by Bowen Song on 10/14/18.
//

#ifndef CPISYNCLIB_IBLTSYNCDIFFTEST_H
#define CPISYNCLIB_IBLTSYNCDIFFTEST_H

#include <cppunit/extensions/HelperMacros.h>

class IBLTSyncDiffTest : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(IBLTSyncDiffTest);

        CPPUNIT_TEST(justSyncTest);
        CPPUNIT_TEST(testAddDelElem);
        CPPUNIT_TEST(stringReconFullTest);

    CPPUNIT_TEST_SUITE_END();
public:
    IBLTSyncDiffTest();

    ~IBLTSyncDiffTest() override;
    void setUp() override;
    void tearDown() override;

    // Test reconciliation
    void justSyncTest();

    // Test adding and deleting elements
    void testAddDelElem();

    // FULL Test designed for string recon k-shinling
    void stringReconFullTest();
};

#endif //CPISYNCLIB_IBLTSYNCDIFFTEST_H
