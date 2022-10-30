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
  %1 = getelementptr [1024 x i64], ptr @Stack, i64 %0
  store i64 200, ptr %1, align 8
  %2 = load i64, ptr @SP, align 8
  %3 = add i64 %2, 1
  store i64 %3, ptr @SP, align 8
  %4 = load i64, ptr @SP, align 8
  %5 = sub i64 %4, 1
  store i64 %5, ptr @SP, align 8
  %6 = load i64, ptr @SP, align 8
  %7 = getelementptr [1024 x i64], ptr @Stack, i64 %6
  %8 = load i64, ptr %7, align 8
  store i64 %8, ptr @R0, align 8
  %9 = load i64, ptr @SP, align 8
  %10 = getelementptr [1024 x i64], ptr @Stack, i64 %9
  store i64 100, ptr %10, align 8
  %11 = load i64, ptr @SP, align 8
  %12 = add i64 %11, 1
  store i64 %12, ptr @SP, align 8
  %13 = load i64, ptr @SP, align 8
  %14 = getelementptr [1024 x i64], ptr @Stack, i64 %13
  store i64 250, ptr %14, align 8
  %15 = load i64, ptr @SP, align 8
  %16 = add i64 %15, 1
  store i64 %16, ptr @SP, align 8
  %17 = load i64, ptr @SP, align 8
  %18 = sub i64 %17, 1
  store i64 %18, ptr @SP, align 8
  %19 = load i64, ptr @SP, align 8
  %20 = getelementptr [1024 x i64], ptr @Stack, i64 %19
  %21 = load i64, ptr %20, align 8
  store i64 %21, ptr @R1, align 8
  %22 = load i64, ptr @SP, align 8
  %23 = sub i64 %22, 1
  store i64 %23, ptr @SP, align 8
  %24 = load i64, ptr @SP, align 8
  %25 = getelementptr [1024 x i64], ptr @Stack, i64 %24
  %26 = load i64, ptr %25, align 8
  store i64 %26, ptr @R0, align 8
  %27 = load i64, ptr @R0, align 8
  %28 = load i64, ptr @R1, align 8
  %29 = add i64 %27, %28
  store i64 %29, ptr @R0, align 8
  %30 = load i64, ptr @R0, align 8
  %31 = load i64, ptr @SP, align 8
  %32 = getelementptr [1024 x i64], ptr @Stack, i64 %31
  store i64 %30, ptr %32, align 8
  %33 = load i64, ptr @SP, align 8
  %34 = add i64 %33, 1
  store i64 %34, ptr @SP, align 8
  %35 = load i64, ptr @SP, align 8
  %36 = sub i64 %35, 1
  store i64 %36, ptr @SP, align 8
  %37 = load i64, ptr @SP, align 8
  %38 = getelementptr [1024 x i64], ptr @Stack, i64 %37
  %39 = load i64, ptr %38, align 8
  store i64 %39, ptr @R0, align 8
  %40 = load i64, ptr @R0, align 8
  call void @__DOT(i64 %40)
  %41 = load i64, ptr @SP, align 8
  %42 = getelementptr [1024 x i64], ptr @Stack, i64 %41
  store i64 100, ptr %42, align 8
  %43 = load i64, ptr @SP, align 8
  %44 = add i64 %43, 1
  store i64 %44, ptr @SP, align 8
  %45 = load i64, ptr @SP, align 8
  %46 = getelementptr [1024 x i64], ptr @Stack, i64 %45
  store i64 250, ptr %46, align 8
  %47 = load i64, ptr @SP, align 8
  %48 = add i64 %47, 1
  store i64 %48, ptr @SP, align 8
  %49 = load i64, ptr @SP, align 8
  %50 = sub i64 %49, 1
  store i64 %50, ptr @SP, align 8
  %51 = load i64, ptr @SP, align 8
  %52 = getelementptr [1024 x i64], ptr @Stack, i64 %51
  %53 = load i64, ptr %52, align 8
  store i64 %53, ptr @R1, align 8
  %54 = load i64, ptr @SP, align 8
  %55 = sub i64 %54, 1
  store i64 %55, ptr @SP, align 8
  %56 = load i64, ptr @SP, align 8
  %57 = getelementptr [1024 x i64], ptr @Stack, i64 %56
  %58 = load i64, ptr %57, align 8
  store i64 %58, ptr @R0, align 8
  %59 = load i64, ptr @R0, align 8
  %60 = load i64, ptr @R1, align 8
  %61 = sub i64 %59, %60
  store i64 %61, ptr @R0, align 8
  %62 = load i64, ptr @R0, align 8
  %63 = load i64, ptr @SP, align 8
  %64 = getelementptr [1024 x i64], ptr @Stack, i64 %63
  store i64 %62, ptr %64, align 8
  %65 = load i64, ptr @SP, align 8
  %66 = add i64 %65, 1
  store i64 %66, ptr @SP, align 8
  %67 = load i64, ptr @SP, align 8
  %68 = sub i64 %67, 1
  store i64 %68, ptr @SP, align 8
  %69 = load i64, ptr @SP, align 8
  %70 = getelementptr [1024 x i64], ptr @Stack, i64 %69
  %71 = load i64, ptr %70, align 8
  store i64 %71, ptr @R0, align 8
  %72 = load i64, ptr @R0, align 8
  call void @__DOT(i64 %72)
  %73 = load i64, ptr @SP, align 8
  %74 = getelementptr [1024 x i64], ptr @Stack, i64 %73
  store i64 123, ptr %74, align 8
  %75 = load i64, ptr @SP, align 8
  %76 = add i64 %75, 1
  store i64 %76, ptr @SP, align 8
  %77 = load i64, ptr @SP, align 8
  %78 = sub i64 %77, 1
  store i64 %78, ptr @SP, align 8
  %79 = load i64, ptr @SP, align 8
  %80 = getelementptr [1024 x i64], ptr @Stack, i64 %79
  %81 = load i64, ptr %80, align 8
  store i64 %81, ptr @R0, align 8
  %82 = load i64, ptr @R0, align 8
  call void @__DOT(i64 %82)
  ret void
}

declare void @__DOT(i64)
