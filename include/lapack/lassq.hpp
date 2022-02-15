/// @file lassq.hpp
/// @author Weslley S Pereira, University of Colorado Denver, USA
/// 
/// Anderson E. (2017)
/// Algorithm 978: Safe Scaling in the Level 1 BLAS
/// ACM Trans Math Softw 44:1--28
/// @see https://doi.org/10.1145/3061665
//
// Copyright (c) 2012-2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __LASSQ_HH__
#define __LASSQ_HH__

#include "lapack/types.hpp"
#include "lapack/utils.hpp"

namespace lapack {

/** Updates a sum of squares represented in scaled form.
 * \[
 *      scl_{[OUT]}^2 sumsq_{[OUT]} = \sum_{i = 0}^n |x_i|^2 + scl_{[IN]}^2 sumsq_{[IN]},
 * \]
 * The value of  sumsq  is assumed to be non-negative.
 * 
 * If (scale * sqrt( sumsq )) > tbig on entry then
 *    we require:   scale >= sqrt( TINY*EPS ) / sbig   on entry,
 * and if 0 < (scale * sqrt( sumsq )) < tsml on entry then
 *    we require:   scale <= sqrt( HUGE ) / ssml       on entry,
 * where
 *    tbig -- upper threshold for values whose square is representable;
 *    sbig -- scaling constant for big numbers; @see blas/constants.hpp
 *    tsml -- lower threshold for values whose square is representable;
 *    ssml -- scaling constant for small numbers; @see blas/constants.hpp
 * and
 *    TINY*EPS -- tiniest representable number;
 *    HUGE     -- biggest representable number.
 * 
 * @tparam TX   Type of the elements in x
 * @tparam Func Type of the function used for computing the absolute value
 *      @see lassq( blas::idx_t, TX const*, blas::int_t, real_type<TX> &, real_type<TX> & )
 *      for the example using blas::abs 
 * 
 * @param[in] n The number of elements to be used from the vector x.
 * @param[in] x Array of dimension $(1+(n-1)*\abs(incx))$.
 * @param[in] incx. The increment between successive values of the vector x.
 *          If incx > 0, X(i*incx) = x_i for 0 <= i < n
 *          If incx < 0, X((n-i-1)*(-incx)) = x_i for 0 <= i < n
 *          If incx = 0, x isn't a vector so there is no need to call
 *          this subroutine.  If you call it anyway, it will count x_0
 *          in the vector norm n times.
 * @param[in] scl
 * @param[in] sumsq
 * 
 * @ingroup norm
 */
template< class vector_t, class abs_t >
void lassq(
    const vector_t& x,
    real_type< type_t<vector_t> > &scl,
    real_type< type_t<vector_t> > &sumsq,
    abs_t absFunc )
{
    using real_t = real_type< type_t<vector_t> >;
    using idx_t  = size_type< vector_t >;
    using blas::isnan;
    using blas::sqrt;

    #define SQUARE(x) (x)*(x)

    // constants
    const idx_t n = size(x);
    const real_t zero( 0 );
    const real_t one( 1 );
    const real_t tsml = blas::blue_min<real_t>();
    const real_t tbig = blas::blue_max<real_t>();
    const real_t ssml = blas::blue_scalingMin<real_t>();
    const real_t sbig = blas::blue_scalingMax<real_t>();

    // quick return
    if( isnan(scl) || isnan(sumsq) ) return;

    if( sumsq == zero ) scl = one;
    if( scl == zero ) {
        scl = one;
        sumsq = zero;
    }

    // quick return
    if( n <= 0 ) return;

    //  Compute the sum of squares in 3 accumulators:
    //     abig -- sums of squares scaled down to avoid overflow
    //     asml -- sums of squares scaled up to avoid underflow
    //     amed -- sums of squares that do not require scaling
    //  The thresholds and multipliers are
    //     tbig -- values bigger than this are scaled down by sbig
    //     tsml -- values smaller than this are scaled up by ssml

    real_t asml = zero;
    real_t amed = zero;
    real_t abig = zero;

    for (idx_t i = 0; i < n; ++i)
    {
        real_t ax = absFunc( x[i] );
        if( ax > tbig )
            abig += SQUARE(ax*sbig);
        else if( ax < tsml ) {
            if( abig == zero ) asml += SQUARE(ax*ssml);
        } else
            amed += SQUARE(ax);
    }

    // Put the existing sum of squares into one of the accumulators
    if( sumsq > zero ) {
        real_t ax = scl * sqrt( sumsq );
        if( ax > tbig )
            abig += SQUARE(scl*sbig) * sumsq;
        else if( ax < tsml ) {
            if( abig == zero ) asml += SQUARE(scl*ssml) * sumsq;
        } else
            amed += SQUARE(scl) * sumsq;
    }

    // Combine abig and amed or amed and asml if
    // more than one accumulator was used.

    if( abig > zero ) {
        // Combine abig and amed if abig > 0
        if( amed > zero || isnan(amed) )
            abig += (amed*sbig)*sbig;
        scl = one / sbig;
        sumsq = abig;
    }
    else if( asml > zero ) {
        // Combine amed and asml if asml > 0
        if( amed > zero || isnan(amed) ) {
            
            amed = sqrt(amed);
            asml = sqrt(asml) / ssml;
            
            real_t ymin, ymax;
            if( asml > amed ) {
                ymin = amed;
                ymax = asml;
            } else {
                ymin = asml;
                ymax = amed;
            }

            scl = one;
            sumsq = SQUARE(ymax) * ( one + SQUARE(ymin/ymax) );
        }
        else {
            scl = one / ssml;
            sumsq = asml;
        }
    }
    else {
        // Otherwise all values are mid-range or zero
        scl = one;
        sumsq = amed;
    }

    #undef SQUARE
}

template< class vector_t >
inline
void lassq(
    const vector_t& x,
    real_type< type_t<vector_t> > &scl,
    real_type< type_t<vector_t> > &sumsq)
{
    using T = type_t<vector_t>;
    return lassq( x, scl, sumsq,
        []( const T& x ){ return blas::abs(x); }
    );
}

} // lapack

#endif // __LASSQ_HH__
