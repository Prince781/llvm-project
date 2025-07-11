//===- RuntimeLibcalls.cpp - Interface for runtime libcalls -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/RuntimeLibcalls.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace RTLIB;

#define GET_INIT_RUNTIME_LIBCALL_NAMES
#define GET_SET_TARGET_RUNTIME_LIBCALL_SETS
#include "llvm/IR/RuntimeLibcalls.inc"
#undef GET_INIT_RUNTIME_LIBCALL_NAMES
#undef GET_SET_TARGET_RUNTIME_LIBCALL_SETS

static cl::opt<bool>
    HexagonEnableFastMathRuntimeCalls("hexagon-fast-math", cl::Hidden,
                                      cl::desc("Enable Fast Math processing"));

static void setARMLibcallNames(RuntimeLibcallsInfo &Info, const Triple &TT,
                               FloatABI::ABIType FloatABIType,
                               EABI EABIVersion) {
  if (!TT.isOSDarwin() && !TT.isiOS() && !TT.isWatchOS() && !TT.isDriverKit()) {
    CallingConv::ID DefaultCC = FloatABIType == FloatABI::Hard
                                    ? CallingConv::ARM_AAPCS_VFP
                                    : CallingConv::ARM_AAPCS;
    for (RTLIB::LibcallImpl LC : RTLIB::libcall_impls())
      Info.setLibcallImplCallingConv(LC, DefaultCC);
  }

  // Register based DivRem for AEABI (RTABI 4.2)
  if (TT.isTargetAEABI() || TT.isAndroid() || TT.isTargetGNUAEABI() ||
      TT.isTargetMuslAEABI() || TT.isOSWindows()) {
    if (TT.isOSWindows()) {
      const struct {
        const Libcall Op;
        const LibcallImpl Impl;
        const CallingConv::ID CC;
      } LibraryCalls[] = {
          {Libcall::SDIVREM_I32, LibcallImpl::__rt_sdiv, CallingConv::ARM_AAPCS},
          {Libcall::SDIVREM_I64, LibcallImpl::__rt_sdiv64, CallingConv::ARM_AAPCS},
          {Libcall::UDIVREM_I32, LibcallImpl::__rt_udiv, CallingConv::ARM_AAPCS},
          {Libcall::UDIVREM_I64, LibcallImpl::__rt_udiv64, CallingConv::ARM_AAPCS},
      };

      for (const auto &LC : LibraryCalls) {
        Info.setLibcallImpl(LC.Op, LC.Impl);
        Info.setLibcallImplCallingConv(LC.Impl, LC.CC);
      }
    } else {
      const struct {
        const Libcall Op;
        const LibcallImpl Impl;
      } LibraryCalls[] = {
          {Libcall::SDIVREM_I32, LibcallImpl::__aeabi_idivmod},
          {Libcall::SDIVREM_I64, LibcallImpl::__aeabi_ldivmod},
          {Libcall::UDIVREM_I32, LibcallImpl::__aeabi_uidivmod},
          {Libcall::UDIVREM_I64, LibcallImpl::__aeabi_uldivmod},
      };

      for (const auto &LC : LibraryCalls)
        Info.setLibcallImpl(LC.Op, LC.Impl);
    }
  }

  if (TT.isOSWindows()) {
    static const struct {
      const Libcall Op;
      const LibcallImpl Impl;
      const CallingConv::ID CC;
    } LibraryCalls[] = {
        {Libcall::FPTOSINT_F32_I64, LibcallImpl::__stoi64, CallingConv::ARM_AAPCS_VFP},
        {Libcall::FPTOSINT_F64_I64, LibcallImpl::__dtoi64, CallingConv::ARM_AAPCS_VFP},
        {Libcall::FPTOUINT_F32_I64, LibcallImpl::__stou64, CallingConv::ARM_AAPCS_VFP},
        {Libcall::FPTOUINT_F64_I64, LibcallImpl::__dtou64, CallingConv::ARM_AAPCS_VFP},
        {Libcall::SINTTOFP_I64_F32, LibcallImpl::__i64tos, CallingConv::ARM_AAPCS_VFP},
        {Libcall::SINTTOFP_I64_F64, LibcallImpl::__i64tod, CallingConv::ARM_AAPCS_VFP},
        {Libcall::UINTTOFP_I64_F32, LibcallImpl::__u64tos, CallingConv::ARM_AAPCS_VFP},
        {Libcall::UINTTOFP_I64_F64, LibcallImpl::__u64tod, CallingConv::ARM_AAPCS_VFP},
    };

    for (const auto &LC : LibraryCalls) {
      Info.setLibcallImpl(LC.Op, LC.Impl);
      Info.setLibcallImplCallingConv(LC.Impl, LC.CC);
    }
  }

  // Use divmod compiler-rt calls for iOS 5.0 and later.
  if (TT.isOSBinFormatMachO() && (!TT.isiOS() || !TT.isOSVersionLT(5, 0))) {
    Info.setLibcallImpl(Libcall::SDIVREM_I32, LibcallImpl::__divmodsi4);
    Info.setLibcallImpl(Libcall::UDIVREM_I32, LibcallImpl::__udivmodsi4);
  }

  static const RTLIB::LibcallImpl AAPCS_Libcalls[] = {
      LibcallImpl::__aeabi_dadd,        LibcallImpl::__aeabi_ddiv,
      LibcallImpl::__aeabi_dmul,        LibcallImpl::__aeabi_dsub,
      LibcallImpl::__aeabi_dcmpeq__oeq, LibcallImpl::__aeabi_dcmpeq__une,
      LibcallImpl::__aeabi_dcmplt,      LibcallImpl::__aeabi_dcmple,
      LibcallImpl::__aeabi_dcmpge,      LibcallImpl::__aeabi_dcmpgt,
      LibcallImpl::__aeabi_dcmpun,      LibcallImpl::__aeabi_fadd,
      LibcallImpl::__aeabi_fdiv,        LibcallImpl::__aeabi_fmul,
      LibcallImpl::__aeabi_fsub,        LibcallImpl::__aeabi_fcmpeq__oeq,
      LibcallImpl::__aeabi_fcmpeq__une, LibcallImpl::__aeabi_fcmplt,
      LibcallImpl::__aeabi_fcmple,      LibcallImpl::__aeabi_fcmpge,
      LibcallImpl::__aeabi_fcmpgt,      LibcallImpl::__aeabi_fcmpun,
      LibcallImpl::__aeabi_d2iz,        LibcallImpl::__aeabi_d2uiz,
      LibcallImpl::__aeabi_d2lz,        LibcallImpl::__aeabi_d2ulz,
      LibcallImpl::__aeabi_f2iz,        LibcallImpl::__aeabi_f2uiz,
      LibcallImpl::__aeabi_f2lz,        LibcallImpl::__aeabi_f2ulz,
      LibcallImpl::__aeabi_d2f,         LibcallImpl::__aeabi_d2h,
      LibcallImpl::__aeabi_f2d,         LibcallImpl::__aeabi_i2d,
      LibcallImpl::__aeabi_ui2d,        LibcallImpl::__aeabi_l2d,
      LibcallImpl::__aeabi_ul2d,        LibcallImpl::__aeabi_i2f,
      LibcallImpl::__aeabi_ui2f,        LibcallImpl::__aeabi_l2f,
      LibcallImpl::__aeabi_ul2f,        LibcallImpl::__aeabi_lmul,
      LibcallImpl::__aeabi_llsl,        LibcallImpl::__aeabi_llsr,
      LibcallImpl::__aeabi_lasr,        LibcallImpl::__aeabi_idiv__i8,
      LibcallImpl::__aeabi_idiv__i16,   LibcallImpl::__aeabi_idiv__i32,
      LibcallImpl::__aeabi_idivmod,     LibcallImpl::__aeabi_uidivmod,
      LibcallImpl::__aeabi_ldivmod,     LibcallImpl::__aeabi_uidiv__i8,
      LibcallImpl::__aeabi_uidiv__i16,  LibcallImpl::__aeabi_uidiv__i32,
      LibcallImpl::__aeabi_uldivmod,    LibcallImpl::__aeabi_f2h,
      LibcallImpl::__aeabi_d2h,         LibcallImpl::__aeabi_h2f,
      LibcallImpl::__aeabi_memcpy,      LibcallImpl::__aeabi_memmove,
      LibcallImpl::__aeabi_memset,      LibcallImpl::__aeabi_memcpy4,
      LibcallImpl::__aeabi_memcpy8,     LibcallImpl::__aeabi_memmove4,
      LibcallImpl::__aeabi_memmove8,    LibcallImpl::__aeabi_memset4,
      LibcallImpl::__aeabi_memset8,     LibcallImpl::__aeabi_memclr,
      LibcallImpl::__aeabi_memclr4,     LibcallImpl::__aeabi_memclr8};

  for (LibcallImpl Impl : AAPCS_Libcalls)
    Info.setLibcallImplCallingConv(Impl, CallingConv::ARM_AAPCS);
}

static void setLongDoubleIsF128Libm(RuntimeLibcallsInfo &Info,
                                    bool FiniteOnlyFuncs = false) {
  Info.setLibcallImpl(Libcall::REM_F128, LibcallImpl::fmodf128);
  Info.setLibcallImpl(Libcall::FMA_F128, LibcallImpl::fmaf128);
  Info.setLibcallImpl(Libcall::SQRT_F128, LibcallImpl::sqrtf128);
  Info.setLibcallImpl(Libcall::CBRT_F128, LibcallImpl::cbrtf128);
  Info.setLibcallImpl(Libcall::LOG_F128, LibcallImpl::logf128);
  Info.setLibcallImpl(Libcall::LOG2_F128, LibcallImpl::log2f128);
  Info.setLibcallImpl(Libcall::LOG10_F128, LibcallImpl::log10f128);
  Info.setLibcallImpl(Libcall::EXP_F128, LibcallImpl::expf128);
  Info.setLibcallImpl(Libcall::EXP2_F128, LibcallImpl::exp2f128);
  Info.setLibcallImpl(Libcall::EXP10_F128, LibcallImpl::exp10f128);
  Info.setLibcallImpl(Libcall::SIN_F128, LibcallImpl::sinf128);
  Info.setLibcallImpl(Libcall::COS_F128, LibcallImpl::cosf128);
  Info.setLibcallImpl(Libcall::TAN_F128, LibcallImpl::tanf128);
  Info.setLibcallImpl(Libcall::SINCOS_F128, LibcallImpl::sincosf128);
  Info.setLibcallImpl(Libcall::ASIN_F128, LibcallImpl::asinf128);
  Info.setLibcallImpl(Libcall::ACOS_F128, LibcallImpl::acosf128);
  Info.setLibcallImpl(Libcall::ATAN_F128, LibcallImpl::atanf128);
  Info.setLibcallImpl(Libcall::ATAN2_F128, LibcallImpl::atan2f128);
  Info.setLibcallImpl(Libcall::SINH_F128, LibcallImpl::sinhf128);
  Info.setLibcallImpl(Libcall::COSH_F128, LibcallImpl::coshf128);
  Info.setLibcallImpl(Libcall::TANH_F128, LibcallImpl::tanhf128);
  Info.setLibcallImpl(Libcall::POW_F128, LibcallImpl::powf128);
  Info.setLibcallImpl(Libcall::CEIL_F128, LibcallImpl::ceilf128);
  Info.setLibcallImpl(Libcall::TRUNC_F128, LibcallImpl::truncf128);
  Info.setLibcallImpl(Libcall::RINT_F128, LibcallImpl::rintf128);
  Info.setLibcallImpl(Libcall::NEARBYINT_F128, LibcallImpl::nearbyintf128);
  Info.setLibcallImpl(Libcall::ROUND_F128, LibcallImpl::roundf128);
  Info.setLibcallImpl(Libcall::ROUNDEVEN_F128, LibcallImpl::roundevenf128);
  Info.setLibcallImpl(Libcall::FLOOR_F128, LibcallImpl::floorf128);
  Info.setLibcallImpl(Libcall::COPYSIGN_F128, LibcallImpl::copysignf128);
  Info.setLibcallImpl(Libcall::FMIN_F128, LibcallImpl::fminf128);
  Info.setLibcallImpl(Libcall::FMAX_F128, LibcallImpl::fmaxf128);
  Info.setLibcallImpl(Libcall::FMINIMUM_F128, LibcallImpl::fminimumf128);
  Info.setLibcallImpl(Libcall::FMAXIMUM_F128, LibcallImpl::fmaximumf128);
  Info.setLibcallImpl(Libcall::FMINIMUM_NUM_F128, LibcallImpl::fminimum_numf128);
  Info.setLibcallImpl(Libcall::FMAXIMUM_NUM_F128, LibcallImpl::fmaximum_numf128);
  Info.setLibcallImpl(Libcall::LROUND_F128, LibcallImpl::lroundf128);
  Info.setLibcallImpl(Libcall::LLROUND_F128, LibcallImpl::llroundf128);
  Info.setLibcallImpl(Libcall::LRINT_F128, LibcallImpl::lrintf128);
  Info.setLibcallImpl(Libcall::LLRINT_F128, LibcallImpl::llrintf128);
  Info.setLibcallImpl(Libcall::LDEXP_F128, LibcallImpl::ldexpf128);
  Info.setLibcallImpl(Libcall::FREXP_F128, LibcallImpl::frexpf128);
  Info.setLibcallImpl(Libcall::MODF_F128, LibcallImpl::modff128);

  if (FiniteOnlyFuncs) {
    Info.setLibcallImpl(Libcall::LOG_FINITE_F128, LibcallImpl::__logf128_finite);
    Info.setLibcallImpl(Libcall::LOG2_FINITE_F128, LibcallImpl::__log2f128_finite);
    Info.setLibcallImpl(Libcall::LOG10_FINITE_F128, LibcallImpl::__log10f128_finite);
    Info.setLibcallImpl(Libcall::EXP_FINITE_F128, LibcallImpl::__expf128_finite);
    Info.setLibcallImpl(Libcall::EXP2_FINITE_F128, LibcallImpl::__exp2f128_finite);
    Info.setLibcallImpl(Libcall::POW_FINITE_F128, LibcallImpl::__powf128_finite);
  } else {
    Info.setLibcallImpl(Libcall::LOG_FINITE_F128, LibcallImpl::Unsupported);
    Info.setLibcallImpl(Libcall::LOG2_FINITE_F128, LibcallImpl::Unsupported);
    Info.setLibcallImpl(Libcall::LOG10_FINITE_F128, LibcallImpl::Unsupported);
    Info.setLibcallImpl(Libcall::EXP_FINITE_F128, LibcallImpl::Unsupported);
    Info.setLibcallImpl(Libcall::EXP2_FINITE_F128, LibcallImpl::Unsupported);
    Info.setLibcallImpl(Libcall::POW_FINITE_F128, LibcallImpl::Unsupported);
  }
}

void RTLIB::RuntimeLibcallsInfo::initDefaultLibCallImpls() {
  std::memcpy(LibcallImpls, DefaultLibcallImpls, sizeof(LibcallImpls));
  static_assert(sizeof(LibcallImpls) == sizeof(DefaultLibcallImpls),
                "libcall array size should match");
}

/// Set default libcall names. If a target wants to opt-out of a libcall it
/// should be placed here.
void RuntimeLibcallsInfo::initLibcalls(const Triple &TT,
                                       ExceptionHandling ExceptionModel,
                                       FloatABI::ABIType FloatABI,
                                       EABI EABIVersion, StringRef ABIName) {
  setTargetRuntimeLibcallSets(TT);

  // Use the f128 variants of math functions on x86
  if (TT.isX86() && TT.isGNUEnvironment())
    setLongDoubleIsF128Libm(*this, /*FiniteOnlyFuncs=*/true);

  if (TT.isX86() || TT.isVE() || TT.isARM() || TT.isThumb()) {
    if (ExceptionModel == ExceptionHandling::SjLj)
      setLibcallImpl(Libcall::UNWIND_RESUME, LibcallImpl::_Unwind_SjLj_Resume);
  }

  // A few names are different on particular architectures or environments.
  if (TT.isOSDarwin()) {
    // For f16/f32 conversions, Darwin uses the standard naming scheme,
    // instead of the gnueabi-style __gnu_*_ieee.
    // FIXME: What about other targets?
    setLibcallImpl(Libcall::FPEXT_F16_F32, LibcallImpl::__extendhfsf2);
    setLibcallImpl(Libcall::FPROUND_F32_F16, LibcallImpl::__truncsfhf2);

    // Some darwins have an optimized __bzero/bzero function.
    if (TT.isX86()) {
      if (TT.isMacOSX() && !TT.isMacOSXVersionLT(10, 6))
        setLibcallImpl(Libcall::BZERO, LibcallImpl::__bzero);
    }

    if (darwinHasSinCosStret(TT)) {
      setLibcallImpl(Libcall::SINCOS_STRET_F32, LibcallImpl::__sincosf_stret);
      setLibcallImpl(Libcall::SINCOS_STRET_F64, LibcallImpl::__sincos_stret);
      if (TT.isWatchABI()) {
        setLibcallImplCallingConv(LibcallImpl::__sincosf_stret,
                                  CallingConv::ARM_AAPCS_VFP);
        setLibcallImplCallingConv(LibcallImpl::__sincos_stret,
                                  CallingConv::ARM_AAPCS_VFP);
      }
    }

    if (darwinHasExp10(TT)) {
      setLibcallImpl(Libcall::EXP10_F32, LibcallImpl::__exp10f);
      setLibcallImpl(Libcall::EXP10_F64, LibcallImpl::__exp10);
    } else {
      setLibcallImpl(Libcall::EXP10_F32, LibcallImpl::Unsupported);
      setLibcallImpl(Libcall::EXP10_F64, LibcallImpl::Unsupported);
    }
  }

  if (hasSinCos(TT)) {
    setLibcallImpl(Libcall::SINCOS_F32, LibcallImpl::sincosf);
    setLibcallImpl(Libcall::SINCOS_F64, LibcallImpl::sincos);
    setLibcallImpl(Libcall::SINCOS_F80, LibcallImpl::sincos_f80);
    setLibcallImpl(Libcall::SINCOS_F128, LibcallImpl::sincos_f128);
    setLibcallImpl(Libcall::SINCOS_PPCF128, LibcallImpl::sincos_ppcf128);
  }

  if (TT.isPS()) {
    setLibcallImpl(Libcall::SINCOS_F32, LibcallImpl::sincosf);
    setLibcallImpl(Libcall::SINCOS_F64, LibcallImpl::sincos);
  }

  if (TT.isOSOpenBSD()) {
    setLibcallImpl(Libcall::STACKPROTECTOR_CHECK_FAIL, LibcallImpl::Unsupported);
  }

  if (TT.isOSWindows() && !TT.isOSCygMing()) {
    setLibcallImpl(Libcall::LDEXP_F32, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::LDEXP_F80, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::LDEXP_F128, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::LDEXP_PPCF128, LibcallImpl::Unsupported);

    setLibcallImpl(Libcall::FREXP_F32, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::FREXP_F80, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::FREXP_F128, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::FREXP_PPCF128, LibcallImpl::Unsupported);
  }

  if (TT.isOSMSVCRT()) {
    // MSVCRT doesn't have powi; fall back to pow
    setLibcallImpl(Libcall::POWI_F32, LibcallImpl::Unsupported);
    setLibcallImpl(Libcall::POWI_F64, LibcallImpl::Unsupported);
  }

  // Setup Windows compiler runtime calls.
  if (TT.getArch() == Triple::x86 &&
      (TT.isWindowsMSVCEnvironment() || TT.isWindowsItaniumEnvironment())) {
    static const struct {
      const Libcall Op;
      const LibcallImpl Impl;
      const CallingConv::ID CC;
    } LibraryCalls[] = {
        {Libcall::SDIV_I64, LibcallImpl::_alldiv, CallingConv::X86_StdCall},
        {Libcall::UDIV_I64, LibcallImpl::_aulldiv, CallingConv::X86_StdCall},
        {Libcall::SREM_I64, LibcallImpl::_allrem, CallingConv::X86_StdCall},
        {Libcall::UREM_I64, LibcallImpl::_aullrem, CallingConv::X86_StdCall},
        {Libcall::MUL_I64, LibcallImpl::_allmul, CallingConv::X86_StdCall},
    };

    for (const auto &LC : LibraryCalls) {
      setLibcallImpl(LC.Op, LC.Impl);
      setLibcallImplCallingConv(LC.Impl, LC.CC);
    }
  }

  if (TT.isARM() || TT.isThumb())
    setARMLibcallNames(*this, TT, FloatABI, EABIVersion);

  if (!TT.isWasm()) {
    // These libcalls are only available in compiler-rt, not libgcc.
    if (TT.isArch32Bit()) {
      setLibcallImpl(Libcall::SHL_I128, LibcallImpl::Unsupported);
      setLibcallImpl(Libcall::SRL_I128, LibcallImpl::Unsupported);
      setLibcallImpl(Libcall::SRA_I128, LibcallImpl::Unsupported);
      setLibcallImpl(Libcall::MUL_I128, LibcallImpl::Unsupported);
      setLibcallImpl(Libcall::MULO_I64, LibcallImpl::Unsupported);
    }

    setLibcallImpl(Libcall::MULO_I128, LibcallImpl::Unsupported);
  }

  if (TT.getArch() == Triple::ArchType::hexagon) {
    setLibcallImpl(Libcall::SDIV_I32, LibcallImpl::__hexagon_divsi3);
    setLibcallImpl(Libcall::SDIV_I64, LibcallImpl::__hexagon_divdi3);
    setLibcallImpl(Libcall::UDIV_I32, LibcallImpl::__hexagon_udivsi3);
    setLibcallImpl(Libcall::UDIV_I64, LibcallImpl::__hexagon_udivdi3);
    setLibcallImpl(Libcall::SREM_I32, LibcallImpl::__hexagon_modsi3);
    setLibcallImpl(Libcall::SREM_I64, LibcallImpl::__hexagon_moddi3);
    setLibcallImpl(Libcall::UREM_I32, LibcallImpl::__hexagon_umodsi3);
    setLibcallImpl(Libcall::UREM_I64, LibcallImpl::__hexagon_umoddi3);

    const bool FastMath = HexagonEnableFastMathRuntimeCalls;
    // This is the only fast library function for sqrtd.
    if (FastMath)
      setLibcallImpl(Libcall::SQRT_F64, LibcallImpl::__hexagon_fast2_sqrtdf2);

    // Prefix is: nothing  for "slow-math",
    //            "fast2_" for V5+ fast-math double-precision
    // (actually, keep fast-math and fast-math2 separate for now)
    if (FastMath) {
      setLibcallImpl(Libcall::ADD_F64, LibcallImpl::__hexagon_fast_adddf3);
      setLibcallImpl(Libcall::SUB_F64, LibcallImpl::__hexagon_fast_subdf3);
      setLibcallImpl(Libcall::MUL_F64, LibcallImpl::__hexagon_fast_muldf3);
      setLibcallImpl(Libcall::DIV_F64, LibcallImpl::__hexagon_fast_divdf3);
      setLibcallImpl(Libcall::DIV_F32, LibcallImpl::__hexagon_fast_divsf3);
    } else {
      setLibcallImpl(Libcall::ADD_F64, LibcallImpl::__hexagon_adddf3);
      setLibcallImpl(Libcall::SUB_F64, LibcallImpl::__hexagon_subdf3);
      setLibcallImpl(Libcall::MUL_F64, LibcallImpl::__hexagon_muldf3);
      setLibcallImpl(Libcall::DIV_F64, LibcallImpl::__hexagon_divdf3);
      setLibcallImpl(Libcall::DIV_F32, LibcallImpl::__hexagon_divsf3);
    }

    if (FastMath)
      setLibcallImpl(Libcall::SQRT_F32, LibcallImpl::__hexagon_fast2_sqrtf);
    else
      setLibcallImpl(Libcall::SQRT_F32, LibcallImpl::__hexagon_sqrtf);

    setLibcallImpl(
        Libcall::HEXAGON_MEMCPY_LIKELY_ALIGNED_MIN32BYTES_MULT8BYTES,
        LibcallImpl::__hexagon_memcpy_likely_aligned_min32bytes_mult8bytes);
  }

  if (TT.getArch() == Triple::ArchType::msp430) {
    setLibcallImplCallingConv(LibcallImpl::__mspabi_mpyll,
                              CallingConv::MSP430_BUILTIN);
  }
}

bool RuntimeLibcallsInfo::darwinHasExp10(const Triple &TT) {
  switch (TT.getOS()) {
  case Triple::MacOSX:
    return !TT.isMacOSXVersionLT(10, 9);
  case Triple::IOS:
    return !TT.isOSVersionLT(7, 0);
  case Triple::DriverKit:
  case Triple::TvOS:
  case Triple::WatchOS:
  case Triple::XROS:
  case Triple::BridgeOS:
    return true;
  default:
    return false;
  }
}
