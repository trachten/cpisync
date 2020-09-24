/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 *
 * Implementation of Cuckoo filters based on
 * https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf
 */

#include <CPISync/Syncs/Cuckoo.h>

std::random_device Cuckoo::rd;

std::mt19937 Cuckoo::prng(Cuckoo::rd());

Cuckoo::~Cuckoo() = default;

int _default_fingerprint(const ZZ& xx, size_t fngprtSize) {
    return to_int(xx) & ((1 << fngprtSize) - 1);
}

ZZ _default_hash(const ZZ& xx, size_t filterSize) {
    std::hash<string> shash;
    return ZZ(shash(toStr(to_long(xx))) % filterSize);
}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks) :
    filter (Compact2DBitArray(fngprtSize, bucketSize, filterSize)),
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (0),
    fingerprint_impl (_default_fingerprint),
    hash_impl (_default_hash) {}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks, fingerprint_impl_t fingerprintFunction,
               hash_impl_t hashFunction) :
    filter (Compact2DBitArray(fngprtSize, bucketSize, filterSize)),
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (0),
    fingerprint_impl (fingerprintFunction),
    hash_impl (hashFunction) {}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks, vector<unsigned char> f, ZZ itemsCount) :
    filter (Compact2DBitArray(fngprtSize, bucketSize, filterSize, f)),
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (itemsCount),
    fingerprint_impl (_default_fingerprint),
    hash_impl (_default_hash) {}

Cuckoo::Cuckoo(size_t capacity, float err) {
    // TODO: if ready to move to C++14 we can let the compiler
    // calculate log2(2*b) for us. In fact, we can make this whole
    // constructor a constexpr for capacity and error rate being compile
    // time constants (which I think could be useful).

    size_t bestF = MAX_FNGPRT_SIZE, bestB = 2;
    for (size_t b=64; b>=4; b--) {
        // we are looking for a small f but a relatively high b. The
        // smaller b, the smaller f. But the smaller b, the higher
        // failed insertion probability.
        size_t f = narrow_cast<size_t>(ceil(log2(1 / err) + log2(2 * b))); // Eq. 6 from the paper

        // if the smaller b does not give us at least two less bits in
        // f keep the best b at the current value
        if (f < bestF - 2) {
            bestF = f;
            bestB = b;
        }
    }

    if (bestF == MAX_FNGPRT_SIZE)
        throw CuckooFilterError("False positive rate of " + toStr(err) +
                                " is unattainable.");

    fngprtSize = bestF;
    bucketSize = bestB;
    filterSize = narrow_cast<size_t>(ceil(capacity / float(bucketSize)));
    maxKicks = DEFAULT_MAX_KICKS;
    itemsCount = 0;
    filter = Compact2DBitArray(fngprtSize, bucketSize, filterSize);
}

size_t Cuckoo::getFilterSize() const {
    return filterSize;
}

size_t Cuckoo::getBucketSize() const {
    return bucketSize;
}

size_t Cuckoo::getFngprtSize() const {
    return fngprtSize;
}

size_t Cuckoo::getMaxKicks() const {
    return maxKicks;
}

ZZ Cuckoo::getItemsCount() const {
    return itemsCount;
}

vector<unsigned char> Cuckoo::getRawFilter() const {
    return filter.getRaw();
}

void Cuckoo::_restore_filter(stack<Slot>& originalSlots) {
    while (!originalSlots.empty()) {
        Slot s = originalSlots.top();
        originalSlots.pop();
        filter.setEntry(s.b, s.c, s.f);
    }
}

bool Cuckoo::insert(const DataObject& datum) {
    PartialHash p = _pHash(datum);

    try {
        addToBucket(p.i1, p.f);
        itemsCount++;
        return true;
    } catch (overflow_error ignored) {}

    try {
        addToBucket(p.i2, p.f);
        itemsCount++;
        return true;
    } catch (overflow_error ignored) {}

    // Choose bucket to kick from
    size_t chosenBucket = _rand(0, 1) ? p.i2 : p.i1;
    // The fingerprint to put in that bucket
    auto f = p.f;

    stack<Slot> originalSlots;
    for (size_t kick=0; kick<maxKicks; kick++) {
        // Choose the fingerprint from the bucket to relocate
        size_t victimIdx = narrow_cast<size_t>(_rand(0, bucketSize - 1));
        size_t victim = filter.getEntry(chosenBucket, victimIdx);
        size_t altBucket = _alternativeBucket(chosenBucket, victim);

        // Overwrite victim with the fingerprint being inserted. Later
        // on, if insert fails, we will restore the filter.
        filter.setEntry(chosenBucket, victimIdx, f);

        try {
            addToBucket(altBucket, victim);
            itemsCount++;
            return true;
        } catch (overflow_error ignored) {}

        Slot s;
        s.b = chosenBucket;
        s.c = victimIdx;
        s.f = victim;
        originalSlots.push(s);

        f = victim;
        chosenBucket = altBucket;
    }

    // Relocation chain exceeded maxKicks
    _restore_filter(originalSlots);
    return false;
}

bool Cuckoo::lookup(const DataObject& datum) const {
    PartialHash p = _pHash(datum);

    try {
        findFingerprint(p.f, p.i1);
        return true;
    } catch (range_error ignored) {}

    try {
        findFingerprint(p.f, p.i2);
        return true;
    } catch (range_error ignored) {}

    return false;

}

bool Cuckoo::erase(const DataObject& datum) {
    if (itemsCount == 0)
        throw CuckooFilterError("You cannot erase from an empty filter!");

    PartialHash p = _pHash(datum);

    try {
        size_t idx = findFingerprint(p.f, p.i1);
        filter.setEntry(p.i1, idx, 0);
        itemsCount--;
        return true;
    } catch (range_error ignored) {}

    try {
        size_t idx = findFingerprint(p.f, p.i2);
        filter.setEntry(p.i2, idx, 0);
        itemsCount--;
        return true;
    } catch (range_error ignored) {}

    return false;
}

long Cuckoo::_rand(size_t min, size_t max) {
    std::uniform_int_distribution<size_t> dist(min, max);

    return dist(prng);
}

bool Cuckoo::isZeroF(const DataObject& d) const {
    return !fingerprint(d.to_ZZ());
}

void Cuckoo::seedPRNG(unsigned int seed) {
    prng.seed(seed);
}

vector<byte> Cuckoo::toByteVector() const {
    vector<byte> res;

    vector<byte> byteRep = toBytes(fngprtSize);
    res.insert(res.end(), byteRep.begin(), byteRep.end());
    byteRep = toBytes(bucketSize);
    res.insert(res.end(), byteRep.begin(), byteRep.end());
    byteRep = toBytes(filterSize);
    res.insert(res.end(), byteRep.begin(), byteRep.end());
    byteRep = toBytes(maxKicks);
    res.insert(res.end(), byteRep.begin(),byteRep.end());
    byteRep = toBytes(itemsCount);
    res.insert(res.end(), byteRep.begin(), byteRep.end());

    vector<unsigned char> table = getRawFilter();
    for (const auto entry: table) {
        res.push_back(entry);
    }
    return res;
}

void Cuckoo::fromByteVector(vector<byte> buffer) {
    const byte *ptr = buffer.data();

    fngprtSize = fromBytes<size_t>(ptr); ptr += sizeof(fngprtSize);
    bucketSize = fromBytes<size_t>(ptr); ptr += sizeof(bucketSize);
    filterSize = fromBytes<size_t>(ptr); ptr += sizeof(filterSize);
    maxKicks = fromBytes<size_t>(ptr); ptr += sizeof(size_t);
    long bytesRead;
    itemsCount = fromBytesZZ(ptr, bytesRead); ptr += bytesRead;

    size_t offset = ptr - buffer.data();
    vector<unsigned char> table;
    for (size_t i = offset;
         i < buffer.size(); i++) {
        table.push_back(buffer[i]);
    }

    filter = Compact2DBitArray(fngprtSize, bucketSize, filterSize, table);
    fingerprint_impl = _default_fingerprint;
    hash_impl = _default_hash;
}


ostream& operator<<(ostream& os, const Cuckoo cf) {
    os << "Cuckoo Filter [ f=" << cf.fngprtSize << ", b=" << cf.bucketSize
       << ", m=" << cf.filterSize << ", n=" << cf.itemsCount << " ]";

    return os;
}

/**
 * Other private member functions
 */

size_t Cuckoo::fingerprint(const ZZ& e) const {
    return fingerprint_impl(e, fngprtSize);
}

ZZ Cuckoo::hash(const ZZ& e) const {
    return hash_impl(e, filterSize);
}

size_t Cuckoo::addToBucket(size_t bucketIdx, size_t f) {
    for (size_t ii=0; ii<bucketSize; ii++)
        // Put the fingerprint in the first available entry
        if (filter.getEntry(bucketIdx, ii) == 0) {
            filter.setEntry(bucketIdx, ii, f);
            return ii;
        }

    throw overflow_error("Bucket "+toStr(bucketIdx)+" is full.");
}

size_t Cuckoo::findFingerprint(unsigned f, size_t bucket) const {
    for (size_t ii=0; ii < bucketSize; ii++)
        if (filter.getEntry(bucket, ii) == f)
            return ii;

    throw range_error("Could not find fingerprint "+toStr(f)+" in bucket "+toStr(bucket));
}

size_t Cuckoo::_alternativeBucket(size_t currentB, size_t f) const {
    // i1 xor hash(f) can wrap around fitlerSize the same way hash can
    // do it on its own.
    return narrow_cast<size_t>(to_int((currentB ^ hash(to_ZZ(f))) % filterSize));
}

Cuckoo::PartialHash Cuckoo::_pHash(const DataObject& datum) const {
    PartialHash p;

    p.f = fingerprint(datum.to_ZZ());
    p.i1 = narrow_cast<unsigned int>(to_int(hash(datum.to_ZZ())));
    p.i2 = _alternativeBucket(p.i1, p.f);

    if (p.i1 != _alternativeBucket(p.i2, p.f))
        throw CuckooFilterError("This hash algorithm is not suitable for cuckoo hashing.");

    return p;
}
