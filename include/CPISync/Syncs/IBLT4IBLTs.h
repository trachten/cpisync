#ifndef CPISYNCLIB_IBLT4Sets_H
#define CPISYNCLIB_IBLT4Sets_H

#include <vector>
#include <string>
#include <NTL/ZZ.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Data/DataObject.h>

using std::hash;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;
using namespace NTL;

const long SET_HASH = 10;

class IBLT4IBLTs
{
public:
  IBLT4IBLTs(size_t expectedNumEntries, size_t _valueSize);
  ~IBLT4IBLTs();
  bool insert(IBLT &chldIBLT, ZZ &chldHash);
  bool erase(IBLT &chldIBLT, ZZ &chldHash);

private:
  IBLT myIBLT;
  hash_t _setHash(multiset<DataObject *> tarSet);
  hash_t _hashK(const ZZ &item, long kk);
  hash_t _hash(const hash_t &initial, long kk);
};

#endif