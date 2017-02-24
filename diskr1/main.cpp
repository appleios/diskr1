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
#include <fstream>


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

Bool x123(BoolVector a) {
    return sumLinear(a, 3);
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

class TableRow {
    string _functionName;
    std::function<Bool (BoolVector)> _f;
    BoolVector _values;
    
public:
    
    virtual string functionName() {return _functionName;}
    virtual BoolVector values() {
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
    
    string rawDescription() {
        string result("");
        auto begin = values().begin();
        auto end = values().end();
        for (auto it = begin; it!=end; it++) {
            Bool isLast = (it+1) == end;
            string a = (*it == 0) ? "0" : "1";
            string b = a + (isLast ? "" : " ");
            result += b;
        }
        return result;
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
    string rawDescription() {
        string result("");
        auto begin = rows.begin();
        auto end = rows.end();
        for (auto it = begin; it!=end; it++) {
            Bool isLast = (it+1) == end;
            string a = it->rawDescription();
            string b = a + (isLast ? "" : "\n");
            result += b;
        }
        return result;
    }
    Bool isEmpty() {
        return rows.empty();
    }
    void addRow(TableRow r) {
        rows.push_back(r);
    }
};

struct AssertError
{
};

void assert(bool expression) {
    if (!expression) {
        throw AssertError();
    }
}

template<class U> class ImmutableMatrix
{
    Int _rowCount;
    Int _columnCount;
    U **_data;
public:
    
    Int rowCount() { return _rowCount; }
    Int columnCount() { return _columnCount; }
    
    U at(Int row, Int column) {
        assert(row >=0 && row < _rowCount && column >=0 && column < _columnCount);
        return _data[row][column];
    }
    
    ImmutableMatrix(Int rowCount, Int columnCount, std::function<U (Int, Int)> generator)
    : _rowCount(rowCount), _columnCount(columnCount)
    {
        assert(rowCount >= 0);
        assert(columnCount >=0);
        
        _data = new U*[rowCount];
        for (Int i=0; i<rowCount; i++) {
            _data[i] = new U[columnCount];
            for (Int j=0; j<columnCount; j++) {
                _data[i][j] = generator(i, j);
            }
        }
    }
    
    string rawDescription(std::function<string (U)> toString) {
        string result("");
        Int i,j, n = rowCount(), m = columnCount();
        for (i=0; i<n; i++) {
            for (j=0; j<m; j++) {
                result += toString(at(i,j)) + " ";
            }
            result += "\n";
        }
        return result;
    }
};

class LinearFunctionsTable {
    Table _table;
public:
    virtual Table table() {
        if(_table.isEmpty()) {
            _table.addRow(TableRow("0", [](BoolVector a){ return 0; }));
            _table.addRow(TableRow("x1", [](BoolVector a){ return a[0]; }));
            _table.addRow(TableRow("x1+x2", [](BoolVector a){ return a[0]+a[1]; }));
            _table.addRow(TableRow("x1+x2+x3", [](BoolVector a){ return sumLinear(a,3); }));
            _table.addRow(TableRow("x1+x2+x3+x4", [](BoolVector a){ return sumLinear(a,4); }));
            _table.addRow(TableRow("x1+x2+x3+x4+x5", [](BoolVector a){ return sumLinear(a,5); }));
            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6", [](BoolVector a){ return sumLinear(a,6); }));
            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6+x7", [](BoolVector a){ return sumLinear(a,7); }));
            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6+x7+x8", [](BoolVector a){ return sumLinear(a,8); }));
//            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6+x7+x8+1", [](BoolVector a){ return neg(sumLinear(a,8)); }));
//            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6+x7+1", [](BoolVector a){ return neg(sumLinear(a,7)); }));
//            _table.addRow(TableRow("x1+x2+x3+x4+x5+x6+1", [](BoolVector a){ return neg(sumLinear(a,6)); }));
//            _table.addRow(TableRow("x1+x2+x3+x4+x5+1", [](BoolVector a){ return neg(sumLinear(a,5)); }));
//            _table.addRow(TableRow("x1+x2+x3+x4+1", [](BoolVector a){ return neg(sumLinear(a,4)); }));
//            _table.addRow(TableRow("x1+x2+x3+1", [](BoolVector a){ return neg(sumLinear(a,3)); }));
//            _table.addRow(TableRow("x1+x2+1", [](BoolVector a){ return neg(a[0]+a[1]); }));
//            _table.addRow(TableRow("x1+1", [](BoolVector a){ return neg(a[0]); }));
//            _table.addRow(TableRow("1", [](BoolVector a){ return 1; }));
        }
        return _table;
    }
    string description() {
        return table().description();
    }
    string rawDescription() {
        return table().rawDescription();
    }
    
    ImmutableMatrix<Bool> toImmutableMatrix() {
        Table t = table();

        Int rowCount = t.rows.size();
        Int columCount = Pow2SpaceDimension;
        
        return ImmutableMatrix<Bool>(rowCount, columCount, [&t](Int row, Int col){
            return t.rows[row].values()[col];
        });
    }
};

class LinearFunctionsTableMatrix
{
    ImmutableMatrix<Bool> _matrix;
    
public:
    
    virtual ImmutableMatrix<Bool> matrix() { return _matrix; }
    
    LinearFunctionsTableMatrix(ImmutableMatrix<Bool> &matrix)
    : _matrix(matrix)
    {
        
    }
    string rawDescription() {
        return matrix().rawDescription([](Bool a){return a == 0 ? string("0") : string("1"); });
    }
};

void runTests();

void writeStringToFile(string s, string fileName) {
    ofstream file;
    file.open(fileName, ios::out);
    file << s;
    file.close();
}

int main(int argc, const char * argv[])
{
//    runTests();
    LinearFunctionsTable linearFunctionsTable;
    ImmutableMatrix<Bool> m = linearFunctionsTable.toImmutableMatrix();
    LinearFunctionsTableMatrix linearFunctionsTableMatrix(m);
    string description = linearFunctionsTableMatrix.rawDescription();
    cout << description;
    writeStringToFile(description, "test.txt");
    int a;
    cin >> a;
    return 0;
}

#pragma mark - Tests -

#pragma mark - Alpha from Index

// TODO: added a test that verifies that matrix representations is equal to table calculation (!)

void runTests() {
    testAlphaFromIndex();
    TableRow r("x1+x2", [](BoolVector a){ return a[0]+a[1]; });
    cout << r.description();
}

void testAlphaFromIndexWithInputIndexAndExpectedOutput(Int inputIndex, Bool expectedOutput[SpaceDimension]);

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
