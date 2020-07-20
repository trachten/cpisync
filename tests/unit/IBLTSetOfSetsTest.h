/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Zifan Wang on 8/1/2019.
//

#ifndef CPISYNCLIB_IBLTSetOfSetsTEST_H
#define CPISYNCLIB_IBLTSetOfSetsTEST_H

#include <cppunit/extensions/HelperMacros.h>

class IBLTSetOfSetsTest : public CPPUNIT_NS::TestFixture
{

	CPPUNIT_TEST_SUITE(IBLTSetOfSetsTest);

	CPPUNIT_TEST(TestIBLTGetName);
//	CPPUNIT_TEST(IBLTSetOfSetsSetReconcileTest);
//	CPPUNIT_TEST(IBLTSetOfSetsLargeSync);
//	CPPUNIT_TEST(IBLTSetOfSetsSimilarSetSync);
//	CPPUNIT_TEST(testAddDelElem);

	CPPUNIT_TEST_SUITE_END();

public:
  IBLTSetOfSetsTest();

  ~IBLTSetOfSetsTest() override;
  void setUp() override;
  void tearDown() override;

  /**
 	* Test that getName() returns some nonempty string
 	*/
  void TestIBLTGetName();

  /**
	 * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
	 * IBLT Sync is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
	 * even for a large amount of tests
	 */
  void IBLTSetOfSetsSetReconcileTest();

  /**
   * Test out of order element addition to make sure that serialization works regardless of the order that elements
   * are added in
   */
   void IBLTSetOfSetsLargeSync();

	/**
	  * In the event that two or more of the sets being synced are very similar or have the same elements before recon, the
	  * order that client server set combos are chosen in may be significant. This happens because each client set is checked against
	  * every other server set and the set pair with the minimum differences is chosen as the sync that will be executed. In this case
	  * two similar sets may have the same set with minimum differences but by keeping track of which sets are already being synced
	  * only one set should be able to choose this set, and the other set should choose the second best option. This test checks
	  * this functionality.
	  */
	void IBLTSetOfSetsSimilarSetSync();


	/**
	 * Test adding and deleting elements
	 */
	void testAddDelElem();



};

#endif //CPISYNCLIB_IBLTSYNCTEST_H
