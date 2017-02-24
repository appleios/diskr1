//
//  Printable.h
//  diskr1
//
//  Created by Aziz Latipov on 24.02.17.
//  Copyright © 2017 Aziz Latipov. All rights reserved.
//

#ifndef Printable_h
#define Printable_h

#include <string>

struct Printable
{
    virtual std::string description() = 0;
};

#endif /* Printable_h */
