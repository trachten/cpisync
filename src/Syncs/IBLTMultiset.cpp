//
// Created by shubham on 7/20/20.
//

#include <CPISync/Syncs/IBLTMultiset.h>


IBLTMultiset::IBLTMultiset(size_t expectedNumEntries, size_t _valueSize)
        : IBLT(expectedNumEntries, _valueSize) {
    // just calls the parent constructor
}

// perform modular subtraction
hash_t _subModHash(hash_t x, hash_t y ) {
    long res = (long(x%LARGE_PRIME) - long(y%LARGE_PRIME)) % LARGE_PRIME;
    // must return positive modulus
    // C++ by default does not give positive modulus
    if (res < 0 ){
        res += LARGE_PRIME;
    }
    return res;
}

// perform modular addition
hash_t _addModHash(hash_t x, hash_t y ) {
    long res = (long(x%LARGE_PRIME) + long(y%LARGE_PRIME)) % LARGE_PRIME;
    // must return positive modulus
    // C++ by default does not give positive modulus
    if (res < 0 ){
        res += LARGE_PRIME;
    }
    return res;
}

void IBLTMultiset::_insertModular(long plusOrMinus, ZZ key, ZZ value) {
//    cout << "insert modular called with key: " << key << endl;
    long bucketsPerHash = hashTable.size() / N_HASH;

    if(sizeof(value) != valueSize) {
        Logger::error_and_quit("The value being inserted is different than the IBLT value size! value size: "
                               + toStr(sizeof(value)) + ". IBLT value size: " + toStr(valueSize));
    }

    for(int ii=0; ii < N_HASH; ii++){
        hash_t hk = _hashK(key, ii);
        long startEntry = ii * bucketsPerHash;
        IBLT::HashTableEntry& entry = hashTable.at(startEntry + (hk%bucketsPerHash));
        hash_t modHashCheck = _hashK(key, N_HASHCHECK) % LARGE_PRIME;

        entry.count += plusOrMinus;
        entry.keySum += plusOrMinus*key;
        if (plusOrMinus == 1) {
            entry.keyCheck = _addModHash(entry.keyCheck, modHashCheck);
        } else if (plusOrMinus == -1) {
            entry.keyCheck = _subModHash(entry.keyCheck, modHashCheck);
        }
        if (entry.empty()) {
            entry.valueSum.kill();
        }
        else {
            entry.valueSum += plusOrMinus * value;
        }
    }
}

void IBLTMultiset::insert(ZZ key, ZZ value) {
    _insertModular(1, key, value);
}

void IBLTMultiset::erase(ZZ key, ZZ value) {
    _insertModular(-1, key, value);
}


bool IBLTMultiset::get(ZZ key, ZZ& result){
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
        else if (entry.isPureModular()) {
            result = entry.valueSum / entry.count;
            return true;
        } else if(entry.isMultiPure()) {
            result = entry.valueSum / entry.count;
            return true;
        }
    }

    // Don't know if k is in table or not; "peel" the IBLT to try to find it:
    long nErased;
    do {
        nErased = 0;
        for (IBLT::HashTableEntry &entry : this->hashTable) {
            if (entry.isPureModular()) {
                if (entry.count == 1 && entry.keySum == key) {
                    result = entry.valueSum;
                    return true;
                } else if (entry.count == -1 && entry.keySum == -key) {
                    result = -entry.valueSum;
                    return true;
                }
                if (entry.count == 1) {
                    this->_insertModular(-entry.count, entry.keySum, entry.valueSum);
                }
                else {
                    this->_insertModular(-entry.count, -entry.keySum, -entry.valueSum);
                }
                nErased++;
            } else if (entry.isMultiPure()) {
                if ( entry.keySum/entry.count == key) {
                    result = entry.valueSum/entry.count;
                    return true;
                }
                this->_insertModular(-entry.count / abs(entry.count), entry.keySum / entry.count, entry.valueSum / entry.count);
                ++nErased;
            }
        }
    } while (nErased > 0);

    return false;
}

//bool IBLTMultiset::HashTableEntry::isPureModular() const
//{
//    if ((count == 1 || count == -1) && keySum!=0) {
//        long plusOrMinus;
//        NTL::conv(plusOrMinus, keySum / abs(keySum));
//        hash_t check = _hashK(keySum*plusOrMinus, N_HASHCHECK);
//        hash_t modHash;
//        if (plusOrMinus == 1) {
//            modHash = _addModHash(0, check);
//        } else {
//            modHash = _subModHash(0, check);
//        }
//        return (keyCheck == modHash);
//    }
//    return false;
//}
//
//bool IBLTMultiset::HashTableEntry::isMultiPure() const {
//    if (count != 0 && keySum!=0) {
//        long absCount = abs(count);
//        long plusOrMinus;
//        NTL::conv(plusOrMinus, keySum / abs(keySum));
//        hash_t singleCountHash = _hashK(keySum / count, N_HASHCHECK) % LARGE_PRIME;
//        long check = 0;
//        int ii = 0;
//        while (ii < absCount) {
//            if(plusOrMinus == 1) {
//                check = _addModHash(check, singleCountHash) ;
//            } else {
//                check = _subModHash(check, singleCountHash) ;
//            }
//            ii++;
//        }
//        return check == keyCheck;
//    }
//    return false;
//}

bool IBLTMultiset::listEntries(vector<pair<ZZ, ZZ>> &positive, vector<pair<ZZ, ZZ>> &negative){
    Logger::gLog(Logger::TEST, "IBLT list modular start");
    long nErased;
    do {
        nErased = 0;
        for(IBLT::HashTableEntry& entry : this->hashTable) {
//            long kSum = 0, vSum = 0, kCheck, count;
//            NTL::conv(kSum, entry.keySum);
//            NTL::conv(vSum, entry.valueSum);
//            kCheck = entry.keyCheck;
//            count = entry.count;

            if (entry.isPureModular()) {
                if (entry.count == 1) {
                    positive.emplace_back(std::make_pair(entry.keySum, entry.valueSum));
//                    Logger::gLog(Logger::TEST,
//                                 "list modular erase: key: " + toStr(entry.keySum) + "value: " + toStr(entry.valueSum));
                    this->_insertModular(-entry.count, entry.keySum, entry.valueSum);
                }
                else {
                    negative.emplace_back(std::make_pair(-entry.keySum, -entry.valueSum));
//                    Logger::gLog(Logger::TEST,
//                                 "2list modular erase: key: " + toStr(entry.keySum) + "value: " + toStr(entry.valueSum));
                    this->_insertModular(-entry.count, -entry.keySum, -entry.valueSum);
                }
                ++nErased;
            }
            else if (entry.isMultiPure()) {
                if (entry.count > 1) {
                    positive.emplace_back(std::make_pair(entry.keySum / entry.count, entry.valueSum / entry.count));
                } else if (entry.count < -1) {
                    negative.emplace_back(std::make_pair(entry.keySum / entry.count, entry.valueSum / entry.count));
                } else {
                    Logger::error_and_quit("Unreachable state. Entry with count zero in IBLT.");
                    return false;
                }
//                Logger::gLog(Logger::TEST,
//                             "3list modular erase: key: " + toStr(entry.keySum) + "value: " + toStr(entry.valueSum));
                this->_insertModular(-entry.count / abs(entry.count), entry.keySum / entry.count, entry.valueSum / entry.count);
                ++nErased;
            }
        }
//        Logger::gLog(Logger::TEST, "list modular erased something");
    } while (nErased > 0);

    // If any buckets for one of the hash functions is not empty,
    // then we didn't peel them all:
    for (IBLT::HashTableEntry& entry : this->hashTable) {
        if (!entry.empty()) {
            cout << "so bucket not empty\n";
            return false;
        }
    }
    return true;
}

IBLTMultiset &IBLTMultiset::operator-=(const IBLTMultiset &other) {
    if (valueSize != other.valueSize)
        Logger::error_and_quit("The value sizes between IBLTs don't match! Ours: "
                               + toStr(valueSize) + ". Theirs: " + toStr(other.valueSize));
    if (hashTable.size() != other.hashTable.size())
        Logger::error_and_quit("The IBLT hash table sizes are different! Ours: "
                               + toStr(hashTable.size()) + ". Theirs: " + toStr(other.valueSize));

    for (unsigned long ii = 0; ii < hashTable.size(); ii++) {
        IBLTMultiset::HashTableEntry &e1 = this->hashTable.at(ii);
        const IBLTMultiset::HashTableEntry &e2 = other.hashTable.at(ii);

        e1.count -= e2.count;
        e1.keySum -= e2.keySum;
        e1.keyCheck = _subModHash(e1.keyCheck, e2.keyCheck);
        if (e1.empty()) {
            e1.valueSum.kill();
        } else {
            e1.valueSum -= e2.valueSum;
        }

    }
    Logger::gLog(Logger::TEST, "IBLT subtract complete");
    return *this;
}

IBLTMultiset IBLTMultiset::operator-(const IBLTMultiset& other) const {
    IBLTMultiset result(*this);
    result-=other;
    return result;
}

size_t IBLTMultiset::size() const {
    return hashTable.size();
}

size_t IBLTMultiset::eltSize() const {
    return valueSize;
}

string IBLTMultiset::toString() const
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

