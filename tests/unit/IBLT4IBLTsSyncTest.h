/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Zifan Wang on 8/1/2019.
//

#ifndef CPISYNCLIB_IBLT4IBLTSSYNCTEST_H
#define CPISYNCLIB_IBLT4IBLTSSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>

class IBLT4IBLTsSyncTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(IBLT4IBLTsSyncTest);

    CPPUNIT_TEST(IBLTSyncSetReconcileTest);
    CPPUNIT_TEST(IBLTSyncMultisetReconcileTest);
    CPPUNIT_TEST(IBLTSyncLargeSetReconcileTest);
    CPPUNIT_TEST(testAddDelElem);
    CPPUNIT_TEST(testGetStrings);
    CPPUNIT_TEST(testIBLTParamMismatch);

    CPPUNIT_TEST_SUITE_END();

  public:
    IBLT4IBLTsSyncTest();

    ~IBLT4IBLTsSyncTest() override;
    void setUp() override;
    void tearDown() override;

    /**
	 * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
	 * IBLT Sync is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
	 * even for a large amount of tests
	 */
    void IBLTSyncSetReconcileTest();

    /**
	 * Tests reconciliation of multisets using IBLTSync
	 */
    void IBLTSyncMultisetReconcileTest();

    /**
	 * Test reconciliation of large sets using IBLTSync
	 */
    void IBLTSyncLargeSetReconcileTest();

    /**
	 * Test adding and deleting elements
	 */
    void testAddDelElem();

    /**
 	* Test that printElem() and getName() return some nonempty string
 	*/
    void testGetStrings();

    /**
 	* Test that IBLT Sync reports failure properly with incompatible sync parameters (Different number of expected elements)
 	*/
    void testIBLTParamMismatch();

    /**
 	* Test that IBLT Functions properly for very large inputs
 	*/
};

#endif //CPISYNCLIB_IBLTSYNCTEST_H
