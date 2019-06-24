

# Data Types/Literals
Type | Literal | About
--|---|---
String | `"literal enclosed in quotes"` | text data (`'single quotes also work'`)
Integer | `2323234234242321`| arbitrary precision whole numbers (booleans are ints)
Float | `1241.3421`| numbers that don't have to be whole and might not be precise
Empty | `empty`| placeholder for something that lacks a value
Macro | `{ "hi" print }` | container for blocks of code
Lambda | `{"hi" print } () lambda` | anonymous function
Object | `{ self.name "yoda" = } object`| mutable keyvalue pairs 
Namespace |  `{ "name" "yoda" define } namespace` | container for definitions (good for containing custom operators)
List | `(1,"two", 3.0)` | array of references to values
Reference | `$a` `null` `$user.name` | pointer/handle for a value stored elsewhere

# Quick Intro
Because YodaScript is in active development, this guide will likley become obsolete as more changes are made
(in some places indicated). Ultimately the best way to learn are through looking at example code and playing
with the language yourself. If you wanna [text me](https://t.me/ridderhoff) about questions you have while
learning I'm always available.

### Postfix Notation
YodaScript is written in postfix notation. What this means is that operators are written after their arguments.
Although there are some shortcuts which deviate from this, their expanded form still follows postfix notation.
In the following example notice that hashtags denote line-comments and that order-of-operations is irellevant in
postfix notation.
```ys
# this prints the result of (1 + 2) * 3 to the terminal 
1 2 + 3 * print
```


### Variables
Although YodaScript isn't strict about the use of varaibles, you should always declare variables before using them
in case one of the same name exists in a previous scope (ys is dynamically scoped). YodaScript has a number of
operators for managing refrences that you can use on varaibles as well. Variables are deleted when they go out of 
scope unless they are referenced outside of the scope (ie - `return`), in that case they are deleted when their reference count
reaches zero. Fun fact: `$var` is actually short for `"var" $`.

```ys
# declare $a and pop it from stack
$a let ;

# set the value $a points to equal to 6
$a 6 = 

# create a variable b that references $a 
# $b -> $a -> 6
$b let $a :=

# set the value that $b points to equal to "hello" 
# $b -> $a -> "hello"
$b "hello" =

# make $b point to 99.99
# $b -> 99.99
# $a -> "hello"
$b 99.99 :=
```

### Macros
Macros are containers for code. Like everything in yodascript they are data as opposed to being special structures.
They can be used in a variety of ways, but you can call them directly using the `@` operator.
```ys
let $greeting {
  "hello\n" print 
} =

# prints hello
$greeting @
```

### Lambdas
Lambdas in YodaScript are more or less equivalent to anonymous functions in a langauge like JavaScript.
Again like macros, they are simply values and behave as such and again you can call with the `@` operator. 
If a parameter isn't passed as an argument it's value is set to `empty` and left to the lambda to handle.
Within lambdas you have access to the `arguments` keyword (gives arguments passed). And if the function is
a member of an object it is given a `self` keyword.

```ys
let $pow2 {
  $n $n * return
} ($n) lambda =

# prints 9
(3) $pow2 @ print
```

### Structured Procedural Programming
This topic is called "structured" however in yodascript things that are usually special syntax structures
are simply additional operators. Although YodaScript supports everything that you should expect some are in
transition phase and thus they aren't listed. 

```ys
# while loop
let $count 0 =
{
  $count ++
  $count '\n' + print
} { $count 10 < } while

# if statements (standin until I re-implement old system)
let $grade 95 
{
  { "You can do better than that" print }
  { "Thats ok" print } $grade 70 >
  { "Thats awesome!" print } $grade 90 >
} cond

# runs code n number of times
#   will likely be removed
{ # prints hihihi
  "hi" print
} 3 repeat

# range-based for
#    change 1: add `for` operaor and overloadable `.__for` method to list
#    change 2: add iterables and make list iterable type
{
  $elem print
} ($elem, $index, $list) lambda (1,2,3,4) List:for_each

# ... probably more that I'm forgetting
```

### Defined Values


### Namespaces


### Objects 


### Modules


# Complete Keyword reference

# builtin modules


