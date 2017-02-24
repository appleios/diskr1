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

#include <iomanip>
#include <locale>
#include <sstream>

template <typename T>
std::string NumberToString(T number)
{
    std::ostringstream converter;
    converter << number;
    return converter.str();
}

template <typename T>
std::string NumberToStringFixed(T number, int width)
{
    std::ostringstream converter;
    converter << fixed << setw(width) << setprecision(width) << number;
    return converter.str();
}

const Int SpaceDimension = 8;
const Int Pow2SpaceDimension = 256;

void testAlphaFromIndex();

template<class I, class O>
vector<I, O> fmap(vector<I> v, std::function<O (I)> transform)
{
    vector<O> result;
    for (auto it = v.begin(); it != v.end(); it++) {
        result.push_back(transform(*it));
    }
    return result;
}

template<class U>
vector<U> filter(vector<U> v, std::function<Bool (U)> includes)
{
    vector<U> result;
    for (auto it = v.begin(); it != v.end(); it++) {
        if (includes(*it)) {
            result.push_back(*it);
        }
    }
    return result;
}

template<class U>
U reduce(vector<U> v, U initial_result, std::function<U (U, U)> next_result)
{
    U result = initial_result;
    for (auto it = v.begin(); it != v.end(); it++) {
        result = next_result(result, *it);
    }
    return result;
}


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

Int indexFromAlpha(BoolVector a) {
    Int result = 0;
    for (auto it = a.begin(); it != a.end(); it++) {
        result = (result << 1) + *it;
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

string stringFromIndex(Int idx, Int expectedWidth = 4) {
    string t = NumberToString(idx);
    if (t.length() < expectedWidth) {
        Int d = expectedWidth - t.size();
        switch (d) {
            case 4:
                t += "     ";
                break;
            case 3:
                t += "    ";
                break;
            case 2:
                t += "   ";
                break;
            case 1:
                t += "  ";
                break;
            case 0  :
                t += " ";
                break;
                
            default:
                break;
        }
    }
    return t;
}


class FunctionValuesComputer
{
public:
    virtual BoolVector calculateFunction(std::function<Bool (BoolVector)> f) = 0;
    virtual ~FunctionValuesComputer() {}
};

class FullTableFunctionValuesComputer : public FunctionValuesComputer
{
public:
    BoolVector calculateFunction(std::function<Bool (BoolVector)> f)
    {
        BoolVector result;
        for (Int i=0; i<Pow2SpaceDimension; i++) {
            auto a = alphaFromIndex(i);
            Bool b = f(a);
            result.push_back(b);
        }
        return result;
    }
};

class PartTableFunctionValuesComputer : public FunctionValuesComputer
{
    vector<BoolVector> _space;
    
public:
    
    virtual void setSpace(vector<BoolVector> &space) { _space = space; }
    virtual vector<BoolVector> space() { return _space; }
    
    BoolVector calculateFunction(std::function<Bool (BoolVector)> f)
    {
        BoolVector result;
        vector<BoolVector> line = space();
        for (auto it = line.begin(); it != line.end(); it++) {
            Bool b = f(*it);
            result.push_back(b);
        }
        return result;
    }
};

class TableRow
{
    string _functionName;
    std::function<Bool (BoolVector)> _f;
    
public:
    
    virtual string functionName() { return _functionName; }
    
    virtual BoolVector computeValuesWithComputer(FunctionValuesComputer &computer) {
        return computer.calculateFunction(_f);
    }
    
    TableRow(string functionName, std::function<Bool (BoolVector)> f) :
    _functionName(functionName), _f(f)
    {
    }
    
};

struct Table
{
    vector<TableRow> rows;
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

const string InterItemSpace = "    ";

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
                result += toString(at(i,j)) + InterItemSpace;
            }
            result += "\n";
        }
        return result;
    }
};

struct PartialFunctionTableResult
{
    BoolVector input;
    vector<Bool> results;
};

class LinearFunctionsTable
{
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
    
    ImmutableMatrix<Bool> toImmutableMatrixWithComputer(FunctionValuesComputer &computer) {
        Table t = table();

        Int rowCount = t.rows.size();
        Int columCount = Pow2SpaceDimension;
        
        return ImmutableMatrix<Bool>(rowCount, columCount, [&t, &computer](Int row, Int col){
            return t.rows[row].computeValuesWithComputer(computer)[col];
        });
    }
    
    vector<vector<Bool>> valuesPerRowOnSpaceWithComputer(vector<BoolVector> space,
                                                         FunctionValuesComputer &computer)
    {
        Table t = table();
        vector<vector<Bool>> valuesPerRow;
        
        for (auto it = t.rows.begin(); it != t.rows.end(); it++) {
            TableRow r = *it;
            auto output = r.computeValuesWithComputer(computer);
            valuesPerRow.push_back(output);
        }

        return valuesPerRow;
    }
};


class BooleanCoub
{
    vector<BoolVector> _allSets;
    
public:
    
    virtual vector<BoolVector> allSets() {
        if (_allSets.empty()) {
            for (Int i=0; i<Pow2SpaceDimension; i++) {
                auto t = alphaFromIndex(i);
                _allSets.push_back(t);
            }
        }
        return _allSets;
    }
    
    vector<BoolVector> layer(Int lvl) {
        return filter<BoolVector>(allSets(), [lvl](BoolVector v) {
            Int sum = 0;
            for (Int i=0; i<v.size(); i++) {
                sum += v[i] == 0 ? 0 : 1;
            }
            return (sum == lvl) ? 1 : 0;
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
    
    string indexesFromZeroTo(Int maxIndex, Int expectedWidth = 4)
    {
        string result;
        
        for (Int idx=0; idx<maxIndex; idx++) {
            result += stringFromIndex(idx);
        }
        
        return result;
    }
    
    string rawDescription()
    {
        string result;
        
        result += indexesFromZeroTo(Pow2SpaceDimension);
        
        result += "\n";
        
        result += matrix().rawDescription([](Bool a){return a == 0 ? string("0") : string("1"); });

        
        vector<Int> numberOfOnes = countInColumn([](Bool b){ return b!=0 ? 1 : 0; });
        vector<Int> numberOfZeros = countInColumn([](Bool b){ return b==0 ? 1 : 0; });
        
        result += "\n";
        
        Int columCount = matrix().columnCount();
        for (Int j=0; j<columCount; j++) {
            result += NumberToString(numberOfOnes[j]) + InterItemSpace;
        }
        result += "\n";
        
        for (Int j=0; j<columCount; j++) {
            result += NumberToString(numberOfZeros[j]) + InterItemSpace;
        }
        result += "\n";


        vector<Int> stats1 = eachValueCountInSequence(numberOfOnes);
        vector<Int> stats0 = eachValueCountInSequence(numberOfZeros);
        result += "\n";

        result += descrptionForEachValueCountInSequence(stats1);
        result += descrptionForEachValueCountInSequence(stats0);
        
        return result;
    }
    
    string descrptionForEachValueCountInSequence(vector<Int> eachValueCountInSequence) {
        string result;
        
        Int idx = 0;
        
        for (auto it = eachValueCountInSequence.begin(); it != eachValueCountInSequence.end(); it++) {
            if (*it != -1) {
                result += NumberToString(idx) + " => " + NumberToString(*it) + ", ";
            }
            idx++;
        }
        
        Int sum = reduce<Int>(eachValueCountInSequence, 0 , [](Int prev, Int next) {
            if(next>-1)
                return prev+next;
            else
                return prev;
        });
        result += "sum: " + NumberToString(sum) + "\n";

        return result;
    }
    
    vector<Int> countInColumn(std::function<Int (Bool)> extractValue) {
        vector<Int> r;
        
        Int rowCount = matrix().rowCount();
        Int columCount = matrix().columnCount();
        
        for (Int j=0; j<columCount; j++) {
            Int s = 0;
            for (Int i=0; i<rowCount; i++) {
                Bool b = matrix().at(i,j);
                s += extractValue(b);
            }
            
            r.push_back(s);
        }
        
        return r;
    }
    
    vector<Int> eachValueCountInSequence(vector<Int> seq) {
        vector<Int> r;
        
        for (Int i=0; i<256; i++) {
            r.push_back(-1);
        }
        
        Int columCount = matrix().columnCount();
        
        for (Int j=0; j<columCount; j++) {
            r[seq[j]]+=1;
        }
        
        for (Int i=0; i<256; i++) {
            if (r[i] > -1) {
                r[i]++;
            }
        }
        
        return r;
    }
};

void runTests();

void writeStringToFile(string s, string fileName)
{
    ofstream file;
    file.open(fileName, ios::out);
    file << s;
    file.close();
}

int main(int argc, const char * argv[])
{
//    runTests();
    FullTableFunctionValuesComputer fullFomputer;
    LinearFunctionsTable linearFunctionsTable;
    
    ImmutableMatrix<Bool> m = linearFunctionsTable.toImmutableMatrixWithComputer(fullFomputer);
    LinearFunctionsTableMatrix linearFunctionsTableMatrix(m);
    string description = linearFunctionsTableMatrix.rawDescription();
    
    BooleanCoub coub;
    PartTableFunctionValuesComputer partComputer;
    
    // print by layer
    for (Int i=1; i<=7; i++) {
        vector<BoolVector> layer = coub.layer(i);
        partComputer.setSpace(layer);
        
        description += "layer: " + NumberToString(i) + "\n";
        description += "space: \n";
        for (Int i=0; i<layer.size(); i++) {
            BoolVector a = layer[i];
            Int idx = indexFromAlpha(a);
            string t = stringFromIndex(idx);
            
            description += t + " ";
        }
        
        description += "\n";

        vector<vector<Bool>> valuesPerRow = linearFunctionsTable.valuesPerRowOnSpaceWithComputer(layer, partComputer);

        for (Int i=0; i<valuesPerRow.size(); i++) {
            vector<Bool> row = valuesPerRow[i];
            for (Int j=0; j<row.size(); j++) {
                description += NumberToString(row[j]) + "     ";
            }
            description += "\n";
        }
        
        description += "\n\n";
    }

    writeStringToFile(description, "test.txt");
    cout << description;

    int a;
    cin >> a;
    return 0;
}

#pragma mark - Tests -

#pragma mark - Alpha from Index

// TODO: added a test that verifies that matrix representations is equal to table calculation (!)

void runTests()
{
    testAlphaFromIndex();
//    TableRow r("x1+x2", [](BoolVector a){ return a[0]+a[1]; });
//    cout << r.description();
}

void testAlphaFromIndexWithInputIndexAndExpectedOutput(Int inputIndex, Bool expectedOutput[SpaceDimension]);

void testAlphaFromIndex()
{
    Bool v0[SpaceDimension] = {0,0,0,0,0,0,0,0};
    Bool v1[SpaceDimension] = {1,0,0,0,0,0,0,0};
    Bool vff[SpaceDimension] = {1,1,1,1,1,1,1,1};
    Bool vfa[SpaceDimension] = {0,1,0,1,1,1,1,1};
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0, v0);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(1, v1);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0xff, vff);
    testAlphaFromIndexWithInputIndexAndExpectedOutput(0xfa, vfa);
}

void testAlphaFromIndexWithInputIndexAndExpectedOutput(Int inputIndex, Bool expectedOutput[SpaceDimension])
{
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
