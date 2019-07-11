/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <NTL/RR.h>
#include <Communicants/Communicant.h>

Communicant::Communicant() {
    resetCommCounters();
    xferBytesTot = recvBytesTot = 0;
    totalTime = std::chrono::high_resolution_clock::now();
    MOD_SIZE = NOT_SET;
}

Communicant::~Communicant() = default;

void Communicant::resetCommCounters() {
    xferBytes = recvBytes = 0;
	resetTime = std::chrono::high_resolution_clock::now();
}

void Communicant::hardResetCommCounters() {
	xferBytes = recvBytes = xferBytesTot = recvBytesTot = 0;
	resetTime = std::chrono::high_resolution_clock::now();
	totalTime = std::chrono::high_resolution_clock::now();
}

string Communicant::getName() {
    return "No name available";
}

long Communicant::getXmitBytes() const {
    return xferBytes;
}

long Communicant::getRecvBytes() {
    return recvBytes;
}

long Communicant::getXmitBytesTot() {
    return xferBytesTot;
}

long Communicant::getRecvBytesTot() {
    return recvBytesTot;
}

std::chrono::high_resolution_clock::time_point Communicant::getResetTime() {
    return resetTime;
}

std::chrono::high_resolution_clock::time_point Communicant::getTotalTime() {
    return totalTime;
}

void Communicant::addXmitBytes(long numBytes) {
    xferBytes += numBytes;
    xferBytesTot += numBytes;
}

void Communicant::addRecvBytes(long numBytes) {
    recvBytes += numBytes;
    recvBytesTot += numBytes;
}

bool Communicant::establishModRecv(bool oneWay /* = false */) {
    ZZ otherModulus = commRecv_ZZ();

    if (otherModulus != ZZ_p::modulus()) {
        Logger::gLog(Logger::COMM, "ZZ_p moduli do not match: " + toStr(ZZ_p::modulus) + " (mine) vs " + toStr(otherModulus) + " (other).");
        if (!oneWay) // one way reconciliation does not send any data
            commSend(SYNC_FAIL_FLAG);
        return false;
    }
    MOD_SIZE = NumBytes(ZZ_p::modulus()); // record the modulus size
    if (!oneWay)
        commSend(SYNC_OK_FLAG);
    return true;
}

bool Communicant::establishModSend(bool oneWay /* = false */) {
    commSend(ZZ_p::modulus());
    MOD_SIZE = NumBytes(ZZ_p::modulus());
    if (oneWay)
        return true;  // i.e. don't want for a response
    else
        return (commRecv_byte() != SYNC_FAIL_FLAG);
}

bool Communicant::establishIBLTSend(size_t size, size_t eltSize, bool oneWay /* = false */) {
    commSend((long) size);
    commSend((long) eltSize);
    if (oneWay)
        return true;  // i.e. don't wait for a response
    else
        return (commRecv_byte() != SYNC_FAIL_FLAG);
}

bool Communicant::establishIBLTRecv(size_t size, size_t eltSize, bool oneWay /* = false */) {
    // receive other size and eltSize. both must be read, even if the first parameter is wrong
    long otherSize = commRecv_long();
    long otherEltSize = commRecv_long();

    if(otherSize == size && otherEltSize == eltSize) {
        if(!oneWay)
            commSend(SYNC_OK_FLAG);
        return true;
    } else {
        Logger::gLog(Logger::COMM, "IBLT params do not match: mine(size=" + toStr(size) + ", eltSize="
        + toStr(eltSize) + ") vs other(size=" + toStr(otherSize) + ", eltSize=" + toStr(otherEltSize) + ").");
        if(!oneWay)
            commSend(SYNC_FAIL_FLAG);
        return false;
    }
}


void Communicant::commSend(const ustring& toSend, const unsigned int numBytes) {
    Logger::gLog(Logger::COMM_DETAILS, "... attempting to send: ustring: "
            + base64_encode(reinterpret_cast<const char *>(toSend.data()), numBytes));

    auto sendptr = reinterpret_cast<const char *> ((unsigned char *) toSend.data());
    commSend(sendptr, numBytes);
}

void Communicant::commSend(const string& str) {
    Logger::gLog(Logger::COMM, "... attempting to send: string " + str);
    commSend((long) str.length());
    commSend(str.data(), str.length());
}

void Communicant::commSend(const ustring& ustr) {

    Logger::gLog(Logger::COMM, "... attempting to send: ustring " + ustrToStr(ustr));
    commSend((long) ustr.length());
    commSend(ustr, ustr.length());
}

void Communicant::commSend(DataObject& dob) {

    Logger::gLog(Logger::COMM, "... attempting to send: DataObject " + dob.to_string());

    // for now, just send the data object as a string ... this can be optimized
    commSend(dob.to_string());
}

void Communicant::commSend(list<DataObject *> &dob) {
  // send the size of the list
  commSend((long) dob.size());

  // then every entry in it
  for (DataObject *&dop : dob) {
    commSend(*dop); // request the data object
  }
}

void Communicant::commSend(DataPriorityObject& dob) {

    Logger::gLog(Logger::COMM, "... attempting to send: DataObject " + dob.to_priority_string());

    // for now, just send the data object as a string ... this can be optimized
    commSend(dob.to_priority_string());
}

void Communicant::commSend(const list<DataObject*> &lst) {
    Logger::gLog(Logger::COMM, "... attempting to send: DataObject list ...");

    list<DataObject*>::const_iterator it;
    commSend((long) lst.size()); // the number of elements in the list

    for (it = lst.begin(); it != lst.end(); it++)
        commSend(**it);
}

void Communicant::commSend(double num) {
    // Convert to an RR type and send mantissa and exponent

    Logger::gLog(Logger::COMM, "... attempting to send: double " + toStr(num));

    RR num_RR;
    num_RR = num;
    commSend(num_RR.mantissa());
    commSend(-num_RR.exponent()); // exponent should always be negative or 0
}

void Communicant::commSend(const long num) {

    unsigned char toSend[XMIT_LONG];
    BytesFromZZ(toSend, to_ZZ(num), XMIT_LONG);
    Logger::gLog(Logger::COMM, "... attempting to send: long " + toStr(num));
    commSend(ustring(toSend, XMIT_LONG), XMIT_LONG);
}

void Communicant::commSend(const byte bt) {

    Logger::gLog(Logger::COMM, string("... attempting to send: byte num ") + toStr((int) bt));
    commSend(&bt, 1);
}

void Communicant::commSend(const int num) {

    unsigned char toSend[XMIT_INT];
    BytesFromZZ(toSend, to_ZZ(num), XMIT_INT);
    Logger::gLog(Logger::COMM, "... attempting to send: int " + toStr(num));
    commSend(ustring(toSend, XMIT_INT), XMIT_INT);
}

void Communicant::commSend(const ZZ_p& num) {

    Logger::gLog(Logger::COMM, "... attempting to send: ZZ_p " + toStr(num));

    // send like a ZZ, but with a known size
    unsigned char toSend[MOD_SIZE];

    BytesFromZZ(toSend, rep(num), MOD_SIZE);
    commSend(ustring(toSend, MOD_SIZE), MOD_SIZE);
}

void Communicant::commSend(const vec_ZZ_p& vec) {
    Logger::gLog(Logger::COMM, "... attempting to send: vec_ZZ_p " + toStr(vec));

    // pack the vec_ZZ_p into a big ZZ and send it along    
    ZZ result;
    result = 0;

    for (int ii = vec.length() - 1; ii >= 0; ii--) // append in reverse order to make decoding easier
        result = (result * (ZZ_p::modulus()+1)) + rep(vec[ii])+1; // added 1 to avoid insignificant 0's in the lead of the vector
    commSend(result);
}

vec_ZZ_p Communicant::commRecv_vec_ZZ_p() {
    // unpack the received ZZ into a vec_ZZ_p
    ZZ received = commRecv_ZZ();
    vec_ZZ_p result;

     
    while (received != 0) {
        ZZ divisor, remainder;
        DivRem(divisor, remainder, received, ZZ_p::modulus()+1);

        append(result, to_ZZ_p(remainder-1)); // subtract back the 1 that was added when sent
        received = divisor;
    }

    Logger::gLog(Logger::COMM, "... received vec_ZZ_p " + toStr(result));

    return result;
}

void Communicant::commSend(const IBLT& iblt, bool sync) {
    if (!sync) {
        commSend((long) iblt.size());
        commSend((long) iblt.eltSize());
    }

    // Access the hashTable representation of iblt to serialize it
    for(const IBLT::HashTableEntry& hte : iblt.hashTable) {
        commSend(hte, iblt.eltSize());
    }
}

void Communicant::commSend(const IBLT::HashTableEntry& hte, size_t eltSize) {
    commSend(hte.count);
    commSend((long) hte.keyCheck);
    commSend(hte.keySum); // not guaranteed to be the same size as all other hash-table-entry key-sums
    commSend(hte.valueSum, (unsigned int) eltSize);
}

void Communicant::commSend(const ZZ& num, int size) {
    Logger::gLog(Logger::COMM, "... attempting to send: ZZ " + toStr(num));

    auto num_size = (unsigned int) (size == NOT_SET ? NumBytes(num) : size);
    if (num_size == 0) num_size = 1; // special case for sending the integer 0 - need one bit
    unsigned char toSend[num_size];

    if (size == NOT_SET) // first send the number of bytes represented by the ZZ
        commSend((int) num_size);

    // next send the actual number, as a byte sequence
    BytesFromZZ(toSend, num, num_size);

    commSend(ustring(toSend, num_size), num_size);

}

ustring Communicant::commRecv_ustring(unsigned int numBytes) {
    string received = commRecv(numBytes);
    ustring result((const unsigned char *) (received.data()), numBytes);
    Logger::gLog(Logger::COMM_DETAILS, "... received ustring: " +
            base64_encode(reinterpret_cast<const char *>(result.data()), numBytes));

    return (result); // return the result as a ustring
}

string Communicant::commRecv_string() {
    long sz = commRecv_long();
    string str = commRecv(sz);

    Logger::gLog(Logger::COMM, "... received: string " + str);

    return str;
}

ustring Communicant::commRecv_ustring() {
    long sz = commRecv_long();
    ustring ustr = commRecv_ustring(sz);

    Logger::gLog(Logger::COMM, "... received: ustring " + ustrToStr(ustr));

    return ustr;
}

DataObject* Communicant::commRecv_DataObject() {
    DataObject *res = new DataObject(commRecv_string());
    Logger::gLog(Logger::COMM, "... received: DataObject " + res->to_string());

    return res;
}

list<DataObject *> Communicant::commRecv_DataObject_List() {
  list<DataObject *> result;

  // receive the size of the list
  long size = commRecv_long();

  // then every entry in it
  for (int ii=0; ii<size; ii++) {
    result.push_back(commRecv_DataObject()); // receive the data object
  }

  return result;
}

DataPriorityObject * Communicant::commRecv_DataObject_Priority() {
    string str = commRecv_string();
    string prio = str.substr(0, str.find(','));
    str = str.substr(str.find(',') + 1);
    auto * res = new DataPriorityObject(str);
    res->setPriority(strTo<ZZ > (prio));
    Logger::gLog(Logger::COMM, "... received: DataPriorityObject " + res->to_string());
    return res;
}

// receives a list of data objects

list<DataObject*> Communicant::commRecv_DoList() {
    list<DataObject*> result;

    long numDiffs = commRecv_long();
    for (long ii = 0; ii < numDiffs; ii++) {
        DataObject *dobp = commRecv_DataObject();
        result.push_back(dobp);
    }

    Logger::gLog(Logger::COMM, "... received: DataObject list ...");

    return result;
}

double Communicant::commRecv_double() {
    ZZ mantissa = commRecv_ZZ();
    long exponent = -commRecv_long();
    RR result_RR = MakeRR(mantissa, exponent);
    Logger::gLog(Logger::COMM, "... received double " + toStr(result_RR));

    return to_double(result_RR);
}

long Communicant::commRecv_long() {
    ustring received = commRecv_ustring(XMIT_LONG);
    ZZ num = ZZFromBytes(received.data(), XMIT_LONG);
    Logger::gLog(Logger::COMM, "... received long " + toStr(num));

    return to_long(num);
}

int Communicant::commRecv_int() {
    ustring received = commRecv_ustring(XMIT_INT);
    ZZ num = ZZFromBytes(received.data(), XMIT_INT);
    Logger::gLog(Logger::COMM, "... received int " + toStr(num));

    return to_int(num);
}

byte Communicant::commRecv_byte() {
    string received = commRecv(1);
    Logger::gLog(Logger::COMM, string("... received byte num ") + toStr((int) received[0]));

    return received[0];
}

ZZ_p Communicant::commRecv_ZZ_p() {
    // the size is fixed by the connection initialization phase
    ustring received = commRecv_ustring(MOD_SIZE);
    ZZ_p result = to_ZZ_p(ZZFromBytes(received.data(), MOD_SIZE));

    Logger::gLog(Logger::COMM, "... received ZZ_p " + toStr(result));

    return result;
}

ZZ Communicant::commRecv_ZZ(int size) {
    int num_size;
    ustring received;
    if (size == 0)
        num_size = commRecv_int(); // first receive the number of bytes represented by the ZZ
    else
        num_size = size;

    // second receive the actual ZZ
    received = commRecv_ustring(num_size);

    ZZ result = ZZFromBytes(received.data(), num_size);
    Logger::gLog(Logger::COMM, "... received ZZ " + toStr(result));

    return result;
}

IBLT Communicant::commRecv_IBLT(size_t size, size_t eltSize) {
    size_t numSize;
    size_t numEltSize;

    if(size == NOT_SET || eltSize == NOT_SET) {
        numSize = (size_t) commRecv_long();
        numEltSize = (size_t) commRecv_long();
    } else {
        numSize = size;
        numEltSize = eltSize;
    }

    IBLT theirs;
    theirs.valueSize = numEltSize;

    for(int ii = 0; ii < numSize; ii++) {
        theirs.hashTable.push_back(commRecv_HashTableEntry(numEltSize));
    }

    return theirs;
}

IBLT::HashTableEntry Communicant::commRecv_HashTableEntry(size_t eltSize) {
    IBLT::HashTableEntry hte;

    hte.count = commRecv_long();
    hte.keyCheck = (hash_t) commRecv_long();
    hte.keySum = commRecv_ZZ();
    hte.valueSum = commRecv_ZZ((unsigned int) eltSize);

    return hte;
}