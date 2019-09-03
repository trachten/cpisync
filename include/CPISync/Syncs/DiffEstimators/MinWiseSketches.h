//
// Created by Sean Brandenburg on 2019-08-14.
//

#ifndef CPISYNC_MINWISESKETCHES_H
#define CPISYNC_MINWISESKETCHES_H

#include <CPISync/Syncs/DiffEstimators/DiffEstimator.h>

using std::shared_ptr;

//Min-wise sketches based on http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.121.8215&rep=rep1&type=pdf

class MinWiseSketches : public DiffEstimator {
	public:

		//Constructor
		MinWiseSketches(int hashes);

		//Destructor
		~MinWiseSketches();

		/**
		* The client side of the set estimation protocol
		* @return The estimate for the amount of symmetric differences between the two sets
		*/
		long ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData) override;

		/**
		 * The server side of the set estimation protocol
		 * @return The estimate for the amount of symmetric differences between the two sets
		 */
		long ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData) override;

	private:

		hash_t _hash(const hash_t& initial, long kk);

		hash_t _hashK(const ZZ &item, long kk);

		/**
		 * The number of hashes that should be used to compare your sets. The more hashes used, the lower error in the
		 * estimate of the size of the set intersection
		 */
		int numHashes;
};


#endif //CPISYNC_MINWISESKETCHES_H
