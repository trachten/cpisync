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
