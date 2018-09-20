//
// Created by Bowen on 9/18/18.
// Break string into a fixed length set of shingles
// reconstruct a possible string from shingle set
// calculate if that is the only sting available
//

#include "kshingling.h"

K_Shingle::K_Shingle() = default;
K_Shingle::~K_Shingle() = default;

K_Shingle::K_Shingle(const string str, const size_t shingle_size) {
    orig_string = stopword + str + stopword;
    k = shingle_size;
    _create();
}

void K_Shingle::_create() {
    if (k > 0 && orig_string.size() >0) {
        const string str = orig_string;
        //create a set of shingle in shingleSet
        for (int i = 0; i < str.size() - k + 1; ++i) {
            string nxt_Shingle = str.substr(i, k);
            incrementEdgeCount(nxt_Shingle);
        }
    } else{
        throw invalid_argument("No input string or Shingle size");
    }
}

bool K_Shingle::get(const string ver, pair<string,int>& edge){
    if (ver.size()>0) {
        for (int i = 0; i < shingleSet.size(); ++i) {
            if (shingleSet[i].first == ver) {
                edge = shingleSet[i];
                return true;
            }
        }
        return false;
    }else{
        throw invalid_argument("No vertex string for searching");
    }
}

void K_Shingle::incrementEdgeCount(const string ver) {
    if (ver.size() > 0) {
        for (vector<pair<string,int>>::iterator edge = shingleSet.begin(); edge != shingleSet.end(); ++edge) {
            if (edge->first == ver) {
                edge->second++;
                return;
            }
        }
        shingleSet.push_back(make_pair(ver, 1));
        return;
    } else {
        throw invalid_argument("No vertex string for searching");
    }
}