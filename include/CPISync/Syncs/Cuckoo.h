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

#ifndef CUCKOO_H
#define CUCKOO_H

#include <cmath>
#include <vector>
#include <random>
#include <functional>
#include <mutex>
#include <NTL/ZZ.h>
#include <CPISync/Data/DataObject.h>
#include <CPISync/Aux/Auxiliary.h>

#define BYTE 8

using std::vector;
using std::shared_ptr;
using std::string;
using std::runtime_error;
using namespace NTL;

class Cuckoo {
public:
    /**
     * Constructs an empty cuckoo filter with certain fingerprint, bucket and
     * overall size.
     * @param fngprntSize The fingerprint size in bits.
     * @param bucketSize The size of bucket in fingerprits.
     * @param size The overall size of the Cuckoo filter in buckets.
     * @param maxKicks The maximum number of kicks. A kick is the event of
     * the fingerprint that is being inserted evicting some other fingerprint
     * from the current bucket to the alternative bucket of the kicked
     * fingerprint due to lack of space in the current bucket.
     */
    Cuckoo(size_t fngprntSize, size_t bucketSize, size_t size, size_t maxKicks);

    typedef std::function<int(const ZZ&, size_t)> fingerprint_impl_t;
    typedef std::function<ZZ(const ZZ&, size_t)> hash_impl_t;

    /**
     * Constructs an empty cuckoo filter the following parameters:
     * @param fngprntSize The fingerprint size in bits.
     * @param bucketSize The size of bucket in fingerprits.
     * @param size The overall size of the Cuckoo filter in buckets.
     * @param maxKicks The maximum number of kicks.
     * @param fingerprint The fingerprint function in use.
     * @param hash The hash function in use.
     */
    Cuckoo(size_t fngprntSize, size_t bucketSize, size_t size, size_t maxKicks,
           fingerprint_impl_t fingerprintFunction, hash_impl_t hashFunction);

    /**
     * Constructs a populated cuckoo filter with certain fingerprint, bucket,
     * overall size, filter content and number of inserted elements.
     * @param fngprntSize The fingerprint size in bits.
     * @param bucketSize The size of bucket in fingerprits.
     * @param size The overall size of the Cuckoo filter in buckets.
     * @param maxKicks The maximum number of kicks.
     * @param itemsCount The items already inserted in the filter.
     * @param f The filter content
     */
    Cuckoo(size_t fngprtSize, size_t bucketSize, size_t size,
           size_t maxKicks, vector<unsigned char> f, ZZ itemsCount);

    /**
     * Constructor that tries to find the optimal fingerprint and
     * bucket sizes for given fasle positive rate. Fails if the error
     * rate is unattainable or if fingerprint size needed for it is
     * higher than 31 bits.
     * @param capacity The capacity of the fitler
     * @param err The targeted error rate
     */
    Cuckoo(size_t capacity, float err);

    /**
     * Default constructor
     */
    Cuckoo();

    /**
     * Default destructor
     */
    ~Cuckoo();

    /**
     * Inserts an element in the Cuckoo filter. Returns false when the
     * filter is considered full.
     * @param datum The element to be inserted
     */
    bool insert(const DataObject& datum);

    /**
     * Queries the cuckoo filter for a given element.
     * @param datum The element that is looked up for
     */
    bool lookup(const DataObject& datum) const;

    /**
     * Deletes the element. Returns false when there is no elements
     * that hashes to the same candidate buckets as the element being
     * deleted.
     *
     * Delete works only when the item that is being deleted is
     * previously inserted! Otherwise, deletion of a non-existing
     * element might result in deletion of the real one.
     * @param datum The element to be deleted
     */
    bool erase(const DataObject& datum);

    size_t getFilterSize() const;

    size_t getBucketSize() const;

    size_t getFngprtSize() const;

    size_t getMaxKicks() const;

    ZZ getItemsCount() const;

    vector<unsigned char> getRawFilter() const;

    void setFilterSize(size_t s);

    void setBucketSize(size_t s);

    void setFngprtSize(size_t s);

    void setMaxKicks(size_t mKicks);
    /**
     * @param bucketIdx The index of a bucket
     * @param entryIdx The index of an entry in the bucket
     * @return Fingerprint
     */
    unsigned getEntry(size_t bucketIdx, size_t entryIdx) const;

    /**
     * @param bucketIdx The index of the bucket
     * @param entryIdx The index of the bucket
     * @param f The fingerprint to enter
     */
    void setEntry(size_t bucketIdx, size_t entryIdx, unsigned f);

    /**
     * PRNG from the range [min, max]
     * @param min The lower limit of the range.
     * @param max The upper limit of the range.
     * @return Random number in the range.
     */
    static long _rand(size_t min, size_t max);

    friend ostream& operator<<(ostream& os, const Cuckoo cf);

    /**
     * Does this data object have zero fingerprint in this cuckoo
     * filter.
     * @param d The data object
     * @param Whether it has zero fingerprint
     */
    bool isZeroF(const DataObject& d) const;
private:
    /**
     * The memory of the Cuckoo filter.
     */
    vector<unsigned char> filter;

    /**
     * Number of buckets in filter.
     */
    size_t filterSize;

    /**
     * Number of fingerprints in bucket.
     */
    size_t bucketSize;

    /**
     * Bits in fingerprint.
     */
    size_t fngprtSize;

    /**
     * Maximum number of kicks when inserting.
     */
    size_t maxKicks;

    /**
     * Current number of items in the filter
     */
    ZZ itemsCount;

    /**
     * Fingerprint size in bytes
     */
    unsigned short fngprtSizeB;

    /**
     * Function used to calculate fingerprint of an entry. The
     * provided function must assure that 0 fingerprint is treated as
     * no fingeprint as described in fingerprint function docstring.
     */
    fingerprint_impl_t fingerprint_impl;

    /**
     * Delegate for fingerprint_impl.
     * Computes the fingerprint to be put in bucket entries.
     * Has to ensure that 0 cannot appear as a fingerprint.
     * 0 is considered an empty entry.
     * The design decision is described in `hasEmptyEntry` docstring.
     * It is also very important that hash != fingerprint for all* items.
     * Otherwise, when an item hashes to bucket 0, it cannot possibly be relocated.
     * i_1 == 0, i_2 == i_1 xor 0 == 0.
     * @param xx The element for which the fingerprint is calculated
     * @return The fngprtSize least significant bits from xx.
     */
    inline int fingerprint(const ZZ& e) const;

    /**
     * Function used to calculate hash of an entry
     */
    hash_impl_t hash_impl;

    /**
     * Delegate for hash_impl.
     * Computes the hash of a set element modulo filterSize.
     * By default uses std::hash<std::string> specialization which
     * if linked with libstdc++ uses Murmur hash
     * https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/hash_bytes.cc#L74
     * @param xx The set element.
     * @return The hash as ZZ.
     */
    inline ZZ hash(const ZZ& e) const;

    /*
     * Random device to initialize the PRNG
     */
    static std::random_device rd;

    /**
     * Mersenne Twister as PRNG
     */
    static std::mt19937 prng;

    /**
     * Recursive cuckoo inserting.
     * @param f The fingerprint for which the relocation is attempted
     * @param bucket The index of the bucket to start with.
     * @param kicks The number of relocations done so far.
     * @return Success indicator.
     */
    bool _insert(unsigned f, int bucket, size_t kicks);

    /**
     * Check if there is an empty bucket and put fingerprint there.
     * An entry with all 0 bits is considered empty.
     * Fingerprint generating function must ensure (0..2^fngprtSize] range.
     * Zero excluded. This is a design decision. The choice is either to store
     * one more bit with each entry or to take one element out from
     * the fingerprint generator function's range.
     * Fingerprints are normally small (less than a byte).
     * One bit per fingerprint would incur a >12.5% space overhead.
     * @param bucketIdx The bucket index to be added to
     * @param f The fingerprint to be added
     * @return The entry index where the fngprt is inserted
     * or -1 if the bucket is full.
     */
    inline int addToBucket(size_t bucketIdx, unsigned f);

    /**
     * @param f The fingerprint to search for.
     * @param bucket The bucket in which to search.
     * @return Index of the fingerprint in the bucket.
     */
    inline int hasF(unsigned f, size_t bucket) const;

    /**
     * Return the next byte of fingerprint.
     * If remained bits in f fit a single byte, the remaining bits are aligned
     * to the most significant bit of the byte and returned.
     * @param f The fingerprint.
     * @param cons The fingerprint bits consumed as of now.
     */
    inline unsigned char _getNextFByte(const vector<unsigned char>& f,
                                       size_t cons) const;

    /**
     * Calculate the alternative bucket for the given bucket and the fingerprint.
     * @param currentB The current bucket.
     * @param f The fingerprint.
     */
    inline int _alternativeBucket(int currentB, int f) const;

    /**
     * Partial hashing return values.
     */
    struct PartialHash {
        int f;  // fingerprint
        int i1; // first bucket
        int i2; // second bucket
    };

    /**
     * Calculate the partial hash of the item.
     * @param datum The item to calculate partial hash for.
     * TODO: for fngprtSize=7 (f=119), fitlerSize=5, bucketSize=4
     * can finish in 0, 1, and can be kicked to 4 (from 0), instead of 1.
     * _alternativeBucket(1, 119) == 0, _alternativeBucket(0, 119) == 4.
     * This introduces false negatives and Cuckoo Filter should not have any.
     */
    inline PartialHash _pHash(const DataObject& datum) const;

    inline void _constructorGuards() const;

    /**
     * Boundary checks for getEntry and SetEntry.
     */
    inline void _assertIdx(size_t bucketIdx, size_t entryIdx) const;

    /**
     * Auxiliary data used by both getEntry and setEntry to describe
     * the position of the certain entry in raw filter that is a plain
     * vector of bytes.
     */
    struct GetSetPrelim {
        size_t entryBits;  // exact bit where the entry starts
        size_t fstByte;    // first byte the entry touches
        size_t lstByte;    // last byte the entry touches
        size_t onsetBits;  // number of bits in fstByte that are before
                           // the entry
        size_t offsetBits; // number of bits in lstByte that are still
                           // the part of the entry
    };

    inline GetSetPrelim _getSetPrelim(size_t bucketIdx, size_t entryIdx) const;

    class CuckooFilterError : public runtime_error {
    public:
        CuckooFilterError(const string& msg) : runtime_error(msg) {}
    };
};

#endif // CUCKOO_H
