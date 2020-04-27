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

#ifndef CPISYNCLIB_CUCKOOSYNC_H
#define CPISYNCLIB_CUCKOOSYNC_H

#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/Cuckoo.h>

class CuckooSync : public SyncMethod {
public:
    CuckooSync(size_t fngprtSize, size_t bucketSize,
               size_t filterSize, size_t maxKicks);

    ~CuckooSync() override;

    bool SyncClient(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>>& selfMinusOther,
                    list<shared_ptr<DataObject>>& otherMinusSelf) override;

    bool SyncServer(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>>& selfMinusOther,
                    list<shared_ptr<DataObject>>& otherMinusSelf) override;

    bool addElem(shared_ptr<DataObject> datum) override;

    string getName() override;
private:
    /**
     * Cuckoo filter for reconciliation
     */
    Cuckoo myCF;
};

#endif // CPISYNCLIB_CUCKOOSYNC_H
