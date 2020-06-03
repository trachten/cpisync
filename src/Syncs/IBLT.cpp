/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18.
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research, which are based on:
// * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 2011 49th Annual Allerton Conference on Communication, Control, and Computing (Allerton). IEEE, 2011.
// * Eppstein, David, et al. "What's the difference?: efficient set reconciliation without prior context." ACM SIGCOMM Computer Communication Review 41.4 (2011): 218-229.
//

#include <CPISync/Syncs/IBLT.h>

IBLT::IBLT() = default;
IBLT::~IBLT() = default;

IBLT::IBLT(size_t expectedNumEntries, size_t _valueSize)
: valueSize(_valueSize)
{
    // 1.5x expectedNumEntries gives very low probability of decoding failure
    size_t nEntries = expectedNumEntries + expectedNumEntries/2;
    // ... make nEntries exactly divisible by N_HASH
    while (N_HASH * (nEntries/N_HASH) != nEntries) ++nEntries;
    hashTable.resize(nEntries);
}

hash_t IBLT::_hash(const hash_t& initial, long kk) {
    if(kk == -1) return initial;
    std::hash<std::string> shash;
    return _hash(shash(toStr(initial)), kk-1);
}

hash_t IBLT::_hashK(const ZZ &item, long kk) {
    std::hash<std::string> shash; // stl uses MurmurHashUnaligned2 for calculating the hash of a string
    return _hash(shash(toStr(item)), kk-1);
}

hash_t IBLT::_setHash(multiset<shared_ptr<DataObject>> &tarSet)
{
    hash_t outHash = 0;
    for (auto itr : tarSet)
    {
        outHash += _hashK(itr->to_ZZ(), 1);
    }
    return outHash;
}

void IBLT::_insert(long plusOrMinus, ZZ key, ZZ value) {
    long bucketsPerHash = hashTable.size() / N_HASH;

    if(sizeof(value) != valueSize) {
        Logger::error_and_quit("The value being inserted is different than the IBLT value size! value size: "
                               + toStr(sizeof(value)) + ". IBLT value size: " + toStr(valueSize));
    }

    for(int ii=0; ii < N_HASH; ii++){
        hash_t hk = _hashK(key, ii);
        long startEntry = ii * bucketsPerHash;
        IBLT::HashTableEntry& entry = hashTable.at(startEntry + (hk%bucketsPerHash));

        entry.count += plusOrMinus;
        entry.keySum ^= key;
        entry.keyCheck ^= _hashK(key, N_HASHCHECK);
        if (entry.empty()) {
            entry.valueSum.kill();
        }
        else {
            entry.valueSum ^= value;
        }
    }
}

void IBLT::insert(ZZ key, ZZ value)
{
    _insert(1, key, value);
}

void IBLT::erase(ZZ key, ZZ value)
{
    _insert(-1, key, value);
}

bool IBLT::get(ZZ key, ZZ& result){
    long bucketsPerHash = hashTable.size()/N_HASH;
    for (long ii = 0; ii < N_HASH; ii++) {
        long startEntry = ii*bucketsPerHash;
        unsigned long hk = _hashK(key, ii);
        const IBLT::HashTableEntry& entry = hashTable[startEntry + (hk%bucketsPerHash)];

        if (entry.empty()) {
            // Definitely not in table. Leave
            // result empty, return true.

            return true;
        }
        else if (entry.isPure()) {
            if (entry.keySum == key) {
                // Found!
                result = entry.valueSum;
                return true;
            }
            else {
                // Definitely not in table
                result.kill();
                return true;
            }
        }
    }

    // Don't know if k is in table or not; "peel" the IBLT to try to find it:
    long nErased;
    do {
        nErased = 0;
        for (IBLT::HashTableEntry &entry : this->hashTable) {
            if (entry.isPure()) {
                if (entry.keySum == key) {
                    string s = toStr(entry.valueSum);
                    result = entry.valueSum;
                    return true;
                }
                nErased++;
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
            }
        }
    } while (nErased > 0);
    return false;
}

bool IBLT::HashTableEntry::isPure() const
{
    if (count == 1 || count == -1) {
        hash_t check = _hashK(keySum, N_HASHCHECK);
        return (keyCheck == check);
    }
    return false;
}

bool IBLT::HashTableEntry::empty() const
{
    return (count == 0 && IsZero(keySum) && keyCheck == 0);
}

bool IBLT::listEntries(vector<pair<ZZ, ZZ>> &positive, vector<pair<ZZ, ZZ>> &negative){
    long nErased;
    do {
        nErased = 0;
        for(IBLT::HashTableEntry& entry : this->hashTable) {
            if (entry.isPure()) {
                if (entry.count == 1) {
                    positive.emplace_back(std::make_pair(entry.keySum, entry.valueSum));
                }
                else {
                    negative.emplace_back(std::make_pair(entry.keySum, entry.valueSum));
                }
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                ++nErased;
            }
        }
    } while (nErased > 0);

    // If any buckets for one of the hash functions is not empty,
    // then we didn't peel them all:
    for (IBLT::HashTableEntry& entry : this->hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}

IBLT& IBLT::operator-=(const IBLT& other) {
    if(valueSize != other.valueSize)
        Logger::error_and_quit("The value sizes between IBLTs don't match! Ours: "
        + toStr(valueSize) + ". Theirs: " + toStr(other.valueSize));
    if(hashTable.size() != other.hashTable.size())
        Logger::error_and_quit("The IBLT hash table sizes are different! Ours: "
        + toStr(hashTable.size()) + ". Theirs: " + toStr(other.valueSize));

    for (unsigned long ii = 0; ii < hashTable.size(); ii++) {
        IBLT::HashTableEntry& e1 = this->hashTable.at(ii);
        const IBLT::HashTableEntry& e2 = other.hashTable.at(ii);
        e1.count -= e2.count;
        e1.keySum ^= e2.keySum;
        e1.keyCheck ^= e2.keyCheck;
        if (e1.empty()) {
            e1.valueSum.kill();
        }
        else {
            e1.valueSum ^= e2.valueSum;
        }
    }
    return *this;
}

IBLT IBLT::operator-(const IBLT& other) const {
    IBLT result(*this);
    result-=other;
    return result;
}

size_t IBLT::size() const {
    return hashTable.size();
}

size_t IBLT::eltSize() const {
    return valueSize;
}

string IBLT::toString() const
{
    string outStr="";
    for (auto entry : hashTable)
    {
        outStr += toStr<long>(entry.count) + ","
                + toStr<hash_t>(entry.keyCheck) + ","
                + toStr<ZZ>(entry.keySum) + "," 
                + toStr<ZZ>(entry.valueSum) 
                + "\n";
    }
    outStr.pop_back();
    return outStr;
}

void IBLT::reBuild(string &inStr)
{
    vector<string> entries = split(inStr, '\n');
    int index = 0;
    for (auto entry : entries)
    {
        vector<string> infos = split(entry, ',');
        HashTableEntry curEntry;
        curEntry.count = strTo<long>(infos[0]);
        curEntry.keyCheck = strTo<hash_t>(infos[1]);
        curEntry.keySum = strTo<ZZ>(infos[2]);
        curEntry.valueSum = strTo<ZZ>(infos[3]);
        this->hashTable[index] = curEntry;
        index++;
    }
}

void IBLT::insert(IBLT &chldIBLT, hash_t &chldHash)
{

    ZZ ibltZZ = strToZZ(chldIBLT.toString());
    // conv can't be applied to hash_t types, have to use toStr&strTo functions
    // instead.
    _insert(1, ibltZZ, strTo<ZZ>(toStr<hash_t>(chldHash)));
}

void IBLT::erase(IBLT &chldIBLT, hash_t &chldHash)
{
    ZZ ibltZZ = strToZZ(chldIBLT.toString());
    _insert(-1, ibltZZ, strTo<ZZ>(toStr<hash_t>(chldHash)));
}

void IBLT::insert(multiset<shared_ptr<DataObject>> tarSet, size_t elemSize, size_t expnChldSet)
{
    hash_t setHash = _setHash(tarSet);

    // make sure the hash is unique even for duplicate sets
    auto it = std::find(hashes.begin(),hashes.end(),setHash);

    while(it != hashes.end()){
        setHash = _hash(setHash,1);
        it = std::find(hashes.begin(),hashes.end(),setHash);
    }
    
    hashes.push_back(setHash);
    // Put chld set into a chld IBLT
    IBLT chldIBLT(expnChldSet, elemSize);
    for (auto itr : tarSet)
    {
        chldIBLT.insert(itr->to_ZZ(), itr->to_ZZ());
    }

    // Put the pair(chld IBLT, hash of set) into the outer IBLT T
    insert(chldIBLT, setHash);

}

void IBLT::erase(multiset<shared_ptr<DataObject>> tarSet, size_t elemSize, size_t expnChldSet)
{
    hash_t setHash = _setHash(tarSet);
    
    // delete set hash in the vector
    bool found = false;

    // if target hash not in current structure, hash again and perform another round of search
    long curInd = 0;
    while(true){
        for (auto itr = hashes.begin(); itr < hashes.end(); itr++)
        {
            if ((*itr) == setHash)
            {
                hashes.erase(itr);
                found = true;
                break;
            }
        }
        if(!found){
            setHash = _hash(setHash,1);
            curInd++;
        }
        else
            break;

        if(curInd > this->size()){
            Logger::error_and_quit("Error deleting target set: Not found in current structure");
        }
    }


    IBLT chldIBLT(expnChldSet, elemSize);
    for (auto itr : tarSet)
    {
        chldIBLT.insert(itr->to_ZZ(), itr->to_ZZ());
    }
    erase(chldIBLT, setHash);

}