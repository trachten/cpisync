/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   DataPriorityObject.h
 * This is a prioritized version of the data object - it maintains a priority for the given object,
 * and a timestamp when the object was created.
 *
 * Created on September 4, 2011, 8:44 PM
 */

#ifndef DATAPRIORITYOBJECT_H
#define	DATAPRIORITYOBJECT_H
#include "NTL/mat_ZZ_p.h"
#include "DataObject.h"

using namespace NTL;

class DataPriorityObject : public DataObject {
public:

    DataPriorityObject(ZZ pri, clock_t ts) : DataObject() {
        priority = pri;
        timestamp=ts;
    }
    // MORE CONSTRUCTORS NEEDED

    ZZ getPriority();

    void setPriority(ZZ priority);

private:
    ZZ priority;
    clock_t timestamp;
};


#endif	/* DATAPRIORITYOBJECT_H */

