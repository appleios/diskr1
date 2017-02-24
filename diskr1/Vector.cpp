//
//  Vector.cpp
//  diskr1
//
//  Created by Aziz Latipov on 24.02.17.
//  Copyright © 2017 Aziz Latipov. All rights reserved.
//

#include "Vector.h"

template<class U> Vector<U> Vector<U>::map(std::function<U (U)> transform)
{
    Vector<U> result = Vector<U>(this->size());
    for (auto it = this->begin(); it!=this->end(); it++) {
        U elem = it.first;
        U r = transform(elem);
        result.push_back(r);
    }
    return result;
}

template<class U>Vector<U> Vector<U>::filter(std::function<Bool (U)> included)
{
    Vector<U> result = Vector<U>(this->size());
    for (auto it = this->begin(); it!=this->end(); it++) {
        U elem = it.first;
        Bool yes = included(elem);
        if (yes) {
            result.push_back(elem);
        }
    }
    return result;
}

template<class U> U Vector<U>::reduce(U initial_result, std::function<U (U, U)> next_result)
{
    U result = initial_result;
    for (auto it = this->begin(); it!=this->end(); it++) {
        U elem = it.first;
        result = next_result(result, elem);
    }
    return result;
}
