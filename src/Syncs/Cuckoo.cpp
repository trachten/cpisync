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
#include <math.h>

Cuckoo::~Cuckoo() = default;

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

Cuckoo::Cuckoo(size_t fngprtSize, size_t bucketSize, size_t size) {
    size_t bytes = ceil(fngprtSize / sizeof(char));
    filter.resize(bytes * bucketSize * size);
}

bool Cuckoo::insert(const DataObject& datum) {

}
