//
// Created by Bowen Song on 10/21/18.
//

#include "StrataEst.h"

StrataEst::StrataEst( size_t value_size, size_t num_strata, size_t num_cells) {
    numStrata =  num_strata, numCells =  num_cells, bits =  value_size;
    for (int i = 0; i < numStrata; ++i) {
        IBLT iblt = IBLT(numCells,bits);
        Strata.push_back(iblt);
    }
    space = pow(2,numStrata);
}

StrataEst::~StrataEst(){};

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

    return floor(log2(shash(toStr(Elem)) % space));
}

void StrataEst::_inject(ZZ item, int index) {
    IBLT &iblt = Strata.at(index);
    iblt.insert(item, item);
}

size_t StrataEst::estimate() {
    size_t count = 0;
    for (int i = 0; i < Strata.size(); ++i) {
        vector<pair<ZZ, ZZ>> pos, neg;
        bool success = Strata[numStrata-i-1].listEntries(pos, neg);
        count += pos.size() + neg.size();

        if (success) {
            count *= (size_t) pow(2, i + 1);
            return count;
        }
    }
    throw invalid_argument("Set diff inestimable with current Strata "
                           "Estimation setting which could predict up to"
                           + to_string(pow(2, numStrata)) + "differences");
}

StrataEst& StrataEst::operator-=(const StrataEst &other) {
    if (numCells != other.numCells)
        Logger::error_and_quit("The Number of cells per IBF is not the same. We have "
                               + toStr(numCells) + " and They have " + toStr(other.numCells));

    if (numStrata != other.numStrata)
        Logger::error_and_quit("The Number of IBFs is not the same. We have "
                               + toStr(numStrata) + " and They have " + toStr(other.numStrata));

    if (bits != other.bits)
        Logger::error_and_quit("The Number of bits per element is not the same. We have "
                               + toStr(bits) + " and They have " + toStr(other.bits));

    for (int i = 0; i < numStrata; ++i) {
        this->Strata[i] -= other.Strata[i];
    }
    return *this;
}
size_t StrataEst::IBFsize() const { return numCells; }

size_t StrataEst::size() const { return numStrata; }

size_t StrataEst::eltSize() const { return bits; }
