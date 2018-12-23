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


bool UniqueDecode::isUD(const string str) {
    //Get shingles based on string input
    //InitGraph
    AdjMtx adjMatrix;
    for (int i = 0; i < str.size(); ++i) {
        auto tmpvex = str.substr(i, shingleLen-1);
        if (!adjMatrix.contains(StrtoZZ(tmpvex))) {
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
    //map<ZZ,int> cycle_pool; // a pool to store all cycles
    for (int j = 1; j < str.size(); ++j) {
        int vex_i = longgestNxtShingle(j, shingle_set, str);  // find the right shingle Vex
        auto cur = shingle_set[vex_i];  // update current
        if (not isVisited[vex_i]) {  // if not visited
            isVisited[vex_i] = true;  // marked it visited
        } else {
            if (adjMatrix.getWeight(prev, cur) == 0) {  // does edge w[i-1] -> w[i] not already exist in G?
                if (isCycle[vex_i]) {  //does w[i] already belongs  to a cycle
                    return false;
                } else {
                    // find the last place of vex occurrence and label all cycles
                    isCycle[vex_i]=true;
                    for (int j = 0; j > 0; --j) {
                        if (str.substr(j,shingle_set[vex_i].size()) == ZZtoStr(shingle_set[vex_i])){
                            break;
                        }
                        isCycle[longgestPrevShingle(j,shingle_set,str)] = true;
                    }
                }
            }
        }
        bool distinct = true; // init distinct
        for (auto tmp_shingle : shingle_set) {
            if (adjMatrix.getWeight(tmp_shingle, cur) == 1 and distinct) {
                distinct = false;
            } else if (adjMatrix.getWeight(tmp_shingle, cur) == 1 and not distinct) {
                return false;
            }
        }
        // connect the graph
        if (!adjMatrix.setWeight(prev,cur,1)){
            throw invalid_argument("don't have an vex");
        }
//        cout<<ZZtoStr(prev) + "->" + ZZtoStr(cur)<<endl;


        j += cur.size() - 1;  // update string read progress
        prev = cur;  // update prev edge
    }
    return true;
}

int UniqueDecode::longgestNxtShingle(int str_i, vector<ZZ> shingle_set, string str){
    auto nxt = str.substr(str_i,shingleLen-1);
    string tmp = "";
    int resi = -1;
    for (int i = 0; i < shingle_set.size(); ++i){
        string s = ZZtoStr(shingle_set[i]);
        if (str.size()<str_i+s.length()){
            invalid_argument("longgestNxtShingle is going out of range");
        }
        if (s.substr(0,shingleLen-1) == nxt and s == str.substr(str_i,s.length()) and s.size()>tmp.size()){
            tmp = s;
            resi = i;
        }
    }

    return (resi>=0)? resi : throw invalid_argument("longgestNxtShingle can not find any shingle");
}

int UniqueDecode::longgestPrevShingle(int str_i, vector<ZZ> shingle_set, string str){
    auto nxt = str.substr(str_i-shingleLen+1,shingleLen-1);
    string tmp = "";
    int resi = -1;
    for (int i = 0; i < shingle_set.size(); ++i){
        string s = ZZtoStr(shingle_set[i]);
        if (0>str_i-s.length()){
            invalid_argument("longgestPrevShingle is going out of range");
        }
        if (s.substr(s.size()-shingleLen+1) == nxt and s == str.substr(str_i-s.size()+1,s.size()) and s.size()>tmp.size()){
            tmp = s;
            resi = i;
        }
    }
    return (resi>=0)? resi : throw invalid_argument("longgestPrevShingle can not find any shingle");
}

string UniqueDecode::reconstructDFS(vector<ZZ> shingle_set_ZZ){
    // init visited vector
    vector<pair<string,bool>> isVisited_pair;
    if (shingle_set_ZZ.size()>0){
        // convert ZZ back to string
        for (auto shingle : shingle_set_ZZ){
            isVisited_pair.push_back(make_pair(ZZtoStr(shingle),false));
        }
    }else{
        throw invalid_argument("reconstructDFS - Input shingle_set is empty");
    }

    //find the head
    string str;
    for (auto shingle : isVisited_pair) {
        if (shingle.first.at(0) == stopWord) {
            str = shingle.first;
        }
    }
    shingle2str(str,isVisited_pair);
    return str;
}

vector<vector<pair<string,bool>>::iterator> UniqueDecode::potNxtLst(const string nxt,vector<pair<string,bool>> &isVisited_pair){
    vector<vector<pair<string,bool>>::iterator>  res;
    for (vector<pair<string,bool>>::iterator shingle =isVisited_pair.begin();shingle!=isVisited_pair.end();++shingle){
        if(not shingle->second and shingle->first.substr(0,nxt.size()) == nxt){ // if not visited
            res.push_back(shingle);
        }
    }
    return res;
}

void UniqueDecode::shingle2str(string& str, vector<pair<string,bool>> &isVisited_pair){
    for (auto shingle : potNxtLst(str.substr(str.size()-shingleLen),isVisited_pair)) {
        shingle->second = true; // set an vex/shingle read
        str += shingle->first.substr(shingle->first.size() - 1);
        shingle2str(str, isVisited_pair);

        // if not all vex is visited
        if (!isAllVisited(isVisited_pair)) {
            str = str.substr(0,str.size()-shingle->first.size());
        }
        //else
        return;
    }
}

bool UniqueDecode::isAllVisited(vector<pair<string,bool>> isVisited_pair){
    for (auto it = isVisited_pair.begin(); it!=isVisited_pair.end();++it){
        if (!it->second){
            return false;
        }
    }
    return true;
}

vector<ZZ> UniqueDecode::getShingleSet(const string str){
    vector<ZZ> res;
    for (int i = 0; i < str.size(); ++i) {
        ZZ shingle = StrtoZZ(str.substr(i, shingleLen));
        auto it = find(res.begin(),res.end(),shingle);
        if (it == res.end()) { // not exist in the res set
            res.push_back(shingle);
        }
    }
    return res;
}

vector<ZZ> UniqueDecode::getMergeInd(const string str){
    vector<ZZ> res;

    return res;
}