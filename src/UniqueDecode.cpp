//
// Created by Bowen on 10/3/18.
//

#include "UniqueDecode.h"
#include "AdjMtx.h"

UniqueDecode::UniqueDecode(const size_t shingle_len, const char stop_word){
    shingleLen = shingle_len;
    stopWord = stop_word;
}

UniqueDecode::~UniqueDecode() = default;


bool UniqueDecode::isUD(const string str, const size_t k) {
    //Get shingles based on string input
    //InitGraph
    AdjMtx adjMatrix;
    for (int i = 0; i < str.size(); ++i) {
        auto tmpvex = str.substr(i, i + k - 1);
        if (adjMatrix.contains(StrtoZZ(tmpvex))) {
            adjMatrix.addNewVex(StrtoZZ(tmpvex));
        }
    }

    //Init visited and cycle
    vector<bool> isCycle;
    isCycle.assign(adjMatrix.getGraphSize(), false);
    vector<bool> isVisited;
    isVisited.assign(adjMatrix.getGraphSize(), false);

    isVisited[0] = true;  // don't sort the edges
    auto shingle_set = adjMatrix.getGraphVex();
    auto prev = shingle_set[0];

    for (int j = 0; j < str.size(); ++j) {
        int vex_i = longgestShingle(j, shingle_set, str);  // find the right shingle Vex
        auto cur = shingle_set[vex_i];  // update current
        if (not isVisited[vex_i]) {  // if not visited
            isVisited[vex_i] = true;  // marked it visited
        } else {
            if (adjMatrix.getWeight(prev, cur) == 0) {  // does edge w[i-1] -> w[i] not already exist in G?
                if (isCycle[vex_i]) {  //does w[i] already belongs to a cycle
                    return false;
                } else {
                    // find the last place of vex occurrence and label all cycles
                }
            }
            // start to preserve cycles ... how 
        }
        bool distinct = true; // init distinct
        for (auto tmp_shingle : shingle_set) {
            if (adjMatrix.getWeight(tmp_shingle, cur) == 1 and distinct) {
                distinct = false;
            } else if (adjMatrix.getWeight(tmp_shingle, cur) == 1 and not distinct) {
                return false;
            }
        }
        adjMatrix.setWeight(prev,cur,1); // connect the graph


        j += cur.size() - 1;  // update string read progress
        prev = cur;  // update prev edge
    }
    return true;
}

int UniqueDecode::longgestShingle(int str_i, vector<ZZ> shingle_set, string str){
    auto nxt = str.substr(str_i,str_i+shingleLen-1);
    string tmp = "";
    int resi = 0;
    for (int i = 0; i < shingle_set.size(); ++i){
        string s = ZZtoStr(shingle_set[i]);
        if (s.substr(str_i,str_i+shingleLen-1) == nxt and s == str.substr(str_i,str_i+s.length()) and s.size()>tmp.size()){
            tmp = s;
            resi = i;
        }
    }
    return resi;
}