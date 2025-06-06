//===-- unittests/Runtime/CUDA/Allocatable.cpp ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "flang/Runtime/allocatable.h"
#include "cuda_runtime.h"
#include "gtest/gtest.h"
#include "flang-rt/runtime/allocator-registry.h"
#include "flang-rt/runtime/descriptor.h"
#include "flang-rt/runtime/terminator.h"
#include "flang/Runtime/CUDA/allocator.h"
#include "flang/Runtime/CUDA/common.h"
#include "flang/Runtime/CUDA/descriptor.h"
#include "flang/Support/Fortran.h"

using namespace Fortran::runtime;
using namespace Fortran::runtime::cuda;

static OwningPtr<Descriptor> createAllocatable(
    Fortran::common::TypeCategory tc, int kind, int rank = 1) {
  return Descriptor::Create(TypeCode{tc, kind}, kind, nullptr, rank, nullptr,
      CFI_attribute_allocatable);
}

TEST(AllocatableCUFTest, SimpleDeviceAllocatable) {
  using Fortran::common::TypeCategory;
  RTNAME(CUFRegisterAllocator)();
  // REAL(4), DEVICE, ALLOCATABLE :: a(:)
  auto a{createAllocatable(TypeCategory::Real, 4)};
  a->SetAllocIdx(kDeviceAllocatorPos);
  EXPECT_EQ((int)kDeviceAllocatorPos, a->GetAllocIdx());
  EXPECT_FALSE(a->HasAddendum());
  RTNAME(AllocatableSetBounds)(*a, 0, 1, 10);

  // Emulate a device descriptor for the purpose of unit testing part of the
  // code.
  Descriptor *device_desc;
  CUDA_REPORT_IF_ERROR(cudaMalloc(&device_desc, a->SizeInBytes()));

  RTNAME(AllocatableAllocate)
  (*a, kNoAsyncObject, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__,
      __LINE__);
  EXPECT_TRUE(a->IsAllocated());
  RTNAME(CUFDescriptorSync)(device_desc, a.get(), __FILE__, __LINE__);
  cudaDeviceSynchronize();

  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableDeallocate)
  (*a, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_FALSE(a->IsAllocated());

  RTNAME(CUFDescriptorSync)(device_desc, a.get(), __FILE__, __LINE__);
  cudaDeviceSynchronize();

  EXPECT_EQ(cudaSuccess, cudaGetLastError());
}

TEST(AllocatableCUFTest, StreamDeviceAllocatable) {
  using Fortran::common::TypeCategory;
  RTNAME(CUFRegisterAllocator)();
  // REAL(4), DEVICE, ALLOCATABLE :: a(:)
  auto a{createAllocatable(TypeCategory::Real, 4)};
  a->SetAllocIdx(kDeviceAllocatorPos);
  EXPECT_EQ((int)kDeviceAllocatorPos, a->GetAllocIdx());
  EXPECT_FALSE(a->HasAddendum());
  RTNAME(AllocatableSetBounds)(*a, 0, 1, 10);

  auto b{createAllocatable(TypeCategory::Real, 4)};
  b->SetAllocIdx(kDeviceAllocatorPos);
  EXPECT_EQ((int)kDeviceAllocatorPos, b->GetAllocIdx());
  EXPECT_FALSE(b->HasAddendum());
  RTNAME(AllocatableSetBounds)(*b, 0, 1, 20);

  auto c{createAllocatable(TypeCategory::Real, 4)};
  c->SetAllocIdx(kDeviceAllocatorPos);
  EXPECT_EQ((int)kDeviceAllocatorPos, c->GetAllocIdx());
  EXPECT_FALSE(b->HasAddendum());
  RTNAME(AllocatableSetBounds)(*c, 0, 1, 100);

  RTNAME(AllocatableAllocate)
  (*a, 1, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_TRUE(a->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableAllocate)
  (*b, 1, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_TRUE(b->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableAllocate)
  (*c, 1, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_TRUE(c->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableDeallocate)
  (*b, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_FALSE(b->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableDeallocate)
  (*a, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_FALSE(a->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());

  RTNAME(AllocatableDeallocate)
  (*c, /*hasStat=*/false, /*errMsg=*/nullptr, __FILE__, __LINE__);
  EXPECT_FALSE(c->IsAllocated());
  cudaDeviceSynchronize();
  EXPECT_EQ(cudaSuccess, cudaGetLastError());
}
