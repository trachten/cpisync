/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include "DataObject.h"
#include "Auxiliary.h"
#include "NTL/mat_ZZ_p.h"
#include "NTL/ZZ_pXFactoring.h"

// namespaces
using namespace NTL;

bool DataObject::RepIsInt = false; /** How DataObject handles strings. */

DataObject::DataObject()  : UID()
{ myBuffer = 0; } // myBuffer should be initially empty

DataObject::DataObject(const ZZ &datum)  : UID() {
    myBuffer = datum;
    
}

DataObject::DataObject(const string str) : UID() {
    myBuffer = RepIsInt?strTo<ZZ>(str):pack(str);
}

ZZ DataObject::pack(const string theStr) {
    return ZZFromBytes(reinterpret_cast<const unsigned char*>(theStr.data()), theStr.length());   
} 

string DataObject::unpack(const ZZ num) {
    int size = NumBytes(num);
    unsigned char *rawResult = new unsigned char[size];
    BytesFromZZ(rawResult, num, size);
    const char *result = reinterpret_cast<const char *> (rawResult);
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

const char *DataObject::to_char_array(int &len) const {
    len = NumBytes(myBuffer);
    return to_string().data();
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

string DataObject::to_priority_string() const{
    return RepIsInt?toStr(priority)+","+toStr(myBuffer):unpack(myBuffer);
}


void DataObject::setPriority(ZZ pri) {
    priority = pri;
};

clock_t DataObject::getTimeStamp() {
    return timestamp;
}

void DataObject::setTimeStamp(clock_t ts) {
    timestamp = ts;
}
ZZ DataObject::getPriority(){
    return priority;
}
