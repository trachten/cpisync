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
#include <CPISync/Data/DataObject.h>

using namespace NTL;

class DataPriorityObject : public DataObject {

public:
    DataPriorityObject() : DataObject() { initFields(); }

    explicit DataPriorityObject(const string &str) : DataObject(str) { initFields(); }

    explicit DataPriorityObject(const ZZ &datum) : DataObject(datum) { initFields(); }

    explicit DataPriorityObject(clock_t ts) : DataObject() {
        timestamp=ts;
    }


    ZZ getPriority() {
        return priority;
    }

        void setPriority(const ZZ &pri) {
            priority = pri;
        };

    string to_priority_string() const {
        return toStr(priority) + "," + (RepIsInt?toStr(myBuffer):unpack(myBuffer));
    }

private:
    void initFields() {priority=0; }
    ZZ priority;
};


#endif	/* DATAPRIORITYOBJECT_H */

