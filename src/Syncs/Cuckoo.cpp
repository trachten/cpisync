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

/**
 * To discern endianness in runtime but only once when a cuckoo
 * constructor is called.
 */
std::once_flag onceEndiannessFlag;
static bool littleEndian = true;

void _discern_endianness() {
    // This assumes size(int) > size(char). I can imagine a tiny
    // embedded device where this is not the case.
    unsigned int x = 1;
    if (!(*(char *) &x == 1))
        littleEndian = false;
}

void Cuckoo::_constructorGuards() const {
    std::call_once(onceEndiannessFlag, _discern_endianness);

    if (fngprtSize > 32 || fngprtSize < 1)
        throw CuckooFilterError("Fingerprint has to be between 1 and 32 bits!");
}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks) :
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (0),
    fngprtSizeB (ceil(fngprtSize / float(BYTE))),
    fingerprint_impl (_default_fingerprint),
    hash_impl (_default_hash)
{
    _constructorGuards();
    filter.resize(ceil((fngprtSize * bucketSize * filterSize) / float(BYTE)));
}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks, fingerprint_impl_t fingerprintFunction,
               hash_impl_t hashFunction) :
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (0),
    fngprtSizeB (ceil(fngprtSize / float(BYTE))),
    fingerprint_impl (fingerprintFunction),
    hash_impl (hashFunction)
{
    _constructorGuards();
    filter.resize(ceil((fngprtSize * bucketSize * filterSize) / float(BYTE)));
}

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t filterSize,
               size_t maxKicks, vector<unsigned char> f, ZZ itemsCount) :
    filter (f),
    filterSize (filterSize),
    bucketSize (bucketSize),
    fngprtSize (fngprtSize),
    maxKicks (maxKicks),
    itemsCount (itemsCount),
    fngprtSizeB (ceil(fngprtSize / float(BYTE))),
    fingerprint_impl (_default_fingerprint),
    hash_impl (_default_hash)
{
    _constructorGuards();
}

Cuckoo::Cuckoo(size_t capacity, float err) {
    // TODO: if ready to move to C++14 we can let the compiler
    // calculate log2(2*b) for us. In fact, we can make this whole
    // constructor a constexpr for capacity and error rate being compile
    // time constants (which I think could be useful).

    size_t bestF = 32, bestB = 2;
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

    if (bestF == 32)
        throw CuckooFilterError("False positive rate of " + toStr(err) +
                                " is unattainable.");

    fngprtSize = bestF;
    bucketSize = bestB;
    filterSize = ceil(capacity / float(bucketSize));
    maxKicks = 300; // provisional
    itemsCount = 0;
    fngprtSizeB = ceil(fngprtSize / float(BYTE));

    _constructorGuards();
    filter.resize(ceil((fngprtSize * bucketSize * filterSize) / float(BYTE)));
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
    return filter;
}

void Cuckoo::setBucketSize(size_t s) {
    bucketSize = s;
}

void Cuckoo::setFngprtSize(size_t s) {
    fngprtSize = s;
}

void Cuckoo::setMaxKicks(size_t mKicks) {
    maxKicks = mKicks;
}

bool Cuckoo::_insert(unsigned f, int bucket, size_t kicks) {
    if (kicks >= maxKicks)
        return false;

    if (addToBucket(bucket, f) != -1)
        return true;

    int victimIdx = _rand(0, bucketSize - 1);
    int victim = getEntry(bucket, victimIdx);
    int altBucket = _alternativeBucket(bucket, victim);
    if (_insert(victim, altBucket, kicks + 1)) {
        setEntry(bucket, victimIdx, f);
        return true;
    }

    return false;
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

    // Choose the fingerprint from the bucket to relocate
    int fstVictimIdx = _rand(0, bucketSize - 1);
    int fstVictim = getEntry(chosenBucket, fstVictimIdx);
    int altBucket = _alternativeBucket(chosenBucket, fstVictim);

    // Recursively relocate the victim fingerprint
    if (_insert(to_int(fstVictim), altBucket, 0)) {
        // Victim is relocated, put f to its place
        setEntry(chosenBucket, fstVictimIdx, p.f);
        itemsCount++;
        return true;
    }

    // Victim failed to relocate
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
        setEntry(p.i1, idx, 0);
        itemsCount--;
        return true;
    } else {
        idx = hasF(p.f, p.i2);
        if (idx != -1) {
            setEntry(p.i2, idx, 0);
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
    return fingerprint(d.to_ZZ());
}

Cuckoo::GetSetPrelim Cuckoo::_getSetPrelim(size_t bucketIdx,
                                           size_t entryIdx) const {
    GetSetPrelim p;

    p.entryBits = fngprtSize * bucketSize * bucketIdx
        + (fngprtSize * entryIdx);
    p.fstByte = p.entryBits / BYTE;
    p.lstByte = (p.entryBits + fngprtSize - 1) / BYTE;
    p.onsetBits = p.entryBits % BYTE;
    p.offsetBits = (p.entryBits + fngprtSize) % BYTE;

    return p;
}

void Cuckoo::_assertIdx(size_t bucketIdx, size_t entryIdx) const {
    if (bucketIdx >= filterSize)
        throw CuckooFilterError("No bucketIdx " + to_string(bucketIdx)
                                + ". filterSize is " + to_string(filterSize)
                                + " buckets long.");
    if (entryIdx >= bucketSize)
        throw CuckooFilterError("No entryIdx " + to_string(entryIdx)
                                + ". bucketSize is " + to_string(bucketSize)
                                + " entries long.");
}

unsigned Cuckoo::getEntry(size_t bucketIdx, size_t entryIdx) const {
    _assertIdx(bucketIdx, entryIdx);
    GetSetPrelim p = _getSetPrelim(bucketIdx, entryIdx);

    if (p.lstByte > p.fstByte) { // Entry in multiple bytes
        unsigned char fstP = filter[p.fstByte] & ((1 << (BYTE - p.onsetBits))
                                                  - 1);
        // % BYTE to accommodate for lstP being nicely aligned to the
        // end of its byte
        unsigned char lstP = filter[p.lstByte] >> ((BYTE - p.offsetBits) % BYTE);

        unsigned entry = fstP;
        for (size_t ii=1; ii<(p.lstByte - p.fstByte); ii++) {
            entry <<= BYTE;
            entry |= filter[p.fstByte + ii];
        }
        // In case of lstByte > fstByte, when offsetBits is 0 then
        // lstP is nicely alligned to the end of its byte. Then, we
        // shift fstP for an entire byte.
        entry <<= (p.offsetBits ? p.offsetBits : BYTE);
        entry |= lstP;

        return entry;
    }

    // Entry spreads only single byte
    return (filter[p.fstByte] >> (BYTE - p.onsetBits - fngprtSize))
        & ((1 << fngprtSize) - 1);
}

void Cuckoo::setEntry(size_t bucketIdx, size_t entryIdx, unsigned f) {
    _assertIdx(bucketIdx, entryIdx);
    GetSetPrelim p = _getSetPrelim(bucketIdx, entryIdx);

    unsigned char* fngprtC = (unsigned char*)&f;
    vector<unsigned char> fBytes; // MS byte first

    if (littleEndian)
        for (int ii=fngprtSizeB; ii >= 0; ii--)
            fBytes.push_back(fngprtC[ii]);
    else
        for (int ii=0; ii<=fngprtSizeB; ii++)
            fBytes.push_back(fngprtC[ii]);

    size_t diff = p.lstByte - p.fstByte;
    unsigned char fstP, lstP, toWrite, onsetP;
    size_t ii, cBits;
    switch (diff) {
    case 0: // Entry starts and ends in the same byte
        // extract the MS bits of the byte that are not the part of
        // current entry and bring it to LS end.
        fstP = filter[p.fstByte] >> (BYTE - p.onsetBits);
        // extract the LS bits of the byte that are not the part of
        // current entry. Keep them in LS positions.
        lstP = filter[p.fstByte] & ((1 << (BYTE - p.onsetBits - fngprtSize))
                                    - 1);

        // bring fstP back to MS bits
        toWrite = fstP << (BYTE - p.onsetBits);
        // move bits of fingerprint to the positions that they will
        // occupy in the current byte and append it to what we are
        // about to write
        toWrite |= *fngprtC << (BYTE - p.onsetBits - fngprtSize);
        // append the last part
        toWrite |= lstP;
        filter[p.fstByte] = toWrite;
        break;

    case 1: // Entry spreads two consequent bytes
    case 2: // ... or three
    case 3: // ... or four
    case 4: // ... or five
        /* Write fstByte */
        cBits = 0; // Bits of f consumed as of now
        toWrite = _getNextFByte(fBytes, cBits);
        // take BYTE - onsetBits MS bits, put them as LS bits of new byte
        toWrite >>= p.onsetBits;
        // construct the onset part from what's already there
        onsetP = filter[p.fstByte] >> (BYTE - p.onsetBits);
        // ... put that as MS bits of new byte
        onsetP <<= (BYTE - p.onsetBits);
        // join the two parts
        toWrite |= onsetP;
        filter[p.fstByte] = toWrite;
        // we just consumed (wrote to filter) this much bits of the
        // fingerprint that we are writing
        cBits += BYTE - p.onsetBits;

        /* Write middle complete bytes */
        for (ii=1; ii<diff; ii++) {
            filter[p.fstByte + ii] = _getNextFByte(fBytes, cBits);
            // we just consumed a complete byte of the fingerprint
            // that we are writing
            cBits += BYTE;
        }

        /* Write lstByte */
        toWrite = fBytes.back() << (BYTE - (fngprtSize - cBits));
        // Pull the rest LSBits from the filter (keep them unchanged)
        if (p.offsetBits)
            // Otherwise lstByte is nicely aligned with its byte. The
            // new lstByte is exactly what we built as of now. No old
            // lstByte part needed to be restored.
            toWrite |= filter[p.lstByte] & ((1 << (BYTE - p.offsetBits)) - 1);

        filter[p.lstByte] = toWrite;
        break;

    default:
        throw CuckooFilterError("Fingerprint cannot spread more than 5 bytes!");
    }
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
        if (getEntry(bucketIdx, ii) == 0) {
            setEntry(bucketIdx, ii, f);
            return ii;
        }

    return -1;
}

int Cuckoo::hasF(unsigned f, size_t bucket) const {
    for (size_t ii=0; ii < bucketSize; ii++)
        if (getEntry(bucket, ii) == f)
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

unsigned char Cuckoo::_getNextFByte(const vector<unsigned char>& f,
                                    size_t cons) const {
    int r = fngprtSize - cons; // remained bits
    if (r <= 0)
        throw CuckooFilterError("Fingerprint has no more bits.");

    size_t left = r % BYTE; // bits in the MSByte of what remained in
                            // f by now (or 0 when 8 bits remained)
    if (r < BYTE)
        return f.front() << (BYTE - r); // align remaining bits to MS bit

    size_t msByteIdx = f.size() - (r / BYTE); // index of MS byte
    if (left)
        msByteIdx--;
    unsigned char ret = f.at(msByteIdx);

    if (!left) // nicely byte-aligned
        return ret;

    // Not byte-aligned
    ret <<= BYTE - left;
    ret |= f.at(msByteIdx + 1) >> left;
    return ret;
}
