//
//  main.cpp
//  diskr1
//
//  Created by Aziz Latipov on 24.02.17.
//  Copyright © 2017 Aziz Latipov. All rights reserved.
//

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "ScalarTypes.h"
#include "Printable.h"
#include "Vector.h"

#include <string>
#include <iostream>


const Int SpaceDimension = 8;
const Int Pow2SpaceDimension = 256;

void testAlphaFromIndex();


class BoolVector : public vector<Bool> {
public:
    Bool isEqualToVector(BoolVector &v) {
        if (this->size() != v.size()) {
            return 0;
        }
        size_type size = this->size();
        for (size_type i=0; i<size; i++) {
            Bool a = this->at(i);
            Bool b = v.at(i);
            if ((a==0 && b!=0) || (b==0 && a!=0)) {
                return 0;
            }
        }
        return 1;
    }
    BoolVector() {
    }
    BoolVector(Bool v[SpaceDimension]) {
        this->clear();
        
        for (size_type i=0; i<SpaceDimension; i++) {
            this->push_back(v[i]);
        }
    }
    string description() {
        string result("[");
        for (auto it = begin(); it!=end(); it++) {
            Bool isLast = (it+1) == end();
            string a = (*it == 0) ? "0" : "1";
            string b = a + (isLast ? "" : ", ");
            result += b;
        }
        return result+"]";
    }
};

// maps index value into a binary form
// example:
// input: index = 13
// output: [1,0,1,1,0,0,0,0] which stans for 00001101b
BoolVector alphaFromIndex(Int index) {
    BoolVector result;
    for (Int i=0; i<SpaceDimension; i++) {
        result.push_back(index%2);
        index >>= 1;
    }
    return result;
}

Bool sumLinear(BoolVector alpha, Int components) {
    Bool result = 0;
    for (Int i=0; i<components; i++) {
        result += alpha[i];
    }
    return result % 2;
}

Bool neg(Bool a) {
    return a == 0 ? 1 : 0;
}

Bool x1(BoolVector alpha) {
    return alpha[0];
}

Bool x12(BoolVector a) {
    return sumLinear(a, 2);
}

Bool x123(BoolVector a) {
    return sumLinear(a, 3);
}

Bool x1234(BoolVector a) {
    return sumLinear(a, 4);
}


BoolVector calculateFunction(std::function<Bool (BoolVector)> f) {
    BoolVector result;
    for (Int i=0; i<Pow2SpaceDimension; i++) {
        auto a = alphaFromIndex(i);
        Bool b = f(a);
        result.push_back(b);
    }
    return result;
}

struct TableRow {
private:
    string _functionName;
    std::function<Bool (BoolVector)> _f;
    BoolVector _values;
    
public:
    
    string functionName() {return _functionName;}
    BoolVector values() {
        if(_values.empty()) {
            _values = calculateFunction(_f);
        }
        return _values;
    }
    
    TableRow(string functionName, std::function<Bool (BoolVector)> f) :
    _functionName(functionName), _f(f)
    {
    }
    
    
    string description() {
        string result("");
        result = functionName() + ": [";
        auto begin = values().begin();
        auto end = values().end();
        for (auto it = begin; it!=end; it++) {
            Bool isLast = (it+1) == end;
            string a = (*it == 0) ? "0" : "1";
            string b = a + (isLast ? "" : ", ");
            result += b;
        }
        return result+"]";
    }
};

struct Table {
    vector<TableRow> rows;
    string description() {
        string result("[");
        auto begin = rows.begin();
        auto end = rows.end();
        for (auto it = begin; it!=end; it++) {
            Bool isLast = (it+1) == end;
            string a = it->description();
            string b = a + (isLast ? "" : "\n");
            result += b;
        }
        return result+"]";
    }
};


int main(int argc, const char * argv[])
{
    testAlphaFromIndex();
    TableRow r("x1+x2", [](BoolVector a){ return a[0]+a[1]; });
    cout << r.description();
    int a;
    cin >> a;
    return 0;
}

#pragma mark - Tests -

#pragma mark - Alpha from Index

void testAlphaFromIndexWithInputIndexAndExpectedOutput(Int inputIndex, Bool expectedOutput[SpaceDimension]) {
    BoolVector output(expectedOutput);
    BoolVector result = alphaFromIndex(inputIndex);
    Bool pass = output.isEqualToVector(result);
    std::cout << "testAlpga with inputIndex: " << inputIndex << " ";
    if (pass) {
        std::cout << "PASSED";
    } else {
        std::cout << "FAILED (got: " << result.description() << ")";
    }
    std::cout << std::endl;
}

void testAlphaFromIndex() {
    Bool v0[SpaceDimension] = {0,0,0,0,0,0,0,0};
    Bool v1[SpaceDimension] = {1,0,0,0,0,0,0,0};
    Bool vff[SpaceDimension] = {1,1,1,1,1,1,1,1};
    Bool vfa[SpaceDimension] = {0,1,0,1,1,1,1,1};
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0, v0);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(1, v1);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0xff, vff);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0xfa, vfa);
}
