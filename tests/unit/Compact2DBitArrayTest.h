/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on April, 2020.
 */

#ifndef COMPACT2DBITARRAYTEST_H
#define COMPACT2DBITARRAYTEST_H

#include <CPISync/Syncs/Compact2DBitArray.h>
#include <cppunit/extensions/HelperMacros.h>

class Compact2DBitArrayTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(Compact2DBitArrayTest);
    CPPUNIT_TEST(readWriteTest);
    CPPUNIT_TEST_SUITE_END();
 public:
    Compact2DBitArrayTest();
    virtual ~Compact2DBitArrayTest();
    void setUp();
    void tearDown();

    // test for f bits [1..32] X columns count [1..64] X rows count [1..12]
    void readWriteTest();
};

#endif // COMPACT2DBITARRAYTEST_H
