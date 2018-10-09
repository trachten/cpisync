//
// Created by Bowen Song on 9/27/18.
//
#include "kshinglingSyncPerfTest.h"
//#include "GenPlot.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KshingleSyncPerf);
KshingleSyncPerf::KshingleSyncPerf() = default;
KshingleSyncPerf::~KshingleSyncPerf() = default;
void KshingleSyncPerf::testAll() {

    // K_shingle packages
    int string_len = 500;

    int shingle_len = ceil(log2(string_len));
    pair<int, int> editDistance_bar_range = make_pair(2, string_len/(shingle_len+2));

    //graph sample parameters
    int Datappts = 20;
    int interval = floor((editDistance_bar_range.second-editDistance_bar_range.first)/Datappts);

    GenSync::SyncComm base_comm = GenSync::SyncComm::socket;

    //file name
    //GenPlot plot = GenPlot("Comm_vs_Edit_Dist_Str_Len=" + to_string(string_len) + "_&_K=" + to_string(shingle_len));
    F_title = "Comm_vs_Edit_Dist_Str_Len=" + to_string(string_len) + "_&_K=" + to_string(shingle_len);
    //labels - order has to be the same as GenSync::SyncProtocol
    F_legend.push_back("CPISync");//G_legend
    F_legend.push_back("InteractiveCPISync mbar=5,numParts=3");//G_legend
    //F_legend.push_back("IBLTSync");//G_legend

    for (auto base_set_proto = GenSync::SyncProtocol::BEGIN;
         base_set_proto != GenSync::SyncProtocol::END; ++base_set_proto) {

        // graph parameters

        vector<long> G_editDistance;  // X
        vector<long> G_commCost;  // Y

        for (int editDistance_bar = editDistance_bar_range.first;
             editDistance_bar < editDistance_bar_range.second;
             editDistance_bar += interval) {
            for (int prep = 0 ; prep < 1000; ++prep){
            // Create K-shingleing packages
            string Alicetxt = randAsciiStr(string_len);
            K_Shingle Alice_content = K_Shingle(shingle_len);
            string Bobtxt = randStringEdit(Alicetxt, editDistance_bar);
            K_Shingle Bob_content = K_Shingle(shingle_len);

            // package them
            vector<pair<string, K_Shingle>> inputPackage;
            inputPackage.push_back(make_pair(Bobtxt, Bob_content));
            inputPackage.push_back(make_pair(Alicetxt, Alice_content));

            //Find best parameters
            auto Alice_set = Alice_content.getShingleSet_str(Alicetxt);
            auto Bob_set = Bob_content.getShingleSet_str(Bobtxt);
            int mbar = multisetDiff(Alice_set, Bob_set).size(); //see the actual num of diff
            size_t bits = 8;
            int numParts = 3;
            int numExpElem = mbar;

            // Adjust parameters based on
            switch (base_set_proto) {
                case GenSync::SyncProtocol::CPISync: // not used
                    bits = 14 + (shingle_len + 2) * 8;//sqaure bits
                    mbar = mbar + mbar + ceil(mbar * 0.3);
                    break;

                case GenSync::SyncProtocol::InteractiveCPISync:
                    bits = 14 + (shingle_len + 2) * 8;//sqaure bits
                    mbar = 5;
                    numParts = 3;//need mbar
                    //setNumPartitions
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
                    continue;
            }

            bool success = false;
            // while not success, we increase the edit distance
            pair<bool, long> res;
            while (!success) {

                res = calculateCosts(inputPackage,
                                     base_set_proto, base_comm,
                                     bits, mbar, numParts, numExpElem);
                mbar += mbar;
                success = res.first;
            }
            // successful sync data record
            G_commCost.push_back(res.second);
            G_editDistance.push_back(editDistance_bar);
        }
        }
        if (G_commCost.size()!=0 && G_editDistance.size()!=0) {
            F_commCost.push_back(G_commCost);
            F_editDistance.push_back(G_editDistance);
        }
    }
    generateCSV();
    //plot.write2file();
}

pair<bool,long> KshingleSyncPerf::calculateCosts(vector<pair<string,K_Shingle>> inputPackage,
                                  GenSync::SyncProtocol base_set_proto,GenSync::SyncComm base_comm,
                                    size_t bits , int mbar, int numParts, int numExpElem) {

    auto Alicetxt = inputPackage[0].first;
    auto Alice_content = inputPackage[0].second;
    auto Bobtxt = inputPackage[1].first;
    auto Bob_content = inputPackage[1].second;


    //number of difference between should alwasy be editDistance_bar*(shingle_len-1)
    kshinglingSync kshingling = kshinglingSync(base_set_proto, base_comm, bits, mbar, numParts, numExpElem);

    GenSync Alice = kshingling.SyncHost(Alicetxt, Alice_content);


    GenSync Bob = kshingling.SyncHost(Bobtxt, Bob_content);

    pair<bool, long> result;
//    int err = 0;
//    int chld_state;
//    int my_opt = 0;
//    pid_t pID = fork();
//    if (pID == 0) {
//        signal(SIGCHLD, SIG_IGN);
//        forkHandleReport report = forkHandle(Bob, Alice);
//        result = make_pair(kshingling.getString(Alice, Alice_content)
//                           == kshingling.getString(Bob, Bob_content), report.bytes);
//        err++;
//        exit(0);
//
//    } else if (pID > 0) {
    forkHandleReport report = kshingling.SyncNreport(Alice, Bob);
    String_Size = Bobtxt.length() * 8; // 8 bits per character
//        auto a = kshingling.getString(Alice, Alice_content);
//        auto b = kshingling.getString(Bob, Bob_content);
    result = make_pair(kshingling.getString(Alice, Alice_content)
                       == kshingling.getString(Bob, Bob_content), report.bytes);
//        waitpid(pID, &chld_state, my_opt);
//    } else {
//        cout << "throw out err = " << err << endl;
//        throw err;
//    }
    return result;

}

void KshingleSyncPerf::generateCSV(){

    // file parameters
//    vector<vector<long>> F_editDistance;  // X
//    vector<vector<long>> F_commCost;  // Y
//    vector<string> F_legend;
//    string F_title;

    //file format title = file name
    // legend;X;Y
    // each num divided by ","
    ofstream myfile;
    myfile.open ("./tests/perf/"+F_title+".txt");
    for (int i = 0; i < F_legend.size(); ++i) {
        myfile << F_legend[i]+";";
        for (int j = 0; j <F_editDistance[i].size() ; j++) {
            myfile << to_string(F_editDistance[i][j])+",";
        }
        myfile << ";";
        for (int j = 0; j <F_commCost[i].size() ; j++) {
            myfile << to_string(F_commCost[i][j])+",";
        }
        myfile << "\n";
    }
    myfile << "String Size; "+ to_string(String_Size); // has to be placed at the end to work with other plots
    myfile.close();

    //system("python ./tests/perf/graphGen.py");
}