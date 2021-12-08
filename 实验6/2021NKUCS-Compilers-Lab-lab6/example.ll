;TypeCheck Begin!
declare i32 @getint()
declare void @putint(i32)
declare i32 @getch()
declare void @putch(i32)
  @a = global i32 0, align 4
define i32 @main(){
B10:
  %t12 = alloca i32, align 4
  %t11 = alloca i32, align 4
  store i32 1, i32* %t11, align 4
  store i32 3, i32* %t12, align 4
  %t3 = load i32, i32* %t11, align 4
  %t4 = load i32, i32* %t12, align 4
  %t5 = icmp eq i32 %t3, %t4
  br i1 %t5, label %B15, label %B14
B13:                               	; preds = %B10, %B15
  ret i32 1
B15:                               	; preds = %B10, %B10
  %t6 = load i32, i32* %t12, align 4
  %t7 = load i32, i32* %t11, align 4
  %t8 = icmp ne i32 %t6, %t7
  br i1 %t8, label %B13, label %B14
}
