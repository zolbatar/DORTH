; ModuleID = 'Dorth'
source_filename = "Dorth"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"

define void @Implicit() local_unnamed_addr {
"Implicit Builder":
  tail call void @__DOT(i64 350)
  tail call void @__DOT(i64 -150)
  tail call void @__DOT(i64 123)
  ret void
}

declare void @__DOT(i64) local_unnamed_addr
