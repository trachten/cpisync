/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   Auxiliary.h
 * Auxiliary items shared throughout the program.  These include constants, types, and functions.
 *
 * Created on August 20, 2011, 10:07 PM
 */

#ifndef AUX_H
#define	AUX_H

#include <sstream>
#include <unistd.h>
#include <NTL/ZZ.h>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <iterator>
#include <set>
#include <signal.h>
#include <sys/wait.h>
#include "GenSync.h"
#include "SyncMethod.h"
#include "ConstantsAndTypes.h"

// some standard names
using namespace NTL;
using std::cout;
using std::clog;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::map;
using std::multiset;
using std::invalid_argument;
using std::runtime_error;

// FUNCTIONS
/**
 * Runs a synchronization between client1 and client2 in a separate process, recording some statistics in the process.
 * @param client1 The first client to sync.
 * @param client2 The second client to sync.
 * @return Exit status for child/parent processes.
 */

/**
 * Report structure for a forkHandle run
  */
struct forkHandleReport {
    forkHandleReport(): bytes(-1), CPUtime(-1), totalTime(-1), success(false) {}
    long bytes;       // the number of bytes communicated
    double CPUtime;   // the amount of CPU time used
    double totalTime; // total time used
    bool success;     // true iff the sync completed successfully
};

/**
 * Runs client1 (child process) and client2 (parent process), returning statistics for client2.
 * @param server The GenSync object that plays the role of server in the sync.
 * @param client The GenSync object that plays the role of client in the sync.
 * @return Synchronization statistics as reported by the server.
 */
inline forkHandleReport forkHandle(GenSync* server, GenSync* client) {   
    int err = 1;
    int chld_state;
    int my_opt = 0;
    forkHandleReport result;
    clock_t start = clock();
    try {
        pid_t pID = fork();
        int method_num = 0;
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);
            //cout << "create a child process" << endl;
            client->listenSync(method_num);
            exit(0);
        } else if (pID < 0) {
            //handle_error("error to fork a child process");
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            cout << "create a parent process" << endl;
            server->startSync(method_num);
            result.totalTime = (double) (clock() - start) / CLOCKS_PER_SEC;
            result.CPUtime = server->getSyncTime(method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
            result.bytes = server->getXmitBytes(method_num) + server->getRecvBytes(method_num);
            waitpid(pID, &chld_state, my_opt);
            result.success=true;
        }

    } catch (int& err) {
        sleep(1); // why?
        cout << "handle_error caught" << endl;
        result.success=false;
    }
    
    return result;
}

/**
 * Converts a string into a vector of bytes
 * @param data The string to be converted
 * @require string must have fewer than MAXINT characters
 * @return A vector of bytes, corresponding, one by one, to the characters of data
 */
inline vector<byte> StrToVec(string data) {
    vector<byte> result; // where we will be build the result to be returned

    const char *data_c_str = data.c_str();
    for (int ii = 0; ii < (int) data.length(); ii++)
        result.push_back(data_c_str[ii]);

    return result;
}

/**
 * Converts a vector of bytes into a string.  The opposite of StrToVec.
 * @param data The vector of bytes to be converted
 * @require vector must have fewer than MAXINT characters
 * @return The string whose characters correspond, one by one, to the bytes of data.
 */
inline string VecToStr(vector<byte> data) {
    string result;
    for (int ii = 0; ii < (int) data.size(); ii++)
        result.push_back(data[ii]);
    return result;
}

/**
 * Helper function to turn a string into a an item that can deal with input stream readers
 * @param str What should be converted
 * @return A converted item.  For example, if templated by T = ZZ_p, returns
 * a ZZ_p corresponding to str in the natural sense (i.e. "23" -> 23 as a ZZ_p).
 * @requires The templated type must be initialized, if necessary (e.g. the modulus selected
 * for a ZZ_p)
 */
template <class T>
inline T strTo(string str) {
    if (str.empty())
        throw invalid_argument(str);

    istringstream tmp(str);
    T result;
    tmp >> result;
    return result;
}

/**
 * Helper function to turn anything with a stream printing capability into a string
 * @param item The thing to be converted
 * @return A string representing the number
 */
template <class T>
inline string toStr(T item) {
    ostringstream tmp;
    tmp << item;
    return tmp.str();
}

/**
 * Reinterprets a ustring into a string
 */
inline string ustrToStr(ustring ustr) {
    return string(reinterpret_cast<const char *> ((unsigned char *) ustr.data()), ustr.length());
}

/**
 * Provides a string representing a human-readable version of a list of pointers
 */
template <class T>
string printListOfPtrs(list<T *> theList) {
    string result = "[";
    typename list<T *>::const_iterator iter;
    for (iter = theList.begin(); iter != theList.end(); iter++)
        result += toStr(**iter) + " ";
    result += "]";
    return result;
}

/**
 * Writes the elements of an array as ints to a return string
 */
template <class T>
string writeInts(T *data, int len) {
    string result = "";
    for (int ii = 0; ii < len; ii++)
        result += toStr((char) data[ii]) + " (" + toStr((int) data[ii]) + ") ";
    return result;
}

/**
 * Prints the contents of a map.
 */
template <class S, class T>
inline string printMap(map<S, T> theMap) {
    string result = "";
    typename map<S, T>::const_iterator iter;
    for (iter = theMap.begin(); iter != theMap.end(); iter++) {
        result += toStr(iter->first) + " -> " + toStr(iter->second) + "\n";
    }
    return result;
}

// MULTI-SET OPERATIONS

/**
 * Prints the contents of a multiset of strings into a string in a human-readable fashion
 * @param container The multiset whose contents should be shown in the string
 * @return a string representing the contents of the container
 */
inline string multisetPrint(multiset<string> container) {
    string result = "";
    multiset<string>::const_iterator ii;
    for (ii = container.begin();
            ii != container.end();
            ii++)
        result += "[" + *ii + "] ";
    //result += "["+ writeInts(ii->data(),ii->length()) + "] ";
    return result;
}

/**
 * Returns the multi-set intersection of <first> and <second>.
 * Not particularly efficient ... but it works.
 * @param first A multiset of objects
 * @param second A multiset of the same type of objects as <first>
 * @return the resulting multiset
 */
template <class T>
multiset<T> multisetIntersect(const multiset<T> first, const multiset<T> second) {
    vector<T> resultVec;
    set_intersection(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
    // convert the result to a multiset
    multiset<T> result(resultVec.begin(), resultVec.end());
    return result;
}

/**
 * Returns the multi-set difference <first> - <second>.
 * Not particularly efficient ... but it works.
 * @param first A multiset of objects
 * @param second A multiset of the same type of objects as <first>
 * @return the resulting multiset
 */
template <class T>
multiset<T> multisetDiff(const multiset<T> first, const multiset<T> second) {
    vector<T> resultVec;
    set_difference(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
    // convert the result to a multiset
    multiset<T> result(resultVec.begin(), resultVec.end());
    return result;
}

/**
 * Returns the multi-set union <first> U <second>.
 * Not particularly efficient ... but it works.
 * @param first A multiset of objects
 * @param second A multiset of the same type of objects as <first>
 * @return the resulting multiset
 */
template <class T>
multiset<T> multisetUnion(const multiset<T> first, const multiset<T> second) {
    vector<T> resultVec;
    set_union(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
    // convert the result to a multiset
    multiset<T> result(resultVec.begin(), resultVec.end());
    return result;
}

/**
 * Returns a subset of <size> elements of <first>
 * @param first The multiset from which to choose elements
 * @param size The number of elements to choose
 * @requires <size> < the number of elements in <first>
 * @return A subset of <first> of size <size>
 */
template <class T>
multiset<T> multisetSubset(const multiset<T> first, const int size) {
    multiset<T> result;
    typename multiset<T>::iterator it; // need typename for dependent scope (?)
    int count = 0;
    for (it = first.begin(); count < size; it++, count++)
        result.insert(*it);
    return result;
}

/**
 A generic p-ary tree of type T
 */
template <typename T>
class paryTree {
public:

    /**
     * Construct a p-ary tree of a fixed arity
     * @param datum The actual datum to add - will be deallocated by the destructor
     * @param pary The number of children per node.
     */
    paryTree(T *theDatum, int pary) : arity(pary) {
        datum = theDatum;
        child = new paryTree<T> *[arity];
        for (int ii = 0; ii < arity; ii++) child[ii] = NULL;
    }

    /** Destructor */
    ~paryTree() {
        delete[] child; /** An array of children of the current node. */
        delete datum; /** The payload of the current node. */
    }

    /** Accessor */
    T *getDatum() {
        return datum;
    }

    paryTree<T> **child; /** Full access to all the children of the node. */
private:
    long arity;
    T *datum;
};

const int min_base64 = 62; // first character of base-64 text
const int signed_shift = 128; // shift to get from unsigned to signed

/**
 * Encodes a given ASCII c-style string into a (base64) string using only characters from '>' to '~'
 * @param bytes_to_encode The bytes to encode base 64
 * @param len The length of the bytes array
 * @return An ascii-armored string.
 */
inline string base64_encode(char const* bytes_to_encode, unsigned int in_len) {
    string ret = "";

    int round3 = 3 * (in_len % 3 == 0 ? in_len / 3 : 1 + (in_len / 3)); // the number of whole groups of 3
    // every 3 ASCII characters get converted into four base64 characters
    for (int ii = 0; ii < round3; ii += 3) {
        unsigned long group = signed_shift + bytes_to_encode[ii] +
                256 * (ii + 1 >= (int) in_len ? 0 : signed_shift + bytes_to_encode[ii + 1]) +
                256 * 256 * (ii + 2 >= (int) in_len ? 0 : signed_shift + bytes_to_encode[ii + 2]);
        ret += (char) min_base64 + group % 64;
        ret += (char) min_base64 + (group >> 6) % 64;
        ret += (char) min_base64 + (group >> 12) % 64;
        ret += (char) min_base64 + (group >> 18) % 64;
    }
    // replace the last characters with "=" as needed
    if (in_len % 3 >= 1)
        ret[ ret.length() - 1 ] = '=';
    if (in_len % 3 == 1)
        ret[ ret.length() - 2 ] = '=';

    return ret;
}

/**
 * Decodes a string of characters in the ASCII range '>' to '~' into an ASCII string.
 * The exact inverse of base64_encode
 */

inline string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.length();
    char tmp[in_len];
    strncpy(tmp, encoded_string.data(), in_len);

    // record how much padding was in the string, and remove it
    int rem = 0;
    if (tmp[in_len - 1] == '=') {
        rem++;
        tmp[in_len - 1] = min_base64;
    }
    if (tmp[in_len - 2] == '=') {
        rem++;
        tmp[in_len - 2] = min_base64;
    }

    string ret = "";
    for (int ii = 0; ii < in_len; ii += 4) {
        unsigned long group = (tmp[ii] - min_base64) + 64 * (tmp[ii + 1] - min_base64) + 64 * 64 * (tmp[ii + 2] - min_base64) + 64 * 64 * 64 * (tmp[ii + 3] - min_base64);
        ret += (char) (group % 256) - signed_shift;
        ret += (char) ((group >> 8) % 256) - signed_shift;
        ret += (char) ((group >> 16) % 256) - signed_shift;
    }

    if (rem > 0)
        ret.erase(ret.length() - rem); // erase the last few characters, depending on the number of ='s in the base64 string

    return ret;
}

// additions

/**
 * A simple interface to base64_encode below
 * @param base64_chars
 * @return 
 */
inline string base64_encode(const string bytes, unsigned int in_len) {
    string foo = base64_encode(bytes.data(), in_len);
    return foo;
}

/**
 * @return The minimum of two NTL ZZ objects
 */
inline ZZ min(const ZZ& aa, const ZZ& bb) {
    if (compare(aa,bb)==1) // (aa>?bb)
        return aa;
    else
        return bb;
}

#endif	/* AUX_H */

