/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <CPISync/Data/DataObject.h>
#include <NTL/ZZ_pXFactoring.h>
#include <CPISync/Aux/Auxiliary.h>

// namespaces
using namespace NTL;

bool DataObject::RepIsInt = false; /** How DataObject handles strings. */

DataObject::DataObject() : UID()
{
    myBuffer = 0;
    timestamp = clock();

} // myBuffer should be initially empty

DataObject::DataObject(const ZZ &datum) : DataObject()
{
    myBuffer = datum;
}

DataObject::DataObject(const string &str) : DataObject()
{
    myBuffer = RepIsInt ? strTo<ZZ>(str) : pack(str);
}

// toStr function convert the ZZ from number directly to a string
// pretty sure space here won't interfere with the content
DataObject::DataObject(const long index, const list<DataObject *> elems) : DataObject()
{
    string str = toStr<long>(index) + " ";
    for (auto i : elems)
    {
        str += i->to_string() + " ";
    }
    str.pop_back();
    myBuffer = pack(str);
}

DataObject::DataObject(const ZZ index, const list<DataObject *> elems) : DataObject()
{
    string str = toStr<ZZ>(index) + " ";
    for (auto i : elems)
    {
        str += i->to_string() + " ";
    }
    str.pop_back();
    myBuffer = pack(str);
}

DataObject::DataObject(const multiset<DataObject *> tarSet) : DataObject()
{
    string str = "";
    for (auto i : tarSet)
    {
        str += i->to_string() + " ";
    }
    str.pop_back(); // delete space at the end
    myBuffer = pack(str);
}

ZZ DataObject::pack(const string &theStr)
{
    return ZZFromBytes(reinterpret_cast<const unsigned char *>(theStr.data()), theStr.length());
}

string DataObject::unpack(const ZZ &num)
{
    int size = NumBytes(num);
    auto *rawResult = new unsigned char[size];
    BytesFromZZ(rawResult, num, size);
    const auto *result = reinterpret_cast<const char *>(rawResult);
    string result_str(result, size);
    delete[] result;
    return result_str;
}

multiset<DataObject *> DataObject::to_Set() const
{
    multiset<DataObject *> result;
    string str = unpack(myBuffer);
    auto splt = split(str, " ");
    for (auto i : splt)
    {
        result.insert(new DataObject(i));
    }
    return result;
}

ZZ DataObject::to_ZZ() const
{
    return myBuffer;
}

pair<long, list<DataObject *>> DataObject::to_pairLong() const
{
    string str = unpack(myBuffer);
    auto splt = split(str, " ");
    long out = strTo<long>(splt[0]);
    list<DataObject *> outList;
    for (int i = 1; i < splt.size(); i++)
    {
        outList.push_back(new DataObject(splt[i]));
    }
    return {out, outList};
}

pair<ZZ, list<DataObject *>> DataObject::to_pairZZ() const
{
    string str = unpack(myBuffer);
    auto splt = split(str, " ");
    ZZ out = strTo<ZZ>(splt[0]);
    list<DataObject *> outList;
    for (int i = 1; i < splt.size(); i++)
    {
        outList.push_back(new DataObject(splt[i]));
    }
    return {out, outList};
}

string DataObject::to_string() const
{
    return RepIsInt ? toStr(myBuffer) : unpack(myBuffer);
}

const char *DataObject::to_char_array(long &len) const
{
    len = NumBytes(myBuffer);
    return strndup(to_string().data(), len);
}

string DataObject::print() const
{
    string str = to_string();
    if (RepIsInt)
        return str;
    else
        return base64_encode(str.data(), str.length());
}

ostream &operator<<(ostream &out, const DataObject &datum)
{
    out << datum.to_string();
    return out;
}

clock_t DataObject::getTimeStamp()
{
    return timestamp;
}

void DataObject::setTimeStamp(clock_t ts)
{
    timestamp = ts;
}
