//
// Created by Bowen Song on 9/27/18.
//
#include "kshinglingSyncPerfTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KshingleSyncPerf);
KshingleSyncPerf::KshingleSyncPerf() = default;
KshingleSyncPerf::~KshingleSyncPerf() = default;

// DEFAULT SETTINGS
const int shingleLen = 4;
const int editDist = 20;
const int strSize = 1000;

const pair<int,int> strSizeRange = make_pair(500, 1000);
const pair<int,int> shingleLenRange = make_pair(2,ceil(log2(strSizeRange.second)));

const int tesPts = 20;// Test Pts per graph
const int target_confidence = 100;// Confidence interval
const int confidenceCap = 40; // after edit distance exceed confidenceCap, confidence go to 1.

const pair<int,int> editDistRange = make_pair(1, 100); // range of edit distance
// Declear what set reconciliation we are testing
auto setReconProto = {GenSync::SyncProtocol::CPISync,GenSync::SyncProtocol::InteractiveCPISync
                      ,GenSync::SyncProtocol::IBLTSyncSetDiff};
// Declear what string reconciliation we are testing
auto strRecon = PerformanceData::StringReconProtocol::KshinglingSync;


void KshingleSyncPerf::setDiffTest3D(){
    PerformanceData test = PerformanceData(tesPts);
    test.StrKDif3D(strSizeRange,shingleLenRange,editDist);
    test.StrEDDif3D(strSizeRange,editDistRange,shingleLen);
    test.EDKDif3D(editDistRange,shingleLenRange,strSize);
    test.genReport("setDiff3D");
}
void KshingleSyncPerf::setDiffTest2D(){
    PerformanceData test = PerformanceData(tesPts);
    test.KDif2D(shingleLenRange,strSize,editDist,target_confidence);
    test.EDDif2D(editDistRange,strSize,editDist,target_confidence);
    test.StrDif2D(strSizeRange,shingleLen,editDist,target_confidence);
    test.genReport("setDiff2D");
}

void KshingleSyncPerf::testperf3D() {
//    PerformanceData test = PerformanceData(tesPts);
//
//    int strSizeinterval = floor((strSizeRange.second - strSizeRange.first) / tesPts);
//
//    // Increment string size
//    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {
//
//        int editDistinterval = floor((editDistRange.second - editDistRange.first) / tesPts);
//        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {
//
//            // string length , shingle length, edit distance
//            test.prepareStringRecon(strSize, shingleLen, editDist);
//
//            for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above
//
////                        for (auto conf = 0; conf < target_confidence; ++conf) {
//                test.prepareSetComm(strRecon, setRecon);
//                test.calCostReport(PerformanceData::PlotType::PLOT3D);
////                    }
//
//            }
//        }
//    }
//    test.genReport("testklgperf3D");
}


void KshingleSyncPerf::testperf2D() {
    PerformanceData test = PerformanceData(tesPts);

//
//    int strSizeinterval = 500;
//
//    // Increment string size
//    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {
//
//        int editDistinterval = floor((editDistRange.second - editDistRange.first) / tesPts);
//        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {
//
//
//            int confidence = target_confidence;
//
//            if (editDist > confidenceCap) {
//                confidence = 1;
//            }
//
//            for (auto conf = 0; conf < confidence; ++conf) {
//                // string length , shingle length, edit distance
//                test.prepareStringRecon(strSize, shingleLen, editDist);
//
//                for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above
//                    test.prepareSetComm(strRecon, setRecon);
//                    test.calCostReport(PerformanceData::PlotType::PLOT2D);
//
//                }
//            }
//        }
//    }
    test.kshingle2D(setReconProto, editDistRange, shingleLen, strSizeRange.first, target_confidence);
    test.kshingle2D(setReconProto, editDistRange, shingleLen, strSizeRange.second, target_confidence);

    test.genReport("testperf2D");
}