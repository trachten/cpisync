/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// DataFileC.h -- class for data objects container from file
#ifndef DATA_FILE_CONTAINER_H
#define DATA_FILE_CONTAINER_H

#include "DataObjC.h"
#include "Exceptions.h"

class DataFileC: public DataObjC
{
public:
	// constructor
	DataFileC();

	// constructor
	DataFileC(string myfile, string mydir);

	// destructor
	~DataFileC();

	// read a data object from the file
	void get(DataObject* mydata, int index);

	// write a data object into the file
	void put(DataObject* mydata, int index);

	// delete a data object by index
	void delObj(int index);
        
	list<DataObject *> dump() {
		throw new UnimplementedMethodException("Not yet implemented!");
	}

private:
	// the name of the file 
	string filename;

	// the directory of the file
	string directory;
};

#endif
