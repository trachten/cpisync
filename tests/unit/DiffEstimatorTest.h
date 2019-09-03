//
// Created by Sean Brandenburg on 2019-08-28.
//

#ifndef CPISYNC_DIFFESTIMATORTEST_H
#define CPISYNC_DIFFESTIMATORTEST_H

#include <cppunit/extensions/HelperMacros.h>

class DiffEstimatorTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(DiffEstimatorTest);
	CPPUNIT_TEST(StrataEstimatorErrorTest);
	CPPUNIT_TEST(WrappedBloomFilterErrorTest);
	CPPUNIT_TEST(MinWiseSketchesErrorTestSmallDifs);
	CPPUNIT_TEST(MinWiseSketchesErrorTest);
	CPPUNIT_TEST(MinWiseSketchesErrorTestLargeDifs);

	CPPUNIT_TEST_SUITE_END();

public:
	DiffEstimatorTest();
	~DiffEstimatorTest() override;

	void setUp() override;
	void tearDown() override;

	/**
	 * Check that the error is bounded properly for small set difs
	 */
	void MinWiseSketchesErrorTestSmallDifs();

	/**
 	* Check that the error for MinWiseSketches are being properly bounded by the error function. Error in Jaccard index is 1/sqrt(hashes)
	* so we need to do a propagation of error calculation to determine the error for (1-JaccardEst)/(1+JaccardEst) * (Constant);
 	*/
	void MinWiseSketchesErrorTest();

	/**
	 * Check that the error is bounded properly for large set difs
	 */
	void MinWiseSketchesErrorTestLargeDifs();
    
    /**
     * Check that the error is bounded properly
     **/
	void StrataEstimatorErrorTest();
    
    /**
     * Check that the error is upper bound of the actual symm differences
     * @require may have can't fork error when testing on many rounds
     **/
	void WrappedBloomFilterErrorTest();



};

#endif //CPISYNC_DIFFESTIMATORTEST_H
