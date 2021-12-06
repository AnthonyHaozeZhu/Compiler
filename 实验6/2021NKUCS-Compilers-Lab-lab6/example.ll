;TypeCheck Begin!
;FunctionDef @main typecheck Begin!
define i32 @main() {
B18:
  %t25 = alloca i32, align 4
  %t21 = alloca i32, align 4
  %t20 = alloca i32, align 4
  %t19 = alloca i32, align 4
  %t4 = add i32 999, 888
  store i32 %t4, i32* %t19, align 4
  %t6 = add i32 777, 666
  store i32 %t6, i32* %t20, align 4
  %t7 = load i32, i32* %t19, align 4
  %t8 = load i32, i32* %t20, align 4
  %t9 = icmp slt i32 %t7, %t8
  br i32 %t9, label %B22, label %B24
B22:                               	; preds = %B18, %B18
  %t11 = load i32, i32* %t19, align 4
  store i32 %t11, i32* %t21, align 4
  %t13 = add i32 333, 444
  store i32 %t13, i32* %t21, align 4
  br label %B23
B24:                               	; preds = %B18, %B18
  store i32 800, i32* %t19, align 4
  br label %B23
B23:                               	; preds = %B22, %B24
  store i32 520131, i32* %t21, align 4
  %t17 = load i32, i32* %t21, align 4
  ret i32 %t17
}
