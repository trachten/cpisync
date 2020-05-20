/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on April, 2020.
 */

#include <CPISync/Syncs/Compact2DBitArray.h>

Compact2DBitArray::Compact2DBitArray() {}

Compact2DBitArray::~Compact2DBitArray() = default;

/**
 * To discern endianness in runtime but only once when the constructor
 * is called.
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

void Compact2DBitArray::_constructorGuards() const {
    if (!(fSize > 0 && fSize <= MAX_F_BITS))
        throw Compact2DBitArrayError("Fingerprint has to be between 1 and 32 bits!");
    if (!(bSize > 0))
        throw Compact2DBitArrayError("Bucket (column) count "
                                     "has to be at least 1!");
    if (!(nBuckets > 0))
        throw Compact2DBitArrayError("Number of buckets (rows) count "
                                     "has to be at least 1!");
}

Compact2DBitArray::Compact2DBitArray(size_t fingerprintSize, size_t bucketSize,
                                     size_t NumOfBuckets) :
    fSize (fingerprintSize),
    fSizeB (ceil(fSize / float(BYTE))),
    bSize (bucketSize),
    nBuckets (NumOfBuckets)
{
    _constructorGuards();
    std::call_once(onceEndiannessFlag, _discern_endianness);
    store.resize(ceil((fSize * bSize * nBuckets) / float(BYTE)));
}

Compact2DBitArray::Compact2DBitArray(size_t fingerprintSize, size_t bucketSize,
                                     size_t NumOfBuckets, vector<unsigned char> f) :
    store (f),
    fSize (fingerprintSize),
    fSizeB (ceil(fSize / float(BYTE))),
    bSize (bucketSize),
    nBuckets (NumOfBuckets)
{
    _constructorGuards();
    std::call_once(onceEndiannessFlag, _discern_endianness);
}

size_t Compact2DBitArray::getF() const {
    return fSize;
}

size_t Compact2DBitArray::getColumns() const {
    return bSize;
}

size_t Compact2DBitArray::getRows() const {
    return nBuckets;
}

Compact2DBitArray::GetSetPrelim
Compact2DBitArray::_getSetPrelim(size_t bucketIdx, size_t entryIdx) const {
    GetSetPrelim p;

    p.entryBits = fSize * bSize * bucketIdx
        + (fSize * entryIdx);
    p.fstByte = p.entryBits / BYTE;
    p.lstByte = (p.entryBits + fSize - 1) / BYTE;
    p.onsetBits = p.entryBits % BYTE;
    p.offsetBits = (p.entryBits + fSize) % BYTE;

    return p;
}

void Compact2DBitArray::_assertIdx(size_t bucketIdx, size_t entryIdx) const {
    if (bucketIdx >= nBuckets)
        throw Compact2DBitArrayError("No bucketIdx " + to_string(bucketIdx)
                                     + ". filterSize is " + to_string(nBuckets)
                                     + " buckets long.");
    if (entryIdx >= bSize)
        throw Compact2DBitArrayError("No entryIdx " + to_string(entryIdx)
                                     + ". bucketSize is " + to_string(bSize)
                                     + " entries long.");
}

unsigned Compact2DBitArray::getEntry(size_t bucketIdx, size_t entryIdx) const {
    _assertIdx(bucketIdx, entryIdx);
    GetSetPrelim p = _getSetPrelim(bucketIdx, entryIdx);

    if (p.lstByte > p.fstByte) { // Entry in multiple bytes
        unsigned char fstP = store[p.fstByte] & ((1 << (BYTE - p.onsetBits))
                                                 - 1);
        // % BYTE to accommodate for lstP being nicely aligned to the
        // end of its byte
        unsigned char lstP = store[p.lstByte] >> ((BYTE - p.offsetBits) % BYTE);

        unsigned entry = fstP;
        for (size_t ii=1; ii<(p.lstByte - p.fstByte); ii++) {
            entry <<= BYTE;
            entry |= store[p.fstByte + ii];
        }
        // In case of lstByte > fstByte, when offsetBits is 0 then
        // lstP is nicely alligned to the end of its byte. Then, we
        // shift fstP for an entire byte.
        entry <<= (p.offsetBits ? p.offsetBits : BYTE);
        entry |= lstP;

        return entry;
    }

    // Entry spreads only single byte
    return (store[p.fstByte] >> (BYTE - p.onsetBits - fSize))
        & ((1 << fSize) - 1);
}

void Compact2DBitArray::setEntry(size_t bucketIdx, size_t entryIdx, unsigned f) {
    _assertIdx(bucketIdx, entryIdx);
    GetSetPrelim p = _getSetPrelim(bucketIdx, entryIdx);

    unsigned char* fngprtC = (unsigned char*)&f;
    vector<unsigned char> fBytes; // MS byte first

    if (littleEndian)
        for (int ii=(fSizeB - 1); ii >= 0; ii--)
            fBytes.push_back(fngprtC[ii]);
    else
        for (int ii=0; ii<=(fSizeB - 1); ii++)
            fBytes.push_back(fngprtC[ii]);

    size_t diff = p.lstByte - p.fstByte;
    unsigned char fstP, lstP, toWrite, onsetP;
    size_t ii, cBits;
    switch (diff) {
    case 0: // Entry starts and ends in the same byte
        // extract the MS bits of the byte that are not the part of
        // current entry and bring it to LS end.
        fstP = store[p.fstByte] >> (BYTE - p.onsetBits);
        // extract the LS bits of the byte that are not the part of
        // current entry. Keep them in LS positions.
        lstP = store[p.fstByte] & ((1 << (BYTE - p.onsetBits - fSize)) - 1);

        // bring fstP back to MS bits
        toWrite = fstP << (BYTE - p.onsetBits);
        // move bits of fingerprint to the positions that they will
        // occupy in the current byte and append it to what we are
        // about to write
        toWrite |= *fngprtC << (BYTE - p.onsetBits - fSize);
        // append the last part
        toWrite |= lstP;
        store[p.fstByte] = toWrite;
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
        onsetP = store[p.fstByte] >> (BYTE - p.onsetBits);
        // ... put that as MS bits of new byte
        onsetP <<= (BYTE - p.onsetBits);
        // join the two parts
        toWrite |= onsetP;
        store[p.fstByte] = toWrite;
        // we just consumed (wrote to filter) this much bits of the
        // fingerprint that we are writing
        cBits += BYTE - p.onsetBits;

        /* Write middle complete bytes */
        for (ii=1; ii<diff; ii++) {
            store[p.fstByte + ii] = _getNextFByte(fBytes, cBits);
            // we just consumed a complete byte of the fingerprint
            // that we are writing
            cBits += BYTE;
        }

        /* Write lstByte */
        toWrite = fBytes.back() << (BYTE - (fSize - cBits));
        // Pull the rest LSBits from the filter (keep them unchanged)
        if (p.offsetBits)
            // Otherwise lstByte is nicely aligned with its byte. The
            // new lstByte is exactly what we built as of now. No old
            // lstByte part needed to be restored.
            toWrite |= store[p.lstByte] & ((1 << (BYTE - p.offsetBits)) - 1);

        store[p.lstByte] = toWrite;
        break;

    default:
        throw Compact2DBitArrayError("Fingerprint cannot spread more than 5 bytes!");
    }
}

vector<unsigned char> Compact2DBitArray::getRaw() const {
    return store;
}

unsigned char Compact2DBitArray::_getNextFByte(const vector<unsigned char>& f,
                                               size_t cons) const {
    int r = fSize - cons; // remained bits
    if (r <= 0)
        throw Compact2DBitArrayError("Fingerprint has no more bits.");

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
