//===- RuntimeLibcalls.h - Interface for runtime libcalls -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a common interface to work with library calls into a
// runtime that may be emitted by a given backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_IR_RUNTIME_LIBCALLS_H
#define LLVM_IR_RUNTIME_LIBCALLS_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Sequence.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/AtomicOrdering.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

/// TableGen will produce 2 enums, RTLIB::Libcall and
/// RTLIB::LibcallImpl. RTLIB::Libcall describes abstract functionality the
/// compiler may choose to access, RTLIB::LibcallImpl describes a particular ABI
/// implementation, which includes a name and type signature.
#define GET_RUNTIME_LIBCALL_ENUM
#include "llvm/IR/RuntimeLibcalls.inc"
#undef GET_RUNTIME_LIBCALL_ENUM

namespace llvm {
namespace RTLIB {

/// A simple container for information about the supported runtime calls.
struct RuntimeLibcallsInfo {
  explicit RuntimeLibcallsInfo(
      const Triple &TT,
      ExceptionHandling ExceptionModel = ExceptionHandling::None,
      FloatABI::ABIType FloatABI = FloatABI::Default,
      EABI EABIVersion = EABI::Default, StringRef ABIName = "") {
    // FIXME: The ExceptionModel parameter is to handle the field in
    // TargetOptions. This interface fails to distinguish the forced disable
    // case for targets which support exceptions by default. This should
    // probably be a module flag and removed from TargetOptions.
    if (ExceptionModel == ExceptionHandling::None)
      ExceptionModel = TT.getDefaultExceptionHandling();

    initLibcalls(TT, ExceptionModel, FloatABI, EABIVersion, ABIName);
  }

  /// Rename the default libcall routine name for the specified libcall.
  void setLibcallImpl(Libcall Call, LibcallImpl Impl) {
    LibcallImpls[Call] = Impl;
  }

  /// Get the libcall routine name for the specified libcall.
  // FIXME: This should be removed. Only LibcallImpl should have a name.
  const char *getLibcallName(Libcall Call) const {
    return LibCallImplNames[LibcallImpls[Call]];
  }

  /// Get the libcall routine name for the specified libcall implementation.
  const char *getLibcallImplName(LibcallImpl CallImpl) const {
    return LibCallImplNames[CallImpl];
  }

  /// Return the lowering's selection of implementation call for \p Call
  LibcallImpl getLibcallImpl(Libcall Call) const {
    return LibcallImpls[Call];
  }

  /// Set the CallingConv that should be used for the specified libcall
  /// implementation
  void setLibcallImplCallingConv(LibcallImpl Call, CallingConv::ID CC) {
    LibcallImplCallingConvs[Call] = CC;
  }

  // FIXME: Remove this wrapper in favor of directly using
  // getLibcallImplCallingConv
  CallingConv::ID getLibcallCallingConv(Libcall Call) const {
    return LibcallImplCallingConvs[LibcallImpls[Call]];
  }

  /// Get the CallingConv that should be used for the specified libcall.
  CallingConv::ID getLibcallImplCallingConv(LibcallImpl Call) const {
    return LibcallImplCallingConvs[Call];
  }

  ArrayRef<LibcallImpl> getLibcallImpls() const {
    // Trim UNKNOWN_LIBCALL from the back
    return ArrayRef(LibcallImpls).drop_back();
  }

  /// Return a function name compatible with Libcall::MEMCPY, or nullptr if fully
  /// unsupported.
  const char *getMemcpyName() const {
    if (const char *Memcpy = getLibcallName(Libcall::MEMCPY))
      return Memcpy;

    // Fallback to memmove if memcpy isn't available.
    return getLibcallName(Libcall::MEMMOVE);
  }

  /// Return the libcall provided by \p Impl
  static Libcall getLibcallFromImpl(LibcallImpl Impl) {
    return ImplToLibcall[Impl];
  }

private:
  static const LibcallImpl
      DefaultLibcallImpls[Libcall::UNKNOWN_LIBCALL + 1];

  /// Stores the implementation choice for each each libcall.
  LibcallImpl LibcallImpls[Libcall::UNKNOWN_LIBCALL + 1] = {
      LibcallImpl::Unsupported};

  static_assert(static_cast<int>(CallingConv::C) == 0,
                "default calling conv should be encoded as 0");

  /// Stores the CallingConv that should be used for each libcall
  /// implementation.;
  CallingConv::ID LibcallImplCallingConvs[LibcallImpl::NumLibcallImpls] = {};

  /// Names of concrete implementations of runtime calls. e.g. __ashlsi3 for
  /// SHL_I32
  LLVM_ABI static const char *const
      LibCallImplNames[LibcallImpl::NumLibcallImpls];

  /// Map from a concrete LibcallImpl implementation to its Libcall kind.
  static const Libcall ImplToLibcall[LibcallImpl::NumLibcallImpls];

  static bool darwinHasSinCosStret(const Triple &TT) {
    if (!TT.isOSDarwin())
      return false;

    // Don't bother with 32 bit x86.
    if (TT.getArch() == Triple::x86)
      return false;
    // Macos < 10.9 has no sincos_stret.
    if (TT.isMacOSX())
      return !TT.isMacOSXVersionLT(10, 9) && TT.isArch64Bit();
    // iOS < 7.0 has no sincos_stret.
    if (TT.isiOS())
      return !TT.isOSVersionLT(7, 0);
    // Any other darwin such as WatchOS/TvOS is new enough.
    return true;
  }

  static bool darwinHasExp10(const Triple &TT);

  /// Return true if the target has sincosf/sincos/sincosl functions
  static bool hasSinCos(const Triple &TT) {
    return TT.isGNUEnvironment() || TT.isOSFuchsia() ||
           (TT.isAndroid() && !TT.isAndroidVersionLT(9));
  }

  LLVM_ABI void initDefaultLibCallImpls();

  /// Generated by tablegen.
  void setTargetRuntimeLibcallSets(const Triple &TT);

  /// Set default libcall names. If a target wants to opt-out of a libcall it
  /// should be placed here.
  LLVM_ABI void initLibcalls(const Triple &TT, ExceptionHandling ExceptionModel,
                             FloatABI::ABIType FloatABI, EABI ABIType,
                             StringRef ABIName);
};

} // namespace RTLIB
} // namespace llvm

#endif // LLVM_IR_RUNTIME_LIBCALLS_H
