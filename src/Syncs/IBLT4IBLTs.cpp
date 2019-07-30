#include "CPISync/Syncs/IBLT.h"
#include <CPISync/Aux/Auxiliary.h>
#include "CPISync/Syncs/IBLT4IBLTs.h"

hash_t IBLT4IBLTs::_hash(const hash_t &initial, long kk)
{
    if (kk == -1)
        return initial;
    std::hash<std::string> shash;
    return _hash(shash(toStr(initial)), kk - 1);
}

hash_t IBLT4IBLTs::_hashK(const ZZ &item, long kk)
{
    std::hash<std::string> shash; // stl uses MurmurHashUnaligned2 for calculating the hash of a string
    return _hash(shash(toStr(item)), kk - 1);
}

hash_t IBLT4IBLTs::_setHash(multiset<DataObject *> tarSet)
{
    hash_t outHash = 0;
    for (auto i : tarSet)
    {
        outHash += _hashK(i->to_ZZ(), SET_HASH);
    }
    return outHash;
}

bool IBLT4IBLTs::insert(IBLT &chldIBLT, ZZ &chldHash)
{
    ZZ ibltZZ = strToZZ(chldIBLT.toString());
    myIBLT.insert(ibltZZ, chldHash);
}

bool IBLT4IBLTs::erase(IBLT &chldIBLT, ZZ &chldHash)
{
    ZZ ibltZZ = strToZZ(chldIBLT.toString());
    myIBLT.erase(ibltZZ, chldHash);
}
