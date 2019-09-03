/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   UID.h
 * A class for generating Uniqued IDed objects.
 * Each object has a unique ID (unique over a run of the program)
 * and an (optional) user-specified hash.
 *
 * Created on September 4, 2011, 6:42 PM
 */

#ifndef UID_H
#define	UID_H
#include "NTL/mat_ZZ_p.h"

using namespace NTL;

class UID {
public:

    /** Constructor
     */
    UID() {
        myID = ID_count++;
    }

    /**
     * @return the unique ID of this object.
     */
    int getObjectID() {
        return myID;
    }

private:
    int myID; /** the ID of this object */
    static int ID_count; /** Maintains a count of the number of UIDs created in the program thus far. */
};

#endif	/* UID_H */

