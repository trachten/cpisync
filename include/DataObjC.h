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

#ifndef DATA_OBJECT_CONTAINER_H 
#define DATA_OBJECT_CONTAINER_H 

#include "DataObject.h" 

/**
 * Data Object Container
 * A generic container for storing data objects, which are elements of
 * the set that is to be synchronized.
 */

class DataObjC 
{ public: 

	// Constructors
	DataObjC(); 


	// Input/output
/**
 * @param index starts at 0, as in C arrays
 * @returns the index-th entry in the data object container
 */
        virtual DataObject* get(int index)=0;

	// pure virtual function for writing a data object
       // index starts at 1?
        virtual void put(DataObject* mydata)=0;

	// pure virtual function for deleting a data object by index
	virtual void delObj(int index)=0;

	// get the number of objects;
	int size();
        
        // output a list of pointers to the objects in the container
        virtual list<DataObject *> dump()=0;

protected:
	// number of objects in this container
	int numObj;
};

#endif
