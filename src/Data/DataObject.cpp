/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <CPISync/Data/DataObject.h>
#include "NTL/ZZ_pXFactoring.h"

// namespaces
using namespace NTL;

bool DataObject::RepIsInt = false; /** How DataObject handles strings. */

DataObject::DataObject()  : UID()
{
    myBuffer = 0; timestamp=clock();

} // myBuffer should be initially empty

DataObject::DataObject(const ZZ &datum)  : DataObject() {
    myBuffer = datum;
}

DataObject::DataObject(const string& str) : DataObject() {
    myBuffer = RepIsInt?strTo<ZZ>(str):pack(str);
}

DataObject::DataObject(const multiset<shared_ptr<DataObject>>& tarSet) : DataObject()
{
    string str;
    for (const auto& ii : tarSet)
        str += base64_encode(ii->to_string().c_str(),ii->to_string().length()) + " ";
    str = base64_encode(str.c_str(), str.length());
    myBuffer = pack(str);
}

multiset<shared_ptr<DataObject>> DataObject::to_Set() const
{
    multiset<shared_ptr<DataObject>> result;
    string str = base64_decode(unpack(myBuffer));
    auto splt = split(str, ' ');
    for (const auto& itr : splt)
        result.insert(make_shared<DataObject>(base64_decode(itr)));
    return result;
}


ZZ DataObject::pack(const string& theStr) {
    return ZZFromBytes(reinterpret_cast<const unsigned char*>(theStr.data()), theStr.length());   
} 

string DataObject::unpack(const ZZ& num) {
    unsigned long size = narrow_cast<unsigned long>(NumBytes(num));
    auto *rawResult = new unsigned char[size];
    BytesFromZZ(rawResult, num, size);
    const auto *result = reinterpret_cast<const char *> (rawResult);
    string result_str(result,size);
    delete[] result;
    return result_str;
}

ZZ DataObject::to_ZZ() const {
    return myBuffer;
}

string DataObject::to_string() const {
    return RepIsInt?toStr(myBuffer):unpack(myBuffer);
}

const char *DataObject::to_char_array(size_t &len) const {
    len = narrow_cast<size_t>(NumBytes(myBuffer));
    return strndup(to_string().data(), len);
}

string DataObject::print() const {
    string str = to_string();
    if (RepIsInt)
      return str;
    else
      return base64_encode(str.data(), str.length());
}

ostream& operator<<( ostream &out, const DataObject &datum)
{
 out << datum.to_string();
 return out;
}


clock_t DataObject::getTimeStamp() {
    return timestamp;
}

void DataObject::setTimeStamp(clock_t ts) {
    timestamp = ts;
}

