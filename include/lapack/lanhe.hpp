/// @file lanhe.hpp Returns the norm of a Hermitian matrix.
/// @author Weslley S Pereira, University of Colorado Denver, USA
//
// Copyright (c) 2012-2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __LANHE_HH__
#define __LANHE_HH__

#include "lapack/types.hpp"
#include "lapack/lassq.hpp"

namespace lapack {

/** Calculates the value of the one norm, Frobenius norm, infinity norm, or element of largest absolute value of a hermitian matrix
 *
 * @return Calculated norm value for the specified type.
 * 
 * @param normType Type should be specified as follows:
 *
 *     Norm::Max = maximum absolute value over all elements in A.
 *         Note: this is not a consistent matrix norm.
 *     Norm::One = one norm of the matrix A, the maximum value of the sums of each column.
 *     Norm::Inf = the infinity norm of the matrix A, the maximum value of the sum of each row.
 *     Norm::Fro = the Frobenius norm of the matrix A.
 *         This the square root of the sum of the squares of each element in A.
 * 
 * @param uplo Indicates whether the hermitian matrix A is stored as upper triangular or lower triangular.
 *      The other triangular part of A is not referenced.
 * @param n Number of columns to be included in the norm. n >= 0
 * @param A hermitian matrix size lda-by-n.
 * @param lda Leading dimension of matrix A.  ldA >= m
 * 
 * @ingroup auxiliary
**/
template< class norm_t, class uplo_t, class matrix_t,
    enable_if_t<
    /* Requires: */
    (   is_same_v<norm_t,max_norm_t> ||
        is_same_v<norm_t,frob_norm_t>
    ) && (
        is_same_v< uplo_t, upper_triangle_t > || 
        is_same_v< uplo_t, lower_triangle_t >
    ), bool > = true
>
real_type< type_t<matrix_t> >
lanhe( norm_t normType, uplo_t uplo, const matrix_t& A )
{
    using T      = type_t<matrix_t>;
    using real_t = real_type< T >;
    using idx_t  = size_type< matrix_t >;
    using pair   = std::pair<idx_t,idx_t>;
    using blas::isnan;
    using blas::sqrt;
    using blas::real;
    using blas::safe_max;

    // constants
    const real_t zero(0.0);
    const idx_t n = nrows(A);

    // quick return
    if ( n <= 0 ) return zero;

    // Norm value
    real_t norm(0.0);

    if( is_same_v<norm_t,max_norm_t> )
    {
        if( is_same_v<uplo_t,upper_triangle_t> ) {
            for (idx_t j = 0; j < n; ++j) {
                for (idx_t i = 0; i < j; ++i)
                {
                    real_t temp = blas::abs( A(i,j) );

                    if (temp > norm)
                        norm = temp;
                    else {
                        if ( isnan(temp) ) 
                            return temp;
                    }
                }
                {
                    real_t temp = blas::abs( real(A(j,j)) );

                    if (temp > norm)
                        norm = temp;
                    else {
                        if ( isnan(temp) ) 
                            return temp;
                    }
                }
            }
        }
        else {
            for (idx_t j = 0; j < n; ++j) {
                {
                    real_t temp = blas::abs( real(A(j,j)) );

                    if (temp > norm)
                        norm = temp;
                    else {
                        if ( isnan(temp) ) 
                            return temp;
                    }
                }
                for (idx_t i = j+1; i < n; ++i)
                {
                    real_t temp = blas::abs( A(i,j) );

                    if (temp > norm)
                        norm = temp;
                    else {
                        if ( isnan(temp) ) 
                            return temp;
                    }
                }
            }
        }
    }
    else
    {
        // Scaled ssq
        real_t scale(0), ssq(1);
        
        // Sum off-diagonals
        if( is_same_v<uplo_t,upper_triangle_t> ) {
            for (idx_t j = 1; j < n; ++j)
                lassq( subvector( col(A,j), pair{0,j} ), scale, ssq );
        }
        else {
            for (idx_t j = 0; j < n-1; ++j)
                lassq( subvector( col(A,j), pair{j+1,n} ), scale, ssq );
        }
        
        // Multiplies the sum by 2
        if( ssq < safe_max<real_t>() )
            ssq *= 2;
        else
            scale *= sqrt(2);

        // Sum the real part in the diagonal
        lassq( diag(A,0), scale, ssq,
            []( const T& x ){ return blas::abs( real(x) ); }
        );

        // Compute the scaled square root
        norm = scale * sqrt(ssq);
    }

    return norm;
}

template< class norm_t, class uplo_t, class matrix_t, class work_t,
    enable_if_t<
    /* Requires: */
    (   is_same_v<norm_t,max_norm_t> || 
        is_same_v<norm_t,one_norm_t> || 
        is_same_v<norm_t,inf_norm_t> || 
        is_same_v<norm_t,frob_norm_t>
    ) && (
        is_same_v< uplo_t, upper_triangle_t > || 
        is_same_v< uplo_t, lower_triangle_t >
    ), bool > = true
>
real_type< type_t<matrix_t> >
lanhe( norm_t normType, uplo_t uplo, const matrix_t& A, work_t& work )
{
    using real_t = real_type< type_t<matrix_t> >;
    using idx_t  = size_type< matrix_t >;
    using blas::isnan;
    using blas::real;
    
    // quick redirect
    if      ( is_same_v<norm_t,max_norm_t>  ) return lansy( max_norm,  uplo, A );
    else if ( is_same_v<norm_t,frob_norm_t> ) return lansy( frob_norm, uplo, A );

    // constants
    const real_t zero(0.0);
    const idx_t n = nrows(A);

    // quick return
    if ( n <= 0 ) return zero;

    // Norm value
    real_t norm(0.0);

    for (idx_t i = 0; i < n; ++i)
        work[i] = type_t<work_t>(0);

    if( is_same_v<uplo_t,upper_triangle_t> ) {
        for (idx_t j = 0; j < n; ++j)
        {
            real_t sum = zero;
            for (idx_t i = 0; i < j; ++i) {
                const real_t absa = blas::abs( A(i,j) );
                sum += absa;
                work[i] += absa;
            }
            work[j] = sum + blas::abs( real(A(j,j)) );
        }
        for (idx_t i = 0; i < n; ++i)
        {
            real_t sum = work[i];
            if (sum > norm)
                norm = sum;
            else {
                if ( isnan(sum) )
                    return sum;
            }
        }
    }
    else {
        for (idx_t j = 0; j < n; ++j)
        {
            real_t sum = work[j] + blas::abs( real(A(j,j)) );
            for (idx_t i = j+1; i < n; ++i) {
                const real_t absa = blas::abs( A(i,j) );
                sum += absa;
                work[i] += absa;
            }
            if (sum > norm)
                norm = sum;
            else {
                if ( isnan(sum) )
                    return sum;
            }
        }
    }

    return norm;
}

} // lapack

#endif // __LANHE_HH__
