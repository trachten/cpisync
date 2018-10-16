//
// Created by Bowen on 10/9/18.
//

#include "PerformanceData.h"

PerformanceData::PerformanceData() = default;

PerformanceData::~PerformanceData() = default;

// Parameters setting
void PerformanceData::prepareStringRecon(int string_size, int shingle_len, int edit_distance){
    //init Parameters
    editDist = edit_distance;
    mbar = edit_distance;
    bits = 8;
    stringSize = string_size;
    shingleLen = shingle_len;


//    shingleLen = ceil(log2(string_size));
//
//    plot.GenPlot("Comm_vs_Edit_Dist_Str_Len=" + to_string(string_len) + "_&_K=lg(str_len)");
}

// Prepare oarameters for specific set recon and genrate strings
void PerformanceData::prepareSetComm(StringReconProtocol string_recon_proto, GenSync::SyncProtocol base_set_proto, string Alice_txt, string Bob_txt){
    baseSetProto = base_set_proto;
    stringReconProto = string_recon_proto;



    // random input
    if (Alice_txt=="" and Bob_txt==""){
        AliceTxt = randAsciiStr(stringSize);
        BobTxt = randStringEdit(AliceTxt, editDist);
    }else{
        AliceTxt = Alice_txt;
        BobTxt = Bob_txt;
    }


    // quote a mbar, else to try and double method
    switch(stringReconProto) {
        case StringReconProtocol::KshinglingSync: {
            K_Shingle Alice_content = K_Shingle(shingleLen);
            K_Shingle Bob_content = K_Shingle(shingleLen);

            auto Alice_set = Alice_content.getShingleSet_str(AliceTxt);
            auto Bob_set = Bob_content.getShingleSet_str(BobTxt);
            mbar = multisetDiff(Alice_set, Bob_set).size();
            break;
        }
        default:
            throw invalid_argument("Not Implemented string recon Protocol");
    }


    switch (baseSetProto) {
        case GenSync::SyncProtocol::CPISync: // not used
            setReconProtoName = "CPI";
            bits = 14 + (shingleLen + 2) * 8;//sqaure bits
            mbar = mbar + mbar + ceil(mbar * 0.3);
            break;

        case GenSync::SyncProtocol::InteractiveCPISync:
            setReconProtoName = "InterCPI";
            bits = 14 + (shingleLen + 2) * 8;//sqaure bits
            mbar = 7;  //need mbar -- fixed
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
            throw invalid_argument("Not Implemented set recon Protocol");
    }
}


// can start timing
void PerformanceData::calCostReport(PlotType plot_type,bool check_outcome) {
    forkHandleReport res;
    bool success = false;
    // while not success, we increase the edit distance


    double time = clock(); // init clock
    double time_final=0;


    switch (stringReconProto) {
        case StringReconProtocol::KshinglingSync:
            stringReconProtoName = "Kshingle";
            while (!success) { // adjust mbar if recon not success

                // Prepare to Sync Alice
                // TODO: Enable IBLT and fill up the last parameter
                kshinglingSync kshingling = kshinglingSync(baseSetProto, GenSync::SyncComm::socket, bits, mbar,
                                                           numParts, 0);

                K_Shingle Alice_content = K_Shingle(shingleLen);
                GenSync Alice = kshingling.SyncHost(AliceTxt, Alice_content);
                time_final =  (double)(clock() - time)/CLOCKS_PER_SEC ;
                // Prepare Bob
                K_Shingle Bob_content = K_Shingle(shingleLen);
                GenSync Bob = kshingling.SyncHost(BobTxt, Bob_content);

                res = kshingling.SyncNreport(Alice, Bob);

                mbar += mbar;  // expected mbar may not be enough, therefore adding 1 or doubling
                time = clock(); // adding the time to piece info back together
                string tmpstr = kshingling.getString(Alice, Alice_content);
                time_final += (double) (clock() - time) / CLOCKS_PER_SEC;

                if (check_outcome) {
                    success = (tmpstr == BobTxt);
                } else {
                    success = check_outcome;
                }
            }
            break;
        default:
            break;
    }
    long comm_cost = res.bytesTot;
    //for Interactive CPI we care bout the total comm cost
    if (baseSetProto==GenSync::SyncProtocol::InteractiveCPISync) comm_cost +=res.bytesRTot;

    if (plot_type == PlotType::PLOT2D || plot_type == PlotType::BOTH) {
        // cpu time plot
        plot2D("CPUTime:" + stringReconProtoName + "-" + setReconProtoName + "-str=" + to_string(stringSize), editDist,
               res.CPUtime+time_final);
        // comm plot
        plot2D("CommCost:" + stringReconProtoName + "-" + setReconProtoName + "-str=" + to_string(stringSize),
               editDist, comm_cost);

        if (baseSetProto!=GenSync::SyncProtocol::InteractiveCPISync) {
            plot2D("CommCost(ED=setDiff):" + stringReconProtoName + "-" + setReconProtoName + "-str=" +
                   to_string(stringSize),
                   mbar, comm_cost);

            plot2D("EditDistNSetDiff:str=" + to_string(stringSize), editDist, mbar); // mbar is actual set difference
        }

        plot2D("CommCost:FullSync-str=" + to_string(stringSize), editDist, 14 + (stringSize) * 8);
    }

    if (plot_type == PlotType::PLOT3D || plot_type == PlotType::BOTH) {
        //
        plot3D("CommCost:" + stringReconProtoName + "-" + setReconProtoName, editDist, stringSize, comm_cost);


        plot3D("CPUTime:" + stringReconProtoName + "-" + setReconProtoName, editDist, stringSize,
               res.CPUtime+time_final);
        if (baseSetProto!=GenSync::SyncProtocol::InteractiveCPISync){
            plot3D("CommCost(ED=setDiff):" + stringReconProtoName + "-" + setReconProtoName, mbar, stringSize, comm_cost);

            plot3D("EditDistNSetDiff:", editDist, stringSize, mbar); // mbar is actual set difference
        }

        plot3D("CommCost:FullSync", editDist, stringSize, 14 + (stringSize) * 8);
    }
}

void PerformanceData::plot2D(string label, double X, double Y){
    if (data2D.find(label)==data2D.end()) { // if no label of such kind is in there
        vector<double> tmp(2);
        tmp[0] = X;
        tmp[1] = Y;
        vector<vector<double>> init;
        init.push_back(tmp);
        data2D.insert(make_pair(label,init));
    } else{
        vector<double> tmp(2);
        tmp[0] = X;
        tmp[1] = Y;
        data2D[label].push_back(tmp);
    }

}

void PerformanceData::plot3D(string label, double X, double Y, double Z){
    if (data3D.find(label)==data3D.end()) { // if no label of such kind is in there
        vector<double> tmp(3);
        tmp[0] = X;
        tmp[1] = Y;
        tmp[2] = Z;
        vector<vector<double>> init;
        init.push_back(tmp);
        data3D.insert(make_pair(label,init));
    } else{
        vector<double> tmp(3);
        tmp[0] = X;
        tmp[1] = Y;
        tmp[2] = Z;
        data3D[label].push_back(tmp);
    }
}

void PerformanceData::write2file(string file_name) {
    ofstream myfile;
    //TODO: do soemthing about the directories, this hard coding is not a long term solution
    myfile.open(file_name + ".txt");
    for (auto item : data3D) {
        myfile << "Label:" + item.first + "\n";
        string tmpx, tmpy, tmpz;
        for (auto num : item.second) {
            tmpx += to_string(num[0]) + ",";
            tmpy += to_string(num[1]) + ",";
            tmpz += to_string(num[2]) + ",";
        }
        myfile << "X:" + tmpx + "\n";
        myfile << "Y:" + tmpy + "\n";
        myfile << "Z:" + tmpz + "\n";
    }

    for (auto item : data2D) {
        myfile << "Label:" + item.first + "\n";
        string tmpx, tmpy;
        for (auto num : item.second) {
            tmpx += to_string(num[0]) + ",";
            tmpy += to_string(num[1]) + ",";
        }
        myfile << "X:" + tmpx + "\n";
        myfile << "Y:" + tmpy + "\n";
    }

    myfile.close();
}

