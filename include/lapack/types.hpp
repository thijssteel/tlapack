// Copyright (c) 2017-2021, University of Tennessee. All rights reserved.
// Copyright (c) 2021-2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __TLAPACK_TYPES_HH__
#define __TLAPACK_TYPES_HH__

#include "blas/types.hpp"

namespace lapack {

// -----------------------------------------------------------------------------
// Use the types from the namespace blas

using blas::real_type;
using blas::complex_type;
using blas::scalar_type;
using blas::is_complex;
using blas::zero_t;

using blas::Op;
using blas::Diag;
using blas::Side;

using blas::ColMajor_t;
using blas::RowMajor_t;
using blas::Banded_t;

// -----------------------------------------------------------------------------
// Diagonal matrices

struct nonUnit_diagonal_t {
    constexpr operator Diag() const { return Diag::NonUnit; }
};
struct unit_diagonal_t {
    constexpr operator Diag() const { return Diag::Unit; }
};

// constants
constexpr nonUnit_diagonal_t nonUnit_diagonal = { };
constexpr unit_diagonal_t unit_diagonal = { };

// -----------------------------------------------------------------------------
// Operations over data

struct noTranspose_t {
    constexpr operator Op() const { return Op::NoTrans; }
};
struct transpose_t {
    constexpr operator Op() const { return Op::Trans; }
};
struct conjTranspose_t {
    constexpr operator Op() const { return Op::ConjTrans; }
};

// Constants
constexpr noTranspose_t noTranspose = { };
constexpr transpose_t transpose = { };
constexpr conjTranspose_t conjTranspose = { };

// -----------------------------------------------------------------------------
// Sides

struct left_side_t {
    constexpr operator Side() const { return Side::Left; }
};
struct right_side_t {
    constexpr operator Side() const { return Side::Right; }
};

// Constants
constexpr left_side_t left_side { };
constexpr right_side_t right_side { };

// -----------------------------------------------------------------------------
// Norm types

enum class Norm {
    One = '1',  // or 'O'
    Two = '2',
    Inf = 'I',
    Fro = 'F',  // or 'E'
    Max = 'M',
};

struct max_norm_t {
    constexpr operator Norm() const { return Norm::Max; }
};
struct one_norm_t {
    constexpr operator Norm() const { return Norm::One; }
};
struct inf_norm_t {
    constexpr operator Norm() const { return Norm::Inf; }
};
struct frob_norm_t {
    constexpr operator Norm() const { return Norm::Fro; }
};

// Constants
constexpr max_norm_t max_norm = { };
constexpr one_norm_t one_norm = { };
constexpr inf_norm_t inf_norm = { };
constexpr frob_norm_t frob_norm = { };

// -----------------------------------------------------------------------------
// Directions

enum class Direction {
    Forward     = 'F',
    Backward    = 'B',
};

struct forward_t {
    constexpr operator Direction() const { return Direction::Forward; }
};
struct backward_t {
    constexpr operator Direction() const { return Direction::Backward; }
};

// Constants
constexpr forward_t forward { };
constexpr backward_t backward { };

// -----------------------------------------------------------------------------
// Storage types

enum class StoreV {
    Columnwise  = 'C',
    Rowwise     = 'R',
};

struct columnwise_storage_t {
    constexpr operator StoreV() const { return StoreV::Columnwise; }
};
struct rowwise_storage_t {
    constexpr operator StoreV() const { return StoreV::Rowwise; }
};

// Constants
constexpr columnwise_storage_t columnwise_storage { };
constexpr rowwise_storage_t rowwise_storage { };

} // namespace lapack

#endif // __TLAPACK_TYPES_HH__
