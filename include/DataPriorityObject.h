/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */

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

