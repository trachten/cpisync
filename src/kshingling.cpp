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

vector<pair<string,int>>  K_Shingle::getEdges(const string verStart, vector<pair<string,int>> changed_shingleSet) {
    if (verStart.size() > 0) {
        vector<pair<string,int>> templst;
        for (vector<pair<string,int>>::iterator edge = changed_shingleSet.begin(); edge != changed_shingleSet.end(); ++edge) {
            if (edge->first.substr(0,k-1) == verStart) {
                templst.push_back(*edge);
                break;
            }
        }
        return templst;
    } else {
        throw invalid_argument("No vertex start string for searching");
    }
}

vector<string> K_Shingle::reconstructString() {
    vector<string> str_collect;
    auto startedge = getEdges(stopword,shingleSet);
    if (startedge.size()==1) {
        string str = startedge[0].first;
        shingle2string(shingleSet, str, str_collect, str.substr(0,k-1));
    } else{
        if (startedge.size()>1)
            throw invalid_argument("Do not have a unique start point");
        else if (startedge.size()<1)
            throw invalid_argument("Do not have a start point at all");
    }
}

void K_Shingle::shingle2string(vector<pair<string,int>> changed_shingleSet, string curEdge, vector<string>& strCollect, string str){

    auto lst= getEdges(curEdge.substr(1), changed_shingleSet);
    for (vector<pair<string,int>>::iterator it = changed_shingleSet.begin(); it < changed_shingleSet.end(); ++it) {
        str += it->first.substr(k-1);

        if (it->second>1)
            it->second--;
        else
            changed_shingleSet.erase(it);

        if (it->first.substr(k-1)==stopword) {
            if (changed_shingleSet.size() == 0)
                strCollect.push_back(str);
        }
        shingle2string(changed_shingleSet,it->first,strCollect,str);
        changed_shingleSet.push_back(*it);
        str.pop_back();


    }

}