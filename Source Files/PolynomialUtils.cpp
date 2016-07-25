//
// Created by root on 7/25/16.
//

#include "../Header Files/PolynomialUtils.h"

NTL::GF2E PolynomialUtils::convertBytesToGF2E(uint8_t *elementByts, uint32_t size) {

    //translate the bytes into a GF2X element.
    NTL::GF2X e;
    NTL::GF2XFromBytes(e, elementByts, size);

    //convert the GF2X to GF2E
    return to_GF2E(e);
}

vector<uint8_t> PolynomialUtils::convertElementToBytes(NTL::GF2E & element) {
    //Get the bytes of the random element.
    NTL::GF2X fromEl = NTL::rep(element); //convert the GF2E element to GF2X element.
    int numBytes = NTL::NumBytes(fromEl); //get the number of element bytes.

    vector<uint8_t> arr(numBytes);
    //the function rep returns the representation of GF2E as the related GF2X, it returns as read only.
    BytesFromGF2X(arr.data(), fromEl, numBytes);
    return arr;
}