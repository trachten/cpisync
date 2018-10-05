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


bool UniqueDecode::isUD(const string str, const size_t k){
    //Get shingles based on string input
    //InitGraph
    AdjMtx adjMatrix;
    for (int i = 0; i < str.size(); ++i) {
        auto tmpvex = str.substr(i,i+k-1);
        if (adjMatrix.contains(StrtoZZ(tmpvex))){
            adjMatrix.addNewVex(StrtoZZ(tmpvex));
        }
    }

    // sort for binary search
    adjMatrix.sortVex();

    //Init visited and cycle
    vector<bool> isCycle;
    isCycle.assign(adjMatrix.getGraphSize(), false);
    vector<bool> isVisited;
    isVisited.assign(adjMatrix.getGraphSize(), false);

    isVisited[0] = true;

    for (int j = 0; j < str.size(); ++j) {

    }



}

vector<ZZ>::iterator UniqueDecode::longgestShingle(int str_i, vector<ZZ> shingleset, string str){
    auto nxt = str.substr(str_i,str_i+shingleLen-1);
    string res = "";
    for (auto shingle : shingleset){
        string s = ZZtoStr(shingle);
        if (s.substr(str_i,str_i+shingleLen-1) == nxt and s == str.substr(str_i,str_i+s.length()) and s.size()>res.size()){
            res = s;
        }
    }
    return binary_search(shingleset.begin(),shingleset.end(),StrtoZZ(res))
}