define i64 @soma(i64 %a, i64 %b) {
entry:
  %tmp3 = add i64 %a, %b
  %c = add i64 %tmp3, 0
  ret i64 %c
}

define i64 @sub(i64 %a, i64 %b) {
entry:
  %tmp6 = sub i64 %a, %b
  ret i64 %tmp6
}

define i64 @main() {
entry:
  %val1 = add i64 2, 0
  %tmp10 = call i64 @soma(i64 3, i64 %val1)
  %tmp101 = call i64 @soma(i64 1, i64 %tmp10)
  %tmp12 = call i64 @sub(i64 3, i64 2)
  %tmp13 = sub i64 %tmp101, %tmp12
  %val2 = add i64 %tmp13, 0
  ret i64 %val2
}