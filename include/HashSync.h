//
// Created by Ari Trachtenberg on 7/18/18.
//

#ifndef CPISYNCLIB_HASHSYNC_H
#define CPISYNCLIB_HASHSYNC_H

#include "SyncMethod.h"
#include "DataObject.h"
#include "GenSync.h"

/**
 * Implements a hash layer on top of an existing sync algorithm (subclass of SyncMethod).
 * HashSync is constructed with a {@code SyncMethod} class and does the following:
 * 0.  All entries are hashed, and an inverse hash is maintained by the object.
 * 1.  Synchronization occurs based on hashes, utilizing the templated sync algorithm.
 * 2.  After synchronization of hashes, another synchronization exchange occurs to translate
 *      hashed entries into actual objects.
 */

class HashSync : SyncMethod {

 public:
     /**
      * Creates a HashSync object, whose elements are hashed to numbers 0..{@code hashUB}-1
      * @param theSyncObject A pointer to a {@code SyncMethod} object capable of synchronizing
      *     data objects produced from ZZ's in the range 0 ... {@code hashUB}.
      * @param hashUB All elements hare hashed to a number 0..{@code hashUB}-1, which is presented
      *     as a {@code DataObject} to the underlying sync engine.
      */
     HashSync(shared_ptr<SyncMethod> theSyncObject, int hashUB);

  // inherited
  bool SyncClient(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);

  bool SyncServer(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);

  bool addElem(DataObject* newDatum);

  bool delElem(DataObject* newDatum);

 protected:
     /**
      * Hashes an input into a (presumably smaller) output as in an oracle, meaning that
      * the same input always hashes to the same hash.
      * May not be invertible, or one-to-one.
      * @param input The input DataObject to be hashed.
      * @return A ZZ in the range 0..{@code hashUB}-1
      */
     ZZ hash(const DataObject *input) {
       return (input->to_ZZ()%largerPrime)%hashUB;
     }

     shared_ptr<SyncMethod> syncObject;

  // maps a hash value to [ the memory where the hashed object is kept, the DataObject that was hashed to get this ]
     std::map<ZZ, std::pair<DataObject*,DataObject *> > myHashMap;
     ZZ hashUB;
     ZZ largerPrime; // a prime number larger than hashUB (for use in hash computations)

 private:

  /**
   * Maps a point to a hashed value to the pointer to the original value that produced the hash.
   */
    DataObject *mapHashToOrig(DataObject *hashPtr) {
      return myHashMap[hashPtr->to_ZZ()].second;
    }
};


#endif //CPISYNCLIB_HASHSYNC_H
