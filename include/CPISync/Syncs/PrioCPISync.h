/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * Implements a Priority CPISync algorithm
 *
 *
 * Author: Paul
 * Created on June 25, 2013, 04:31 PM
 */

#ifndef PRIO_CPI_H
#define	PRIO_CPI_H

#include <string>
#include <CPISync/Data/DataObject.h>
#include <CPISync/Syncs/InterCPISync.h>
using namespace std;

class PriorityCPISync : public InterCPISync {

public:

  PriorityCPISync(long m_bar, long bits, int epsilon, int partition): InterCPISync(m_bar, bits +1, epsilon,partition)
	{
		SyncID = SYNC_TYPE::Priority_CPISync;
		useExisting = true;
	}

  ~PriorityCPISync() {}
  string getName() { return "Priority CPISync\n";}

protected:
        map< DataObject , shared_ptr<DataObject> > priority_map;
};

#endif
