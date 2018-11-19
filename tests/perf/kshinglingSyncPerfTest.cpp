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
const int strSize = 5;

const pair<int,int> strSizeRange = make_pair(1000, 50000);
const pair<int,int> shingleLenRange = make_pair(2,ceil(log2(strSizeRange.second)));

const int tesPts = 20;// Test Pts per graph
const int target_confidence = 50;// Confidence interval
const int confidenceCap = 40; // after edit distance exceed confidenceCap, confidence go to 1.

const pair<int,int> editDistRange = make_pair(1, 1000); // range of edit distance
// Declear what set reconciliation we are testing
//auto setReconProto = {GenSync::SyncProtocol::CPISync,GenSync::SyncProtocol::InteractiveCPISync
//                      ,GenSync::SyncProtocol::IBLTSyncSetDiff};
auto setReconProto = {GenSync::SyncProtocol::IBLTSyncSetDiff};
// Declear what string reconciliation we are testing
auto strRecon = PerformanceData::StringReconProtocol::KshinglingSync;


//void KshingleSyncPerf::setDiffTest3D(){
//    PerformanceData test = PerformanceData(tesPts);
//    test.StrKDif3D(strSizeRange,shingleLenRange,editDist);
//    test.StrEDDif3D(strSizeRange,editDistRange,shingleLen);
//    test.EDKDif3D(editDistRange,shingleLenRange,strSize);
//    test.genReport("setDiff3D");
//}
void KshingleSyncPerf::kshingleTest3D(){
    PerformanceData test = PerformanceData(tesPts);
    test.kshingle3D(setReconProto,editDistRange,strSizeRange,0);
    test.genReport("kshingleTest3D");

//    PerformanceData test3 = PerformanceData(tesPts);
//    test3.kshingleBook3D(editDistRange,strSizeRange);
//    test3.genReport("kshingleBookTest3D");
//
//    PerformanceData test2 = PerformanceData(tesPts);
//    test2.kshingleCode3D(editDistRange,strSizeRange);
//    test2.genReport("kshingleTestCode3D");


}
//
//void KshingleSyncPerf::testperf3D() {
//    PerformanceData test = PerformanceData(tesPts);
//
//    test.kshingle3D(setReconProto, editDistRange, strSizeRange, shingleLen);
//    test.genReport("testperf3D");
//}
//
//
//void KshingleSyncPerf::testperf2D() {
//    PerformanceData test = PerformanceData(tesPts);
//
//    test.kshingle2D(setReconProto, editDistRange, shingleLen, strSizeRange.first, target_confidence);
//    test.kshingle2D(setReconProto, editDistRange, shingleLen, strSizeRange.second, target_confidence);
//    test.genReport("testperf2D");
//}


//void KshingleSyncPerf::testStrataEst3D() {
//    PerformanceData test = PerformanceData(tesPts);
//    test.strataEst3D(make_pair(1000,10000), target_confidence);
//    test.genReport("StrataReport2D");
//}
