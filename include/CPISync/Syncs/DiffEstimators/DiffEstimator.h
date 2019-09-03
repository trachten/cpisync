//
// Created by Sean Brandenburg on 2019-08-14.
//

#ifndef CPISYNC_DIFFESTIMATOR_H
#define CPISYNC_DIFFESTIMATOR_H

#include <CPISync/Aux/ConstantsAndTypes.h>
#include <CPISync/Communicants/Communicant.h>
#include <CPISync/Data/DataObject.h>
#include <list>


using std::shared_ptr;

/**
 * A class for managing the estimation of symmetric differences with various protocols when a parameter is not provided
 */
class DiffEstimator {
	public:

		/**
		 * @return the protocol that this object uses to estimate the number of set differences
		 */
		 DIFF_EST getProtocol() {return myProtocol;};

		/**
		 * The client side of the set estimation protocol
		 * @return The estimate for the amount of symmetric differences between the two sets
		 */
		virtual long ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>>) = 0;

		/**
		 * The server side of the set estimation protocol
		 * @return The estimate for the amount of symmetric differences between the two sets
		 */
		virtual long ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>>) = 0;

	protected:
		/**
		 * The protocol you are using to estimate the amount of set differences
		 */
		DIFF_EST myProtocol;
};


#endif //CPISYNC_DIFFESTIMATOR_H
