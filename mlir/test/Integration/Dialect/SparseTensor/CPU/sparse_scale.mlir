//--------------------------------------------------------------------------------------------------
// WHEN CREATING A NEW TEST, PLEASE JUST COPY & PASTE WITHOUT EDITS.
//
// Set-up that's shared across all tests in this directory. In principle, this
// config could be moved to lit.local.cfg. However, there are downstream users that
//  do not use these LIT config files. Hence why this is kept inline.
//
// DEFINE: %{sparsifier_opts} = enable-runtime-library=true
// DEFINE: %{sparsifier_opts_sve} = enable-arm-sve=true %{sparsifier_opts}
// DEFINE: %{compile} = mlir-opt %s --sparsifier="%{sparsifier_opts}"
// DEFINE: %{compile_sve} = mlir-opt %s --sparsifier="%{sparsifier_opts_sve}"
// DEFINE: %{run_libs} = -shared-libs=%mlir_c_runner_utils,%mlir_runner_utils
// DEFINE: %{run_libs_sve} = -shared-libs=%native_mlir_runner_utils,%native_mlir_c_runner_utils
// DEFINE: %{run_opts} = -e main -entry-point-result=void
// DEFINE: %{run} = mlir-runner %{run_opts} %{run_libs}
// DEFINE: %{run_sve} = %mcr_aarch64_cmd --march=aarch64 --mattr="+sve" %{run_opts} %{run_libs_sve}
//
// DEFINE: %{env} =
//--------------------------------------------------------------------------------------------------

// RUN: %{compile} | %{run} | FileCheck %s
//
// Do the same run, but now with direct IR generation.
// REDEFINE: %{sparsifier_opts} = enable-runtime-library=false
// RUN: %{compile} | %{run} | FileCheck %s
//
// Do the same run, but now with vectorization.
// REDEFINE: %{sparsifier_opts} = enable-runtime-library=false vl=4 
// RUN: %{compile} | %{run} | FileCheck %s
//
// Do the same run, but now with  VLA vectorization.
// RUN: %if mlir_arm_sve_tests %{ %{compile_sve} | %{run_sve} | FileCheck %s %}

#CSR = #sparse_tensor.encoding<{ map = (d0, d1) -> (d0 : dense, d1 : compressed) }>

#trait_scale = {
  indexing_maps = [
    affine_map<(i,j) -> (i,j)>   // X (out)
  ],
  iterator_types = ["parallel", "parallel"],
  doc = "X(i,j) = X(i,j) * 2"
}

//
// Integration test that lowers a kernel annotated as sparse to actual sparse
// code, initializes a matching sparse storage scheme from a dense tensor,
// and runs the resulting code with the JIT compiler.
//
module {
  //
  // A kernel that scales a sparse matrix A by a factor of 2.0.
  //
  func.func @sparse_scale(%argx: tensor<8x8xf32, #CSR>) -> tensor<8x8xf32, #CSR> {
    %c = arith.constant 2.0 : f32
    %0 = linalg.generic #trait_scale
      outs(%argx: tensor<8x8xf32, #CSR>) {
        ^bb(%x: f32):
          %1 = arith.mulf %x, %c : f32
          linalg.yield %1 : f32
    } -> tensor<8x8xf32, #CSR>
    return %0 : tensor<8x8xf32, #CSR>
  }

  //
  // Main driver that converts a dense tensor into a sparse tensor
  // and then calls the sparse scaling kernel with the sparse tensor
  // as input argument.
  //
  func.func @main() {
    %c0 = arith.constant 0 : index
    %f0 = arith.constant 0.0 : f32

    // Initialize a dense tensor.
    %0 = arith.constant dense<[
       [1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0],
       [0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
       [0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0],
       [0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0],
       [0.0, 1.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0],
       [0.0, 1.0, 1.0, 0.0, 0.0, 6.0, 0.0, 0.0],
       [0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 7.0, 1.0],
       [0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 8.0]
    ]> : tensor<8x8xf32>

    // Convert dense tensor to sparse tensor and call sparse kernel.
    %1 = sparse_tensor.convert %0 : tensor<8x8xf32> to tensor<8x8xf32, #CSR>
    %2 = call @sparse_scale(%1)
      : (tensor<8x8xf32, #CSR>) -> tensor<8x8xf32, #CSR>

    // Print the resulting compacted values for verification.
    //
    // CHECK:      ---- Sparse Tensor ----
    // CHECK-NEXT: nse = 16
    // CHECK-NEXT: dim = ( 8, 8 )
    // CHECK-NEXT: lvl = ( 8, 8 )
    // CHECK-NEXT: pos[1] : ( 0, 3, 4, 5, 6, 8, 11, 14, 16 )
    // CHECK-NEXT: crd[1] : ( 0, 2, 7, 1, 2, 3, 1, 4, 1, 2, 5, 2, 6, 7, 2, 7 )
    // CHECK-NEXT: values : ( 2, 2, 2, 4, 6, 8, 2, 10, 2, 2, 12, 2, 14, 2, 2, 16 )
    // CHECK-NEXT: ----
    //
    sparse_tensor.print %2 : tensor<8x8xf32, #CSR>

    // Release the resources.
    bufferization.dealloc_tensor %1 : tensor<8x8xf32, #CSR>

    return
  }
}
