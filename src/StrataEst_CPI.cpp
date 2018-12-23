//
// Created by Bowen Song on 10/26/18.
//

#include "StrataEst_CPI.h"

StrataEst_CPI::StrataEst_CPI(size_t value_size, size_t num_strata, size_t num_Eval_Pts)
: bits(value_size), numStrata(num_strata), numEvalPts(num_Eval_Pts) {
    space = pow(2, numStrata);
    for (int i = 0; i < numStrata; ++i) {
        CPISync cpi = CPISync(numEvalPts, bits, 0, 1);
        Strata.push_back(cpi);
    }
}

StrataEst_CPI::~StrataEst_CPI() {};

void StrataEst_CPI::insert(DataObject *datum) {
    _inject(datum, get_Assign_Ind(datum));
}

void StrataEst_CPI::_inject(DataObject *item, int index) {
    CPISync &cpi = Strata[index];
    cpi.addElem(item);
}

int StrataEst_CPI::get_Assign_Ind(DataObject* Elem) {
    std::hash<std::string> shash;
    auto hashMod = shash(toStr(*Elem)) % space;
    if (hashMod==0) return 0;
    return floor(log2(hashMod));
    // Core part: more chance to return higher int with highest int at 1/2 sample rate
    // hash fxn -> every time sample the same thing
    // Consider hash function spread out set elements evenly in the universe
    // biggest index returned is going to be the subspace from (space+1)/2 to (space)
}

size_t StrataEst_CPI::estimate() {
//    size_t count = 0;
//    for (int i = 0; i < Strata.size(); ++i) {
//        vector<pair<ZZ, ZZ>> pos, neg;
//        bool success = Strata[numStrata-i-1].listEntries(pos, neg);
//        count += pos.size() + neg.size();
//
//        if (success) {
//            count *= (size_t) pow(2, i + 1);
//            return count;
//        }
//    }
    throw invalid_argument("Set diff inestimable with current Strata "
                           "Estimation setting which could predict up to"
                           + to_string(pow(2, numStrata)) + "differences");
}