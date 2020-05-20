/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// DataFileC.h -- class for data objects container from file
#ifndef DATA_FILE_CONTAINER_H
#define DATA_FILE_CONTAINER_H

#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Aux/Auxiliary.h>
#include "DataObjC.h"

class DataFileC: public DataObjC, public UnimplementedClassException
{
public:
	// constructor
	DataFileC();

	// constructor
	DataFileC(string myfile, string mydir);

	// destructor
	~DataFileC();

	// read a data object from the file
	void get(shared_ptr<DataObject> mydata, int index);

	// write a data object into the file
	void put(shared_ptr<DataObject>, int index);

	// delete a data object by index
	void delObj(int index);
        
	list<shared_ptr<DataObject>> dump() {
		throw new UnimplementedMethodException();
	}

private:
	// the name of the file 
	string filename;

	// the directory of the file
	string directory;
};

#endif
