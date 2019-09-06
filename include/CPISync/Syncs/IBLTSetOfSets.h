/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * The IBLT sync on set of sets data structure is performed by regarding each child set as a small IBLT and putting them
 * into a bigger IBLT together with their hash. Use generic IBLT sync method to find different child sets and try to decode
 * each pair of child sets to find missing elements, which takes O(d^3) where d is the symmetric difference.
 * This algorithm is mentioned by Michael Mitzenmacher Tom Morgan in https://arxiv.org/pdf/1707.05867.pdf.
 *
 * There is a small probability that most, but not all, of the differences will be uncovered as a result
 * of this sync. The probability as mentioned in the paper is 1/poly(d), where d is the difference between two set.
 * 
 * TODO: parameter in constructor to control failing probablity
 *
 * Created by Zifan Wang on 7/30/2019.
 */
#ifndef CPISYNCLIB_IBLTSetOfSets_H
#define CPISYNCLIB_IBLTSetOfSets_H

#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/IBLT.h>
#include <NTL/ZZ.h>

class IBLTSetOfSets : public SyncMethod
{
public:
  /*
     * Constructor. This IBLT is the T in the paper which is also the only thing needed to be transmitted between server and client
     * @param expected The expected number of elements being stored
     * @param numElemChld upper bound of # elements in the child set
     * @param elemSize size of the single element in the child set
     */
  IBLTSetOfSets(size_t expected, size_t numElemChld, size_t elemSize);
  ~IBLTSetOfSets() override;

  // Implemented parent class methods
  bool SyncClient(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
  bool SyncServer(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;

  /**
   * add/delete  element in current IBLT
   * the elem is a multiset represented as a dataobject pointer
   */
  bool addElem(shared_ptr<DataObject> elem) override;
  bool delElem(shared_ptr<DataObject> elem) override;

  /** Delete target object pointed by index
  * reconstruct a dataobject by adding elems to the current target object
  * @param index index for the target child set stored in the data structure
  * @param elems missing elements
  * @return a dataobject reconstructed from a set after adding the missing elems
  **/
  shared_ptr<DataObject> reWrite(long index, list<shared_ptr<DataObject>> elems);
  string getName() override;


  /** Add the new child sets with missing elements added, and remove the old one by index
   *      in the decreasing order, which won't influence the following content to be deleted
   * @param otherMinusSelf pairs indicating {index, missing elements} gained from IBLTsetofsets
   *        sync.
   * @param myData data structure containing all the elements, should be passed by myData in
   *        genSync function
   * @param *add function pointer, should be passed by addElem in genSync function
   * @param *del funciton pointer, should be passed by delElem in genSync function
   * @param *pGenSync pointer to current genSync object.
   */
  template <class T>
  static void postProcessing_IBLTSetOfSets(list<shared_ptr<DataObject>> otherMinusSelf, list<shared_ptr<DataObject>> myData, void (T::*add)(shared_ptr<DataObject>), bool (T::*del)(shared_ptr<DataObject>), T *pGenSync)
  {
    list<shared_ptr<DataObject>>::iterator itDO;
    vector<long> toErase;

    for (itDO = otherMinusSelf.begin(); itDO != otherMinusSelf.end(); itDO++)
    {

      auto info = (*itDO)->to_pair<long>();

      toErase.push_back(info.first);

      multiset<shared_ptr<DataObject>> tmpSet;
      for (auto ii : info.second)
      {
        tmpSet.insert(ii);
      }
      auto out = make_shared<DataObject>(tmpSet);
      (pGenSync->*add)(out);
    }
    if (toErase.size() != 0)
    {

      sort(toErase.begin(), toErase.end(), greater<long>());
      for (auto ii : toErase)
      {
        auto it = myData.begin();
        advance(it, ii);
        (pGenSync->*del)(*it);
      }
      toErase.clear();
    }
  }

protected:
  // one way flag
  bool oneWay;

private:
  // IBLT instance variable for storing data
  // Every pair element put in this IBLT is actually like (chldIBLT, chldSet.hash)
  IBLT myIBLT;
  
  /**
   *  decode procedure for inner IBLT
   * @param postiveChld serialized IBLT {IBLT, hash} peeled from positive count
   * @param negativeChld serialized IBLT {IBLT, hash} peeled from negative count
   * @return two list containing missing elements, elements are pair of {hash, missing elements} serialized in a data object type
   **/
  pair<list<shared_ptr<DataObject>>,list<shared_ptr<DataObject>>> _decodeInnerIBLT(vector<pair<ZZ, ZZ>> &positiveChld, vector<pair<ZZ, ZZ>> &negativeChld);

  // a data object containing all child set in this class
  // each child set is represented as a dataobject
  vector<shared_ptr<DataObject>> mySet;

  // Instance variable to store the expected number of elements
  size_t expNumElems;
  // Instance variable to store the expected number of elements in child set
  size_t childSize;
  // instance variable to store the size for elements in the child set
  size_t elemSize;
};

#endif //CPISYNCLIB_IBLTSetOfSets_H
