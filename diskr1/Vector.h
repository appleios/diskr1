//
//  Vector.hpp
//  diskr1
//
//  Created by Aziz Latipov on 24.02.17.
//  Copyright © 2017 Aziz Latipov. All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include <functional>

#include "ScalarTypes.h"

template<class U> class Vector : public std::vector<U>
{
public:
    Vector<U> map(std::function<U (U)> transform);
    
    Vector<U> filter(std::function<Bool (U)> included);
    
    U reduce(U initial_result, std::function<U (U, U)> next_result);
};

#endif /* Vector_hpp */
