//
// Created by shubham on 9/11/20.
//

#ifndef CPISYNC_SERIALIZABLE_H
#define CPISYNC_SERIALIZABLE_H

#include <vector>

using namespace std;

class Serializable {
public:

    virtual ~Serializable() = default;

    /**
     * create a byte representation of data structure, to send over socket
     * @return vector<byte> representation of members
     */
    virtual vector<byte> toByteVector() const = 0;

    /**
     * recreate internal data structure from byte representation received over socket
     * @param buffer vector<byte>
     */
    virtual void fromByteVector(vector<byte> buffer) = 0;
};

#endif //CPISYNC_SERIALIZABLE_H
