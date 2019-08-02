/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18. Modified by Zifan Wang on 7/30/2019
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research.
//

#include <CPISync/Syncs/IBLT.h>

IBLT::IBLT() = default;
IBLT::~IBLT() = default;

IBLT::IBLT(size_t expectedNumEntries, size_t _valueSize)
    : valueSize(_valueSize)
{
    // 1.5x expectedNumEntries gives very low probability of decoding failure
    size_t nEntries = expectedNumEntries + expectedNumEntries / 2;
    // ... make nEntries exactly divisible by N_HASH
    while (N_HASH * (nEntries / N_HASH) != nEntries)
        ++nEntries;
    hashTable.resize(nEntries);
}

hash_t IBLT::_hash(const hash_t &initial, long kk)
{
    if (kk == -1)
        return initial;
    std::hash<std::string> shash;
    return _hash(shash(toStr(initial)), kk - 1);
}

hash_t IBLT::_hashK(const ZZ &item, long kk)
{
    std::hash<std::string> shash; // stl uses MurmurHashUnaligned2 for calculating the hash of a string
    return _hash(shash(toStr(item)), kk - 1);
}

hash_t IBLT::_setHash(multiset<DataObject *> &tarSet)
{
    hash_t outHash = 0;
    for (auto i : tarSet)
    {
        outHash += _hashK(i->to_ZZ(), 1);
    }
    return outHash;
}

void IBLT::_insert(long plusOrMinus, ZZ key, ZZ value)
{
    long bucketsPerHash = hashTable.size() / N_HASH;

    if (sizeof(value) != valueSize)
    {
        Logger::error_and_quit("The value being inserted is different than the IBLT value size! value size: " + toStr(sizeof(value)) + ". IBLT value size: " + toStr(valueSize));
    }

    for (int ii = 0; ii < N_HASH; ii++)
    {
        hash_t hk = _hashK(key, ii);
        long startEntry = ii * bucketsPerHash;
        IBLT::HashTableEntry &entry = hashTable.at(startEntry + (hk % bucketsPerHash));

        entry.count += plusOrMinus;
        entry.keySum ^= key;
        entry.keyCheck ^= _hashK(key, N_HASHCHECK);
        if (entry.empty())
        {
            entry.valueSum.kill();
        }
        else
        {
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

bool IBLT::get(ZZ key, ZZ &result)
{
    long bucketsPerHash = hashTable.size() / N_HASH;
    for (long ii = 0; ii < N_HASH; ii++)
    {
        long startEntry = ii * bucketsPerHash;
        unsigned long hk = _hashK(key, ii);
        const IBLT::HashTableEntry &entry = hashTable[startEntry + (hk % bucketsPerHash)];

        if (entry.empty())
        {
            // Definitely not in table. Leave
            // result empty, return true.

            return true;
        }
        else if (entry.isPure())
        {
            if (entry.keySum == key)
            {
                // Found!
                result = entry.valueSum;
                return true;
            }
            else
            {
                // Definitely not in table
                result.kill();
                return true;
            }
        }
    }

    // Don't know if k is in table or not; "peel" the IBLT to try to find it:
    long nErased;
    do
    {
        nErased = 0;
        for (IBLT::HashTableEntry &entry : this->hashTable)
        {
            if (entry.isPure())
            {
                if (entry.keySum == key)
                {
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
    if (count == 1 || count == -1)
    {
        hash_t check = _hashK(keySum, N_HASHCHECK);
        return (keyCheck == check);
    }
    return false;
}

bool IBLT::HashTableEntry::empty() const
{
    //cout << "count: " << count << " keySum: " << keySum << " keyCheck: " << keyCheck << endl;
    return (count == 0 && IsZero(keySum) && keyCheck == 0);
}

bool IBLT::listEntries(vector<pair<ZZ, ZZ>> &positive, vector<pair<ZZ, ZZ>> &negative)
{
    long nErased = 0;
    do
    {
        nErased = 0;
        for (IBLT::HashTableEntry &entry : this->hashTable)
        {
            if (entry.isPure())
            {
                if (entry.count == 1)
                {
                    positive.emplace_back(std::make_pair(entry.keySum, entry.valueSum));
                }
                else
                {
                    negative.emplace_back(std::make_pair(entry.keySum, entry.valueSum));
                }
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                ++nErased;
            }
        }
    } while (nErased > 0);

    // If any buckets for one of the hash functions is not empty,
    // then we didn't peel them all:
    for (IBLT::HashTableEntry &entry : this->hashTable)
    {
        if (!entry.empty())
            return false;
    }
    return true;
}

IBLT &IBLT::operator-=(const IBLT &other)
{
    if (valueSize != other.valueSize)
        Logger::error_and_quit("The value sizes between IBLTs don't match! Ours: " + toStr(valueSize) + ". Theirs: " + toStr(other.valueSize));
    if (hashTable.size() != other.hashTable.size())
        Logger::error_and_quit("The IBLT hash table sizes are different! Ours: " + toStr(hashTable.size()) + ". Theirs: " + toStr(other.valueSize));

    for (unsigned long ii = 0; ii < hashTable.size(); ii++)
    {
        IBLT::HashTableEntry &e1 = this->hashTable.at(ii);
        const IBLT::HashTableEntry &e2 = other.hashTable.at(ii);
        e1.count -= e2.count;
        e1.keySum ^= e2.keySum;
        e1.keyCheck ^= e2.keyCheck;
        if (e1.empty())
        {
            e1.valueSum.kill();
        }
        else
        {
            e1.valueSum ^= e2.valueSum;
        }
    }
    return *this;
}

IBLT IBLT::operator-(const IBLT &other) const
{
    IBLT result(*this);
    return result -= other;
}

size_t IBLT::size() const
{
    return hashTable.size();
}

size_t IBLT::eltSize() const
{
    return valueSize;
}

string IBLT::toString() const
{
    string outStr = "";
    for (auto entry : hashTable)
    {
        //cout << to_string(entry.count) << endl;
        //cout << to_string(entry.keyCheck) << endl;
        //cout << zzToStr(entry.keySum) << endl;
        //cout << zzToStr(entry.valueSum) << endl;
        outStr += to_string(entry.count) + "," + to_string(entry.keyCheck) + "," + toStr<ZZ>(entry.keySum) + "," + toStr<ZZ>(entry.valueSum) + "\n";
    }
    return outStr;
}

void IBLT::reBuild(string &inStr)
{
    vector<string> entries = split(inStr, "\n");
    int index = 0;

    for (auto entry : entries)
    {
        vector<string> infos = split(entry, ",");
        HashTableEntry curEntry;
        curEntry.count = stol(infos[0]);
        curEntry.keyCheck = stoul(infos[1]);
        curEntry.keySum = strTo<ZZ>(infos[2]);
        curEntry.valueSum = strTo<ZZ>(infos[3]);
        this->hashTable[index] = curEntry;
        index++;
    }
}

void IBLT::insertIBLT(IBLT &chldIBLT, hash_t &chldHash)
{
    ZZ ibltZZ = strTo<ZZ>(chldIBLT.toString());
    _insert(1, ibltZZ, (ZZ)chldHash);
}

void IBLT::eraseIBLT(IBLT &chldIBLT, hash_t &chldHash)
{
    ZZ ibltZZ = strTo<ZZ>(chldIBLT.toString());
    _insert(-1, ibltZZ, (ZZ)chldHash);
}

void IBLT::insertIBLT(multiset<DataObject *> tarSet, size_t elemSize)
{
    hash_t setHash = _setHash(tarSet);

    // Put chld set into a chld IBLT
    IBLT chldIBLT(tarSet.size(), elemSize);
    for (auto i : tarSet)
    {
        chldIBLT.insert(i->to_ZZ(), i->to_ZZ());
    }

    // Put the pair(chld IBLT, hash of set) into the outer IBLT T
    this->insertIBLT(chldIBLT, setHash);

    hashes.push_back(setHash);
}

void IBLT::eraseIBLT(multiset<DataObject *> tarSet, size_t elemSize)
{
    hash_t setHash = _setHash(tarSet);

    IBLT chldIBLT(tarSet.size(), elemSize);
    for (auto i : tarSet)
    {
        chldIBLT.insert(i->to_ZZ(), i->to_ZZ());
    }
    this->eraseIBLT(chldIBLT, setHash);
    // delete set hash in the vector
    for (auto i = hashes.begin(); i < hashes.end(); i++)
    {
        if (*i == setHash)
        {
            hashes.erase(i);
            break;
        }
    }
}

ZZ IBLT::getHash(multiset<DataObject *> tarSet)
{
    return (ZZ)_setHash(tarSet);
}