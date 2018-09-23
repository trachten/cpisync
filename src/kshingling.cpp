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

    //  Sanity check
    for (int i = 0; i < str.size(); ++i) {
        if(str.substr(i,1)==stopword){
            throw invalid_argument("Input string includes Stopword");
        }
    }
    if(k>orig_string.size()){
        throw invalid_argument("Shingle size has to be smaller than the string length");
    }else if(k<2){
        throw invalid_argument("Minimum shingle size has to be bigger than 2");
    }

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
        sort(shingleSet.begin(), shingleSet.end());
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
            }
        }
        return templst;
    } else {
        throw invalid_argument("No vertex start string for searching");
    }
}

pair<string,int> K_Shingle::reconstructStringBacktracking(int strOrder) {
    vector<string> str_collect;
    vector<pair<string,int>> changed_shingleSet = shingleSet;
    string startString;
    for (auto it = changed_shingleSet.begin(); it != changed_shingleSet.end(); ++it) {
        if (it->first.substr(0,1)==stopword){
            startString = it->first;
            changed_shingleSet.erase(it);
            break;
        }
    }

    if (startString.size()>0) {
//        // Delete the first edge by value
//        changed_shingleSet.erase(remove(changed_shingleSet.begin(), changed_shingleSet.end(), startedge[0]), changed_shingleSet.end());
        shingle2string(changed_shingleSet, startString, str_collect,strOrder, startString);
        return make_pair(str_collect[strOrder],strOrder);
    } else {
        throw invalid_argument("Shingle Set does not have a start point");
    }
}



void K_Shingle::shingle2string(vector<pair<string,int>> changed_shingleSet, string curEdge, vector<string>& strCollect,int &str_order, string str) {

    auto lst = getEdges(curEdge.substr(1), changed_shingleSet);
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        pair<string, int> tempedge = *it;
        str += tempedge.first.substr(k - 1);

        if (it->second > 1) {
            it->second--;
            find(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge)->second--;
        } else {
            //lst.erase(it);
            changed_shingleSet.erase(remove(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge), changed_shingleSet.end());
        }
        if (tempedge.first.substr(k - 1) == stopword) {
            if (changed_shingleSet.size() == 0) {
                strCollect.push_back(str);
                if (str == orig_string || strCollect.size() - 1 == str_order) {
                    str_order = strCollect.size() - 1;
                }
            }

        }
        shingle2string(changed_shingleSet, tempedge.first, strCollect, str_order, str);
        if (strCollect.size()-1==str_order && str_order!=-1){
            return;
        }
        if (tempedge.second > 1) {
            it->second++;
            find(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge)->second++;
        } else {
            //lst.push_back(*it);
            changed_shingleSet.push_back(tempedge);
        }
        str.pop_back();


    }

}