/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   Auxiliary.h
 * Auxiliary items shared throughout the program.  These include constants, types, and functions.
 *
 * Created on August 20, 2011, 10:07 PM
 */

#ifndef AUX_H
#define	AUX_H

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <NTL/ZZ.h>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <iterator>
#include <list>
#include <set>
#include <algorithm>
#include <csignal>
#include <algorithm>
#include <sys/wait.h>
#include <climits>
#include <cstring>
#include <memory>
#include <CPISync/Aux/ConstantsAndTypes.h>
#include <CPISync/Aux/Logger.h>

// some standard names
using std::cout;
using std::clog;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::list;
using std::map;
using std::multiset;
using std::invalid_argument;
using std::runtime_error;


// FUNCTIONS

// Based on Bjarne Stroustrup. The C++ Programming Language (4th edition). 2013.
//   ISBN: 978-0-321-56384-2. Chapter 11.5: Explicit type conversion. page 299.
template <class Target, class Source>
Target narrow_cast(Source v)
{
    auto r = static_cast<Target>(v);
    if (static_cast<Source>(r)!=v)
        throw std::runtime_error("narrow_cast<>() failed");
    return r;
}

/**
 * Converts a string into a vector of bytes
 * @param data The string to be converted
 * @require string must have fewer than MAXINT characters
 * @return A vector of bytes, corresponding, one by one, to the characters of data
 */
inline vector<byte> StrToVec(const string& data) {
    vector<byte> result; // where we will be build the result to be returned

    const char *data_c_str = data.c_str();
    result.reserve((int) data.length()); result.reserve((int) data.length()); for (int ii = 0; ii < (int) data.length(); ii++)
        result.push_back(static_cast<byte>(data_c_str[ii]));

    return result;
}

/**
 * Helper function to split string with the appointed char
 * @param str The target string to be splitted
 * @param sep Appointed char where string should be splitted
 * @return arr A vector containing the content of string after splitting
 *
 * Similar to code at https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
 */
inline vector<string> split(const string& str, char sep)
{
    string token;
    vector<std::string> tokens;

    stringstream myStream(str);
    while(std::getline(myStream, token, sep)) {
        tokens.emplace_back(token);
    }

    return tokens;
}

/**
 * Use large number to represent an ascii string
 * @param str the ascii string to be represented
 * @return a large number representing the whole string
 */
inline ZZ strToZZ(string str)
{
    
    const int c_range = narrow_cast<int>(pow(2.0, 8.0 * sizeof(char))); // value range for the char in output string

    ZZ number = conv<ZZ>(str[0]);
    long len = str.length();
    for (long ii = 1; ii < len; ii++)
    {
        number *= c_range;
        number += conv<ZZ>(str[ii]);
    }

    return number;
}

/**
 * Decode an encoded large numebr from strToZZ function into an  ascii string
 * @param num the ZZ to be converted
 * @require input ZZ number must be generated from strToZZ function 
 * @return an ascii string
 */
inline string zzToString(ZZ num)
{
    long len;
    const int c_range = narrow_cast<int>(pow(2.0, 8.0 * sizeof(char))); // value range for the char in input string
    if (num == 0)
        len = 1;
    else
    {
        if (num < 0)
            num = -num;
        len = ceil(log(num) / log(c_range));
    }
    char str[len];

    for (long ii = len - 1; ii >= 0; ii--)
    {
        str[ii] = conv<int>(num % c_range);
        num /= c_range;
    }

    string out;
    for (long ii = 0; ii<len; ii++)
        out += str[ii];
    return out;
}


/**
 * Converts a vector of bytes into a string.  The opposite of StrToVec.
 * @param data The vector of bytes to be converted
 * @require vector must have fewer than MAXINT characters
 * @return The string whose characters correspond, one by one, to the bytes of data.
 */
inline string VecToStr(vector<byte>&& data) {
    string result;
    for (unsigned char ii : data)
        result.push_back(ii);
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
inline T strTo(const string& str) {
    if (str.empty())
        throw invalid_argument(str);

    istringstream tmp(str);
    T result;
    tmp >> result;
    return result;
}

/**
 * Converts a C-style string into a type T.  Similar to {@see strTo(const string& str)}.
 */
template <class T>
inline T charArrayTo(const char* cStr) {
    if (cStr==nullptr)
        throw invalid_argument(cStr);
    return strTo<T>(string(cStr));
}

/**
 * Helper function to turn anything with a stream printing capability into a string
 * @param item The thing to be converted
 * @return A string representing the number
 */
template <class T>
inline string toStr(const T item) {
    ostringstream tmp;
    tmp << item;
    return tmp.str();
}

/**
 * Reinterprets a ustring into a string
 */
inline string ustrToStr(const ustring& ustr) {
    return string(reinterpret_cast<const char *> ((unsigned char *) ustr.data()), ustr.length());
}

/**
 * Provides a string representing a human-readable version of a list of pointers
 */
template <class T>
string printListOfPtrs(list<T *> theList) {
    string result="[";
    typename list<T *>::const_iterator iter;
    for (iter = theList.begin(); iter != theList.end(); iter++)
        result += toStr(**iter) + " ";
    result += "]";
    return result;
}

/**
 * Helper functions for printing data in type of set of sets
 **/
class AuxSetOfSets{
    public:
        /**
         * Function to print set of sets from an iterable input variable
         * @param theList a list with set of sets structure and can be iterated
         * @return a formatted string representing the whole set
         */
        template <class T>
        static string printSetofSets(T theList)
        {
            string result="{ ";
            for (auto itr : theList)
            {
                auto curSet = itr->to_Set();
                result += "[ ";
                for (auto element : curSet)
                {
                    result += toStr(element->to_ZZ()) + " ";
                }
                result += "] \n";
            }
            result += " }";
            return result;
        }
        /**
         * Function to print set from an iterable input variable
         * @param theList a list with set structure and can be iterated
         * @return a formatted string representing the whole set
         */
        template <class T>
        static string printSet(T thelist)
        {
            string result="[ ";
            for (auto itr : thelist)
            {
                result += toStr<ZZ>(itr->to_ZZ()) + " ";
            }
            result += " ]";
            return result;
        }
};

/**
 * Provides a string representing a human-readable version of a list of shared_ptrs
 */
template <class T>
string printListOfSharedPtrs(list<shared_ptr<T>> theList) {
	string result="[";
	typename list<shared_ptr<T>>::const_iterator iter;
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
    string result;
    for (int ii = 0; ii < len; ii++)
        result += toStr((char) data[ii]) + " (" + toStr((int) data[ii]) + ") ";
    return result;
}

/**
 * Prints the contents of a map.
 */
template <class S, class T>
inline string printMap(map<S, T> theMap) {
    string result;
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
inline string multisetPrint(const multiset<string>& container) {
    string result;
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
    std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
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
multiset<T> multisetDiff(multiset<T> first, multiset<T> second) {
    vector<T> resultVec;
    std::set_difference(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
    // convert the result to a multiset
    multiset<T> result(resultVec.begin(), resultVec.end());
    return result;
}

/**
 * Compare if two set have elements with same content. Can also use multisetDiff functions
 *          but that one won't work when input variable is a pointer
 * @param first first multiset to be compared
 * @param second second multiset to be compared
 * @return true iff two sets have same contents
 */
template <class T>
bool cmpMultiset(const multiset<T> first, const multiset<T> second)
{
    multiset<ZZ> fir_vec, sec_vec;

    for(auto ii:first)
        fir_vec.insert(ii->to_ZZ());
    for(auto jj:second)
        sec_vec.insert(jj->to_ZZ());
    
    return (multisetDiff(fir_vec,sec_vec).size() + multisetDiff(sec_vec,fir_vec).size() == 0);
}

/**
 * Functor for comparing pointers to objects by dereferencing these pointers and comparing the resulting objects
 * @tparam T A pointer to an object that s.t. (*T)::operator<(const (*T)&) is defined
 */
template <typename T>
class cmp {
public:
    bool operator()(const T a, const T b) {
        return (*a) < (*b);
    }
};

/**
 * Calculates the set-difference of two containers of pointers, [begA, endA] - [begB, endB].
 * The set-difference is calculated using the cmp functor as a way to order the container
 * Results are appended to coll.
 * @param [begA, endA] Iterators pointing to the beginning and end of the first container.
 * @param [begB, endB] Iterators pointing to the beginning and end of the second container.
 * @param coll Iterator onto which the results of the set difference will be pushed.
 */
template <class IteratorA, class IteratorB, class IteratorOut>
void rangeDiff(IteratorA begA, IteratorA endA, IteratorB begB, IteratorB endB, IteratorOut coll) {
    typedef typename std::iterator_traits<IteratorA>::value_type T;
    set_difference(begA, endA, begB, endB, coll, cmp<T>());
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
    std::set_union(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVec));
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
    paryTree(T *theDatum, long pary) : arity(pary) {
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

/// BASE64 ENCODE/DECODE
const int min_base64 = 62; // first character of base-64 text
const unsigned int signed_shift = 128; // shift to get from unsigned to signed

/**
 * Encodes a given ASCII c-style string into a (base64) string using only characters from '>' to '~'
 * @param bytes_to_encode The bytes to encode base 64
 * @param len The length of the bytes array
 * @return An ASCII-armored string.
 */
inline string base64_encode(char const* bytes_to_encode, long in_len) {
    string ret;

    int round3 = 3 * (in_len % 3 == 0 ? in_len / 3 : 1 + (in_len / 3)); // the number of whole groups of 3
    // every 3 ASCII characters get converted into four base64 characters
    for (int ii = 0; ii < round3; ii += 3) {
        unsigned int group = signed_shift + bytes_to_encode[ii] +
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
    size_t in_len = encoded_string.length();
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

    string ret;
    for (int ii = 0; ii < in_len; ii += 4) {
        auto group = narrow_cast<unsigned long>((tmp[ii] - min_base64) + 64 * (tmp[ii + 1] - min_base64) +
                                                         64 * 64 * (tmp[ii + 2] - min_base64) +
                                                         64 * 64 * 64 * (tmp[ii + 3] - min_base64));
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
inline string base64_encode(const string& bytes) {
    string foo = base64_encode(bytes.data(), bytes.length());
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

// ... NTL functions
/**
 * @return A random integer in [lower, upper]
 * @require srand() must've been called
 */
inline int randLenBetween(int lower, int upper) {
    int length = (rand() % (upper+1));
    if(length < lower) length = lower;
    return length;
}

/**
 * @return A random long
 * @require srand() must've been called
 */
inline long randLong() {
    return (static_cast<long>(rand()) << (sizeof(int) * CHAR_BIT)) | rand(); // lshift the amount of bits in an int and then bitwise or a random int
}

/**
 * @return A random byte
 * @require srand() must've been called
 */
inline byte randByte() {
    return (byte) (rand() % (int) pow(2, CHAR_BIT));
}

/**
 * @return A string of random characters with a random length in [lower, upper]
 * @require srand() must've been called
 */
inline string randString(int lower=0, int upper=10) {
    stringstream str;

    // pick a length in between lower and upper, inclusive
    int length = randLenBetween(lower, upper);

    for(int jj = 0; jj < length; jj++)
        str << (char) randByte(); // generate a random character and add to the stringstream

    return str.str();
}

/**
 * @return A random double in [lower, upper]
 * @require srand() must've been called
 */
inline double randDouble(double lower=0.0, double upper=1.0) {
    return ((double)rand() * (upper - lower)) / (double)RAND_MAX + lower;
}

/**
 * @return A random ZZ s.t. it is <= ZZ(LONG_MAX)
 * @require srand() must've been called
 */
inline ZZ randZZ() {
    return ZZ(randLong());
}

// ... ENUM operations
/**
 * Converts an enum to a byte, signalling an compile-time error if the enum's underlying class is not byte.
 */
template <class T>
inline byte enumToByte(T theEnum) {
    static_assert(std::is_same<byte, typename std::underlying_type<T>::type>::value,
        "Underlying enum class is not byte - cannot convert to byte!");
    return static_cast< byte >(theEnum);
}

/**
 * An awkward helper for iterating enums.
 * @param curr The current enum value
 * @return the next enum value
 */
template <typename T>
inline T &operator++(T& curr) {
    curr = (T)(((int) (curr) + 1));
    return curr;
}

// ... FILES
/**
 * Get the temp directory of the system (POSIX).
 * In C++17, this can be replaced with std::filesystem::temp_directory_path.
 * @return path to temp directory
 */
inline string temporaryDir() {
    // possible environment variables containing path to temp directory
    const char* opts[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

    // return the first defined env var in opts
    for(const char* ss : opts) {

        // true iff ss is an env var
        if(const char* path = getenv(ss)) {
            return string(path);
        }
    }

    // default temp directory if no env var is found
    return string("/tmp");
}

// CLASSES
/**
 * Represents a nullable value.  Either it contains nothing, or it contains an object of type T
 * @tparam T The type of the nullable object
 */
template <class T>
class Nullable {
public:
    /**
     * nulled unless initializated.
     */
     Nullable() { nulled = true; }
    Nullable(T param) { nulled=false; val=param;}

    /**
     * @return The value represented by this class.
     * @throws bad_exception if the object is nulled
     */
    T& operator*() {
        if (nulled)
            throw bad_exception();
        return val;
    }

    /**
     * Assignment from the base class
     */
    Nullable<T>& operator=(const T &other) {
        nulled=false;
        val=other;
        return *this;
    }

    bool isNullQ() { return nulled; }
private:
    bool nulled=true;
    T val;
};

template <class T>
Nullable<T> NOT_SET() { return Nullable<T>(); }
#endif	/* AUX_H */

