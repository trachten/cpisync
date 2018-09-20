//
// Created by Bowen on 9/18/18.
//

#include "kshingling.h"

K_Shingle::K_Shingle() = default;
K_Shingle::~K_Shingle() = default;

K_Shingle::K_Shingle(const size_t k, const string str) {
    for (int i = 0; i < str.size()-k+1; ++i) {

        shingleSet.push_back(make_pair(str.substr(i, k),1));
    }

}