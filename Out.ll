; ModuleID = 'Dorth'
source_filename = "Dorth"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"

@Stack = internal global [1024 x i64] zeroinitializer
@SP = internal global i64 0
@R0 = internal global i64 0
@R1 = internal global i64 0

define void @Implicit() {
"Implicit Builder":
  %0 = load i64, ptr @SP, align 8
  %1 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %0
  store i64 200, ptr %1, align 8
  %2 = load i64, ptr @SP, align 8
  %3 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %2
  store i64 200, ptr %3, align 8
  %4 = load i64, ptr @SP, align 8
  %5 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %4
  %6 = load i64, ptr %5, align 8
  store i64 %6, ptr @R1, align 8
  %7 = load i64, ptr @SP, align 8
  %8 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %7
  %9 = load i64, ptr %8, align 8
  store i64 %9, ptr @R0, align 8
  %10 = load i64, ptr @R0, align 8
  %11 = load i64, ptr @R1, align 8
  %12 = add i64 %10, %11
  store i64 %12, ptr @R0, align 8
  %13 = load i64, ptr @R0, align 8
  %14 = load i64, ptr @SP, align 8
  %15 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %14
  store i64 %13, ptr %15, align 8
  %16 = load i64, ptr @SP, align 8
  %17 = getelementptr [1024 x i64], ptr @Stack, i64 0, i64 %16
  %18 = load i64, ptr %17, align 8
  store i64 %18, ptr @R0, align 8
  %19 = load i64, ptr @R0, align 8
  call void @__DOT(i64 %19)
  ret void
}

declare void @__DOT(i64)
