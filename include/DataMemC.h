/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// DataMemC.h -- class for data objects container from memory
#ifndef DATA_MEM_CONTAINER_H
#define DATA_MEM_CONTAINER_H

#include "DataObjC.h"

class DataMemC: public DataObjC
{
public:
	// constructor
	DataMemC();

	// constructor
	DataMemC(string mAddr);

	// destructor
	~DataMemC();

        // read data from memory 
        DataObject* get(int index);

        // write data into memory
        void put(DataObject* mydata, int index);

        // write data into memory
        void delObj(int index);

private:
        // the name of the file associated with data object
	int memsize;

	// the address of the memory block
	char memaddr[CBUFSIZE];


};

#endif
