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

#include <vector>
#include <NTL/ZZ.h>
#include <CPISync/Data/DataObject.h>

using std::vector;
using std::shared_ptr;
using namespace NTL;

class Cuckoo {
public:
    /**
     * Constructs an empty cuckoo filter with certain fingerprint, bucket and
     * overall size.
     * @param fngprntSize The fingerprint size in bits
     * @param bucketSize The size of bucket in fingerprits
     * @param size The overall size of the Cuckoo filter in buckets
     */
    Cuckoo(size_t fngprntSize, size_t bucketSize, size_t size);

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
     * Looks up for the element.
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

private:
    /**
     * The memory of the Cuckoo filter.
     */
    vector<char> filter;

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
     * Checks if there is an empty bucket.
     * @param bucketIdx The index of the bucket to check
     * @return The index of the bucket.
     */
    size_t hasEmptyEntry(size_t bucketIdx) const;

    /**
     * Adds new element to the bucket.
     * @param bucketIdx The bucket index to be added to
     * @param xx The element to be added
     * @return The indicator whether insert succeeded
     */
    inline bool addToBucket(size_t bucketIdx, ZZ xx);
};

#endif // CUCKOO_H
