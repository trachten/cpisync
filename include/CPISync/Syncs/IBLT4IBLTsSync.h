/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * The IBLTSync sync method syncs with another IBLTSync by sending an IBLT containing
 * its set. Upon receiving this IBLT, the server performs a subtract operation on both IBLTs
 * and uses the resulting IBLT to calculate the symmetric set difference. These differences
 * are then sent back to the client (in the case of a two-way sync), or not communicated (one-way sync).
 *
 * There is a small probability that most, but not all, of the differences will be uncovered as a result
 * of this sync.
 *
 * Created by Zifan Wang on 7/30/2019.
 */
#ifndef CPISYNCLIB_IBLT4IBLTsSync_H
#define CPISYNCLIB_IBLT4IBLTsSync_H

#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/IBLT.h>
#include <NTL/ZZ.h>

class IBLT4IBLTsSync : public SyncMethod
{
public:
  /*
     * Constructor. This IBLT is the T in the paper which is also the only thing needed to be transmitted between server and client
     * @param expected The expected number of elements being stored
     * @param eltSize The size for IBLT(ZZ type)
     * @param chldSize # elements in the child set
     * @param innerSize size of the single element in the child set
     */
  IBLT4IBLTsSync(size_t expected, size_t eltSize, size_t chldSize, size_t innerSize);
  ~IBLT4IBLTsSync() override;

  // Implemented parent class methods
  bool SyncClient(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf) override;
  bool SyncServer(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf) override;
  bool addElem(DataObject *elem) override;
  bool delElem(DataObject *elem) override;
  string getName() override;

protected:
  // one way flag
  bool oneWay;

private:
  // IBLT instance variable for storing data
  // Every pair element put in this IBLT is actually like (chldIBLT, chldSet.hash)
  IBLT myIBLT;

  // Instance variable to sore the expected number of elements
  size_t expNumElems;
  size_t childSize;
  size_t elemSize;
};

#endif //CPISYNCLIB_IBLT4IBLTsSync_H
