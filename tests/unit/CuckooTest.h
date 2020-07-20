/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 */

#ifndef CPISYNCLIB_CUCKOOTEST_H
#define CPISYNCLIB_CUCKOOTEST_H

#include <CPISync/Syncs/Cuckoo.h>
#include <cppunit/extensions/HelperMacros.h>

class CuckooTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(CuckooTest);
    CPPUNIT_TEST(testInsert);
    // CPPUNIT_TEST(testInsertHuge);
    CPPUNIT_TEST(testLookup);
//    CPPUNIT_TEST(testErase);
    CPPUNIT_TEST(testSmartConstructor);
    CPPUNIT_TEST(testConfigF3);
    CPPUNIT_TEST(testConfigF7);
    CPPUNIT_TEST(testConfigF13);
    CPPUNIT_TEST(testConfigF30);
    CPPUNIT_TEST(testConfigF3B1);
    CPPUNIT_TEST(testConfigF14B1);
    CPPUNIT_TEST(testConfigF7B15);
    CPPUNIT_TEST(testConfigF7B3);
    CPPUNIT_TEST_SUITE_END();

public:
    CuckooTest();
    ~CuckooTest() override;
    void setUp() override;
    void tearDown() override;

    /*
     * Causes kicks of level 1, 2, 3, and possibly failed
     * inserts. Aims to inserts 100% of capacity.
     */
    static void testInsert();

    /*
     * Makes and populates 2^25 buckets, each bucket has 4 12-bit
     * entries. Total size is ~200MB.
     */
    static void testInsertHuge();

    /*
     * Makes the same table as testInsertHuge but of size
     * 2^16. Inserts from [0..2^18] range to induce some fingerprint
     * overlaps. Checks the failed insert rate to be less than 3%,
     * then looks up for the inserted items and checks for the same 3%
     * fails bound. Takes long because of max Cuckoo relocation count
     * being 500.
     */
    static void testLookup();

    /*
     * Uses twice as big filter as testLookup. Inserts from the same
     * range as testLookup. Checks:
     * - All inserted elements have to be successfully erased,
     * - All elements that are NOT erased are positive look ups afterwards,
     * - All erased elements are negative look ups afterwards
     *   (See the implementation for the details).
     */
    static void testErase();

    /**
     * Tests the automatic constructor process in which the caller
     * provides only target false positive error rate and the
     * capacity.
     */
    static void testSmartConstructor();

    /**
     * Tests for different configurations of Cuckoo Filter.
     * TODO: Here we generate random numbers in each test run and thus
     * the test are NOT deterministic. Alternative of storing test samples
     * and loading them in test runs would not be perfect either.
     * E.g., cuckoo kicking chooses the victim item to relocate at random.
     */
    static void testConfigF3();
    static void testConfigF7();
    static void testConfigF8();
    static void testConfigF13();
    static void testConfigF30();
    static void testConfigF3B1();
    static void testConfigF14B1();
    static void testConfigF7B15();
    static void testConfigF7B3();
};

#endif // CPISYNCLIB_CUCKOOTEST_H
