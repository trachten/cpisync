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
CPPUNIT_TEST(testInterCPIAddDelElem);
CPPUNIT_TEST_SUITE_END();

public:
	CPISyncTest();

	~CPISyncTest() override;
	void setUp() override;
	void tearDown() override;

	/**
	 * Test adding and deleting elements
	 */
	void testCPIAddDelElem();

	void testInterCPIAddDelElem();

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CPISyncTest, CPISyncTest);


#endif //CPISYNCLIB_CPISYNCTEST_H
