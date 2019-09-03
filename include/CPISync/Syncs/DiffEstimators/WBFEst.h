//
// Created by Zifan Wang on 2019-08-20.
//


#ifndef CPISYNC_WBFEST_H
#define CPISYNC_WBFEST_H

#include <CPISync/Syncs/DiffEstimators/DiffEstimator.h>

using std::shared_ptr;


class WBFEst : public DiffEstimator{
    public:

		WBFEst(long difEstError);

		~WBFEst();

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
		long errProb = -log2(0.1); /* negative log of probablity of error */
};

#endif