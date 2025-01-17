// Copyright (c) 2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __TBLAS_LEGACYARRAY_HH__
#define __TBLAS_LEGACYARRAY_HH__

#include "legacy_api/legacyArray.hpp"

namespace blas {

namespace internal {

    template< typename T >
    inline constexpr auto colmajor_matrix(
        T* A, 
        BLAS_SIZE_T m, 
        BLAS_SIZE_T n, 
        BLAS_SIZE_T lda ) noexcept
    {
        return legacyMatrix<T,Layout::ColMajor>{ m, n, A, lda };
    }
    
    template< typename T >
    inline constexpr auto colmajor_matrix(
        T* A, 
        BLAS_SIZE_T m, 
        BLAS_SIZE_T n ) noexcept
    {
        return legacyMatrix<T,Layout::ColMajor>{ m, n, A, m };
    }

    template< typename T >
    inline constexpr auto rowmajor_matrix(
        T* A, 
        BLAS_SIZE_T m, 
        BLAS_SIZE_T n, 
        BLAS_SIZE_T lda ) noexcept
    {
        return legacyMatrix<T,Layout::RowMajor>{ m, n, A, lda };
    }

    template< typename T >
    inline constexpr auto rowmajor_matrix(
        T* A, 
        BLAS_SIZE_T m, 
        BLAS_SIZE_T n ) noexcept
    {
        return legacyMatrix<T,Layout::RowMajor>{ m, n, A, n };
    }

    template< typename T >
    inline constexpr auto banded_matrix(
        T* A, 
        BLAS_SIZE_T m, 
        BLAS_SIZE_T n, 
        BLAS_SIZE_T kl, 
        BLAS_SIZE_T ku ) noexcept
    {
        return legacyBandedMatrix<T>{ m, n, kl, ku, A };
    }

    template< typename T, typename int_t >
    inline constexpr auto vector( T* x, BLAS_SIZE_T n, int_t inc ) noexcept
    {
        return legacyVector<T,int_t>{ n, x, inc };
    }

    template< typename T >
    inline constexpr auto vector( T* x, BLAS_SIZE_T n ) noexcept
    {
        return legacyVector<T>{ n, x, one };
    }

    template< typename T, typename int_t >
    inline constexpr auto backward_vector( T* x, BLAS_SIZE_T n, int_t inc ) noexcept
    {
        return legacyVector<T,int_t,Direction::Backward>{ n, x, inc };
    }

    template< typename T >
    inline constexpr auto backward_vector( T* x, BLAS_SIZE_T n ) noexcept
    {
        return legacyVector<T,one_t,Direction::Backward>{ n, x, one };
    }

    // template< typename T >
    // inline constexpr auto backward_vector(
    //     T* x,
    //     BLAS_SIZE_T n,
    //     BLAS_INT_T  inc ) noexcept
    // {
    //     return legacyVector<T,Direction::Backward>{ n, x, inc };
    // }

    // template< typename T >
    // inline constexpr auto backward_vector(
    //     T* x,
    //     BLAS_SIZE_T n ) noexcept
    // {
    //     return legacyVector<T,Direction::Backward>{ n, x, 1 };
    // }

    // Transpose
    template< typename T >
    inline constexpr auto transpose(
        const legacyMatrix<T,Layout::ColMajor>& A ) noexcept
    {
        return legacyMatrix<T,Layout::RowMajor>{ A.n, A.m, A.ptr, A.ldim };
    }

    // Transpose
    template< typename T >
    inline constexpr auto transpose(
        const legacyMatrix<T,Layout::RowMajor>& A ) noexcept
    {
        return legacyMatrix<T,Layout::ColMajor>{ A.n, A.m, A.ptr, A.ldim };
    }

} // namespace internal

} // namespace blas

#endif // __TBLAS_LEGACYARRAY_HH__
