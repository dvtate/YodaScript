# YodaScript 2
New interpreter for YodaScript that isn't held back by the same flaws of the old one. This version of YodaScript is not backwards compatible with the old one however the syntax should be familiar.


# Quickstart Guide

### YodaScript is postfix
The syntax is different from most other other langauges. Although there are some shortcuts that emulate infix/prefix notation, the spelled out form is always postfix.
```
> # add 1 and 2 together
> 1 2 + 
3
> 1 2 + 4 *
12
```

### References
This is probably the most confusing part of YodaScript, to summarize, apart from literals, everything is a reference. This includes anything assignable.

Variables start with a `$`. Although YodaScript isn't very strict about how you use variables, if you want to avoid problems you should use them as described here. All variables are `empty` until given a value (`$var ~ empty ==`). The copy operator `~` is required to convert from references to values.
- `let`: declare variable in current scope and push a reference to it onto stack
- `=`: (value change) alter the value that variable refers to
- `=:`: (reference change) make the variable reference a different value
- `~`: (copy) get value from a reference

In the following example, the value that a refers to is changed from `empty` and set to the integer 5. We then change 5 into the string "hello". Because we left a reference to $a on the stack it stays there after the reassignment and we see the value update after the reassignment.
```
> $a let 5 = 
> $a
5
> $a 'hello' = 
"hello"
```

We can declare a reference before using it by naming it and then using `let`. The `;` is used to remove the reference from the stack. References can refer to other references. Here we use the reference change operator (`:=`) to make $b reference $a (from previous example).
```
> $b let ;
> # $b->$a->5 
> $b $a :=
> $b print
hello
```

Notice that if we use the value change operator on b, it changes the value that a references
```
> $b 1.23 =
> $a print
1.23
```

If we want to copy a variable by value we have to deference it using the copy operator `~`. 
```
# $c->1.23   $b->$a->1.23
> $c let $b ~ =
> $b $c ==
0
> $b ~ $c ~ ==
1
```

### Macros
Macros are a datatype used to store code. Use the `@` operator to execute macros.
```
> $macro let {
.     "hi" print
. } =
> $macro @
hi
```

### Conditionals
Eventually I'll add old YodaScript's if statement syntactic sugar. For now you can use `cond` which acts the same way as before.
```
> $gpa let input float =
3.863
> {
.     { "You can do better than that" print }
.     { "C's get degrees" print } $gpa 2 >=
.     { "That's great" print } $gpa 3 >=
.     { "WOW!" print } $gpa 4 >=
. } cond
Thats great
```

### Lists
Lists are Arrays of references and behave about like you'd expect with indicies starting at zero.
```
> $values let (1, "dog", 1.2, empty, { "hi" print }) =
> $values 1 ] "cat" =
> $values 1 ] print
cat
> $values -1 ] @
hi
> /* list currently has it's relevant operators in a namespace.
. * This is likely temporary, see namespaces
. */
> List
{
	"map"	<native> @define
	"for_each"	<native> @define
	"push"	<native> @define
	"pop"	<native> @define
} namespace
```

### Lambdas
Lambdas are like macros but with some extra infastructure attached. They are basically just anonymous functions. Lambdas take a list of `arguments` as references defined in a new scope when the `@` operator is called. 
- Note: the `arguments` operator replaces old YodaScript missing handlers and variable arguments 
```
> $add {
     $v1 $v2 + return
. } ($v1, $v2) lambda =
> (1,2) $add @ print
3
```

### Loops

#### While
While loops are like C but backwards
- Note: `++` is internally defined as `1 +=`
```
> $n let 0 =
> {
.     $n ++
.     $n ',' + print
. } { $n 10 < } while
1,2,3,4,5,6,7,8,9
```

#### Repeat
Run same macro a given number of times
```
> {
.     "hip hip hooray! " print
. } 4 repeat
hip hip hooray! hip hip hooray! hip hip hooray! hip hip hooray!
```

#### For loop (WIP)
I'm currently deciding on how how best to change this. Although I recognize that iterables are pretty cool, I like JavaScript/C/C++ more than Python. (also note that `List:map` also exists)
```
> $letters let ('a', 'b', 'c') =
> {
.     $letter ' ' + print
.     $index ' ' + print
.     $list '\n' + print
. } ($letter, $index, $list) lambda $letters List:for_each
a 0 ("a", "b", "c")
b 1 ("a", "b", "c")
c 2 ("a", "b", "c")
```

### Definitions
You can define new keywords. The run by default option might be removed or reworked in future releases. Several built-in operators are actually defined in terms of other operators by the interpreter.
```
> "five" 5 define
> five five + print
10
> "@is_even?" { 2 % 0 == } define
> five is_even?
0
> five five + is_even?
1
```

### Namespaces
Immutable key-value set. There are several useful global namespaces defined by the interpreter.
```
> $num {
.     "five" 5 define
.     "@sum" {+} define
. } namespace =
> $num:five print
5
> $num "five" : print
5
> $num:five $num :five $num:sum print
10
```

### Objects
Mutable key-value set (hashtable). Note 
```
> $account {
.     self.balance 0 =
.     self.deposit {
.         self.balance $amount +=
.     } ($amount) lambda
. } object =
> 
```

### Update 2020-7
[new lang](https://github.com/dvtate/dlang20)

### Update 2020-1
I plan on making a new language inspired by yodascript, that will involve the following:
- change syntax to make json a native type
- formal langauge spec created before I start development
- stack-based bytecode compilation target
- will also likely make a lisp-like syntax for broader appeal
- increased focus on modules and extension system instead of kitchen sink approach


### Update 2019-9
After taking a few months break from the language I've come to recognize that there are some flaws with the langauge. In it's current form. Although it has a lot of features, many of these features make the language harder to grasp and serve purposes provided by other langauge components. I think I need to focus on yodascript's strengths and trim down the language spec so I can focus on optimizing what I have. In addition there are some flaws I need to work out which will likely involve major implementation changes. 

I've come to the following realizations:
- I've added a lot of features (it's essentially like javascript with all the features of C++)
- I think I need to strip down the langauge to it's bare essentials
  - And then expand on these to provide the rest of the desired functionality
