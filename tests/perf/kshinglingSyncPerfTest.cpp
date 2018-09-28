//
// Created by Bowen Song on 9/27/18.
//
#include "kshinglingSyncPerfTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KshingleSyncPerf);
KshingleSyncPerf::KshingleSyncPerf() = default;
KshingleSyncPerf::~KshingleSyncPerf() = default;
void KshingleSyncPerf::testAll() {





    // graph parameters

    vector<long> G_editDistance;  // X
    vector<long> G_commCost;  // Y
    vector<string> G_legend;

    // K_shingle packages
    int string_len = 200;
    pair<int,int> editDistance_bar_range = make_pair(2,10);
    int shingle_len = 3;
    GenSync::SyncProtocol base_set_proto = GenSync::SyncProtocol::CPISync;
    GenSync::SyncComm base_comm = GenSync::SyncComm::socket;

    F_title = "Comm vs. Edit_Dist Str Len="+to_string(string_len)+" & K="+to_string(shingle_len);
    for (int editDistance_bar = editDistance_bar_range.first;
         editDistance_bar < editDistance_bar_range.second;
         editDistance_bar+=2) {


        // Create K-shingleing packages
        string Alicetxt = genRandString(string_len);
        K_Shingle Alice_content = K_Shingle(shingle_len);
        string Bobtxt = randStringEdit(Alicetxt, editDistance_bar);
        K_Shingle Bob_content = K_Shingle(shingle_len);

        // package them
        vector<pair<string, K_Shingle>> inputPackage;
        inputPackage.push_back(make_pair(Bobtxt, Bob_content));
        inputPackage.push_back(make_pair(Alicetxt, Alice_content));

        bool success = false;
        // while not success, we increase the edit distance
        pair<bool, long> res;
        while (!success) {

            res = findCosts(inputPackage, string_len, editDistance_bar, shingle_len,
                            base_set_proto, base_comm);
            editDistance_bar++;
            success = res.first;
        }
        G_commCost.push_back(res.second);
    }

}

pair<bool,long> KshingleSyncPerf::findCosts(vector<pair<string,K_Shingle>> inputPackage, int string_len, int editDistance_bar,int shingle_len,
                                  GenSync::SyncProtocol base_set_proto,GenSync::SyncComm base_comm){

    auto Alicetxt = inputPackage[0].first;
    auto Alice_content = inputPackage[0].second;
    auto Bobtxt = inputPackage[1].first;
    auto Bob_content = inputPackage[1].second;

    //see the actual num of diff
    auto Alice_set = Alice_content.getShingleSet_str(Alicetxt);
    auto Bob_set = Bob_content.getShingleSet_str(Bobtxt);
    int numDif = multisetDiff(Alice_set, Bob_set).size();

//number of difference between should alwasy be editDistance_bar*(shingle_len-1)
    kshinglingSync kshingling = kshinglingSync(shingle_len, base_set_proto, base_comm, editDistance_bar*(shingle_len-1), 8);


    GenSync Alice = kshingling.SyncHost(Alicetxt, Alice_content);


    GenSync Bob = kshingling.SyncHost(Bobtxt, Bob_content);


    forkHandleReport report = kshingling.SyncNreport(Alice, Bob);


    return make_pair(kshingling.getString(Alice, Alice_content)
                     == kshingling.getString(Bob, Bob_content),report.bytes);
}

void generateCSV(string title){

    // file parameters
    vector<vector<long>> F_editDistance;  // X
    vector<vector<long>> F_commCost;  // Y
    vector<string> F_legend;
    string F_title;

    //file format title = file name
    // legend;X;Y
    // each num divided by ","
    ofstream myfile;
    myfile.open (F_title+".csv");
    for (int i = 0; i < F_legend.size(); ++i) {
        myfile << F_legend[i]+";";
        for (int j = 0; j < F_commCost.size(); ++j) {
            myfile << to_string(F_editDistance[i][j])+",";
        }
        myfile << ";";
        for (int j = 0; j < F_commCost.size(); ++j) {
            myfile << to_string(F_commCost[i][j])+",";
        }
        myfile << "\n";
    }
    myfile.close();
}