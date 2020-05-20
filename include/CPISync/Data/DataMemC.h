/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// DataMemC.h -- class for data objects container from memory
#ifndef DATA_MEM_CONTAINER_H
#define DATA_MEM_CONTAINER_H

#include <CPISync/Data/DataObjC.h>
#include <CPISync/Aux/Exceptions.h>

class DataMemC: public DataObjC, public UnimplementedClassException
{
public:
	// constructor
	DataMemC();

	// constructor
	DataMemC(string mAddr);

	// destructor
	~DataMemC();

    // read data from memory
    shared_ptr<DataObject> get(int index);

    // write data into memory
    void put(shared_ptr<DataObject> mydata, int index);

    // write data into memory
    void delObj(int index);

private:
    // the name of the file associated with data object
	int memsize;

    // memory buffer size
	const int CBUFSIZE=1000;

	// the address of the memory block
	char memaddr[CBUFSIZE];

};

#endif
