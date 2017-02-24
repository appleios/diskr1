//
//  BooleanFunction.cpp
//  diskr1
//
//  Created by Aziz Latipov on 24.02.17.
//  Copyright © 2017 Aziz Latipov. All rights reserved.
//

#include "BooleanFunction.hpp"

//class BooleanVector : public Vector<Bool>, public Printable
//{
//public:
//    BooleanVector() {
//    }
//    BooleanVector(Vector<Bool> &other) {
//        for (auto it = other.begin(); it!=other.end(); it++) {
//            this->push_back(*it);
//        }
//    }
//    string description() {
//        string result("[");
//        for (auto it = begin(); it!=end(); it++) {
//            Bool isLast = (it+1) == end();
//            string a = (*it == 0) ? "0" : "1";
//            string b = a + (isLast ? "" : ", ");
//            result += b;
//        }
//        return result+"]";
//    }
//};
//
//
//typedef BooleanVector RawValues;
//
//
//class BooleanFunction : public Printable
//{
//public:
//    RawValues value; // of size 2^SpaceDimention
//    BooleanFunction() {
//        value = prepareValues(SpaceDimension);
//    }
//    string description() {
//        return value.description();
//    }
//protected:
//    virtual RawValues prepareValues(Int dim) {
//        return RawValues();
//    }
//};
//
//class Zero : public BooleanFunction
//{
//public:
//    Zero() {
//        value = prepareValues(SpaceDimension);
//    }
//protected:
//    RawValues prepareValues(Int dim) {
//        RawValues values = RawValues();
//        for (Int i=0; i<Pow2SpaceDimension; i++) {
//            values.push_back(Int(0));
//        }
//        return values;
//    }
//};
//
//void print(Printable *i) {
//    cout << i->description() << endl;
//}

// how to use:
//
//Zero z;
//print(&z);
//BooleanVector zeros = z.value;
//    Vector<Bool> ones = zeros.map([](Bool x){return 1;});
//    BooleanVector o(ones);
//    print(&o);

