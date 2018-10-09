//
// Created by Bowen on 10/9/18.
//

#include "PerformanceData.h"

PerformanceData::PerformanceData() = default;

PerformanceData::~PerformanceData() = default;

void PerformanceData::prepareStringRecon(int string_size, int shingle_len, int edit_distance){
    //init Parameters
    editDist = edit_distance;
    mbar = edit_distance;
    bits = 8;
    stringSize = string_size;
    shingleLen = shingle_len;

    plot.GenPlot("Comm_vs_Edit_Dist_Str_Len=" + to_string(string_len) + "_&_K=" + to_string(shingle_len));

//    shingleLen = ceil(log2(string_size));
//
//    plot.GenPlot("Comm_vs_Edit_Dist_Str_Len=" + to_string(string_len) + "_&_K=lg(str_len)");
}

void PerformanceData::prepareSetComm(StringReconProtocol string_recon_proto, GenSync::SyncProtocol base_set_proto, string Alice_txt, string Bob_txt){
    baseSetProto = base_set_proto;
    stringReconProto = string_recon_proto;



    // random input
    if (Alice_txt=="" and Bob_txt==""){
         Alice_txt = randAsciiStr(string_len);
         Bobtxt = randStringEdit(Alice_txt, editDistance_bar);
    }
    switch(stringReconProto) {
        case StringReconProtocol::KshinglingSync:
            K_Shingle Alice_content = K_Shingle(shingle_len);
            K_Shingle Bob_content = K_Shingle(shingle_len);

            auto Alice_set = Alice_content.getShingleSet_str(Alice_txt);
            auto Bob_set = Bob_content.getShingleSet_str(Bob_txt);
            int mbar = multisetDiff(Alice_set, Bob_set).size();
            break;
        default:
    }


    switch (baseSetProto) {
        case GenSync::SyncProtocol::CPISync: // not used
            setReconProtoName = "CPI";
            bits = 14 + (shingle_len + 2) * 8;//sqaure bits
            mbar = mbar + mbar + ceil(mbar * 0.3);
            break;

        case GenSync::SyncProtocol::InteractiveCPISync:
            setReconProtoName = "InterCPI"
            bits = 14 + (shingleLen + 2) * 8;//sqaure bits
            mbar = 5;  //need mbar -- fixed
            numParts = 3;  //setNumPartitions -- fixed

            break;
//                case GenSync::SyncProtocol::FullSync:
//                    G_legend.push_back("FullSync");//G_legend
//                    break;

//                case GenSync::SyncProtocol::IBLTSync:
//                    numExpElem = mbar;
//                    //bits
//                    //numExpElem
//                    break;

        default:

    }
}


// can start timing
void PerformanceData::calCostReport(bool check_outcome) {
    forkHandleReport res;
    bool success = false;
    // while not success, we increase the edit distance


    switch (stringReconProto) {
        case StringReconProtocol::KshinglingSync:
            stringReconProtoName = "KS";
            while (!success) { // adjust mbar if recon not success

                res = kshingling.SyncNreport(Alice, Bob);

                mbar += mbar;  // expected mbar may not be enough, therefore adding 1 or doubling
                if (check_outcome) {
                    success = (kshingling.getString(Alice, Alice_content) == kshingling.getString(Bob, Bob_content))
                } else {
                    success = check_outcome;
                }
            }
            break;
        default:
    }
    // time plot
    plot.plot2D("Time" + stringReconProtoName + "-" + setReconProtoName, editDist, res.totalTime);
    // cpu time plot
    plot.plot2D("CPU Time" + stringReconProtoName + "-" + setReconProtoName, editDist, res.CPUtime);
    // comm plot
    plot.plot2D("Comm Cost" + stringReconProtoName + "-" + setReconProtoName, editDist, res.bytes);

}
