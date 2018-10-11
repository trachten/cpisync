//
// Created by Bowen Song on 9/27/18.
//
#include "kshinglingSyncPerfTest.h"
#include "PerformanceData.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KshingleSyncPerf);
KshingleSyncPerf::KshingleSyncPerf() = default;
KshingleSyncPerf::~KshingleSyncPerf() = default;


void KshingleSyncPerf::testklgperf() {
    PerformanceData test;

    // Test Pts per graph
    int tesPts = 2;
    // Confidence interval
    int confidence = 2;

    auto strSizeRange = make_pair(500, 1000);
    int strSizeinterval = floor((strSizeRange.second - strSizeRange.first) / tesPts);

    // Increment string size
    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {


        int shingleLen = ceil(log2(strSize));

        auto editDistRange = make_pair(1, floor(strSize / shingleLen));
        int editDistinterval = floor((editDistRange.second - editDistRange.first) / tesPts);
        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {

            // Declear what set reconciliation we are testing
            auto setReconProto = {GenSync::SyncProtocol::CPISync, GenSync::SyncProtocol::InteractiveCPISync};
            // Declear what string reconciliation we are testing
            auto strReconProto = {PerformanceData::StringReconProtocol::KshinglingSync};

            for (auto conf = 0; conf < confidence; ++conf) {
                // string length , shingle length, edit distance
                test.prepareStringRecon(strSize, shingleLen, editDist);

                for (auto strRecon: strReconProto) { // go through all Str Recon methods mentioned above
                    for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above
                        test.prepareSetComm(strRecon, setRecon);
                        test.calCostReport();
                    }
                }

            }
        }
    }


    test.genReport("testklgperf");
}


void KshingleSyncPerf::testFixedKperf() {
//    PerformanceData test;
//
//    // Test Pts per graph
//    int tesPts = 20;
//    // Confidence interval
//    int confidence = 3;
//
//    auto strSizeRange = make_pair(50, 100);
//    int strSizeinterval = floor((strSizeRange.second - strSizeRange.first) / tesPts);
//
//    // Increment string size
//    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {
//
//
//        int shingleLen = 4;
//
//        auto editDistRange = make_pair(0, floor(strSize / shingleLen));
//        int editDistinterval = floor((strSizeRange.second - strSizeRange.first) / tesPts);
//        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {
//            // Declear what set reconciliation we are testing
//            auto setReconProto = {GenSync::SyncProtocol::CPISync, GenSync::SyncProtocol::InteractiveCPISync};
//            // Declear what string reconciliation we are testing
//            auto strReconProto = {PerformanceData::StringReconProtocol::KshinglingSync};
//            for (auto conf = 0; conf < confidence; ++conf) {
//                // string length , shingle length, edit distance
//                test.prepareStringRecon(strSize, shingleLen, editDist);
//
//                for (auto strRecon: strReconProto) { // go through all Str Recon methods mentioned above
//                    for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above
//                        test.prepareSetComm(strRecon, setRecon);
//                        test.calCostReport();
//                    }
//                }
//            }
//
//        }
//    }
//    test.genReport("FixedKperf");
}