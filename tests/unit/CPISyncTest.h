/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Sean Brandenburg on 2019-06-10.
//

#ifndef CPISYNCLIB_CPISYNCTEST_H
#define CPISYNCLIB_CPISYNCTEST_H

/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <cppunit/extensions/HelperMacros.h>

class CPISyncTest : public CPPUNIT_NS::TestFixture {

	CPPUNIT_TEST_SUITE(CPISyncTest);

	CPPUNIT_TEST(testCPIAddDelElem);
	CPPUNIT_TEST(CPISyncSetReconcileTest);
	CPPUNIT_TEST(CPISyncMultisetReconcileTest);
	CPPUNIT_TEST(CPISyncLargeSetReconcileTest);
	CPPUNIT_TEST(ProbCPISyncSetReconcileTest);
	CPPUNIT_TEST(ProbCPISyncMultisetReconcileTest);
	CPPUNIT_TEST(ProbCPISyncLargeSetReconcileTest);
	CPPUNIT_TEST(testInterCPIAddDelElem);
	CPPUNIT_TEST(InterCPISyncSetReconcileTest);
	CPPUNIT_TEST(InterCPISyncMultisetReconcileTest);
	CPPUNIT_TEST(InterCPISyncLargeSetReconcileTest);

	CPPUNIT_TEST_SUITE_END();

public:
	CPISyncTest();

	~CPISyncTest() override;
	void setUp() override;
	void tearDown() override;

	//CPISync Test Cases

	/**
	 * Test adding and deleting elements from CPISync
	 */
	static void testCPIAddDelElem();

	/**
 	* Test a synchronization of sets with CPISync
	 * CPISync does have a very small probability of failure but is not a probabilistic sync because it doesn't do partial reconcilliation
	 * CPISync can also fail if mBar is smaller than the number of differences but the differences have been capped by mbar for this test
	 * probSync false does a more complete check of the functionality of CPISync and passes the first 10,000 test cases with this seed
 	*/
	static void CPISyncSetReconcileTest();

	/**
 	* Test a synchronization of multisets with CPISync (Elements can be repeated)
	 * CPISync does have a very small probability of failure but is not a probabilistic sync because it doesn't do partial reconcilliation
	 * CPISync can also fail if mBar is smaller than the number of differences but the differences have been capped by mbar for this test
	 * probSync false does a more complete check of the functionality of CPISync and passes the first 10,000 test cases with this seed
 	*/
	static void CPISyncMultisetReconcileTest();

	/*
	 * Test a synchronization of large sets using CPISync. This test may be limited by the heap size of a users machine
	 * TODO: Add a number here once the sie of "Large" is chosen
	 */
	static void CPISyncLargeSetReconcileTest();

	/**
	 * Test the synchronization of sets using ProbCPISync
	 * Same as CPISync but if more than m_bar differences are present the CPISync divides into smaller subproblems
	 */
	static void ProbCPISyncSetReconcileTest();

	/**
	 * Test the synchronization of multisets using ProbCPISync
 	 * Same as CPISync but if more than m_bar differences are present the CPISync divides into smaller subproblems
	 */
	static void ProbCPISyncMultisetReconcileTest();

	/**
	 * Test the synchronization of large sets using ProbCPISync
	 * Same as CPISync but if more than m_bar differences are present the CPISync divides into smaller subproblems
	 */
	static void ProbCPISyncLargeSetReconcileTest();

	//InterCPISync Test cases

	/**
 	* Test adding and deleting elements from InterCPISync
 	*/
	static void testInterCPIAddDelElem();

	/**
 	 * Test a synchronization with InterCPISync
	 * InterCPISync is tested with prob = false for the same reason as CPISYnc but InterCPISync does not have mBar < m as a
	 * fail condition because it will recurse and break the set into p smaller sets recursively until the sync is successful
	 *
	 * Also tests that children are being deleted properly because the GenSync object needs to be cleared every run in order to
	 * sync properly when called again in syncTest. If delete does not succeed then the sync will fail when called multiple times
	 * because the real reconciled set will have additional elements that the expected reconcilled set does not have
 	*/
	static void InterCPISyncSetReconcileTest();

	/**
 	 * Test a synchronization with InterCPISync
 	*/
	static void InterCPISyncMultisetReconcileTest();

	/**
	 * Test a synchronization with InterCPISync
	 * InterCPISync is tested with prob = false for the same reason as CPISYnc but InterCPISync does not have mBar < m as a
	 * fail condition because it will recurse and break the set into p smaller sets recursively until the sync is successful
	 *
	 * Also tests that children are being deleted properly because the GenSync object needs to be cleared every run in order to
	 * sync properly when called again in syncTest. If delete does not succeed then the sync will fail when called multiple times
	 * because the real reconciled set will have additional elements that the expected reconcilled set does not have
	 */
	static void InterCPISyncLargeSetReconcileTest();


};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CPISyncTest, CPISyncTest);


#endif //CPISYNCLIB_CPISYNCTEST_H
