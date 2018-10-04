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
        //if (adjMatrix.contains())
    }


    //Init visited and cycle
    vector<bool> isCycle;
    isCycle.assign(adjMatrix.getGraphSize(), false);
    vector<bool> isVisited;
    isVisited.assign(adjMatrix.getGraphSize(), false);



}
