// RUN: llvm-tblgen -gen-runtime-libcalls -I %p/../../include %s | FileCheck %s

include "llvm/IR/RuntimeLibcallsImpl.td"


def SHL_I32 : RuntimeLibcall;
def SRL_I64 : RuntimeLibcall;

def SQRT_F128 : RuntimeLibcall;
def SQRT_F80 : RuntimeLibcall;
def BZERO : RuntimeLibcall;
def MEMCPY : RuntimeLibcall;
def MEMSET : RuntimeLibcall;
def CALLOC : RuntimeLibcall;

// Test default names.
let IsDefault = true in {
  def __ashlsi3 : RuntimeLibcallImpl<SHL_I32>;
  def __lshrdi3 : RuntimeLibcallImpl<SRL_I64>;

  def sqrtl_f128 : RuntimeLibcallImpl<SQRT_F128, "sqrtl">;
  def sqrtl_f80 : RuntimeLibcallImpl<SQRT_F80, "sqrtl">;
}

// Ignore non-default in initDefaultLibCallNames.
def bzero : RuntimeLibcallImpl<BZERO>;

def ___memset : RuntimeLibcallImpl<MEMSET, "___memset">;
def ___memcpy : RuntimeLibcallImpl<MEMCPY, "___memcpy">;

def calloc  : RuntimeLibcallImpl<CALLOC, "calloc">;

def CompilerRTLibcalls : LibcallImpls<(add __ashlsi3, __lshrdi3)>;
def LibmLibcalls : LibcallImpls<(add sqrtl_f80)>;

def isSimpleArch : RuntimeLibcallPredicate<[{TT.getArch() == Triple::simple}]>;
def isFooArch : RuntimeLibcallPredicate<[{TT.getArch() == Triple::foo}]>;


def isZOS : RuntimeLibcallPredicate<[{TT.getOS() == Triple::zos}]>;
def isPPC : RuntimeLibcallPredicate<[{TT.getArch().isPPC()}]>;
def isPPC64 : RuntimeLibcallPredicate<[{TT.getArch().isPPC64()}]>;


def isFoo : RuntimeLibcallPredicate<[{isFOO()}]>;
def isBarOS : RuntimeLibcallPredicate<[{TT.getOS() == Triple::bar}]>;
def isBuzzArch : RuntimeLibcallPredicate<[{TT.getArch() == Triple::buzz}]>;
def isBlahArch : RuntimeLibcallPredicate<[{TT.getArch() == Triple::blah}]>;

def hasCompilerRT : RuntimeLibcallPredicate<[{TT.hasCompilerRT()}]>;


def SimpleLibrary : SystemRuntimeLibrary<isSimpleArch,
  (add LibmLibcalls, calloc, CompilerRTLibcalls)>;


def LibraryWithConditionalFunc : LibcallImpls<(add sqrtl_f128, bzero, AvailableIf<___memset, isBarOS>)>;

def FooLibrary : SystemRuntimeLibrary<isFooArch, (add LibraryWithConditionalFunc)>;

def BuzzLibrary : SystemRuntimeLibrary<isBuzzArch, (add sqrtl_f80, CompilerRTLibcalls)>;



def LibraryWithConditionalSet : LibcallImpls<(add sqrtl_f128, bzero,
  LibcallImpls<(add CompilerRTLibcalls), hasCompilerRT>)>;



def BlahLibrary : SystemRuntimeLibrary<isBlahArch, (add calloc, LibraryWithConditionalSet, AvailableIf<___memset, isBarOS>)>;



// All entries should be emitted in Libcall enum.
// CHECK: #ifdef GET_RUNTIME_LIBCALL_ENUM
// CHECK-NEXT: namespace llvm {
// CHECK-NEXT: namespace RTLIB {
// CHECK-NEXT: class Libcall {
// CHECK: enum Value : unsigned short {
// CHECK-NEXT: BZERO = 0,
// CHECK-NEXT: CALLOC = 1,
// CHECK-NEXT: MEMCPY = 2,
// CHECK-NEXT: MEMSET = 3,
// CHECK-NEXT: SHL_I32 = 4,
// CHECK-NEXT: SQRT_F80 = 5,
// CHECK-NEXT: SQRT_F128 = 6,
// CHECK-NEXT: SRL_I64 = 7,
// CHECK-NEXT: UNKNOWN_LIBCALL = 8
// CHECK-NEXT: };
// CHECK: }; // End class Libcall
// CHECK:class LibcallImpl {
// CHECK:enum Value : unsigned short {
// CHECK-NEXT:  Unsupported = 0,
// CHECK-NEXT:  ___memcpy = 1, // ___memcpy
// CHECK-NEXT:  ___memset = 2, // ___memset
// CHECK-NEXT:  __ashlsi3 = 3, // __ashlsi3
// CHECK-NEXT:  __lshrdi3 = 4, // __lshrdi3
// CHECK-NEXT:  bzero = 5, // bzero
// CHECK-NEXT:  calloc = 6, // calloc
// CHECK-NEXT:  sqrtl_f80 = 7, // sqrtl
// CHECK-NEXT:  sqrtl_f128 = 8, // sqrtl
// CHECK-NEXT:  NumLibcallImpls = 9
// CHECK-NEXT: };
// CHECK: }; // End class LibcallImpl
// CHECK: } // End namespace RTLIB
// CHECK: } // End namespace llvm
// CHECK-NEXT: #endif

// CHECK: #ifdef GET_INIT_RUNTIME_LIBCALL_NAMES
// CHECK-NEXT: const RTLIB::LibcallImpl llvm::RTLIB::RuntimeLibcallsInfo::DefaultLibcallImpls[RTLIB::Libcall::UNKNOWN_LIBCALL + 1] = {
// CHECK-NEXT:   RTLIB::LibcallImpl::Unsupported, // RTLIB::Libcall::BZERO
// CHECK-NEXT:   RTLIB::LibcallImpl::Unsupported, // RTLIB::Libcall::CALLOC
// CHECK-NEXT:   RTLIB::LibcallImpl::Unsupported, // RTLIB::Libcall::MEMCPY
// CHECK-NEXT:   RTLIB::LibcallImpl::Unsupported, // RTLIB::Libcall::MEMSET
// CHECK-NEXT:   RTLIB::LibcallImpl::__ashlsi3, // RTLIB::Libcall::SHL_I32
// CHECK-NEXT:   RTLIB::LibcallImpl::sqrtl_f80, // RTLIB::Libcall::SQRT_F80
// CHECK-NEXT:   RTLIB::LibcallImpl::sqrtl_f128, // RTLIB::Libcall::SQRT_F128
// CHECK-NEXT:   RTLIB::LibcallImpl::__lshrdi3, // RTLIB::Libcall::SRL_I64
// CHECK-NEXT:   RTLIB::LibcallImpl::Unsupported
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: const char *const llvm::RTLIB::RuntimeLibcallsInfo::LibCallImplNames[RTLIB::LibcallImpl::NumLibcallImpls] = {
// CHECK-NEXT: nullptr, // RTLIB::LibcallImpl::Unsupported
// CHECK-NEXT: "___memcpy", // RTLIB::LibcallImpl::___memcpy
// CHECK-NEXT: "___memset", // RTLIB::LibcallImpl::___memset
// CHECK-NEXT: "__ashlsi3", // RTLIB::LibcallImpl::__ashlsi3
// CHECK-NEXT: "__lshrdi3", // RTLIB::LibcallImpl::__lshrdi3
// CHECK-NEXT: "bzero", // RTLIB::LibcallImpl::bzero
// CHECK-NEXT: "calloc", // RTLIB::LibcallImpl::calloc
// CHECK-NEXT: "sqrtl", // RTLIB::LibcallImpl::sqrtl_f80
// CHECK-NEXT: "sqrtl", // RTLIB::LibcallImpl::sqrtl_f128
// CHECK-NEXT: };

// CHECK: const RTLIB::Libcall llvm::RTLIB::RuntimeLibcallsInfo::ImplToLibcall[RTLIB::LibcallImpl::NumLibcallImpls] = {
// CHECK-NEXT: RTLIB::Libcall::UNKNOWN_LIBCALL, // RTLIB::LibcallImpl::Unsupported
// CHECK-NEXT: RTLIB::Libcall::MEMCPY, // RTLIB::LibcallImpl::___memcpy
// CHECK-NEXT: RTLIB::Libcall::MEMSET, // RTLIB::LibcallImpl::___memset
// CHECK-NEXT: RTLIB::Libcall::SHL_I32, // RTLIB::LibcallImpl::__ashlsi3
// CHECK-NEXT: RTLIB::Libcall::SRL_I64, // RTLIB::LibcallImpl::__lshrdi3
// CHECK-NEXT: RTLIB::Libcall::BZERO, // RTLIB::LibcallImpl::bzero
// CHECK-NEXT: RTLIB::Libcall::CALLOC, // RTLIB::LibcallImpl::calloc
// CHECK-NEXT: RTLIB::Libcall::SQRT_F80, // RTLIB::LibcallImpl::sqrtl_f80
// CHECK-NEXT: RTLIB::Libcall::SQRT_F128, // RTLIB::LibcallImpl::sqrtl_f128
// CHECK-NEXT: };


// CHECK: void llvm::RTLIB::RuntimeLibcallsInfo::setTargetRuntimeLibcallSets(const llvm::Triple &TT) {
// CHECK-NEXT:  struct LibcallImplPair {
// CHECK-NEXT:    RTLIB::Libcall Func;
// CHECK-NEXT:    RTLIB::LibcallImpl Impl;
// CHECK-NEXT:  };
// CHECK-EMPTY:
// CHECK-NEXT: if (TT.getArch() == Triple::blah) {
// CHECK-NEXT:     static const LibcallImplPair LibraryCalls[] = {
// CHECK-NEXT:         {RTLIB::Libcall::BZERO, RTLIB::LibcallImpl::bzero}, // bzero
// CHECK-NEXT:         {RTLIB::Libcall::CALLOC, RTLIB::LibcallImpl::calloc}, // calloc
// CHECK-NEXT:         {RTLIB::Libcall::SQRT_F128, RTLIB::LibcallImpl::sqrtl_f128}, // sqrtl
// CHECK-NEXT:     };
// CHECK-EMPTY:
// CHECK-NEXT:     for (const auto [Func, Impl] : LibraryCalls) {
// CHECK-NEXT:       setLibcallImpl(Func, Impl);
// CHECK-NEXT:     }
// CHECK-EMPTY:
// CHECK-NEXT:    if (TT.hasCompilerRT()) {
// CHECK-NEXT:      static const LibcallImplPair LibraryCalls_hasCompilerRT[] = {
// CHECK-NEXT:          {RTLIB::Libcall::SHL_I32, RTLIB::LibcallImpl::__ashlsi3}, // __ashlsi3
// CHECK-NEXT:          {RTLIB::Libcall::SRL_I64, RTLIB::LibcallImpl::__lshrdi3}, // __lshrdi3
// CHECK-NEXT:      };
// CHECK-EMPTY:
// CHECK-NEXT:      for (const auto [Func, Impl] : LibraryCalls_hasCompilerRT) {
// CHECK-NEXT:        setLibcallImpl(Func, Impl);
// CHECK-NEXT:      }
// CHECK-EMPTY:
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:    if (TT.getOS() == Triple::bar) {
// CHECK-NEXT:      static const LibcallImplPair LibraryCalls_isBarOS[] = {
// CHECK-NEXT:          {RTLIB::Libcall::MEMSET, RTLIB::LibcallImpl::___memset}, // ___memset
// CHECK-NEXT:      };
// CHECK-EMPTY:
// CHECK-NEXT:      for (const auto [Func, Impl] : LibraryCalls_isBarOS) {
// CHECK-NEXT:        setLibcallImpl(Func, Impl);
// CHECK-NEXT:      }
// CHECK-EMPTY:
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:   return;
// CHECK-NEXT: }
// CHECK-EMPTY:
// CHECK-NEXT: if (TT.getArch() == Triple::buzz) {
// CHECK-NEXT:    static const LibcallImplPair LibraryCalls[] = {
// CHECK-NEXT:        {RTLIB::Libcall::SHL_I32, RTLIB::LibcallImpl::__ashlsi3}, // __ashlsi3
// CHECK-NEXT:        {RTLIB::Libcall::SQRT_F80, RTLIB::LibcallImpl::sqrtl_f80}, // sqrtl
// CHECK-NEXT:        {RTLIB::Libcall::SRL_I64, RTLIB::LibcallImpl::__lshrdi3}, // __lshrdi3
// CHECK-NEXT:    };
// CHECK-EMPTY:
// CHECK-NEXT:    for (const auto [Func, Impl] : LibraryCalls) {
// CHECK-NEXT:      setLibcallImpl(Func, Impl);
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:   return;
// CHECK-NEXT: }
// CHECK-EMPTY:
// CHECK-NEXT: if (TT.getArch() == Triple::foo) {
// CHECK-NEXT:    static const LibcallImplPair LibraryCalls[] = {
// CHECK-NEXT:        {RTLIB::Libcall::BZERO, RTLIB::LibcallImpl::bzero}, // bzero
// CHECK-NEXT:        {RTLIB::Libcall::SQRT_F128, RTLIB::LibcallImpl::sqrtl_f128}, // sqrtl
// CHECK-NEXT:    };
// CHECK-EMPTY:
// CHECK-NEXT:    for (const auto [Func, Impl] : LibraryCalls) {
// CHECK-NEXT:      setLibcallImpl(Func, Impl);
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:    if (TT.getOS() == Triple::bar) {
// CHECK-NEXT:      static const LibcallImplPair LibraryCalls_isBarOS[] = {
// CHECK-NEXT:          {RTLIB::Libcall::MEMSET, RTLIB::LibcallImpl::___memset}, // ___memset
// CHECK-NEXT:      };
// CHECK-EMPTY:
// CHECK-NEXT:      for (const auto [Func, Impl] : LibraryCalls_isBarOS) {
// CHECK-NEXT:        setLibcallImpl(Func, Impl);
// CHECK-NEXT:      }
// CHECK-EMPTY:
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:    return;
// CHECK-NEXT:  }
// CHECK-EMPTY:
// CHECK-NEXT: if (TT.getArch() == Triple::simple) {
// CHECK-NEXT:    static const LibcallImplPair LibraryCalls[] = {
// CHECK-NEXT:        {RTLIB::Libcall::CALLOC, RTLIB::LibcallImpl::calloc}, // calloc
// CHECK-NEXT:        {RTLIB::Libcall::SHL_I32, RTLIB::LibcallImpl::__ashlsi3}, // __ashlsi3
// CHECK-NEXT:        {RTLIB::Libcall::SQRT_F80, RTLIB::LibcallImpl::sqrtl_f80}, // sqrtl
// CHECK-NEXT:        {RTLIB::Libcall::SRL_I64, RTLIB::LibcallImpl::__lshrdi3}, // __lshrdi3
// CHECK-NEXT:    };
// CHECK-EMPTY:
// CHECK-NEXT:    for (const auto [Func, Impl] : LibraryCalls) {
// CHECK-NEXT:      setLibcallImpl(Func, Impl);
// CHECK-NEXT:    }
// CHECK-EMPTY:
// CHECK-NEXT:   return;
// CHECK-NEXT: }
// CHECK-NEXT:  initDefaultLibCallImpls();
// CHECK-NEXT: }
// CHECK-EMPTY:
// CHECK: #endif
