//
// Created by Bowen on 10/3/18.
// This is based on uniquely decodable shingles
//

#ifndef CPISYNCLIB_UNIQUEDECODE_H
#define CPISYNCLIB_UNIQUEDECODE_H

#include <NTL/ZZ.h>
#include <vector>
#include <utility>
#include <string>
#include "AdjMtx.h"
#include "Auxiliary.h"
#include "DataObject.h"

using std::vector;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;


class UniqueDecode {
public:
    /**
     * Construct a K_Shingle set object with k as each shingle size
     * @param k fixing shingle size to be k
     */
    UniqueDecode(const size_t shingle_len, const char stop_word);
    // Default deconstructor
    ~UniqueDecode();

    bool isUD(const string str);
    string reconstructDFS(vector<ZZ> shingle_set);
    int longgestNxtShingle(int str_i, vector<ZZ> shingle_set, string str);
    int longgestPrevShingle(int str_i, vector<ZZ> shingle_set, string str);

    vector<string> potNxtLst(string nxt,const map<string,bool> &isVisited);
    void shingle2str(string& str, map<string,bool> &isVisited);
private:
    char stopWord;
    size_t shingleLen;
    string origStr;
};

#endif //CPISYNCLIB_UNIQUEDECODE_H
