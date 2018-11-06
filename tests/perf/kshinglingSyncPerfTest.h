//
// Created by Bowen Song on 9/27/18.
//

#ifndef CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
#define CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kshinglingSync.h"
#include "PerformanceData.h"

//TODO: Later write a abstract class for this
class KshingleSyncPerf : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(KshingleSyncPerf);
//        CPPUNIT_TEST(setDiffTest3D);
//        CPPUNIT_TEST(testperf3D);
//        CPPUNIT_TEST(setDiffTest2D);
//        CPPUNIT_TEST(testperf2D);
        CPPUNIT_TEST(testStrataEst3D);
    CPPUNIT_TEST_SUITE_END();

public:
    KshingleSyncPerf();

    ~KshingleSyncPerf();

    /**
     * explore the edit distance and resulting set difference
     */
//    void setDiffTest2D();
//
//    void setDiffTest3D();
//
//    void testperf2D();
//
//    void testperf3D();

    void testStrataEst3D();

private:
    // file parameters
    vector<vector<long>> F_editDistance;  // X
    vector<vector<long>> F_commCost;  // Y
    vector<string> F_legend;
    string F_title;
    long String_Size;


};

#endif //CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
