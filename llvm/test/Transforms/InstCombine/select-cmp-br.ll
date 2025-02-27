; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; Replace a 'select' with 'or' in 'select - cmp [eq|ne] - br' sequence
; RUN: opt -passes=instcombine -S < %s | FileCheck %s

%struct.S = type { ptr, i32, i32 }
%C = type <{ %struct.S }>

declare void @bar(ptr)
declare void @foobar()

define void @test1(ptr %arg) {
; CHECK-LABEL: @test1(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[M:%.*]] = load ptr, ptr [[ARG:%.*]], align 8
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds nuw i8, ptr [[ARG]], i64 16
; CHECK-NEXT:    [[N:%.*]] = load ptr, ptr [[TMP1]], align 8
; CHECK-NEXT:    [[TMP5_NOT:%.*]] = icmp eq ptr [[M]], [[N]]
; CHECK-NEXT:    br i1 [[TMP5_NOT]], label [[BB8:%.*]], label [[BB10:%.*]]
; CHECK:       bb:
; CHECK-NEXT:    ret void
; CHECK:       bb8:
; CHECK-NEXT:    tail call void @bar(ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB:%.*]]
; CHECK:       bb10:
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds nuw i8, ptr [[M]], i64 72
; CHECK-NEXT:    [[TMP4:%.*]] = load ptr, ptr [[TMP2]], align 8
; CHECK-NEXT:    [[TMP11:%.*]] = tail call i64 [[TMP4]](ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB]]
;
entry:
  %m = load ptr, ptr %arg, align 8
  %tmp1 = getelementptr inbounds %C, ptr %arg, i64 1, i32 0, i32 0
  %n = load ptr, ptr %tmp1, align 8
  %tmp2 = getelementptr inbounds i64, ptr %m, i64 9
  %tmp4 = load ptr, ptr %tmp2, align 8
  %tmp5 = icmp eq ptr %m, %n
  %tmp6 = select i1 %tmp5, ptr %arg, ptr null
  %tmp7 = icmp eq ptr %tmp6, null
  br i1 %tmp7, label %bb10, label %bb8

bb:                                               ; preds = %bb10, %bb8
  ret void

bb8:                                              ; preds = %entry
  tail call void @bar(ptr %tmp6)
  br label %bb

bb10:                                             ; preds = %entry
  %tmp11 = tail call i64 %tmp4(ptr %arg)
  br label %bb
}

define void @test2(ptr %arg) {
; CHECK-LABEL: @test2(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[M:%.*]] = load ptr, ptr [[ARG:%.*]], align 8
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds nuw i8, ptr [[ARG]], i64 16
; CHECK-NEXT:    [[N:%.*]] = load ptr, ptr [[TMP1]], align 8
; CHECK-NEXT:    [[TMP5:%.*]] = icmp eq ptr [[M]], [[N]]
; CHECK-NEXT:    br i1 [[TMP5]], label [[BB10:%.*]], label [[BB8:%.*]]
; CHECK:       bb:
; CHECK-NEXT:    ret void
; CHECK:       bb8:
; CHECK-NEXT:    tail call void @bar(ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB:%.*]]
; CHECK:       bb10:
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds nuw i8, ptr [[M]], i64 72
; CHECK-NEXT:    [[TMP4:%.*]] = load ptr, ptr [[TMP2]], align 8
; CHECK-NEXT:    [[TMP11:%.*]] = tail call i64 [[TMP4]](ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB]]
;
entry:
  %m = load ptr, ptr %arg, align 8
  %tmp1 = getelementptr inbounds %C, ptr %arg, i64 1, i32 0, i32 0
  %n = load ptr, ptr %tmp1, align 8
  %tmp2 = getelementptr inbounds i64, ptr %m, i64 9
  %tmp4 = load ptr, ptr %tmp2, align 8
  %tmp5 = icmp eq ptr %m, %n
  %tmp6 = select i1 %tmp5, ptr null, ptr %arg
  %tmp7 = icmp eq ptr %tmp6, null
  br i1 %tmp7, label %bb10, label %bb8

bb:                                               ; preds = %bb10, %bb8
  ret void

bb8:                                              ; preds = %entry
  tail call void @bar(ptr %tmp6)
  br label %bb

bb10:                                             ; preds = %entry
  %tmp11 = tail call i64 %tmp4(ptr %arg)
  br label %bb
}

define void @test3(ptr %arg) {
; CHECK-LABEL: @test3(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[M:%.*]] = load ptr, ptr [[ARG:%.*]], align 8
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds nuw i8, ptr [[ARG]], i64 16
; CHECK-NEXT:    [[N:%.*]] = load ptr, ptr [[TMP1]], align 8
; CHECK-NEXT:    [[TMP5:%.*]] = icmp eq ptr [[M]], [[N]]
; CHECK-NEXT:    br i1 [[TMP5]], label [[BB8:%.*]], label [[BB10:%.*]]
; CHECK:       bb:
; CHECK-NEXT:    ret void
; CHECK:       bb8:
; CHECK-NEXT:    tail call void @bar(ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB:%.*]]
; CHECK:       bb10:
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds nuw i8, ptr [[M]], i64 72
; CHECK-NEXT:    [[TMP4:%.*]] = load ptr, ptr [[TMP2]], align 8
; CHECK-NEXT:    [[TMP11:%.*]] = tail call i64 [[TMP4]](ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB]]
;
entry:
  %m = load ptr, ptr %arg, align 8
  %tmp1 = getelementptr inbounds %C, ptr %arg, i64 1, i32 0, i32 0
  %n = load ptr, ptr %tmp1, align 8
  %tmp2 = getelementptr inbounds i64, ptr %m, i64 9
  %tmp4 = load ptr, ptr %tmp2, align 8
  %tmp5 = icmp eq ptr %m, %n
  %tmp6 = select i1 %tmp5, ptr %arg, ptr null
  %tmp7 = icmp ne ptr %tmp6, null
  br i1 %tmp7, label %bb8, label %bb10

bb:                                               ; preds = %bb10, %bb8
  ret void

bb8:                                              ; preds = %entry
  tail call void @bar(ptr %tmp6)
  br label %bb

bb10:                                             ; preds = %entry
  %tmp11 = tail call i64 %tmp4(ptr %arg)
  br label %bb
}

define void @test4(ptr %arg) {
; CHECK-LABEL: @test4(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[M:%.*]] = load ptr, ptr [[ARG:%.*]], align 8
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds nuw i8, ptr [[ARG]], i64 16
; CHECK-NEXT:    [[N:%.*]] = load ptr, ptr [[TMP1]], align 8
; CHECK-NEXT:    [[TMP5:%.*]] = icmp eq ptr [[M]], [[N]]
; CHECK-NEXT:    br i1 [[TMP5]], label [[BB10:%.*]], label [[BB8:%.*]]
; CHECK:       bb:
; CHECK-NEXT:    ret void
; CHECK:       bb8:
; CHECK-NEXT:    tail call void @bar(ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB:%.*]]
; CHECK:       bb10:
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds nuw i8, ptr [[M]], i64 72
; CHECK-NEXT:    [[TMP4:%.*]] = load ptr, ptr [[TMP2]], align 8
; CHECK-NEXT:    [[TMP11:%.*]] = tail call i64 [[TMP4]](ptr nonnull [[ARG]])
; CHECK-NEXT:    br label [[BB]]
;
entry:
  %m = load ptr, ptr %arg, align 8
  %tmp1 = getelementptr inbounds %C, ptr %arg, i64 1, i32 0, i32 0
  %n = load ptr, ptr %tmp1, align 8
  %tmp2 = getelementptr inbounds i64, ptr %m, i64 9
  %tmp4 = load ptr, ptr %tmp2, align 8
  %tmp5 = icmp eq ptr %m, %n
  %tmp6 = select i1 %tmp5, ptr null, ptr %arg
  %tmp7 = icmp ne ptr %tmp6, null
  br i1 %tmp7, label %bb8, label %bb10

bb:                                               ; preds = %bb10, %bb8
  ret void

bb8:                                              ; preds = %entry
  tail call void @bar(ptr %tmp6)
  br label %bb

bb10:                                             ; preds = %entry
  %tmp11 = tail call i64 %tmp4(ptr %arg)
  br label %bb
}

define void @test5(ptr %arg, i1 %arg1) {
; CHECK-LABEL: @test5(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP2_NOT1:%.*]] = icmp eq ptr [[ARG:%.*]], null
; CHECK-NEXT:    [[TMP2_NOT:%.*]] = select i1 [[ARG1:%.*]], i1 true, i1 [[TMP2_NOT1]]
; CHECK-NEXT:    br i1 [[TMP2_NOT]], label [[BB5:%.*]], label [[BB3:%.*]]
; CHECK:       bb:
; CHECK-NEXT:    ret void
; CHECK:       bb3:
; CHECK-NEXT:    tail call void @bar(ptr [[ARG]])
; CHECK-NEXT:    br label [[BB:%.*]]
; CHECK:       bb5:
; CHECK-NEXT:    tail call void @foobar()
; CHECK-NEXT:    br label [[BB]]
;
entry:
  %tmp = select i1 %arg1, ptr null, ptr %arg
  %tmp2 = icmp ne ptr %tmp, null
  br i1 %tmp2, label %bb3, label %bb5

bb:                                               ; preds = %bb5, %bb3
  ret void

bb3:                                              ; preds = %entry
  tail call void @bar(ptr %tmp)
  br label %bb

bb5:                                              ; preds = %entry
  tail call void @foobar()
  br label %bb
}

; Negative test. Must not trigger the select-cmp-br combine because the result
; of the select is used in both flows following the br (the special case where
; the conditional branch has the same target for both flows).
define i32 @test6(i32 %arg, i1 %arg1) {
; CHECK-LABEL: @test6(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br i1 false, label [[BB:%.*]], label [[BB]]
; CHECK:       bb:
; CHECK-NEXT:    [[TMP:%.*]] = select i1 [[ARG1:%.*]], i32 [[ARG:%.*]], i32 0
; CHECK-NEXT:    ret i32 [[TMP]]
;
entry:
  %tmp = select i1 %arg1, i32 %arg, i32 0
  %tmp2 = icmp eq i32 %tmp, 0
  br i1 %tmp2, label %bb, label %bb

bb:                                               ; preds = %entry, %entry
  ret i32 %tmp
}
