//
// Created by Shubham Arora on 7/20/20.
//

#include <CPISync/Syncs/IBLTMultiset.h>


IBLTMultiset::IBLTMultiset()
: valueSize(0){
}

IBLTMultiset::IBLTMultiset(size_t expectedNumEntries, size_t _valueSize)
        : valueSize(_valueSize) {
    // atleast 2x expectedNumEntries
    size_t nEntries = expectedNumEntries * 2 ;
    // ... make nEntries exactly divisible by N_HASH
    while (N_HASH * (nEntries/N_HASH) != nEntries) ++nEntries;
    hashTable.resize(nEntries);
}

// perform modular subtraction
hash_t subModHash(hash_t x, hash_t y ) {
    long res = (long(x%LARGE_PRIME) - long(y%LARGE_PRIME)) % LARGE_PRIME;
    // must return positive modulus
    // C++ by default does not give positive modulus
    if (res < 0)
        res += LARGE_PRIME;

    return res;
}

// perform modular addition
hash_t addModHash(hash_t x, hash_t y ) {
    long res = (long(x%LARGE_PRIME) + long(y%LARGE_PRIME)) % LARGE_PRIME;
    // must return positive modulus
    // C++ by default does not give positive modulus
    if (res < 0)
        res += LARGE_PRIME;

    return res;
}

void IBLTMultiset::_insertModular(long plusOrMinus, const ZZ &key, const ZZ& value) {
    long bucketsPerHash = hashTable.size() / N_HASH;

    if(sizeof(value) != valueSize)
        Logger::error_and_quit("The value being inserted is different than the IBLT value size! value size: "
                               + toStr(sizeof(value)) + ". IBLT value size: " + toStr(valueSize));

    for(int ii=0; ii < N_HASH; ii++){
        hash_t hk = _hashK(key, ii);
        long startEntry = ii * bucketsPerHash;
        long pos = startEntry + (hk%bucketsPerHash);
        IBLTMultiset::HashTableEntry& entry = hashTable.at(startEntry + (hk%bucketsPerHash));
        hash_t modHashCheck = _hashK(key, N_HASHCHECK) % LARGE_PRIME;

        entry.count += plusOrMinus;
        entry.keySum += plusOrMinus*key;
        if (plusOrMinus == 1) {
            entry.keyCheck = addModHash(entry.keyCheck, modHashCheck);
        } else if (plusOrMinus == -1) {
            entry.keyCheck = subModHash(entry.keyCheck, modHashCheck);
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
        const IBLTMultiset::HashTableEntry& entry = hashTable[startEntry + (hk%bucketsPerHash)];

        if (entry.empty()) {
            // Definitely not in table. Leave
            // result empty, return true.
            return true;
        } else if(entry.isPure()) {
            result = entry.valueSum / entry.count;
            return true;
        }
    }

    // Don't know if k is in table or not; "peel" the IBLT to try to find it:
    long nErased;
    do {
        nErased = 0;
        for (IBLTMultiset::HashTableEntry &entry : this->hashTable) {
            if (entry.isPure()) {
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

bool IBLTMultiset::HashTableEntry::isPure() const {
    if (count != 0 && keySum!=0) {
        long absCount = abs(count);
        long plusOrMinus = conv<long>(keySum / abs(keySum));
        hash_t singleCountHash = _hashK(keySum / count, N_HASHCHECK) % LARGE_PRIME;
        long check = 0;
        int ii = 0;
        while (ii < absCount) {
            if(plusOrMinus == 1)
                check = addModHash(check, singleCountHash);
            else
                check = subModHash(check, singleCountHash);

            ii++;
        }
        return check == keyCheck;
    }
    return false;
}

bool IBLTMultiset::listEntries(vector<pair<ZZ, ZZ>> &positive, vector<pair<ZZ, ZZ>> &negative){
    long nErased;
    do {
        nErased = 0;
        for(IBLTMultiset::HashTableEntry& entry : this->hashTable) {
            if (entry.isPure()) {
                if (entry.count >= 1) {
                    positive.emplace_back(std::make_pair(entry.keySum / entry.count, entry.valueSum / entry.count));
                } else if (entry.count <= -1) {
                    negative.emplace_back(std::make_pair(entry.keySum / entry.count, entry.valueSum / entry.count));
                } else {
                    Logger::error_and_quit("Unreachable state. Entry with count zero in IBLT.");
                    return false;
                }
                this->_insertModular(-entry.count / abs(entry.count), entry.keySum / entry.count, entry.valueSum / entry.count);

                ++nErased;
            }
        }
    } while (nErased > 0);

    // If any buckets for one of the hash functions is not empty,
    // then we didn't peel them all:
    for (IBLTMultiset::HashTableEntry& entry : this->hashTable) {
        if (!entry.empty())
            return false;
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
        e1.keyCheck = subModHash(e1.keyCheck, e2.keyCheck);

        if (e1.empty())
            e1.valueSum.kill();
        else
            e1.valueSum -= e2.valueSum;

    }
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

vector<byte> IBLTMultiset::toByteVector() const {
    vector<byte> res;
    for (auto entry: hashTable) {
        vector<byte> byteRep = toBytes(entry.count);
        res.insert(res.end(), byteRep.begin(), byteRep.end());
        byteRep = toBytes(entry.keyCheck);
        res.insert(res.end(), byteRep.begin(), byteRep.end());
        byteRep = toBytes(entry.keySum);
        res.insert(res.end(), byteRep.begin(), byteRep.end());
        byteRep = toBytes(entry.valueSum);
        res.insert(res.end(), byteRep.begin(), byteRep.end());
    }
    return res;
}

void IBLTMultiset::fromByteVector(vector<byte> data) {
    vector<byte> res;
    byte *buf = data.data();
    for (long index = 0; index < hashTable.size(); index++) {
        hashTable[index].count = fromBytes<long>(buf);
        buf += sizeof(long);
        hashTable[index].keyCheck = fromBytes<hash_t>(buf);
        buf += sizeof(long);
        long bytesRead;
        hashTable[index].keySum = fromBytesZZ(buf, bytesRead);
        buf += bytesRead;
        hashTable[index].valueSum = fromBytesZZ(buf, bytesRead);
        buf += bytesRead;
    }
    Logger::gLog(Logger::METHOD_DETAILS, "IBLTMultiset fromByteVectorComplete");
}

