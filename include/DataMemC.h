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
