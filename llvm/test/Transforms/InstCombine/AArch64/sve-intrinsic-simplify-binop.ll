; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -S -passes=instcombine < %s | FileCheck %s

target triple = "aarch64-unknown-linux-gnu"

; The follow tests verify the mechanics of simplification. The operation is not
; important beyond being commutative with a known identity value.

define <vscale x 4 x i32> @commute_constant_to_rhs(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @commute_constant_to_rhs(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> [[A]], <vscale x 4 x i32> splat (i32 303))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 303), <vscale x 4 x i32> %a)
  ret <vscale x 4 x i32> %r
}

; Operation is not commutative.
define <vscale x 4 x i32> @cannot_commute_constant_to_rhs_1(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @cannot_commute_constant_to_rhs_1(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sub.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 303), <vscale x 4 x i32> [[A]])
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sub.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 303), <vscale x 4 x i32> %a)
  ret <vscale x 4 x i32> %r
}

; Inactive lanes are important, which make the operation non-commutative.
define <vscale x 4 x i32> @cannot_commute_constant_to_rhs_2(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @cannot_commute_constant_to_rhs_2(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 303), <vscale x 4 x i32> [[A]])
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 303), <vscale x 4 x i32> %a)
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @idempotent_mul(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @idempotent_mul(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> [[A]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a, <vscale x 4 x i32> splat (i32 1))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @idempotent_mul_ops_reverse(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @idempotent_mul_ops_reverse(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> [[A]], <vscale x 4 x i32> splat (i32 1)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 1), <vscale x 4 x i32> %a)
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @idempotent_mul_u(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @idempotent_mul_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> [[A]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a, <vscale x 4 x i32> splat (i32 1))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @idempotent_mul_u_ops_reverse(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @idempotent_mul_u_ops_reverse(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> [[A]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 1), <vscale x 4 x i32> %a)
  ret <vscale x 4 x i32> %r
}

; Show that we only need to know the active lanes are constant.
; TODO: We can do better here because we can use %a directly as part of the
; select because we know only its inactive lanes will be used.
define <vscale x 4 x i32> @constant_mul_after_striping_inactive_lanes(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a, <vscale x 4 x i32> %b) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_mul_after_striping_inactive_lanes(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]], <vscale x 4 x i32> [[B:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[A_DUP:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32> [[A]], <vscale x 4 x i1> [[PG]], i32 3)
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 6), <vscale x 4 x i32> [[A_DUP]]
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %a.dup = call <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32> %a, <vscale x 4 x i1> %pg, i32 3)
  %b.dup = call <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32> %b, <vscale x 4 x i1> %pg, i32 2)
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a.dup, <vscale x 4 x i32> %b.dup)
  ret <vscale x 4 x i32> %r
}

; Show that we only need to know the active lanes are constant.
define <vscale x 4 x i32> @constant_mul_u_after_striping_inactive_lanes(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a, <vscale x 4 x i32> %b) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_mul_u_after_striping_inactive_lanes(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]], <vscale x 4 x i32> [[B:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 6)
;
  %a.dup = call <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32> %a, <vscale x 4 x i1> %pg, i32 3)
  %b.dup = call <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32> %b, <vscale x 4 x i1> %pg, i32 2)
  %3 = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a.dup, <vscale x 4 x i32> %b.dup)
  ret <vscale x 4 x i32> %3
}

; SVE intrinsics don't have the same poison propagation rules as the IR.
define <vscale x 4 x i32> @dont_propagate_poison(<vscale x 4 x i1> %pg, <vscale x 4 x i32> %a) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @dont_propagate_poison(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]], <vscale x 4 x i32> [[A:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.and.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> poison, <vscale x 4 x i32> splat (i32 1))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.and.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> poison, <vscale x 4 x i32> splat (i32 1))
  ret <vscale x 4 x i32> %r
}

; The follow tests demonstrate the operations for which hooks are in place to
; enable simplification. Given the simplications themselves are common code, it
; is assumed they are already well tested elsewhere.

define <vscale x 4 x i32> @constant_add(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_add(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 10), <vscale x 4 x i32> splat (i32 7)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.add.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_add_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_add_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 10)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.add.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_and(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_and(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 6), <vscale x 4 x i32> splat (i32 7)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.and.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 14))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_and_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_and_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 6)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.and.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 14))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_eor(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_eor(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 4), <vscale x 4 x i32> splat (i32 7)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.eor.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_eor_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_eor_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 4)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.eor.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x float> @constant_fadd(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fadd(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 1.300000e+01), <vscale x 4 x float> splat (float 7.000000e+00)
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fadd.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fadd_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fadd_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x float> splat (float 1.300000e+01)
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fadd.u.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fdiv(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fdiv(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 2.000000e+00), <vscale x 4 x float> splat (float 1.200000e+01)
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fdiv.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 12.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fdiv_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fdiv_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x float> splat (float 0x3FF2AAAAA0000000)
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fdiv.u.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fdivr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fdivr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x float> @llvm.aarch64.sve.fdivr.nxv4f32(<vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 6.000000e+00), <vscale x 4 x float> splat (float 1.200000e+01))
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fdivr.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 6.0), <vscale x 4 x float> splat (float 12.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fmul(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fmul(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 4.200000e+01), <vscale x 4 x float> splat (float 7.000000e+00)
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fmul.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fmul_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fmul_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x float> splat (float 4.200000e+01)
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fmul.u.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fsub(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fsub(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 1.000000e+00), <vscale x 4 x float> splat (float 7.000000e+00)
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fsub.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fsub_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fsub_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x float> splat (float 1.000000e+00)
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fsub.u.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 7.0), <vscale x 4 x float> splat (float 6.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x float> @constant_fsubr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x float> @constant_fsubr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x float> @llvm.aarch64.sve.fsubr.nxv4f32(<vscale x 4 x i1> [[PG]], <vscale x 4 x float> splat (float 6.000000e+00), <vscale x 4 x float> splat (float 7.000000e+00))
; CHECK-NEXT:    ret <vscale x 4 x float> [[R]]
;
  %r = call <vscale x 4 x float> @llvm.aarch64.sve.fsubr.nxv4f32(<vscale x 4 x i1> %pg, <vscale x 4 x float> splat (float 6.0), <vscale x 4 x float> splat (float 7.0))
  ret <vscale x 4 x float> %r
}

define <vscale x 4 x i32> @constant_mul(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_mul(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 21), <vscale x 4 x i32> splat (i32 7)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_mul_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_mul_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 21)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_orr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_orr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 15), <vscale x 4 x i32> splat (i32 13)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.orr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 13), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_orr_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_orr_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 15)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.orr.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 13), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_sdiv(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 3))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_sdiv_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> zeroinitializer)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 0))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support overflow.
define <vscale x 4 x i32> @constant_sdiv_with_overflow(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv_with_overflow(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -2147483648), <vscale x 4 x i32> splat (i32 -1))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -2147483648), <vscale x 4 x i32> splat (i32 -1))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_sdiv_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 -3))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 -3))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_sdiv_u_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv_u_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> zeroinitializer)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -7), <vscale x 4 x i32> splat (i32 0))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support overflow.
define <vscale x 4 x i32> @constant_sdiv_u_with_overflow(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdiv_u_with_overflow(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -2147483648), <vscale x 4 x i32> splat (i32 -1))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -2147483648), <vscale x 4 x i32> splat (i32 -1))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_sdivr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdivr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 3), <vscale x 4 x i32> splat (i32 -7))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 3), <vscale x 4 x i32> splat (i32 -7))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_sdivr_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdivr_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> zeroinitializer, <vscale x 4 x i32> splat (i32 -7))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 0), <vscale x 4 x i32> splat (i32 -7))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support overflow.
define <vscale x 4 x i32> @constant_sdivr_with_overflow(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sdivr_with_overflow(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 -1), <vscale x 4 x i32> splat (i32 -2147483648))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 -1), <vscale x 4 x i32> splat (i32 -2147483648))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_sub(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sub(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = select <vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 4), <vscale x 4 x i32> splat (i32 7)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sub.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_sub_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_sub_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    ret <vscale x 4 x i32> splat (i32 4)
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.sub.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_subr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_subr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.subr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.subr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_udiv(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udiv(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_udiv_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udiv_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> zeroinitializer)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 7), <vscale x 4 x i32> splat (i32 0))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_udiv_u(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udiv_u(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 9), <vscale x 4 x i32> splat (i32 3))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 9), <vscale x 4 x i32> splat (i32 3))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_udiv_u_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udiv_u_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.u.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 9), <vscale x 4 x i32> zeroinitializer)
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udiv.u.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 9), <vscale x 4 x i32> splat (i32 0))
  ret <vscale x 4 x i32> %r
}

define <vscale x 4 x i32> @constant_udivr(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udivr(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udivr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> splat (i32 3), <vscale x 4 x i32> splat (i32 7))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udivr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 3), <vscale x 4 x i32> splat (i32 7))
  ret <vscale x 4 x i32> %r
}

; The intrinsic's IR equivalent does not support divide-by-zero.
define <vscale x 4 x i32> @constant_udivr_by_zero(<vscale x 4 x i1> %pg) #0 {
; CHECK-LABEL: define <vscale x 4 x i32> @constant_udivr_by_zero(
; CHECK-SAME: <vscale x 4 x i1> [[PG:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[R:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.udivr.nxv4i32(<vscale x 4 x i1> [[PG]], <vscale x 4 x i32> zeroinitializer, <vscale x 4 x i32> splat (i32 7))
; CHECK-NEXT:    ret <vscale x 4 x i32> [[R]]
;
  %r = call <vscale x 4 x i32> @llvm.aarch64.sve.udivr.nxv4i32(<vscale x 4 x i1> %pg, <vscale x 4 x i32> splat (i32 0), <vscale x 4 x i32> splat (i32 7))
  ret <vscale x 4 x i32> %r
}

declare <vscale x 4 x i32> @llvm.aarch64.sve.dup.nxv4i32(<vscale x 4 x i32>, <vscale x 4 x i1>, i32)

declare <vscale x 4 x i32> @llvm.aarch64.sve.add.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.and.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.eor.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.mul.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.orr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.sdivr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.sub.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.subr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.udiv.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.udivr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)

declare <vscale x 4 x i32> @llvm.aarch64.sve.add.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.and.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.eor.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.mul.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.orr.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.sdiv.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.sub.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)
declare <vscale x 4 x i32> @llvm.aarch64.sve.udiv.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x i32>, <vscale x 4 x i32>)

declare <vscale x 4 x float> @llvm.aarch64.sve.fadd.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fdiv.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fdivr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fmul.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fsub.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fsubr.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)

declare <vscale x 4 x float> @llvm.aarch64.sve.fadd.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fdiv.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fmul.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)
declare <vscale x 4 x float> @llvm.aarch64.sve.fsub.u.nxv4i32(<vscale x 4 x i1>, <vscale x 4 x float>, <vscale x 4 x float>)

attributes #0 = { "target-features"="+sve" }
