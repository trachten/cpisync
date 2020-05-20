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

Cuckoo::Cuckoo() {};

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
        size_t f = ceil(log2(1/err) + log2(2*b)); // Eq. 6 from the
                                                  // paper

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
    filterSize = ceil(capacity / float(bucketSize));
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

void Cuckoo::_commit_relocation_chain(stack<Reloc>& relocStack) {
    while (!relocStack.empty()) {
        Reloc r = relocStack.top();
        relocStack.pop();
        filter.setEntry(r.b, r.c, r.f);
    }
}

bool Cuckoo::insert(const DataObject& datum) {
    PartialHash p = _pHash(datum);

    if (addToBucket(p.i1, p.f) != -1) {
        itemsCount++;
        return true;
    }

    if (addToBucket(p.i2, p.f) != -1) {
        itemsCount++;
        return true;
    }

    // Choose bucket to kick from
    int chosenBucket = _rand(0, 1) ? p.i2 : p.i1;
    // The fingerprint to put in that bucket
    int f = p.f;

    stack<Reloc> relocStack;
    for (size_t kick=0; kick<maxKicks; kick++) {
        // Choose the fingerprint from the bucket to relocate
        int victimIdx = _rand(0, bucketSize - 1);
        int victim = filter.getEntry(chosenBucket, victimIdx);
        int altBucket = _alternativeBucket(chosenBucket, victim);

        Reloc r;
        r.b = chosenBucket;
        r.c = victimIdx;
        r.f = f;
        relocStack.push(r);

        if (addToBucket(altBucket, victim) != -1) {
            _commit_relocation_chain(relocStack);
            itemsCount++;
            return true;
        }

        f = victim;
        chosenBucket = altBucket;
    }

    // Relocation chain exceeded maxKicks
    return false;
}

bool Cuckoo::lookup(const DataObject& datum) const {
    PartialHash p = _pHash(datum);

    if ((hasF(p.f, p.i1) != -1) || (hasF(p.f, p.i2) != -1))
        return true;

    return false;
}

bool Cuckoo::erase(const DataObject& datum) {
    if (itemsCount == 0)
        throw CuckooFilterError("You cannot erase from an empty filter!");

    PartialHash p = _pHash(datum);

    int idx = hasF(p.f, p.i1);
    if (idx != -1) {
        filter.setEntry(p.i1, idx, 0);
        itemsCount--;
        return true;
    } else {
        idx = hasF(p.f, p.i2);
        if (idx != -1) {
            filter.setEntry(p.i2, idx, 0);
            itemsCount--;
            return true;
        }
    }

    return false;
}

long Cuckoo::_rand(size_t min, size_t max) {
    std::uniform_int_distribution<> dist(min, max);

    return dist(prng);
}

bool Cuckoo::isZeroF(const DataObject& d) const {
    return !fingerprint(d.to_ZZ());
}

void Cuckoo::seedPRNG(int seed) {
    prng.seed(seed);
}

ostream& operator<<(ostream& os, const Cuckoo cf) {
    os << "Cuckoo Filter [ f=" << cf.fngprtSize << ", b=" << cf.bucketSize
       << ", m=" << cf.filterSize << ", n=" << cf.itemsCount << " ]";

    return os;
}

/**
 * Other private member functions
 */

int Cuckoo::fingerprint(const ZZ& e) const {
    return fingerprint_impl(e, fngprtSize);
}

ZZ Cuckoo::hash(const ZZ& e) const {
    return hash_impl(e, filterSize);
}

int Cuckoo::addToBucket(size_t bucketIdx, unsigned f) {
    for (size_t ii=0; ii<bucketSize; ii++)
        // Put the fingerprint in the first available entry
        if (filter.getEntry(bucketIdx, ii) == 0) {
            filter.setEntry(bucketIdx, ii, f);
            return ii;
        }

    return -1;
}

int Cuckoo::hasF(unsigned f, size_t bucket) const {
    for (size_t ii=0; ii < bucketSize; ii++)
        if (filter.getEntry(bucket, ii) == f)
            return ii;

    return -1;
}

int Cuckoo::_alternativeBucket(int currentB, int f) const {
    // i1 xor hash(f) can wrap around fitlerSize the same way hash can
    // do it on its own.
    return to_int((currentB ^ hash(to_ZZ(f))) % filterSize);
}

Cuckoo::PartialHash Cuckoo::_pHash(const DataObject& datum) const {
    PartialHash p;

    p.f = fingerprint(datum.to_ZZ());
    p.i1 = to_int(hash(datum.to_ZZ()));
    p.i2 = _alternativeBucket(p.i1, p.f);

    return p;
}
