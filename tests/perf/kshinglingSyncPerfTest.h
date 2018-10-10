//
// Created by Bowen Song on 9/27/18.
//

#ifndef CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
#define CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kshinglingSync.h"

class KshingleSyncPerf : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(KshingleSyncPerf);
        CPPUNIT_TEST(testklgperf);
        CPPUNIT_TEST(testFixedKperf);
    CPPUNIT_TEST_SUITE_END();

public:
    KshingleSyncPerf();

    ~KshingleSyncPerf();

    // k = lg(string len)
    void testklgperf();

    // k = fixed constant
    void testFixedKperf();


private:
    // file parameters
    vector<vector<long>> F_editDistance;  // X
    vector<vector<long>> F_commCost;  // Y
    vector<string> F_legend;
    string F_title;
    long String_Size;

};
#endif //CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
