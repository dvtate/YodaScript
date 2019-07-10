# ys-alpha
I'm writing a new interpreter for yodascript that isn't held back by the same flaws of the old one.

This will probably be renamed to `dvtate/yoda` and the old interpreter will be named something like `dvtate/yoda-v1`

Changes in this version will break compatibility with the old version, however the syntax will remain largely unchanged.


# New langauge quickstart

### YodaScript is postfix
The syntax is different from most other other langauges but isn't hard to use. Although there 
are some shortcuts that emulate infix notation, the spelled out form is always postfix.
```
> # add 1 and 2 together
> 1 2 + 
3
> # equivalent to: 4 * (1 + 2)
> # order of operations is irrelevant
> 1 2 + 4 *
12
```

### Varaibles
Variables start with a `$`. Although YodaScript isn't very strict about how you use variables, if you want to avoid problems you should use them as described here.
- `let`: declare variable in current scope and push a reference to it onto stack
- `=`: set value that variable refers to equal to given value
- `=:`: make the variable reference a different value
```
> $a let 5 = 
> $a print 
5
> $a 'hello' =
> $a print
hello
> # declare $b and pop reference from stack
> $b let ;
> # make $b reference $a ($b->$a->5)
> $a :=
> $b print
hello
> # using `=` on $b changes $a's value as $b->$a->[value to change]
> $b 1.23 =
> $a print
1.23

> # to copy by value instead of by reference use the copy operator `~`
> # $c->1.23   $b->1.23
> $c let $b ~ =
> # compare references
> $b $c ==
0
> # compare values
> $b ~ $c ~ ==
1
```

### Conditionals
I haven't added old YodaScript's if statement syntax in yet as it is O(N^2) for no reason. For now you can use `cond`. 
```
> # user defined gpa
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

### Macros
Macros are like special strings used to store code. 
```
> $macro let {
.     "hi" print
. } =
> # the @ operator is used to run the macro within a new scope
> $macro @
hi
```

### Lists
Lists are Arrays of references. Note 
```
> $values let (1, "dog", 1.2, empty, { "hi" print }) =
> $values 1 ] "cat" =
> $values 1 ] print
cat
> # take the last element, which is a macro, and run it in current scope
> $values -1 ] @
hi
> /* list currently has it's relevant operators in a namespace. These will
. * probably be removed and replaced w/ emulated object members (like JS)
. */
> List
{ # namespace gets explained later
	"map"	<native> @define
	"for_each"	<native> @define
	"push"	<native> @define
	"pop"	<native> @define
} namespace
> 
```

### Lambdas
Lambdas are like macros but with infastructure attached. They are very similar in concept to JavaScript's arrow functions. Lambdas take a list of `arguments`.

```
> # lambda that adds 2 values (note: return is optional)
> $add {
     $v1 $v2 + return
. } ($v1, $v2) lambda =
> # call a lambda in new scope with @ operator
> (1,2) $add @ print
3
> # arguments list

```

### Loops

#### While
```
> $n let 0 =
> {
.     $n ++
.     $n ',' + print
. } { $n 10 < } while
1,2,3,4,5,6,7,8,9
```
#### Repeat
This is only here as I have yet to decide on a proper syntax for `for` loops.
```
> { # runs macro 4x
.     "hip hip hooray! " print
. } 4 repeat
hip hip hooray! hip hip hooray! hip hip hooray! hip hip hooray!
```

#### For loop (WIP)
I'm currently deciding on how how best to change this. Although I recognize that iterables are pretty cool, I like JavaScript/C/C++ more than Python. (also note that `List:map` also exists)
```
> # make a list of letters
> $letters let ('a', 'b', 'c') =
> { # print args
.     $letter ' ' + print
.     $index ' ' + print
.     $list '\n' + print
. } ($letter, $index, $list) lambda $letters List:for_each
a 0 ("a", "b", "c")
b 1 ("a", "b", "c")
c 2 ("a", "b", "c")
```

### Definitions
You can define new keywords. The run by default option might be removed or at least 
```
> "five" 5 define
> five five + print
10
> definitions can be run by default when they're named
> "@say_hi" "
```
### Namespaces
Storeable Scope for definitions. (immutable)

```
> $num {
.     "five" 5 define
.     "@sum" {+} define
. } namespace
> $num:five print
5
> $num "five" : print
5
> 
> $num:five $num :five $num:sum print
10
```

### Objects
Scope Scope for variables (hashtable)
```
> $bank {
.     # if we forget the =, the interpreter adds it for us
.     self.users () =
.     self.add_user {
.         self.users 
.     } ($name, $start_balance)
. } object
> 
```
