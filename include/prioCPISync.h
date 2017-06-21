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
#include "DataObject.h"
#include "InterCPISync.h"
using namespace std;

class PriorityCPISync : public InterCPISync {

public:

  PriorityCPISync(long m_bar, long bits, int epsilon, int partition): InterCPISync(m_bar, bits +1, epsilon,partition)
	{SyncID = SYNCTYPE_Priority_CPISync; useExisting = true;}
  ~PriorityCPISync();
  string getName() { return "Priority CPISync\n";}
   
  bool addElem(DataObject* newDatum);
  bool delElem(DataObject* newDatum);
  bool SyncClient(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf);
  bool SyncServer(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf);
  ZZ_p hash(DataObject* datum) const;
  void translateMaps(list<DataObject*>& tempSelfMinusOther,  list<DataObject*>& selfMinusOther);
  void sendResults(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf);
  void receiveResults(Communicant* commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf);
protected:
        map< DataObject , DataObject * > priority_map;
};

#endif
