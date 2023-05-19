### Operators

+ 四则运算：+-*/
+ 比较运算：< <= > >=
+ 一元运算：! -
+ 其它运算：() [] {} ,  ;

### Keywords

+ 定义变量：let

+ 定义函数：fn
+ boolean：true、false
+ if/else判断：if、else
+ 返回值：return 

## Statement and Expression 

### Expression

每个 Expression 都有返回值，未定义则返回 nil。

### Statement

简单理解为没有返回值的 Expression。

## statements and special expressions

### let statement

`let <variable> = <expression>` 

### return statement

`return <expression>`

### if expression

`if (<expression>) { [expression1 expression2 ...] } [else { [expression1 expression2 ...] }]`

### function literal 

`fn ([identifier1, identifier2, ...]) { [expression1 expression2, ...] } `

### array literal 

`[[expression1, expression2, ...]]`

### call expression

`<expression that evaluates to array>([expression, ..., expression])`

### index expression

`<expression that evaluates to array>[expression]`