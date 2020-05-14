/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on April, 2020.
 *
 * This class implements 2D bit array such that the maximum unused
 * space is 7 bits.
 *
 * Terminology:
 * - bucket can be thought of as a row in the table,
 * - entry can be thought of as a cell in the table,
 * - fingerprint can be thought of as the data that is written to the cell.
 */

#ifndef COMPACT2DBITARRAY_H
#define COMPACT2DBITARRAY_H

#include <cmath>
#include <vector>
#include <cstddef>
#include <mutex>
#include <CPISync/Aux/Auxiliary.h>

#define BYTE 8

class Compact2DBitArray {
public:
    Compact2DBitArray();

    ~Compact2DBitArray();

    /**
     * @param fingerprintSize The size of a cell
     * @param bucketSize The number of columns
     * @param NumOfBuckets The number of rows
     */
    Compact2DBitArray(size_t fingerprintSize, size_t bucketSize,
                      size_t NumOfBuckets);

    /**
     * Reconstruction constructor
     */
    Compact2DBitArray(size_t fingerprintSize, size_t bucketSize,
                      size_t NumOfBuckets, vector<unsigned char> f);

    /**
     * @param bucketIdx The index of a bucket (row)
     * @param entryIdx The index of an entry in the bucket (column)
     * @return Fingerprint
     */
    unsigned getEntry(size_t bucketIdx, size_t entryIdx) const;

    /**
     * @param bucketIdx The index of the bucket (row)
     * @param entryIdx The index of an entry in the bucket (column)
     * @param f The fingerprint to enter
     */
    void setEntry(size_t bucketIdx, size_t entryIdx, unsigned f);

    /**
     * Get fingerprint size
     */
    size_t getF() const;

    /**
     * Get bucket size (number of columns)
     */
    size_t getColumns() const;

    /**
     * Get number of buckets (number of rows)
     */
    size_t getRows() const;

    vector<unsigned char> getRaw() const;

private:
    /**
     * Storage of a Compact2DBitArray
     */
    vector<unsigned char> store;

    /**
     * Fingerprint size in bits
     */
    size_t fSize;

    /**
     * Fingerprint size in bytes
     */
    unsigned short fSizeB;

    /**
     * Bucket size (number of columns)
     */
    size_t bSize;

    /**
     * Number of buckets in filter (number of rows)
     */
    size_t nBuckets;

    /**
     * Boundary checks for getEntry and setEntry
     */
    inline void _assertIdx(size_t bucketIdx, size_t entryIdx) const;

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
     * Auxiliary data used by both getEntry and setEntry to describe
     * the position of the certain entry in Compact2DBitArray that is
     * a plain vector of bytes.
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

    class Compact2DBitArrayError : public runtime_error {
    public:
        Compact2DBitArrayError(const string& msg) : runtime_error(msg) {}
    };
};

#endif // COMPACT2DBITARRAY_H
