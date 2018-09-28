//
// Created by Bowen Song on 9/27/18.
//

#ifndef CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
#define CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kshinglingSync.h"

class KshingleSyncPerf : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(KshingleSyncPerf);
        CPPUNIT_TEST(testAll);
    CPPUNIT_TEST_SUITE_END();

public:
    KshingleSyncPerf();

    ~KshingleSyncPerf();

    void testAll();

    void generateCSV(string title);

    pair<bool, long> findCosts(vector<pair<string,K_Shingle>> inputPackage,
                               int string_len, int editDistance_bar,
                               GenSync::SyncProtocol base_set_proto,
                               GenSync::SyncComm base_comm);

private:
    // file parameters
    vector<long> F_editDistance;  // X
    vector<long> F_commCost;  // Y
    vector<string> F_legend;
    string F_title;

};
#endif //CPISYNCLIB_KSHINGLINGSYNCPERFTEST_H
