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

  CPPUNIT_TEST(testGetStrings);

  CPPUNIT_TEST(testAddDelElem);
  CPPUNIT_TEST(IBLTSetOfSetsSetReconcileTest);

  CPPUNIT_TEST_SUITE_END();

public:
  IBLTSetOfSetsTest();

  ~IBLTSetOfSetsTest() override;
  void setUp() override;
  void tearDown() override;

  /**
 	* Test that printElem() and getName() return some nonempty string
 	*/
  void testGetStrings();

  /**
	 * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
	 * IBLT Sync is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
	 * even for a large amount of tests
	 */
  void IBLTSetOfSetsSetReconcileTest();



  /**
	 * Test adding and deleting elements
	 */
  void testAddDelElem();



};

#endif //CPISYNCLIB_IBLTSYNCTEST_H
