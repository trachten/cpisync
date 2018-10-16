//
// Created by Bowen Song on 9/27/18.
//
#include "kshinglingSyncPerfTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KshingleSyncPerf);
KshingleSyncPerf::KshingleSyncPerf() = default;
KshingleSyncPerf::~KshingleSyncPerf() = default;

const int shingleLen = 4;
const pair<int,int> strSizeRange = make_pair(500, 1000);
const int tesPts = 20;// Test Pts per graph
const int target_confidence = 5;// Confidence interval
const int confidenceCap = 40; // after edit distance exceed confidenceCap, confidence go to 1.
const pair<int,int> editDistRange = make_pair(1, 100); // range of edit distance
// Declear what set reconciliation we are testing
auto setReconProto = {GenSync::SyncProtocol::CPISync,GenSync::SyncProtocol::InteractiveCPISync};
// Declear what string reconciliation we are testing
auto strReconProto = {PerformanceData::StringReconProtocol::KshinglingSync};

void KshingleSyncPerf::testklgperf() {
    PerformanceData test;

    int strSizeinterval = floor((strSizeRange.second - strSizeRange.first) / tesPts);

    // Increment string size
    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {

        int editDistinterval = floor((editDistRange.second - editDistRange.first) / tesPts);
        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {

                // string length , shingle length, edit distance
                test.prepareStringRecon(strSize, shingleLen, editDist);

                for (auto strRecon: strReconProto) { // go through all Str Recon methods mentioned above
                    for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above

//                        for (auto conf = 0; conf < target_confidence; ++conf) {
                        test.prepareSetComm(strRecon, setRecon);
                        test.calCostReport(PerformanceData::PlotType::PLOT3D);
//                    }
                }
            }
        }
    }
    test.genReport("testklgperf3D");
}


void KshingleSyncPerf::testFixedKperf() {
    PerformanceData test;


    int strSizeinterval = 500;

    // Increment string size
    for (int strSize = strSizeRange.first; strSize <= strSizeRange.second; strSize += strSizeinterval) {

        int editDistinterval = floor((editDistRange.second - editDistRange.first) / tesPts);
        for (int editDist = editDistRange.first; editDist <= editDistRange.second; editDist += editDistinterval) {


            int confidence = target_confidence;

            if (editDist > confidenceCap) {
                confidence = 1;
            }
//            else{
//                int editPercent = (editDist/editDistinterval+1)/(confidenceCap/editDistinterval
//                        +confidenceCap%editDistinterval);
//
//                int strPercent = (strSize/strSizeinterval+1)/((strSizeRange.second-strSizeRange.first)/strSizeinterval
//                        +(strSizeRange.second-strSizeRange.first)%strSizeinterval);
//
//                cout<<to_string(100*strPercent*editPercent)
//                +"% finished"<<endl;
//            }

            for (auto conf = 0; conf < confidence; ++conf) {
                // string length , shingle length, edit distance
                test.prepareStringRecon(strSize, shingleLen, editDist);

                for (auto strRecon: strReconProto) { // go through all Str Recon methods mentioned above
                    for (auto setRecon: setReconProto) { // go through all set Recon methods mentioned above
                        test.prepareSetComm(strRecon, setRecon);
                        test.calCostReport(PerformanceData::PlotType::PLOT2D);
                    }
                }
            }
        }
    }
    test.genReport("testklgperf2D");
}