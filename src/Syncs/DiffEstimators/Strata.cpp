//
// Created by Zifan Wang on 2019-08-26.
//

#include <CPISync/Syncs/DiffEstimators/Strata.h>
#include <CPISync/Syncs/IBLT.h>


Strata::Strata(size_t elemSize, size_t innerLen, size_t outerLen){
    elmSize = elemSize;
    outLen = outerLen;
    for(long ii=0; ii<outLen; ii++){
        IBLT curIBLT(innerLen,elmSize);
        cells.push_back(curIBLT);
    }
    inLen = cells.back().size();
}

Strata::Strata() = default;

Strata::~Strata() = default;

Strata::Strata(const vector<IBLT> &datums){
    if(datums.size()!=0){
        auto sample = datums.back();

        elmSize = sample.eltSize();
        outLen = datums.size();
        inLen = sample.size();

        for(auto datum : datums)
            cells.push_back(datum);
    }

    else
        return;
}

void Strata::insert(shared_ptr<DataObject> &datum){
    ZZ elem = datum->to_ZZ();
    long index = _getIndex(elem);
    IBLT &tar = cells[index];
    tar.insert(elem, elem);
}

void Strata::erase(shared_ptr<DataObject> &datum){
    ZZ elem = datum->to_ZZ();
    long index =  _getIndex(elem);
    cells[index].erase(elem, elem);
}

Strata& Strata::operator-=(const Strata &other){
    if(this->elmSize != other.elmSize)
        Logger::error_and_quit("strata element size doesn't match");
    if(this->inLen != other.inLen)
        Logger::error_and_quit("strata inner length doesn't match. My inLen: "+toStr(inLen)+" other inLen: "+toStr(inLen));
    if(this->outLen != other.outLen)
        Logger::error_and_quit("Strata length doesn't match");

    
    for(long ii=0; ii<outLen; ii++)
        this->cells[ii] -= other.cells[ii];
    return *this;
}

long Strata::decode(){
    long count = 0;
    for(long ii = outLen-1; ii >= -1; ii--){
        vector<pair<ZZ,ZZ>> pos, neg;
        if(ii < 0 || !cells[ii].listEntries(pos, neg))
            return pow(2, ii+1) * count;
        count += pos.size() + neg.size();
    }
}

long Strata::_getIndex(const ZZ datum){
    std::hash<std::string> shash;  
    long space = pow(2,outLen);
    auto hashMod = shash(toStr(datum)) % space;
    if(hashMod == 0) return 0;
    return trailingZeros(hashMod);
}

size_t Strata::IBFsize() const { return inLen; }

size_t Strata::size() const { return outLen; }

size_t Strata::eltSize() const { return elmSize; }

IBLT Strata::getCell(long index){
    return cells[index];
}