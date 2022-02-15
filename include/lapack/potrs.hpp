/// @file potrs.hpp Apply the Cholesky factorization to solve a linear system.
/// @author Weslley S Pereira, University of Colorado Denver, USA
//
// Copyright (c) 2021-2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __POTRS_HH__
#define __POTRS_HH__

#include "lapack/types.hpp"
#include "lapack/utils.hpp"

#include "tblas.hpp"

namespace lapack {

/** Apply the Cholesky factorization to solve a linear system.
 *
 * The factorization has the form
 *     $A = U^H U,$ if uplo = Upper, or
 *     $A = L L^H,$ if uplo = Lower,
 * where U is an upper triangular matrix and L is lower triangular.
 *
 * @param[in] uplo
 *     - lapack::upper_triangle_t: Upper triangle of A is stored;
 *     - lapack::lower_triangle_t: Lower triangle of A is stored.
 *
 * @param[in,out] A
 *     The factor U or L from the Cholesky
 *     factorization $A = U^H U$ or $A = L L^H.$
 *
 * @return = 0: successful exit
 *
 * @ingroup posv_computational
 */
template< class uplo_t, class matrixA_t, class matrixB_t,
    enable_if_t<(
    /* Requires: */
        is_same_v< uplo_t, upper_triangle_t > || 
        is_same_v< uplo_t, lower_triangle_t >
    ), int > = 0
>
int potrs( uplo_t uplo, const matrixA_t& A, matrixB_t& B )
{
    using T = type_t< matrixB_t >;
    using blas::trsm;

    // Constants
    const T one( 1.0 );

    // Check arguments
    lapack_error_if( nrows(A) != ncols(A), -2 );
    lapack_error_if( nrows(B) != ncols(A), -3 );

    if( is_same_v< uplo_t, upper_triangle_t > ) {
        // Solve A*X = B where A = U**H *U.
        trsm( left_side, uplo, conjTranspose, nonUnit_diagonal, one, A, B );
        trsm( left_side, uplo, noTranspose,   nonUnit_diagonal, one, A, B );
    }
    else {
        // Solve A*X = B where A = L*L**H.
        trsm( left_side, uplo, noTranspose,   nonUnit_diagonal, one, A, B );
        trsm( left_side, uplo, conjTranspose, nonUnit_diagonal, one, A, B );
    }
    return 0;
}

} // lapack

#endif // __POTRS_HH__
