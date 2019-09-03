//
// Created by Sean Brandenburg on 2019-08-14.
//

#ifndef CPISYNC_STRATAEST_H
#define CPISYNC_STRATAEST_H

#include <CPISync/Syncs/DiffEstimators/DiffEstimator.h>

using std::shared_ptr;

//IBLT based difference estimation based on https://www.ics.uci.edu/~eppstein/pubs/EppGooUye-SIGCOMM-11.pdf

class StrataEst : public DiffEstimator{
	public:
		StrataEst(long difEstError);
		~StrataEst();
		/**
		 * The client side of the set estimation protocol
		 * @return The estimate for the amount of symmetric differences between the two sets
		 */
		long ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>>) override;

		/**
		 * The server side of the set estimation protocol
		 * @return The estimate for the amount of symmetric differences between the two sets
		 */
		long ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>>) override;
	private:
		long estError = -log2(0.1);
};


#endif //CPISYNC_STRATAEST_H
