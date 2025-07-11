// CodeGen/RuntimeLibcallSignatures.cpp - R.T. Lib. Call Signatures -*- C++ -*--
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains signature information for runtime libcalls.
///
/// CodeGen uses external symbols, which it refers to by name. The WebAssembly
/// target needs type information for all functions. This file contains a big
/// table providing type signatures for all runtime library functions that LLVM
/// uses.
///
/// This is currently a fairly heavy-handed solution.
///
//===----------------------------------------------------------------------===//

#include "WebAssemblyRuntimeLibcallSignatures.h"
#include "WebAssemblySubtarget.h"
#include "WebAssemblyUtilities.h"

using namespace llvm;

namespace {

enum RuntimeLibcallSignature {
  func,
  f32_func_f32,
  f32_func_f64,
  f32_func_i32,
  f32_func_i64,
  f32_func_i16,
  f64_func_f32,
  f64_func_f64,
  f64_func_i32,
  f64_func_i64,
  i32_func_f32,
  i32_func_f64,
  i32_func_i32,
  i64_func_f32,
  i64_func_f64,
  i64_func_i64,
  f32_func_f32_f32,
  f32_func_f32_i32,
  f32_func_f32_iPTR,
  f32_func_i64_i64,
  f64_func_f64_f64,
  f64_func_f64_i32,
  f64_func_f64_iPTR,
  f64_func_i64_i64,
  i16_func_f32,
  i16_func_f64,
  i16_func_i64_i64,
  i8_func_i8_i8,
  func_f32_iPTR_iPTR,
  func_f64_iPTR_iPTR,
  i16_func_i16_i16,
  i32_func_f32_f32,
  i32_func_f64_f64,
  i32_func_i32_i32,
  i32_func_i32_i32_iPTR,
  i64_func_i64_i64,
  i64_func_i64_i64_iPTR,
  i64_i64_func_i32,
  i64_i64_func_i64,
  i64_i64_func_f32,
  i64_i64_func_f64,
  i16_i16_func_i16_i16,
  i32_i32_func_i32_i32,
  i64_i64_func_i64_i64,
  i64_i64_func_i64_i64_iPTR,
  i64_i64_func_i64_i64_i64_i64,
  i64_i64_func_i64_i64_i64_i64_iPTR,
  i64_i64_i64_i64_func_i64_i64_i64_i64,
  i64_i64_func_i64_i64_i32,
  i64_i64_func_i64_i64_i64_i64_i64_i64,
  iPTR_func_i32,
  iPTR_func_iPTR_i32_iPTR,
  iPTR_func_iPTR_iPTR_iPTR,
  f32_func_f32_f32_f32,
  f64_func_f64_f64_f64,
  func_i64_i64_iPTR_iPTR,
  i32_func_i64_i64,
  i32_func_i64_i64_i64_i64,
  iPTR_func_f32,
  iPTR_func_f64,
  iPTR_func_i64_i64,
  unsupported
};

struct RuntimeLibcallSignatureTable {
  std::vector<RuntimeLibcallSignature> Table;

  // Any newly-added libcalls will be unsupported by default.
  RuntimeLibcallSignatureTable() : Table(RTLIB::Libcall::UNKNOWN_LIBCALL, unsupported) {
    // Integer
    Table[RTLIB::Libcall::SHL_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::SHL_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::SHL_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SHL_I128] = i64_i64_func_i64_i64_i32;
    Table[RTLIB::Libcall::SRL_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::SRL_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::SRL_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SRL_I128] = i64_i64_func_i64_i64_i32;
    Table[RTLIB::Libcall::SRA_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::SRA_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::SRA_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SRA_I128] = i64_i64_func_i64_i64_i32;
    Table[RTLIB::Libcall::MUL_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::MUL_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::MUL_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::MUL_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::MUL_I128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::MULO_I32] = i32_func_i32_i32_iPTR;
    Table[RTLIB::Libcall::MULO_I64] = i64_func_i64_i64_iPTR;
    Table[RTLIB::Libcall::MULO_I128] = i64_i64_func_i64_i64_i64_i64_iPTR;
    Table[RTLIB::Libcall::SDIV_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::SDIV_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::SDIV_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::SDIV_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SDIV_I128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::UDIV_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::UDIV_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::UDIV_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::UDIV_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::UDIV_I128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::SREM_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::SREM_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::SREM_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::SREM_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SREM_I128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::UREM_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::UREM_I16] = i16_func_i16_i16;
    Table[RTLIB::Libcall::UREM_I32] = i32_func_i32_i32;
    Table[RTLIB::Libcall::UREM_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::UREM_I128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::SDIVREM_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::SDIVREM_I16] = i16_i16_func_i16_i16;
    Table[RTLIB::Libcall::SDIVREM_I32] = i32_i32_func_i32_i32;
    Table[RTLIB::Libcall::SDIVREM_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::SDIVREM_I128] = i64_i64_i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::UDIVREM_I8] = i8_func_i8_i8;
    Table[RTLIB::Libcall::UDIVREM_I16] = i16_i16_func_i16_i16;
    Table[RTLIB::Libcall::UDIVREM_I32] = i32_i32_func_i32_i32;
    Table[RTLIB::Libcall::UDIVREM_I64] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::UDIVREM_I128] = i64_i64_i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::NEG_I32] = i32_func_i32;
    Table[RTLIB::Libcall::NEG_I64] = i64_func_i64;

    // Floating-point.
    // All F80 and PPCF128 routines are unsupported.
    Table[RTLIB::Libcall::ADD_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::ADD_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::ADD_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::SUB_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::SUB_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::SUB_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::MUL_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::MUL_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::MUL_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::DIV_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::DIV_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::DIV_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::REM_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::REM_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::REM_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::FMA_F32] = f32_func_f32_f32_f32;
    Table[RTLIB::Libcall::FMA_F64] = f64_func_f64_f64_f64;
    Table[RTLIB::Libcall::FMA_F128] = i64_i64_func_i64_i64_i64_i64_i64_i64;
    Table[RTLIB::Libcall::POWI_F32] = f32_func_f32_i32;
    Table[RTLIB::Libcall::POWI_F64] = f64_func_f64_i32;
    Table[RTLIB::Libcall::POWI_F128] = i64_i64_func_i64_i64_i32;
    Table[RTLIB::Libcall::SQRT_F32] = f32_func_f32;
    Table[RTLIB::Libcall::SQRT_F64] = f64_func_f64;
    Table[RTLIB::Libcall::SQRT_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::CBRT_F32] = f32_func_f32;
    Table[RTLIB::Libcall::CBRT_F64] = f64_func_f64;
    Table[RTLIB::Libcall::CBRT_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::LOG_F32] = f32_func_f32;
    Table[RTLIB::Libcall::LOG_F64] = f64_func_f64;
    Table[RTLIB::Libcall::LOG_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::LOG2_F32] = f32_func_f32;
    Table[RTLIB::Libcall::LOG2_F64] = f64_func_f64;
    Table[RTLIB::Libcall::LOG2_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::LOG10_F32] = f32_func_f32;
    Table[RTLIB::Libcall::LOG10_F64] = f64_func_f64;
    Table[RTLIB::Libcall::LOG10_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::EXP_F32] = f32_func_f32;
    Table[RTLIB::Libcall::EXP_F64] = f64_func_f64;
    Table[RTLIB::Libcall::EXP_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::EXP2_F32] = f32_func_f32;
    Table[RTLIB::Libcall::EXP2_F64] = f64_func_f64;
    Table[RTLIB::Libcall::EXP2_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::EXP10_F32] = f32_func_f32;
    Table[RTLIB::Libcall::EXP10_F64] = f64_func_f64;
    Table[RTLIB::Libcall::EXP10_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::SIN_F32] = f32_func_f32;
    Table[RTLIB::Libcall::SIN_F64] = f64_func_f64;
    Table[RTLIB::Libcall::SIN_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::COS_F32] = f32_func_f32;
    Table[RTLIB::Libcall::COS_F64] = f64_func_f64;
    Table[RTLIB::Libcall::COS_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::TAN_F32] = f32_func_f32;
    Table[RTLIB::Libcall::TAN_F64] = f64_func_f64;
    Table[RTLIB::Libcall::TAN_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ASIN_F32] = f32_func_f32;
    Table[RTLIB::Libcall::ASIN_F64] = f64_func_f64;
    Table[RTLIB::Libcall::ASIN_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ACOS_F32] = f32_func_f32;
    Table[RTLIB::Libcall::ACOS_F64] = f64_func_f64;
    Table[RTLIB::Libcall::ACOS_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ATAN_F32] = f32_func_f32;
    Table[RTLIB::Libcall::ATAN_F64] = f64_func_f64;
    Table[RTLIB::Libcall::ATAN_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ATAN2_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::ATAN2_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::ATAN2_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::SINH_F32] = f32_func_f32;
    Table[RTLIB::Libcall::SINH_F64] = f64_func_f64;
    Table[RTLIB::Libcall::SINH_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::COSH_F32] = f32_func_f32;
    Table[RTLIB::Libcall::COSH_F64] = f64_func_f64;
    Table[RTLIB::Libcall::COSH_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::TANH_F32] = f32_func_f32;
    Table[RTLIB::Libcall::TANH_F64] = f64_func_f64;
    Table[RTLIB::Libcall::TANH_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::SINCOS_F32] = func_f32_iPTR_iPTR;
    Table[RTLIB::Libcall::SINCOS_F64] = func_f64_iPTR_iPTR;
    Table[RTLIB::Libcall::SINCOS_F128] = func_i64_i64_iPTR_iPTR;
    Table[RTLIB::Libcall::POW_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::POW_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::POW_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::CEIL_F32] = f32_func_f32;
    Table[RTLIB::Libcall::CEIL_F64] = f64_func_f64;
    Table[RTLIB::Libcall::CEIL_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::TRUNC_F32] = f32_func_f32;
    Table[RTLIB::Libcall::TRUNC_F64] = f64_func_f64;
    Table[RTLIB::Libcall::TRUNC_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::RINT_F32] = f32_func_f32;
    Table[RTLIB::Libcall::RINT_F64] = f64_func_f64;
    Table[RTLIB::Libcall::RINT_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::NEARBYINT_F32] = f32_func_f32;
    Table[RTLIB::Libcall::NEARBYINT_F64] = f64_func_f64;
    Table[RTLIB::Libcall::NEARBYINT_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ROUND_F32] = f32_func_f32;
    Table[RTLIB::Libcall::ROUND_F64] = f64_func_f64;
    Table[RTLIB::Libcall::ROUND_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::ROUNDEVEN_F32] = f32_func_f32;
    Table[RTLIB::Libcall::ROUNDEVEN_F64] = f64_func_f64;
    Table[RTLIB::Libcall::ROUNDEVEN_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::LROUND_F32] = iPTR_func_f32;
    Table[RTLIB::Libcall::LROUND_F64] = iPTR_func_f64;
    Table[RTLIB::Libcall::LROUND_F128] = iPTR_func_i64_i64;
    Table[RTLIB::Libcall::LLROUND_F32] = i64_func_f32;
    Table[RTLIB::Libcall::LLROUND_F64] = i64_func_f64;
    Table[RTLIB::Libcall::LLROUND_F128] = i64_func_i64_i64;
    Table[RTLIB::Libcall::LRINT_F32] = iPTR_func_f32;
    Table[RTLIB::Libcall::LRINT_F64] = iPTR_func_f64;
    Table[RTLIB::Libcall::LRINT_F128] = iPTR_func_i64_i64;
    Table[RTLIB::Libcall::LLRINT_F32] = i64_func_f32;
    Table[RTLIB::Libcall::LLRINT_F64] = i64_func_f64;
    Table[RTLIB::Libcall::LLRINT_F128] = i64_func_i64_i64;
    Table[RTLIB::Libcall::FLOOR_F32] = f32_func_f32;
    Table[RTLIB::Libcall::FLOOR_F64] = f64_func_f64;
    Table[RTLIB::Libcall::FLOOR_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::COPYSIGN_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::COPYSIGN_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::COPYSIGN_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::FMIN_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::FMIN_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::FMIN_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::FMAX_F32] = f32_func_f32_f32;
    Table[RTLIB::Libcall::FMAX_F64] = f64_func_f64_f64;
    Table[RTLIB::Libcall::FMAX_F128] = i64_i64_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::LDEXP_F32] = f32_func_f32_i32;
    Table[RTLIB::Libcall::LDEXP_F64] = f64_func_f64_i32;
    Table[RTLIB::Libcall::LDEXP_F128] = i64_i64_func_i64_i64_i32;
    Table[RTLIB::Libcall::FREXP_F32] = f32_func_f32_iPTR;
    Table[RTLIB::Libcall::FREXP_F64] = f64_func_f64_iPTR;
    Table[RTLIB::Libcall::FREXP_F128] = i64_i64_func_i64_i64_iPTR;
    Table[RTLIB::Libcall::MODF_F32] = f32_func_f32_iPTR;
    Table[RTLIB::Libcall::MODF_F64] = f64_func_f64_iPTR;
    Table[RTLIB::Libcall::MODF_F128] = i64_i64_func_i64_i64_iPTR;

    // Conversion
    // All F80 and PPCF128 routines are unsupported.
    Table[RTLIB::Libcall::FPEXT_F64_F128] = i64_i64_func_f64;
    Table[RTLIB::Libcall::FPEXT_F32_F128] = i64_i64_func_f32;
    Table[RTLIB::Libcall::FPEXT_F32_F64] = f64_func_f32;
    Table[RTLIB::Libcall::FPEXT_F16_F32] = f32_func_i16;
    Table[RTLIB::Libcall::FPROUND_F32_F16] = i16_func_f32;
    Table[RTLIB::Libcall::FPROUND_F64_F16] = i16_func_f64;
    Table[RTLIB::Libcall::FPROUND_F64_F32] = f32_func_f64;
    Table[RTLIB::Libcall::FPROUND_F128_F16] = i16_func_i64_i64;
    Table[RTLIB::Libcall::FPROUND_F128_F32] = f32_func_i64_i64;
    Table[RTLIB::Libcall::FPROUND_F128_F64] = f64_func_i64_i64;
    Table[RTLIB::Libcall::FPTOSINT_F32_I32] = i32_func_f32;
    Table[RTLIB::Libcall::FPTOSINT_F32_I64] = i64_func_f32;
    Table[RTLIB::Libcall::FPTOSINT_F32_I128] = i64_i64_func_f32;
    Table[RTLIB::Libcall::FPTOSINT_F64_I32] = i32_func_f64;
    Table[RTLIB::Libcall::FPTOSINT_F64_I64] = i64_func_f64;
    Table[RTLIB::Libcall::FPTOSINT_F64_I128] = i64_i64_func_f64;
    Table[RTLIB::Libcall::FPTOSINT_F128_I32] = i32_func_i64_i64;
    Table[RTLIB::Libcall::FPTOSINT_F128_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::FPTOSINT_F128_I128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::FPTOUINT_F32_I32] = i32_func_f32;
    Table[RTLIB::Libcall::FPTOUINT_F32_I64] = i64_func_f32;
    Table[RTLIB::Libcall::FPTOUINT_F32_I128] = i64_i64_func_f32;
    Table[RTLIB::Libcall::FPTOUINT_F64_I32] = i32_func_f64;
    Table[RTLIB::Libcall::FPTOUINT_F64_I64] = i64_func_f64;
    Table[RTLIB::Libcall::FPTOUINT_F64_I128] = i64_i64_func_f64;
    Table[RTLIB::Libcall::FPTOUINT_F128_I32] = i32_func_i64_i64;
    Table[RTLIB::Libcall::FPTOUINT_F128_I64] = i64_func_i64_i64;
    Table[RTLIB::Libcall::FPTOUINT_F128_I128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::SINTTOFP_I32_F32] = f32_func_i32;
    Table[RTLIB::Libcall::SINTTOFP_I32_F64] = f64_func_i32;
    Table[RTLIB::Libcall::SINTTOFP_I32_F128] = i64_i64_func_i32;
    Table[RTLIB::Libcall::SINTTOFP_I64_F32] = f32_func_i64;
    Table[RTLIB::Libcall::SINTTOFP_I64_F64] = f64_func_i64;
    Table[RTLIB::Libcall::SINTTOFP_I64_F128] = i64_i64_func_i64;
    Table[RTLIB::Libcall::SINTTOFP_I128_F32] = f32_func_i64_i64;
    Table[RTLIB::Libcall::SINTTOFP_I128_F64] = f64_func_i64_i64;
    Table[RTLIB::Libcall::SINTTOFP_I128_F128] = i64_i64_func_i64_i64;
    Table[RTLIB::Libcall::UINTTOFP_I32_F32] = f32_func_i32;
    Table[RTLIB::Libcall::UINTTOFP_I32_F64] = f64_func_i64;
    Table[RTLIB::Libcall::UINTTOFP_I32_F128] = i64_i64_func_i32;
    Table[RTLIB::Libcall::UINTTOFP_I64_F32] = f32_func_i64;
    Table[RTLIB::Libcall::UINTTOFP_I64_F64] = f64_func_i64;
    Table[RTLIB::Libcall::UINTTOFP_I64_F128] = i64_i64_func_i64;
    Table[RTLIB::Libcall::UINTTOFP_I128_F32] = f32_func_i64_i64;
    Table[RTLIB::Libcall::UINTTOFP_I128_F64] = f64_func_i64_i64;
    Table[RTLIB::Libcall::UINTTOFP_I128_F128] = i64_i64_func_i64_i64;

    // Comparison
    // ALl F80 and PPCF128 routines are unsupported.
    Table[RTLIB::Libcall::OEQ_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::OEQ_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::OEQ_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::UNE_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::UNE_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::UNE_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::OGE_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::OGE_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::OGE_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::OLT_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::OLT_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::OLT_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::OLE_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::OLE_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::OLE_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::OGT_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::OGT_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::OGT_F128] = i32_func_i64_i64_i64_i64;
    Table[RTLIB::Libcall::UO_F32] = i32_func_f32_f32;
    Table[RTLIB::Libcall::UO_F64] = i32_func_f64_f64;
    Table[RTLIB::Libcall::UO_F128] = i32_func_i64_i64_i64_i64;

    // Memory
    Table[RTLIB::Libcall::MEMCPY] = iPTR_func_iPTR_iPTR_iPTR;
    Table[RTLIB::Libcall::MEMSET] = iPTR_func_iPTR_i32_iPTR;
    Table[RTLIB::Libcall::MEMMOVE] = iPTR_func_iPTR_iPTR_iPTR;

    // __stack_chk_fail
    Table[RTLIB::Libcall::STACKPROTECTOR_CHECK_FAIL] = func;

    // Return address handling
    Table[RTLIB::Libcall::RETURN_ADDRESS] = iPTR_func_i32;

    // Element-wise Atomic memory
    // TODO: Fix these when we implement atomic support
    Table[RTLIB::Libcall::MEMCPY_ELEMENT_UNORDERED_ATOMIC_1] = unsupported;
    Table[RTLIB::Libcall::MEMCPY_ELEMENT_UNORDERED_ATOMIC_2] = unsupported;
    Table[RTLIB::Libcall::MEMCPY_ELEMENT_UNORDERED_ATOMIC_4] = unsupported;
    Table[RTLIB::Libcall::MEMCPY_ELEMENT_UNORDERED_ATOMIC_8] = unsupported;
    Table[RTLIB::Libcall::MEMCPY_ELEMENT_UNORDERED_ATOMIC_16] = unsupported;
    Table[RTLIB::Libcall::MEMMOVE_ELEMENT_UNORDERED_ATOMIC_1] = unsupported;
    Table[RTLIB::Libcall::MEMMOVE_ELEMENT_UNORDERED_ATOMIC_2] = unsupported;
    Table[RTLIB::Libcall::MEMMOVE_ELEMENT_UNORDERED_ATOMIC_4] = unsupported;
    Table[RTLIB::Libcall::MEMMOVE_ELEMENT_UNORDERED_ATOMIC_8] = unsupported;
    Table[RTLIB::Libcall::MEMMOVE_ELEMENT_UNORDERED_ATOMIC_16] = unsupported;

    Table[RTLIB::Libcall::MEMSET_ELEMENT_UNORDERED_ATOMIC_1] = unsupported;
    Table[RTLIB::Libcall::MEMSET_ELEMENT_UNORDERED_ATOMIC_2] = unsupported;
    Table[RTLIB::Libcall::MEMSET_ELEMENT_UNORDERED_ATOMIC_4] = unsupported;
    Table[RTLIB::Libcall::MEMSET_ELEMENT_UNORDERED_ATOMIC_8] = unsupported;
    Table[RTLIB::Libcall::MEMSET_ELEMENT_UNORDERED_ATOMIC_16] = unsupported;

    // Atomic '__sync_*' libcalls.
    // TODO: Fix these when we implement atomic support
    Table[RTLIB::Libcall::SYNC_VAL_COMPARE_AND_SWAP_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_VAL_COMPARE_AND_SWAP_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_VAL_COMPARE_AND_SWAP_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_VAL_COMPARE_AND_SWAP_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_VAL_COMPARE_AND_SWAP_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_LOCK_TEST_AND_SET_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_LOCK_TEST_AND_SET_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_LOCK_TEST_AND_SET_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_LOCK_TEST_AND_SET_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_LOCK_TEST_AND_SET_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_ADD_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_ADD_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_ADD_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_ADD_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_ADD_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_SUB_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_SUB_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_SUB_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_SUB_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_SUB_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_AND_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_AND_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_AND_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_AND_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_AND_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_OR_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_OR_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_OR_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_OR_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_OR_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_XOR_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_XOR_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_XOR_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_XOR_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_XOR_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_NAND_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_NAND_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_NAND_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_NAND_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_NAND_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MAX_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MAX_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MAX_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MAX_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MAX_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMAX_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMAX_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMAX_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMAX_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMAX_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MIN_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MIN_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MIN_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MIN_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_MIN_16] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMIN_1] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMIN_2] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMIN_4] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMIN_8] = unsupported;
    Table[RTLIB::Libcall::SYNC_FETCH_AND_UMIN_16] = unsupported;

    // Atomic '__atomic_*' libcalls.
    // TODO: Fix these when we implement atomic support
    Table[RTLIB::Libcall::ATOMIC_LOAD] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_LOAD_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_LOAD_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_LOAD_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_LOAD_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_LOAD_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_STORE] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_STORE_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_STORE_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_STORE_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_STORE_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_STORE_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_EXCHANGE] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_EXCHANGE_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_EXCHANGE_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_EXCHANGE_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_EXCHANGE_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_EXCHANGE_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_COMPARE_EXCHANGE_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_ADD_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_ADD_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_ADD_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_ADD_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_ADD_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_SUB_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_SUB_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_SUB_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_SUB_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_SUB_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_AND_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_AND_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_AND_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_AND_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_AND_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_OR_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_OR_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_OR_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_OR_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_OR_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_XOR_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_XOR_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_XOR_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_XOR_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_XOR_16] = unsupported;

    Table[RTLIB::Libcall::ATOMIC_FETCH_NAND_1] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_NAND_2] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_NAND_4] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_NAND_8] = unsupported;
    Table[RTLIB::Libcall::ATOMIC_FETCH_NAND_16] = unsupported;
  }
};

RuntimeLibcallSignatureTable &getRuntimeLibcallSignatures() {
  static RuntimeLibcallSignatureTable RuntimeLibcallSignatures;
  return RuntimeLibcallSignatures;
}

// Maps libcall names to their RTLIB::Libcall number. Builds the map in a
// constructor for use with a static variable
struct StaticLibcallNameMap {
  StringMap<RTLIB::Libcall> Map;
  StaticLibcallNameMap(const Triple &TT) {
    // FIXME: This is broken if there are ever different triples compiled with
    // different libcalls.
    RTLIB::RuntimeLibcallsInfo RTCI(TT);
    for (RTLIB::Libcall LC : RTLIB::libcalls()) {
      const char *NameLibcall = RTCI.getLibcallName(LC);
      if (NameLibcall != nullptr &&
          getRuntimeLibcallSignatures().Table[LC] != unsupported) {
        assert(!Map.contains(NameLibcall) &&
               "duplicate libcall names in name map");
        Map[NameLibcall] = LC;
      }
    }
  }
};

} // end anonymous namespace

void WebAssembly::getLibcallSignature(const WebAssemblySubtarget &Subtarget,
                                      RTLIB::Libcall LC,
                                      SmallVectorImpl<wasm::ValType> &Rets,
                                      SmallVectorImpl<wasm::ValType> &Params) {
  assert(Rets.empty());
  assert(Params.empty());

  wasm::ValType PtrTy =
      Subtarget.hasAddr64() ? wasm::ValType::I64 : wasm::ValType::I32;

  auto &Table = getRuntimeLibcallSignatures().Table;
  switch (Table[LC]) {
  case func:
    break;
  case f32_func_f32:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    break;
  case f32_func_f64:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F64);
    break;
  case f32_func_i32:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::I32);
    break;
  case f32_func_i64:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::I64);
    break;
  case f32_func_i16:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::I32);
    break;
  case f64_func_f32:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F32);
    break;
  case f64_func_f64:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    break;
  case f64_func_i32:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::I32);
    break;
  case f64_func_i64:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i32_func_f32:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F32);
    break;
  case i32_func_f64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F64);
    break;
  case i32_func_i32:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case i64_func_f32:
    Rets.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::F32);
    break;
  case i64_func_f64:
    Rets.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::F64);
    break;
  case i64_func_i64:
    Rets.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case f32_func_f32_f32:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    break;
  case f32_func_f32_i32:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::I32);
    break;
  case f32_func_f32_iPTR:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(PtrTy);
    break;
  case f32_func_i64_i64:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case f64_func_f64_f64:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    break;
  case f64_func_f64_i32:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::I32);
    break;
  case f64_func_i64_i64:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case f64_func_f64_iPTR:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(PtrTy);
    break;
  case i16_func_f32:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F32);
    break;
  case i16_func_f64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F64);
    break;
  case i16_func_i64_i64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i8_func_i8_i8:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case func_f32_iPTR_iPTR:
    Params.push_back(wasm::ValType::F32);
    Params.push_back(PtrTy);
    Params.push_back(PtrTy);
    break;
  case func_f64_iPTR_iPTR:
    Params.push_back(wasm::ValType::F64);
    Params.push_back(PtrTy);
    Params.push_back(PtrTy);
    break;
  case i16_func_i16_i16:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case i32_func_f32_f32:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    break;
  case i32_func_f64_f64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    break;
  case i32_func_i32_i32:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case i32_func_i32_i32_iPTR:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(PtrTy);
    break;
  case i64_func_i64_i64:
    Rets.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_func_i64_i64_iPTR:
    Rets.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(PtrTy);
    break;
  case i64_i64_func_f32:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::F32);
    break;
  case i64_i64_func_f64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::F64);
    break;
  case i16_i16_func_i16_i16:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I32);
      Rets.push_back(wasm::ValType::I32);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case i32_i32_func_i32_i32:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I32);
      Rets.push_back(wasm::ValType::I32);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I32);
    break;
  case i64_i64_func_i64_i64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_i64_func_i64_i64_iPTR:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(PtrTy);
    break;
  case i64_i64_func_i64_i64_i64_i64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_i64_func_i64_i64_i64_i64_iPTR:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(PtrTy);
    break;
  case i64_i64_i64_i64_func_i64_i64_i64_i64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_i64_func_i64_i64_i32:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I32);
    break;
  case iPTR_func_i32:
    Rets.push_back(PtrTy);
    Params.push_back(wasm::ValType::I32);
    break;
  case iPTR_func_iPTR_i32_iPTR:
    Rets.push_back(PtrTy);
    Params.push_back(PtrTy);
    Params.push_back(wasm::ValType::I32);
    Params.push_back(PtrTy);
    break;
  case iPTR_func_iPTR_iPTR_iPTR:
    Rets.push_back(PtrTy);
    Params.push_back(PtrTy);
    Params.push_back(PtrTy);
    Params.push_back(PtrTy);
    break;
  case f32_func_f32_f32_f32:
    Rets.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    Params.push_back(wasm::ValType::F32);
    break;
  case f64_func_f64_f64_f64:
    Rets.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    Params.push_back(wasm::ValType::F64);
    break;
  case func_i64_i64_iPTR_iPTR:
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(PtrTy);
    Params.push_back(PtrTy);
    break;
  case i32_func_i64_i64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i32_func_i64_i64_i64_i64:
    Rets.push_back(wasm::ValType::I32);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case iPTR_func_f32:
    Rets.push_back(PtrTy);
    Params.push_back(wasm::ValType::F32);
    break;
  case iPTR_func_f64:
    Rets.push_back(PtrTy);
    Params.push_back(wasm::ValType::F64);
    break;
  case iPTR_func_i64_i64:
    Rets.push_back(PtrTy);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_i64_func_i64_i64_i64_i64_i64_i64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    Params.push_back(wasm::ValType::I64);
    break;
  case i64_i64_func_i32:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I32);
    break;
  case i64_i64_func_i64:
    if (WebAssembly::canLowerMultivalueReturn(&Subtarget)) {
      Rets.push_back(wasm::ValType::I64);
      Rets.push_back(wasm::ValType::I64);
    } else {
      Params.push_back(PtrTy);
    }
    Params.push_back(wasm::ValType::I64);
    break;
  case unsupported:
    llvm_unreachable("unsupported runtime library signature");
  }
}

// TODO: If the RTLIB::Libcall-taking flavor of GetSignature remains unused
// other than here, just roll its logic into this version.
void WebAssembly::getLibcallSignature(const WebAssemblySubtarget &Subtarget,
                                      StringRef Name,
                                      SmallVectorImpl<wasm::ValType> &Rets,
                                      SmallVectorImpl<wasm::ValType> &Params) {
  static StaticLibcallNameMap LibcallNameMap(Subtarget.getTargetTriple());
  auto &Map = LibcallNameMap.Map;
  auto Val = Map.find(Name);
#ifndef NDEBUG
  if (Val == Map.end()) {
    auto Message =
        std::string("unexpected runtime library name: ") + std::string(Name);
    llvm_unreachable(Message.c_str());
  }
#endif
  return getLibcallSignature(Subtarget, Val->second, Rets, Params);
}
