//
// Created by Bowen Song on 10/21/18.
//

#include "StrataEst.h"

StrataEst::StrataEst(size_t num_strata, size_t num_cells, size_t value_size) {
    numStrata =  num_strata, numCells =  num_cells, bits =  value_size;
    for (int i = 0; i < numStrata; ++i) {
        IBLT iblt = IBLT(numCells,bits);
        Strata.push_back(iblt);
    }
}

void StrataEst::insert(DataObject *datum) {
    _insert(datum->to_ZZ());
}

void StrataEst::_insert(ZZ Elem) {
    setElems.push_back(Elem);
}

vector<IBLT> StrataEst::exportStrata() {
    // feed all element rough a hash function get their designated IBF
    for (ZZ elem : setElems) {
        //put the element into the designated IBF
        _inject(elem, get_Assign_Ind(elem));
    }
    return Strata;
}

int StrataEst::get_Assign_Ind(ZZ& Elem) {
    std::hash<std::string> shash;
    size_t space = pow(2,numStrata);
    return ceil((shash(toStr(Elem)) % space)/2);
}

void StrataEst::_inject(ZZ item, int index) {
    IBLT &iblt = Strata.at(index);
    iblt.insert(item, item);
}

size_t StrataEst::estimate() {
    size_t count = 0;
    for (int i = 0; i < Strata.size(); ++i) {
        vector<pair<ZZ, ZZ>> pos, neg;
        bool success = Strata[i].listEntries(pos, neg);
        count += pos.size()+neg.size();
        if (success){
            count *= (size_t) pow(2,i+1);
            return count;
        }
    }
    return
}

size_t StrataEst::IBFsize() const { return numCells; }

size_t StrataEst::size() const { return numStrata; }

size_t StrataEst::eltSize() const { return bits; }
