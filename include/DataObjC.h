/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

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
