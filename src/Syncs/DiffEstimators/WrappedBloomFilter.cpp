//
// Created by Zifan Wang on 2019-08-20.
//


#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/DiffEstimators/WrappedBloomFilter.h>
#include <NTL/ZZ.h>

WrappedBloomFilter::WrappedBloomFilter() = default;
WrappedBloomFilter::~WrappedBloomFilter() = default;

WrappedBloomFilter::WrappedBloomFilter(vector<long> cells){
    for(auto cell : cells)
        this->hashTable.push_back(cell);
}

WrappedBloomFilter::WrappedBloomFilter(size_t expEntries){
    // ratio A for w/m should usually be less than 1 for small false positive values
    // where w is denoted as initial weight (#elements * #hashFunctions)
    // m denoted as length of WBF
    // In this case ration A is set to 2/3
    size_t nEntries = 1.5 * numHashFuncs * expEntries;

    while(numHashFuncs * (nEntries / numHashFuncs) != nEntries) nEntries ++;

    hashTable.resize(nEntries);   
}



void WrappedBloomFilter::_insert(int plus, ZZ elem){
    long bucketPerHash = hashTable.size() / numHashFuncs;

    for(long ii=0; ii< numHashFuncs ; ii++){
        hash_t hk = _hashK(elem, ii);
        long curStart = ii * bucketPerHash;
        long &entry = hashTable.at(curStart + hk%bucketPerHash);
        entry += plus;
    }
}

void WrappedBloomFilter::insert(ZZ elem){
    _insert(1,elem);
}

void WrappedBloomFilter::erase(ZZ elem){
    _insert(-1, elem);
}

string WrappedBloomFilter::toString(){
    string out;
    int curLoc =0;
    for(auto itr: hashTable){
        out += "loc: " + toStr(curLoc) + " val: " + toStr(itr) + "\n"; 
        curLoc++;
    }
    return out;
}

long WrappedBloomFilter::errCorrect(long setA, long errProbLog, long expDiff){

    long phi = 1;
    double aa = pow((1 - pow((1 - (double)1/this->size()), numHashFuncs * setA)), numHashFuncs);
    double bb = pow((1 - pow((1 - pow((1 - (double)1/this->size()), expDiff)), numHashFuncs)), setA);
    

    double errProb = pow(2, -errProbLog);

    double upperBound = pow(aa, phi) * pow(bb, -phi) * nCr(setA, phi);

    while(errProb < upperBound){
        phi ++;
        upperBound = pow(aa, phi) * pow(bb, -phi) * nCr(setA, phi);
    }

    return phi;
}

hash_t WrappedBloomFilter::_hashK(const ZZ &item, long kk){
    std::hash<std::string> shash; // stl uses MurmurHashUnaligned2 for calculating the hash of a string
    return _hash(shash(toStr(item)), kk-1);
}
hash_t WrappedBloomFilter::_hash(const hash_t& initial, long kk){
    if(kk == -1) return initial;
    std::hash<std::string> shash;
    return _hash(shash(toStr(initial)), kk-1);
}

long WrappedBloomFilter::getCell(long index){
    return hashTable[index];
}