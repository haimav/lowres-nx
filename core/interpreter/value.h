//
// Copyright 2017 Timo Kloss
//
// This file is part of LowRes NX.
//
// LowRes NX is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LowRes NX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef value_h
#define value_h

#include <stdio.h>
#include "error.h"
#include "rcstring.h"

enum ValueType {
    ValueTypeNull,
    ValueTypeError,
    ValueTypeFloat,
    ValueTypeString,
    ValueForceSize = 0xFFFFFFFF
};

union Value {
    float floatValue;
    struct RCString *stringValue;
    union Value *reference;
    enum ErrorCode errorCode;
};

struct TypedValue {
    enum ValueType type;
    union Value v;
};

enum TypeClass {
    TypeClassAny,
    TypeClassNumeric,
    TypeClassString
};

extern union Value ValueDummy;

struct TypedValue val_makeError(enum ErrorCode errorCode);

#endif /* value_h */
