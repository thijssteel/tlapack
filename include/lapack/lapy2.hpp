// Copyright (c) 2012-2021, University of Colorado Denver. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.
//
// Created by
/// @author Weslley S Pereira, University of Colorado Denver, USA
//
// Adapted from https://github.com/langou/latl/blob/master/include/lapy2.h
/// @author Rodney James, University of Colorado Denver, USA

#ifndef __LAPY2_HH__
#define __LAPY2_HH__

#include "lapack/types.hpp"

#include "blas/utils.hpp"

namespace lapack {

/** Finds $\sqrt{x^2+y^2}$, taking care not to cause unnecessary overflow.
 * 
 * @return $\sqrt{x^2+y^2}$
 *
 * @tparam real_t Floating-point type.
 * @param[in] x scalar value x
 * @param[in] y scalar value y
 * 
 * @ingroup auxiliary
 */
template< typename real_t >
void lapy2(
    real_t x, real_t y )
{
    using blas::abs;
    using blas::sqrt;

    // constants
    const real_t one(1.0);
    const real_t zero(0.0);
    const real_t xabs = abs(x);
    const real_t yabs = abs(y);

    real_t w, z;
    if( xabs > yabs ) {
        w = xabs;
        z = yabs;
    }
    else {
        w = yabs;
        z = xabs;
    }

    return ( z == zero )
        ? w
        : w * sqrt( one + (z/w)*(z/w) );
}

}

#endif // __LAPY2_HH__