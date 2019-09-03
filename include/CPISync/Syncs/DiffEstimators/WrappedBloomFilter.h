//
// Created by Zifan Wang on 2019-08-20.
//

#ifndef CPISYNC_WRAPPEDBLOOMFILTER_H
#define CPISYNC_WRAPPEDBLOOMFILTER_H

#include <CPISync/Aux/ConstantsAndTypes.h>
#include <vector>

typedef unsigned long int hash_t;

using std::shared_ptr;
using std::vector;
using std::pair;


//Wrapped Bloom Filter based on http://www.bu.edu/phpbin/cise/download.php?publication_id=652

class WrappedBloomFilter {

	public:

		// Constructor
		WrappedBloomFilter(size_t expEntries);
		WrappedBloomFilter(vector<long> cells);


		// Destructor
		~WrappedBloomFilter();

		WrappedBloomFilter(); 			/* default constructor*/


		/**
		 * Insert an element into wrapped filter
		 * @param value the element to be inserted
		 */
		void insert(ZZ value);

		/**
		 * Delete an element in the wrapped filter by its value
		 * @param value the elem to be deleted
		 */
		void erase(ZZ value);

		/**
		 * Total number of cells in the current WBF
		 * @return # cells
		 **/
		size_t size(){return hashTable.size();}

		/**
		 * Add errors caused by false postive and make sure the result gives a result no less than actual
		 * one with probablity prob
		 * @param sizeA # of elements inserted in current WBF
		 * @param expDiff estimated difference
		 * @param errProb success probablity 
		 * */
		long errCorrect(long setA, long errProb, long expDiff);

		/**
		 * Print out current entries in the WBF in pretty format
		 * @return formatted strings containing all entries' info
		 **/
		string toString();

		/**
		 * Unwrap WBF from a given dataset
		 * @param theList dataset used to unwrap WBF
		 * @return symm difference (lower bound) using WBF estimation
		 **/
		template <class T>
		long unwrap(T theList){
			long bucketPerHash = hashTable.size() / numHashFuncs;
			long deltaB = 0;
			for(auto elem: theList){
				vector<long> tmp = hashTable;

				bool exist = true;
				for(long ii=0; ii< numHashFuncs; ii++){

					hash_t hk = _hashK(elem->to_ZZ(), ii);
					
					auto &curEntry = tmp.at(ii * bucketPerHash + hk % bucketPerHash);
					if(curEntry != 0)
						curEntry --;
					else{
						exist = false;
						break;
					}
				}
				if(exist)
					hashTable = tmp;
				else
					deltaB ++;
			
			}
			
			long sumA = 0;

			for(auto elem: hashTable)
				sumA += elem;

			return sumA/numHashFuncs + deltaB;
		}
		
		/**
		 * Get content in current WBF by index
		 * @param index the content's index
		 * @return hashTable[index]
		 * */
		long getCell(long index);

	private:
		const long numHashFuncs = 8; 	/* # of hash functions for this bloom filter*/

		// Returns the kk-th unique hash of the zz that produced initial.
		static hash_t _hashK(const ZZ &item, long kk);
		static hash_t _hash(const hash_t& initial, long kk);
		void _insert(int plus, ZZ value);
		
		// bookkeeping all entries
		vector<long> hashTable;


};




#endif //CPISYNC_WRAPPEDBLOOMFILTER_H
