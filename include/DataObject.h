/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#ifndef DATA_OBJECT_H
#define DATA_OBJECT_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <list>

#include "UID.h"
#include "Auxiliary.h"

// namespaces
using std::string;
using std::ostream;

/**
 *  DataObject.h -- a wrapper for a generic set element to be synchronized
 */
class DataObject : public UID {
public:
    // Constructors

    /**
     * Construct an empty data object.
     */
    DataObject();

    /**
     * Constructs a data object that contains the given byte vector (not a copy!).
     * @param data An vector of bytes containing the data of the object.
     */
    explicit DataObject(const ZZ &datum);

    /**
     * Constructs a data object that contains the given string (in an encoded format).
     * @param data The string to place in the DataObject.
     */
    explicit DataObject(string str);
    
    /**
     * Constructs a data object that contains the given object of type T, which must
     * be translatable to a string with the global toStr function.  In effect, this constructor
     * simply converts the object to a string and then does the same as the string constructor.
     * @param item The item to place in the DataObject.
     */
    template<typename T>
    explicit DataObject(const T item) {
        myBuffer = pack(toStr(item));
    }
    
    // INFORMATION METHODS


    // MANIPULATION METHODS
    
    /**
     * @return A ZZ encoding of the string
     */
    ZZ to_ZZ() const;

    /**
     * @return The string version of a copy of the contents this data object.
     *         The string could have null bytes and non-printable characters.
     */
    string to_string() const;

    /**
     * @param len Stores the length of the char array (upon return)
     * @return A char array version of the contents of this data object.
     *         The string could have null bytes and non-printable characters.
     */
    const char *to_char_array(long &len) const;

    /**
     * @return A string of printable characters representing the object, essentially
     * a base64 encoding of the internal contents of the DataObject.
     */
    string print() const;

    /**
     *    Allows stream printing of the object
     */
    friend ostream& operator<<(ostream& out, const DataObject &datum);

    // comparisons
    bool operator < (const DataObject& second) const{
        return this->myBuffer < second.to_ZZ();
    }

    bool operator== (const DataObject& second) const {
        return this->myBuffer == second.to_ZZ();
    }
       
    // static variables
    static bool RepIsInt; /** if true, then DataObject string inputs are interpreted as
                           * arbitrarily-sized integers.  Otherwise, DataObject string inputs
                           * are interpreted as byte sequences.
                           */
    void setTimeStamp(clock_t ts);
    clock_t getTimeStamp();
protected:
    ZZ myBuffer; /** The buffer for the data object container itself. */
    clock_t timestamp;

    /**
  * Unpacks a ZZ into a string
  * @param num
  * @return
  */
    static string unpack(ZZ num);

private:
    /**
     * Packs a string into a ZZ in a memory-efficient manner
     * @param theStr The string to pack
     * @return a ZZ representing the string
     */
    static ZZ pack(string theStr);

};
#endif
