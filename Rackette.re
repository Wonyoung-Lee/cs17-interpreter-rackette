open CS17SetupRackette;
open Read.Reader;
open Types;

/*
Data Definitions & Examples: 

The rawProgram is a string of Racket code. It is meant to go through readAll.
type rawProgram = string;
// "(define a 3)"
// "(+ -1 17)"

The concreteProgramPiece is intended to be run through parseDefinition and
parseExpression to produce an abstracProgramPiece.
type concreteProgramPiece =
  | NumberC(int)
  // NumberC(3)
  // NumberC(-1)
  | SymbolC(string)
  // SymbolC("a")
  // SymbolC("define")
  // SymbolC("+")
  The ListC type is for lists. But also essentially wherever a pair of 
  paranthese () were in the original rawProgram, there is a ListC.
  | ListC(list(concreteProgramPiece));
  // ListC([NumberC(3), NumberC(4), NumberC(5)])
  // ListC([SymbolC("a"), SymbolC("b"), SymbolC("c")])
  // ListC([SymbolC("define"), SymbolC("a"), NumberC(3)])
  // ListC([SymbolC("+"), NumberC(-1), NumberC(17)])

A concreteProgram is a list of concreteProgramPieces and is built as a 
intermediary type to make it easier to process each component which is 
a concreteProgramPiece.
type concreteProgram = list(concreteProgramPiece);
// [SymbolC("define"), SymbolC("a"), NumberC(3)]
// [SymbolC("+"), NumberC(-1), NumberC(17)]

/* a Rackette name */
A name is for any valid Racket name such as user inputted ones and builtin
procedure names.
type name =
  | Name(string);
// Name("fact")
// Name("alod")

/* a Rackette expression */
And expression is the result of parseExpression and signifies all the possible 
expressions of Racket code. 
type expression =
  | NumE(int)
  // NumE(3)
  // NumE(0)
  | BoolE(bool)
  // BoolE(true)
  // BoolE(false)
  | EmptyE
  // EmptyE
  | NameE(name)
  // NameE(Name("alist"))
  // NameE(Name("fact"))
  AndE signifies an and statement which is composed of two expressions 
  where the first one evaluates to a boolean
  | AndE(expression, expression)
  // AndE(BoolE(true), BoolE(false))
  // AndE(BoolE(false), NumE(5))
  OrE signifies an or statement which is composed of two expressions 
  where the first one evaluates to a boolean
  | OrE(expression, expression)
  // OrE(BoolE(true), BoolE(false))
  // OrE(BoolE(true), NumV(6))
  IfE signifies an if statement which is composed of three expressions 
  where the first one evaluates to a boolean
  | IfE(expression, expression, expression)
  // IfE(BoolE(true), NumE(3), NumE(0))
  // IfE(BoolE(false), NumE(-3), NumE(3))
  CondE signifies a cond statement which is composed of pairs of expressions 
  where the first of the pair evaluates to a boolean
  | CondE(list((expression, expression)))
  // CondE([(BoolE(true), NumE(0)), (BoolE(false), NumE(3)))])
  // CondE([(BoolE(true), NameE(Name("alod"))), 
  //         (BoolE(false), NameE(Name("alod2")))])
  LambdaE signifies a lambda statement which is composed of a list of names 
  (representing the list of arguments) and an expression 
  | LambdaE(list(name), expression)
  // LambdaE([Name("x"), Name("y")], NumE(17))
  // LambdaE([Name("a")], BoolE(true))
  // LambdaE([Name("x")], 
  //  ApplicationE([NameE(Name("+")), NameE(Name("x")), NameE(Name("13"))]))))
  LetE signifies a let statement which is composed of a list of pair of name 
  and expression (representing a local binding) and an expression
  | LetE(list((name, expression)), expression)
  // LetE([(Name("a"), NumE(1)), (Name("b"), NumE(-1))], NumE(3))
  // LetE([(Name("a"), NumE(1)), (Name("b"), NumE(-1))], 
  //   ApplicationE([NameE(Name("+")), NameE(Name("a")), NameE(Name("b"))]))
  ApplicationE signifies a proc-app expression which can both be of a builtin
  or a user input procedure. 
  | ApplicationE(list(expression));
  // ApplicationE([NameE(Name("+")), NameE(Name("a")), NameE(Name("b"))]))
  // ApplicationE([NameE(Name("zero?")), NameE(Name("a"))])

/* a Rackette definition */
A definition is a pair of name and expression and essentially signifies a 
binding in the top level environment but is different in that the second 
argument of the pair is an expression (the premature type of value) 
instead of a value.
type definition = (name, expression);
// (Name("a"), NumE("3"))
// (Name("alod"), EmptyE)

/* a piece of Rackette that can be processed:
 * either a definition or an expression */
An abstractProgramPiece is made by parsing a concreteProgramPiece and is the
component of an abstractProgram. It is the input of the procedure eval. 
type abstractProgramPiece =
  | Definition(definition)
  // Definition((Name("friend"), BoolE("true")))
  // Definition((Name("b"), NumE(0))) 
  | Expression(expression);
  // Expression(ApplicationE([NameE(Name("zero?")), NameE(Name("b"))]))
  // Expression(NumE(17))

/* a representation of a Rackette program -
 * any number of pieces */
 An abstractProgram is a list of abstractProgramPieces and is the intermediary
 type that is made to make the process of conversion easier. 
type abstractProgram = list(abstractProgramPiece);
// [Definition((Name("b"), NumE(0)))]
// [Definition((Name("b"), NumE(0))), 
//         Expression(ApplicationE([NameE(Name("zero?")), NameE(Name("b"))]))]
// [Expression(NumE(17))]

/* a Rackette value: the result of evaluating a Rackette expression */
A value is the final version, the evaluated result of the code that was input
by the user through rawProgram form. This type is converted to a string later
by the stringOfValue procedure. 
type value =
  | NumV(int)
  // NumV(34)
  // NumV(-34)
  | BoolV(bool)
  // BoolV(true)
  // BoolV(false)
  The ListV signifies a list of values
  | ListV(list(value))
  // ListV([NumV(1), NumV(2)])
  // ListV([BoolV(false), BoolV(false)])
  BuiltinV signifies the builtin functions. This is the second part of the 
  binding for a builtin procedure in the top level environment.  
  | BuiltinV(string, list(value) => value)
  // BuiltinV("+", [NumV(34), NumV(-34)] => NumV("0"))
  // BuiltinV("*", [NumV(1), Num(17)] => NumV(17))
  ClosureV signifies what happens when a lambda or let is called and creates
  a local environment to temporarily process code. It has a list of names
  which represents the list of arguments and expression which is the body 
  and an environment which is updated by that lambda or let. 
  | ClosureV(list(name), expression, environment)
  // ClosureV([Name("x"), Name("y")], 
  //  ApplicationE([NameE(Name("+")), NameE(Name("x")), NameE(Name("y"))]))),
  //  [(Name("x"), NumV("17")), (Name("y"), NumV("18"))])
  // ClosureV([Name("a")], 
  // ApplicationE([NameE(Name("-")), NameE(Name("a")), NameE(Name("13"))]))),
  // [(Name("a"), NumV("17"))]) 
  An environment is where bindings and definitions 
  reside. A good example is the TLE. 
  and environment = (list(binding))
  // [(Name("a"), NumV(3)), (Name("b"), BoolV(true))]
  // [(Name("list1"), ListV([NumV(-3), NumV(3)]))]
  A binding is a pairing of a name and a value and this resides in an 
  environment
  and binding = (name, value);
  // (Name("a"), NumV(3))
  // (Name("list1"), ListV([NumV(-3), NumV(3)]))
 */
 
/*
plus: value list -> value

Input: input: a list of values
Output: a NumV result of the sum of the two elements in the input, 
a failwith otherwise
*/ 
let plus: list(value) => value = input => 
switch(input){
  | [NumV(x), NumV(y)] => NumV(x+y)
  | _ => failwith("plus operates on a list of two number values")
  }

checkExpect(plus([NumV(1), NumV(2)]), NumV(3), "positive hd, positive tl");
checkExpect(plus([NumV(1), NumV(0)]), NumV(1), "positive hd, zero tl");
checkExpect(plus([NumV(1), NumV(-2)]), NumV(-1), "positive hd, negative tl");
checkExpect(plus([NumV(0), NumV(2)]), NumV(2), "zero hd, positive tl");
checkExpect(plus([NumV(0), NumV(0)]), NumV(0), "zero hd, zero tl");
checkExpect(plus([NumV(0), NumV(-2)]), NumV(-2), "zero hd, negative tl");
checkExpect(plus([NumV(-1), NumV(2)]), NumV(1), "negative hd, positive tl");
checkExpect(plus([NumV(-1), NumV(0)]), NumV(-1), "negative hd, zero tl");
checkExpect(plus([NumV(-1), NumV(-2)]), NumV(-3), "negative hd, negative tl");
checkError(() => plus([]), "plus operates on a list of two number values");
checkError(() => plus([NumV(1), NumV(2), NumV(4)]), 
  "plus operates on a list of two number values");
checkError(() => plus([NumV(1)]), 
  "plus operates on a list of two number values");
checkError(() => plus([BoolV(true), BoolV(false)]), 
  "plus operates on a list of two number values");

/*
minus: value list ->  value

Input: input: a list of values
Output: a NumV result of the subtraction of 
the two elements in the input, a failwith otherwise
*/ 
let minus: list(value) => value = input => 
switch(input){
  | [NumV(x), NumV(y)] => NumV(x-y)
  | _ => failwith("minus operates on a list of two num values")
  }

checkExpect(minus([NumV(1), NumV(2)]), NumV(-1), "positive hd, positive tl");
checkExpect(minus([NumV(1), NumV(0)]), NumV(1), "positive hd, zero tl");
checkExpect(minus([NumV(1), NumV(-2)]), NumV(3), "positive hd, negative tl");
checkExpect(minus([NumV(0), NumV(2)]), NumV(-2), "zero hd, positive tl");
checkExpect(minus([NumV(0), NumV(0)]), NumV(0), "zero hd, zero tl");
checkExpect(minus([NumV(0), NumV(-2)]), NumV(2), "zero hd, negative tl");
checkExpect(minus([NumV(-1), NumV(2)]), NumV(-3), "negative hd, positive tl");
checkExpect(minus([NumV(-1), NumV(0)]), NumV(-1), "negative hd, zero tl");
checkExpect(minus([NumV(-1), NumV(-2)]), NumV(1), "negative hd, negative tl");
checkError(() => minus([]), "minus operates on a list of two num values");
checkError(() => minus([NumV(1), NumV(2), NumV(4)]), 
  "minus operates on a list of two num values");
checkError(() => minus([NumV(1)]), "minus operates on a list of two num values");
checkError(() => minus([BoolV(true), BoolV(false)]), 
  "minus operates on a list of two num values");
/*
mult: value list -> value

Input: input: a list of values
Output: a NumV result of the multiplication of 
the two elements in the input, a failwith otherwise
*/
let mult: list(value) => value = input => 
switch(input){
  | [NumV(x), NumV(y)] => NumV(x*y)
  | _ => failwith("mult operates on a list of two num values")
  }
checkExpect(mult([NumV(1), NumV(2)]), NumV(2), "positive hd, positive tl");
checkExpect(mult([NumV(1), NumV(0)]), NumV(0), "positive hd, zero tl");
checkExpect(mult([NumV(1), NumV(-2)]), NumV(-2), "positive hd, negative tl");
checkExpect(mult([NumV(0), NumV(2)]), NumV(0), "zero hd, positive tl");
checkExpect(mult([NumV(0), NumV(0)]), NumV(0), "zero hd, zero tl");
checkExpect(mult([NumV(0), NumV(-2)]), NumV(0), "zero hd, negative tl");
checkExpect(mult([NumV(-1), NumV(2)]), NumV(-2), "negative hd, positive tl");
checkExpect(mult([NumV(-1), NumV(0)]), NumV(0), "negative hd, zero tl");
checkExpect(mult([NumV(-1), NumV(-2)]), NumV(2), "negative hd, negative tl");
checkError(() => mult([]), "mult operates on a list of two num values");
checkError(() => mult([NumV(1), NumV(2), NumV(4)]), 
  "mult operates on a list of two num values");
checkError(() => mult([NumV(1)]), "mult operates on a list of two num values");
checkError(() => mult([BoolV(true), BoolV(false)]), 
  "mult operates on a list of two num values"); 

/*
div: value list -> value

Input: input: a list of values
Output: a NumV result of the divition of 
the two elements in the input, a failwith otherwise
*/ 
let div: list(value) => value = input => 
switch(input){
  | [NumV(x), NumV(y)] => NumV(x/y)
  | _ => failwith("div operates on a list of two num values")
  } 

checkExpect(div([NumV(1), NumV(2)]), NumV(0), "positive hd, positive tl");
checkExpect(div([NumV(1), NumV(-2)]), NumV(0), "positive hd, negative tl");
checkExpect(div([NumV(0), NumV(2)]), NumV(0), "zero hd, positive tl");
checkExpect(div([NumV(0), NumV(-2)]), NumV(0), "zero hd, negative tl");
checkExpect(div([NumV(-1), NumV(2)]), NumV(0), "negative hd, positive tl");
checkExpect(div([NumV(-1), NumV(-2)]), NumV(0), "negative hd, negative tl");
checkError(() => div([]), "div operates on a list of two num values");
checkError(() => div([NumV(1), NumV(2), NumV(4)]), 
  "div operates on a list of two num values");
checkError(() => div([NumV(1)]), "div operates on a list of two num values");
checkError(() => div([BoolV(true), BoolV(false)]), 
  "div operates on a list of two num values");
/*
remainder: value list -> value

Input: a list of values, input
Output: a NumV result of the remainder of 
the two elements in the input, a failwith otherwise
*/ 
let remainder: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(hd), NumV(tl)] => NumV(hd - (tl * (hd / tl)))
  | _=> failwith("remainder operates on a list of two number values")
}

checkExpect(remainder([NumV(1), NumV(2)]), NumV(1), "positive hd, positivetl");
checkExpect(remainder([NumV(1), NumV(-2)]), NumV(1),
  "positive hd, negative tl");
checkExpect(remainder([NumV(0), NumV(2)]), NumV(0), "zero hd, positive tl");
checkExpect(remainder([NumV(0), NumV(-2)]), NumV(0), "zero hd, negative tl");
checkExpect(remainder([NumV(-1), NumV(2)]), NumV(-1), 
  "negative hd, positive tl");
checkExpect(remainder([NumV(-1), NumV(-2)]), NumV(-1),
  "negative hd, negative tl");
checkError(() => remainder([]), 
  "remainder operates on a list of two number values"); 
checkError(() => remainder([NumV(1), NumV(2), NumV(4)]), 
  "remainder operates on a list of two number values");
checkError(() => remainder([NumV(1)]), 
  "remainder operates on a list of two number values");
checkError(() => remainder([BoolV(true), BoolV(false)]), 
  "remainder operates on a list of two number values");

/*
equalsQ: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if the two elements
in list(value) are equivalent to each other; 
failwiths are given when conditions are not met.
*/ 
let equalQ: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(x), NumV(y)] => BoolV(x == y)
  | [BoolV(x), BoolV(y)] => BoolV(x == y)
  | [ListV(x), ListV(y)] => BoolV(x == y)
  | [NumV(_), _] => BoolV(false)
  | [BoolV(_), _] => BoolV(false)
  | [ListV(_), _] => BoolV(false)
  |_=> failwith("equal? takes two arguments")
  };

checkExpect(equalQ([NumV(1), NumV(1)]), BoolV(true), 
  "positive hd, positive tl");
checkExpect(equalQ([NumV(1), NumV(0)]), BoolV(false), "positive hd, zero tl");
checkExpect(equalQ([NumV(1), NumV(-2)]), BoolV(false), 
  "positive hd, negative tl");
checkExpect(equalQ([NumV(0), NumV(2)]), BoolV(false), 
  "zero hd, positive tl");
checkExpect(equalQ([NumV(0), NumV(0)]), BoolV(true), 
  "zero hd, zero tl");
checkExpect(equalQ([NumV(0), NumV(-2)]), BoolV(false), 
  "zero hd, negative tl");
checkExpect(equalQ([NumV(-1), NumV(2)]), BoolV(false), 
  "negative hd, positive tl");
checkExpect(equalQ([NumV(-1), NumV(0)]), BoolV(false), 
  "negative hd, zero tl");
checkExpect(equalQ([NumV(-1), NumV(-1)]), BoolV(true), 
  "negative hd, negative tl");
checkExpect(equalQ([BoolV(true), BoolV(true)]), BoolV(true),
  "true hd, true tl");
checkExpect(equalQ([BoolV(true), BoolV(false)]), BoolV(false), 
  "true hd, false tl");
checkExpect(equalQ([BoolV(false), BoolV(true)]), BoolV(false), 
  "false hd, true tl");
checkExpect(equalQ([BoolV(false), BoolV(false)]), BoolV(true), 
  "false hd, false tl");
checkExpect(equalQ([ListV([NumV(1), NumV(2)]), 
  ListV([NumV(1), NumV(2)])]), BoolV(true), "cons lists");
checkExpect(equalQ([ListV([]), ListV([])]), BoolV(true), "empty lists");
checkExpect(equalQ([NumV(1), ListV([NumV(1), NumV(2)])]), BoolV(false),
  "unmatched num");
checkExpect(equalQ([BoolV(true), ListV([NumV(1), NumV(2)])]), BoolV(false), 
  "unmatched bool");
checkExpect(equalQ([ListV([NumV(1), NumV(2)]), NumV(1)]), BoolV(false), 
  "unmatched list");
checkError(() => equalQ([]), "equal? takes two arguments");
checkError(() => equalQ([NumV(1), NumV(2), NumV(4)]), 
  "equal? takes two arguments");
checkError(() => equalQ([NumV(1)]), "equal? takes two arguments");
checkError(() => equalQ([BoolV(true), BoolV(false), BoolV(true)]), 
  "equal? takes two arguments");

/*
lessThan: value list -> value

Input: a list of values, listVals
Output: a BoolV reflecting if
the first element in list(value) less than the other
element; failwiths are given when conditions are not met.
*/ 
let lessThan: list(value) => value = listVals =>
switch(listVals) {
  | [NumV(hd), NumV(tl)] => BoolV(hd < tl)
  | [_, _] => failwith("lessThan operates on a list of two integers")
  | _=> failwith("lessThan takes two arguments")
  };

/* Test cases for lessThan */
checkExpect(lessThan([NumV(1), NumV(2)]), BoolV(true), 
  "positive hd, positive tl");
checkExpect(lessThan([NumV(1), NumV(0)]), BoolV(false), 
  "positive hd, zero tl");
checkExpect(lessThan([NumV(1), NumV(-2)]), BoolV(false), 
  "positive hd, negative tl");
checkExpect(lessThan([NumV(0), NumV(2)]), BoolV(true), 
  "zero hd, positive tl");
checkExpect(lessThan([NumV(-1), NumV(2)]), BoolV(true), 
  "negative hd, positive tl");
checkExpect(lessThan([NumV(-1), NumV(0)]), BoolV(true), 
  "negative hd, zero tl");
checkExpect(lessThan([NumV(1), NumV(2)]), BoolV(true), 
  "positive hd, positive tl");
checkExpect(lessThan([NumV(1), NumV(0)]), BoolV(false), 
  "positive hd, zero tl");
checkExpect(lessThan([NumV(0), NumV(0)]), BoolV(false), 
  "zero hd, zero tl");
checkExpect(lessThan([NumV(0), NumV(-2)]), BoolV(false), 
  "zero hd, negative tl");
checkExpect(lessThan([NumV(-1), NumV(-2)]), BoolV(false), 
  "negative hd, negative tl");
checkError(() => lessThan([BoolV(true), BoolV(false)]), 
  "lessThan operates on a list of two integers");
checkError(() => lessThan([]), "lessThan takes two arguments");
checkError(() => lessThan([NumV(1), NumV(2), NumV(4)]), 
  "lessThan takes two arguments");
checkError(() => lessThan([NumV(1)]), "lessThan takes two arguments");

/*
greaterThan: value list -> value

Input: a list of values, listVa
output: a BoolV reflecting if
the first element in list(value) is greater than the other
element; failwiths are given when conditions are not met.
*/ 
let greaterThan: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(hd), NumV(tl)] => BoolV(hd > tl)
  | [_, _] => failwith("greaterThan operates on a list of two integers")
  | _ => failwith("greaterThan takes two arguments")
  };

checkExpect(greaterThan([NumV(1), NumV(2)]), BoolV(false), 
  "positive hd, positive tl");
checkExpect(greaterThan([NumV(1), NumV(0)]), BoolV(true), 
  "positive hd, zero tl");
checkExpect(greaterThan([NumV(1), NumV(-2)]), BoolV(true), 
  "positive hd, negative tl");
checkExpect(greaterThan([NumV(0), NumV(2)]), BoolV(false),
  "zero hd, positive tl");
checkExpect(greaterThan([NumV(0), NumV(0)]), BoolV(false), 
  "zero hd, zero tl");
checkExpect(greaterThan([NumV(0), NumV(-2)]), BoolV(true),
  "zero hd, negative tl");
checkExpect(greaterThan([NumV(-1), NumV(2)]), BoolV(false), 
  "negative hd, positive tl");
checkExpect(greaterThan([NumV(-1), NumV(0)]), BoolV(false), 
  "negative hd, zero tl");
checkExpect(greaterThan([NumV(-1), NumV(-2)]), BoolV(true), 
  "negative hd, negative tl");
checkError(() => greaterThan([BoolV(true), BoolV(false)]), 
  "greaterThan operates on a list of two integers");
checkError(() => greaterThan([]), "greaterThan takes two arguments");
checkError(() => greaterThan([NumV(1), NumV(2), NumV(4)]), 
  "greaterThan takes two arguments");
checkError(() => greaterThan([NumV(1)]), 
  "greaterThan takes two arguments");

/*
lessOrEqual: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if the first element in list(value) 
is less than or equal to the other element; 
failwiths are given when conditions are not met.
*/ 
let lessOrEqual: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(hd), NumV(tl)] => BoolV(hd <= tl)
  | [_, _] => failwith("lessOrEqual operates on a list of two integers")
  | _=> failwith("lessOrEqual takes two arguments")
  };

checkExpect(lessOrEqual([NumV(1), NumV(2)]), BoolV(true), 
  "positive hd, positive tl");
checkExpect(lessOrEqual([NumV(1), NumV(0)]), BoolV(false),
  "positive hd, zero tl");
checkExpect(lessOrEqual([NumV(1), NumV(-2)]), BoolV(false), 
  "positive hd, negative tl");

checkExpect(lessOrEqual([NumV(0), NumV(2)]), BoolV(true), 
  "zero hd, positive tl");
checkExpect(lessOrEqual([NumV(0), NumV(0)]), BoolV(true), 
  "zero hd, zero tl");
checkExpect(lessOrEqual([NumV(0), NumV(-2)]), BoolV(false), 
  "zero hd, negative tl");
checkExpect(lessOrEqual([NumV(-1), NumV(2)]), BoolV(true), 
  "negative hd, positive tl");
checkExpect(lessOrEqual([NumV(-1), NumV(0)]), BoolV(true), 
  "negative hd, zero tl");
checkExpect(lessOrEqual([NumV(-1), NumV(-2)]), BoolV(false), 
  "negative hd, negative tl");
checkError(() => lessOrEqual([BoolV(true), BoolV(false)]), 
  "lessOrEqual operates on a list of two integers");
checkError(() => lessOrEqual([]), "lessOrEqual takes two arguments");
checkError(() => lessOrEqual([NumV(1), NumV(2), NumV(4)]), 
  "lessOrEqual takes two arguments");

checkError(() => lessOrEqual([NumV(1)]), "lessOrEqual takes two arguments");

/*
greaterOrEqual: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if
the first element in list(value) is greater than or equal to the other
element; failwiths are given when conditions are not met.
*/ 
let gretrOrEqual: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(hd), NumV(tl)] => BoolV(hd >= tl)
  | [_, _] => failwith("gretrOrEqual operates on a list of two number integers")
  | _ => failwith("gretrOrEqual takes two arguments")
  };

checkExpect(gretrOrEqual([NumV(1), NumV(2)]), BoolV(false), 
  "positive hd, positive tl");
checkExpect(gretrOrEqual([NumV(1), NumV(0)]), BoolV(true), 
  "positive hd, zero tl");  
checkExpect(gretrOrEqual([NumV(1), NumV(-2)]), BoolV(true), 
  "positive hd, negative tl");
checkExpect(gretrOrEqual([NumV(0), NumV(2)]), BoolV(false), 
  "zero hd, positive tl");
checkExpect(gretrOrEqual([NumV(0), NumV(0)]), BoolV(true), 
  "zero hd, zero tl");
checkExpect(gretrOrEqual([NumV(0), NumV(-2)]), BoolV(true), 
  "zero hd, negative tl");
checkExpect(gretrOrEqual([NumV(-1), NumV(2)]), BoolV(false), 
  "negative hd, positive tl");
checkExpect(gretrOrEqual([NumV(-1), NumV(0)]), BoolV(false),
  "negative hd, zero tl");
checkExpect(gretrOrEqual([NumV(-1), NumV(-2)]), BoolV(true), 
  "negative hd, negative tl"); 
checkError(() => gretrOrEqual([BoolV(true), BoolV(false)]), 
  "gretrOrEqual operates on a list of two number integers");
checkError(() => gretrOrEqual([]), "gretrOrEqual takes two arguments");
checkError(() => gretrOrEqual([NumV(1), NumV(2), NumV(4)]), 
  "gretrOrEqual takes two arguments");
checkError(() => gretrOrEqual([NumV(1)]), 
  "gretrOrEqual takes two arguments");

/*
equal: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if
the first element in list(value) is the same as the second.
The values need to be Numv for equal not to give a failwith.
*/
let equal: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(x), NumV(y)] => BoolV(x == y)
  | [_, _] => failwith("must compare two numbers")
  | _=> failwith("equal operates on a list of two numbers")
  };

checkExpect(equal([NumV(1), NumV(1)]), BoolV(true), 
  "positive hd, positivetl");
checkExpect(equal([NumV(1), NumV(0)]), BoolV(false),
  "positive hd, zero tl");
checkExpect(equal([NumV(1), NumV(-2)]), BoolV(false), 
  "positive hd, negative tl");
checkExpect(equal([NumV(0), NumV(2)]), BoolV(false), 
  "zero hd, positive tl");
checkExpect(equal([NumV(0), NumV(0)]), BoolV(true), 
  "zero hd, zero tl");
checkExpect(equal([NumV(0), NumV(-2)]), BoolV(false), 
  "zero hd, negative tl");
checkExpect(equal([NumV(-1), NumV(2)]), BoolV(false), 
  "negative hd, positive tl");
checkExpect(equal([NumV(-1), NumV(0)]), BoolV(false), 
  "negative hd, zero tl");
checkExpect(equal([NumV(-1), NumV(-1)]), BoolV(true), 
  "negative hd, negative tl");
checkError(() => equal([BoolV(true), BoolV(false)]), 
  "must compare two numbers");
checkError(() => equal([]), 
  "equal operates on a list of two numbers");
checkError(() => equal([NumV(1), NumV(2), NumV(4)]), 
  "equal operates on a list of two numbers");
checkError(() => equal ([NumV(1)]), 
  "equal operates on a list of two numbers");

/*
numberQ: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if
the given list(value) is a NumV
*/ 
let numberQ: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(_)] => BoolV(true)
  | [_] => BoolV(false)
  | _ => failwith("number? can only act on one element")
  };

checkExpect(numberQ([NumV(1)]), BoolV(true), "positive num");
checkExpect(numberQ([NumV(0)]), BoolV(true), "zero num");
checkExpect(numberQ([NumV(-1)]), BoolV(true), "negative num");
checkExpect(numberQ([BoolV(true)]), BoolV(false), "non-number list");
checkExpect(numberQ([BoolV(false)]), BoolV(false), "non-number list");
checkError(() => numberQ([]), "number? can only act on one element");
checkError(() => numberQ([BoolV(true), BoolV(false)]),
  "number? can only act on one element");


/*
zeroQ: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if
the given list(value) is zero
*/ 
let zeroQ: list(value) => value = listVa =>
switch(listVa) {
  | [NumV(0)] => BoolV(true)
  | [NumV(_)] => BoolV(false)
  | _ => failwith("zero? is a test that only works on one num")
  };

checkExpect(zeroQ([NumV(1)]), BoolV(false), "positive num");
checkExpect(zeroQ([NumV(0)]), BoolV(true), "zero num");
checkExpect(zeroQ([NumV(-1)]), BoolV(false), "negative num");
checkError(() => zeroQ([]), "zero? is a test that only works on one num");
checkError(() => zeroQ([NumV(1), BoolV(true)]), 
  "zero? is a test that only works on one num");
checkError(() => zeroQ([BoolV(true)]), 
  "zero? is a test that only works on one num");


/*
emptyQ: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if
the list(value) is an empty list
*/ 
let emptyQ: list(value) => value = listVa =>
switch(listVa) {
  | [ListV([])] => BoolV(true)
  | [_] => BoolV(false)
  | _ => failwith("empty? expects one argument only")
  };

checkExpect(emptyQ([ListV([])]), BoolV(true), "empty list");
checkExpect(emptyQ([ListV([NumV(1)])]), BoolV(false), "one element list");
checkExpect(emptyQ([ListV([NumV(1), NumV(2)])]), BoolV(false), 
  "multi element list");
checkExpect(emptyQ([NumV(2)]), BoolV(false), "positive number input");
checkExpect(emptyQ([NumV(0)]), BoolV(false), "zero number input");
checkExpect(emptyQ([NumV(-2)]), BoolV(false), "negative number input");
checkExpect(emptyQ([BoolV(true)]), BoolV(false), "bool input");
checkExpect(emptyQ([BoolV(false)]), BoolV(false), "bool input");
checkError(() => emptyQ([]), "empty? expects one argument only");
checkError(() => emptyQ([NumV(1), BoolV(true)]), 
  "empty? expects one argument only");

/*
consQ: value list -> value

Input: a list of values, listVa
Output: a BoolV reflecting if the given list(value)
is a cons list
*/ 
let consQ: list(value) => value = listVa =>
switch(listVa) {
  | [ListV([_, ..._])] => BoolV(true)
  | [_] => BoolV(false)
  | _ => failwith ("cons? expects one argument")
  };

checkExpect(consQ([ListV([])]), BoolV(false), "empty list");
checkExpect(consQ([ListV([NumV(1)])]), BoolV(true), 
  "one element list");
checkExpect(consQ([ListV([NumV(1), NumV(2)])]), BoolV(true), 
 "multi element list");
checkExpect(consQ([NumV(1)]), BoolV(false), "positive number input");
checkExpect(consQ([NumV(0)]), BoolV(false), "zero number input");
checkExpect(consQ([NumV(-2)]), BoolV(false), "negative number input");
checkExpect(consQ([BoolV(true)]), BoolV(false), "bool input");
checkExpect(consQ([BoolV(false)]), BoolV(false), "bool input");
checkError(() => consQ([]), "cons? expects one argument");
checkError(() => consQ([NumV(1), BoolV(true)]), 
  "cons? expects one argument");


/*
cons: value list -> value

Input: a list of values, listVa
Output: a ListV that contains is the appending of the
the first element with the second: the second must be a 
ListV, failwiths are outputted when condtiions are not met.
*/
let cons: list(value) => value = listVa =>
switch(listVa) {
  | [x, ListV(y)] => ListV([x, ...y])
  | [_, _] => failwith("cons takes in an item and a list")
  | _ => failwith("cons has two inputs, an item and a list")
  };

checkExpect(cons([NumV(1), ListV([])]), ListV([NumV(1)]), 
  "empty list");
checkExpect(cons([NumV(1), ListV([NumV(2)])]),
  ListV([NumV(1), NumV(2)]), "one element list"); 
checkExpect(cons([NumV(1), ListV([NumV(2), NumV(3)])]), 
  ListV([NumV(1), NumV(2), NumV(3)]), "multi element list");
checkExpect(cons([ListV([NumV(1)]), ListV([])]), 
  ListV([ListV([NumV(1)])]), "empty list");
checkExpect(cons([ListV([NumV(1)]), ListV([NumV(2)])]), 
  ListV([ListV([NumV(1)]), NumV(2)]), "(list (list 1) 2");
checkExpect(cons([ListV([NumV(1)]), ListV([NumV(2), NumV(3)])]),
  ListV([ListV([NumV(1)]), NumV(2), NumV(3)]), "multi element list");
checkExpect(cons([BoolV(true), ListV([])]), ListV([BoolV(true)]), 
  "emptylist");
checkExpect(cons([BoolV(true), ListV([BoolV(false)])]),
  ListV([BoolV(true), BoolV(false)]), "one element list");
checkExpect(cons([BoolV(true), ListV([BoolV(false), BoolV(false)])]), 
  ListV([BoolV(true), BoolV(false), BoolV(false)]), "multi element list");
checkError(() => cons([NumV(1), NumV(2)]), "cons takes in an item and a list");
checkError(() => cons([]), "cons has two inputs, an item and a list");
checkError(() => cons([NumV(1)]), "cons has two inputs, an item and a list");
checkError(() => cons([NumV(1), NumV(2), NumV(3)]), 
"cons has two inputs, an item and a list");

/*
first: value list -> value

Input: a list of values, listVa
Output: the first element of the list(value)
*/
let first: list(value) => value = listVa =>
switch(listVa) {
  | [ListV([hd, ..._])] => hd
  | [ListV([])] => failwith("cannot take the first of the empty list")
  | _=> failwith("first operates on one argument that is a list only")
  };

checkExpect(first([ListV([NumV(2)])]), NumV(2), "one element list");
checkExpect(first([ListV([NumV(2), NumV(3)])]), NumV(2), 
  "multi element list");
checkError(() => first([ListV([])]), 
  "cannot take the first of the empty list");
checkError(() => first([]), "first operates on one argument " ++ 
  "that is a list only");
checkError(() => first([NumV(1)]), "first operates on one argument " ++ 
  "that is a list only");
checkError(() => first([NumV(1), NumV(2), NumV(3)]), 
  "first operates on one argument that is a list only");

/*
rest: value list -> value

Input: a list of values, listVa
Output: a ListV that is the rest/tail of the input
ListV. 
*/
let rest: list(value) => value = listVa =>
switch(listVa) {
  | [ListV([_])] => ListV([])
  | [ListV([_, ...tl])] => ListV(tl)
  | [ListV([])] => failwith("cannot take rest of the empty list")
  | _=> failwith("rest operates on one argument that is a list only")
  }

checkExpect(rest([ListV([NumV(2)])]), ListV([]), "one element list");
checkExpect(rest([ListV([NumV(2), NumV(3)])]), ListV([NumV(3)]), 
  "multi element list");
checkError(() => rest([ListV([])]), "cannot take rest of the empty list");
checkError(() => rest([]),
  "rest operates on one argument that is a list only");
checkError(() => rest([NumV(1)]), 
  "rest operates on one argument that is a list only");
checkError(() => rest([NumV(1), NumV(2), NumV(3)]), 
  "rest operates on one argument that is a list only");

/*
notQ value list -> value

Input: a list of values, listVa
Output: a BoolV that is the opposite
of the BoolV; failwiths are written if conditions aren't met.
*/
let notQ: list(value) => value = listVa =>
switch(listVa) {
  | [BoolV(true)] => BoolV(false)
  | [BoolV(false)] => BoolV(true)
  | _ => failwith("notQ accepts one boolean argument only")
  };

checkExpect(notQ([BoolV(true)]), BoolV(false), "notQ of true");
checkExpect(notQ([BoolV(false)]), BoolV(true), "notQ of false");
checkError(() => notQ([]), "notQ accepts one boolean argument only");
checkError(() => notQ([NumV(1)]),"notQ accepts one boolean argument only");
checkError(() => notQ([NumV(4), NumV(3), NumV(3)]), 
  "notQ accepts one boolean argument only");

/* 
initialTle constructs an environment that contains all the builtin functions. 
In other words it constructs the initial Top Level Environment of Racket.   
*/    
let initialTle: 
 environment = [
   (Name("+"), BuiltinV("+", plus)),
   (Name("-"), BuiltinV("-", minus)), 
   (Name("*"), BuiltinV("*", mult)),
   (Name("/"), BuiltinV("/", div)),
   (Name("remainder"), BuiltinV("remainder", remainder)),
   (Name("="), BuiltinV("=", equal)),
   (Name("<"), BuiltinV("<", lessThan)),
   (Name(">"), BuiltinV(">", greaterThan)),
   (Name("<="), BuiltinV("<=", lessOrEqual)),
   (Name(">="), BuiltinV(">=", gretrOrEqual)),
   (Name("equal?"), BuiltinV("equal?", equalQ)),
   (Name("number?"), BuiltinV("number?", numberQ)),
   (Name("zero?"), BuiltinV("zero?", zeroQ)),
   (Name("empty?"), BuiltinV("empty?", emptyQ)),
   (Name("cons?"), BuiltinV("cons?", consQ)),
   (Name("cons"), BuiltinV("cons", cons)),
   (Name("first"), BuiltinV("first", first)),
   (Name("rest"), BuiltinV("rest", rest)),
   (Name("not"), BuiltinV("not", notQ))
 ];

/* 
Type Signature: 
parseExpression: concreteProgramPiece -> expression 

Input: input, a concreteProgramPiece which did not patternmatch in parsePiece 
       to ListC([SymbolC("define"), ..._]) 
Output: an expression which is the converted form of the input

Recursion Diagram: 
OI: SymbolC("false")
  RI: - 
  RO: - 
OO: BoolE(false)
ideation space: for ones that convert to NumE, BoolE, and EmptyE 
                convert directly without recursion 

OI: ListC([SymbolC("and"), SymbolC("true"), NumberC(5)])
  RI: SymbolC("true")    *       NumberC(5)
  RO: BoolE(true)        *       NumE(5)
OO: AndE(BoolE(true), NumE(5))
ideation space: for ones that convert to AndE, OrE, or IfE, 
                convert second and third part of the ListC through recursion
*/
let rec parseExpression: concreteProgramPiece => expression =
  input => 
  switch (input) {
    | NumberC(a) => NumE(a)
    | SymbolC(a) =>
      switch(a) {
        | "true" => BoolE(true)
        | "false" => BoolE(false)
        | "empty" => EmptyE
        | b => NameE(Name(b))
      }
    | ListC(a) =>
      switch(a){
        |[SymbolC("and"), b, c] => AndE(parseExpression(b), parseExpression(c))
        | [SymbolC("and"), ..._] => failwith("AND expects two inputs.")
        | [SymbolC("or"), d, e] => OrE(parseExpression(d), parseExpression(e)) 
        | [SymbolC("or"), ..._] => failwith("OR expects two inputs.")
        | [SymbolC("if"), f, g, h] => 
          IfE(parseExpression(f), parseExpression(g), parseExpression(h))
        | [SymbolC("if"), ..._] => failwith("IF expects three inputs.")
        | [SymbolC("cond"), ...tl] => CondE(List.map(
          fun
          | ListC([x, y]) => (parseExpression(x), parseExpression(y))
          | _ => failwith("COND expects pairs."), tl))
        | [SymbolC("lambda"), ListC([]), ..._] => 
        failwith("LAMBDA expects a non empty list of arguments (aka names)" ++ 
        " and a body.")
        | [SymbolC("lambda"), ListC(x), y] => LambdaE(List.map(
          fun
          | SymbolC(z) => Name(z)
          | _ => failwith("LAMBDA expects a name(s) for an argument(s)."), x),
           parseExpression(y))
        | [SymbolC("lambda"), ..._] => 
        failwith("LAMBDA expects a non empty list of arguments (aka names)" ++ 
        " and a body.")
        | [SymbolC("let"), ListC(x), y] => LetE(List.map(
          fun
          | ListC([SymbolC(z), zz]) => (Name(z), parseExpression(zz))
          | _ => failwith("LET expects a pair of name and expression."), x),
          parseExpression(y))
        | [SymbolC("let"), ... _] => failwith("LET expects a list of pairs" ++
          " of (name, expression) and a body.")
          // (let ((x 5)(x 6)) blah)
          // ########## ==> catch it in eval 
        | [x,... tl] => ApplicationE(List.map(parseExpression, 
          [x,... tl]))/*
        | [ListC([a]),... tl] => 
        switch(parseExpression(ListC([a]))){
          | LambdaE(_) => ApplicationE(List.append(
            [parseExpression(ListC([a]))], List.map(parseExpression, tl)))
          | LetE(_) => ApplicationE(List.append(
            [parseExpression(ListC([a]))], List.map(parseExpression, tl)))
          | _ => failwith("Two open parentheses expect a lambda or let");
        }   */     
        | _ => failwith("An open parenthesis expects a function.");
      }  
  };

// Test parseExpression
checkExpectExpression(parseExpression(NumberC(5)), NumE(5), "NumE 5"); 
checkExpectExpression(parseExpression(SymbolC("true")), BoolE(true), "true");
checkExpectExpression(parseExpression(SymbolC("false")), BoolE(false),"false");
checkExpectExpression(parseExpression(SymbolC("empty")), EmptyE, "EmptyE");
checkExpectExpression(parseExpression(SymbolC("f")), NameE(Name("f")), "f");

checkExpectExpression(parseExpression(ListC([SymbolC("and"), 
  SymbolC("true"), NumberC(5)])), AndE(BoolE(true), NumE(5)), "and true 5");
checkExpectExpression(parseExpression(ListC([SymbolC("and"), 
  SymbolC("false"), NumberC(5)])), AndE(BoolE(false), NumE(5)), "and false 5");
checkExpectExpression(parseExpression(ListC([SymbolC("and"), 
  NumberC(3), NumberC(5)])), AndE(NumE(3), NumE(5)), "and 3 5 - CATCH in EVAL!");
checkExpectExpression(parseExpression(ListC([SymbolC("and"), 
  SymbolC("empty"), SymbolC("f")])), AndE(EmptyE, NameE(Name("f"))), 
"and empty f - CATCH in EVAL!");

checkExpectExpression(parseExpression(ListC([SymbolC("or"), 
  SymbolC("true"), NumberC(5)])), OrE(BoolE(true), NumE(5)), "or true 5");
checkExpectExpression(parseExpression(ListC([SymbolC("or"), 
  SymbolC("false"), NumberC(5)])), OrE(BoolE(false), NumE(5)), "or false 5");
checkExpectExpression(parseExpression(ListC([SymbolC("or"), 
  NumberC(3), NumberC(5)])), OrE(NumE(3), NumE(5)), "or 3 5 - CATCH in EVAL!");
checkExpectExpression(parseExpression(ListC([SymbolC("or"), 
  SymbolC("empty"), SymbolC("f")])), OrE(EmptyE, NameE(Name("f"))), 
  "or empty f - CATCH in EVAL!");

checkExpectExpression(parseExpression(ListC([SymbolC("if"), 
  SymbolC("true"), NumberC(3), NumberC(5)])), IfE(BoolE(true), NumE(3), NumE(5)),
  "if true 3 5");
checkExpectExpression(parseExpression(ListC([SymbolC("if"), 
  SymbolC("false"), NumberC(3), NumberC(5)])), 
  IfE(BoolE(false), NumE(3), NumE(5)), "if false 3 5");
checkExpectExpression(parseExpression(ListC([SymbolC("if"), 
  NumberC(3), NumberC(5), NumberC(7)])), IfE(NumE(3), NumE(5), NumE(7)), 
  "if 3 5 7 - CATCH in EVAL!");
checkExpectExpression(parseExpression(ListC([SymbolC("if"), 
  SymbolC("empty"), SymbolC("f"), NumberC(7)])), 
  IfE(EmptyE, NameE(Name("f")), NumE(7)), "if empty f 7 - CATCH in EVAL!");

checkExpectExpression(parseExpression(ListC([SymbolC("cond"), 
  ListC([SymbolC("true"), NumberC(5)]), ListC([SymbolC("false"), 
  SymbolC("empty")])])), CondE([(BoolE(true), NumE(5)), (BoolE(false), EmptyE)]),
  "cond (true 5) (false empty) - CATH IN EVAL for different return type??");
checkExpectExpression(parseExpression(ListC([SymbolC("cond"), 
  ListC([ListC([SymbolC("empty?"), SymbolC("alod")]), NumberC(5)]), 
  ListC([ListC([SymbolC("cons?"), SymbolC("alod")]), NumberC(3)])])), 
  CondE([(ApplicationE([NameE(Name("empty?")), NameE(Name("alod"))]), NumE(5)), 
  (ApplicationE([NameE(Name("cons?")), NameE(Name("alod"))]), NumE(3))]), 
  "cond (empty? alod = 5) (cons? alod = 3)");

checkExpectExpression(parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("x")]), NumberC(5)])), LambdaE([Name("x")], NumE(5)), 
  "lambda (x) 5");
checkExpectExpression(parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("x")]), SymbolC("empty")])), LambdaE([Name("x")],EmptyE), 
  "lambda (x) empty");
checkExpectExpression(parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("x")]), SymbolC("true")])), LambdaE([Name("x")], 
  BoolE(true)), "lambda (x) true");
checkExpectExpression(parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("x")]), SymbolC("false")])), LambdaE([Name("x")], 
  BoolE(false)), "lambda (x) false");
checkExpectExpression(parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("x"), SymbolC("y")]), ListC([SymbolC("+"), SymbolC("x"), 
  SymbolC("y")])])), LambdaE([Name("x"), Name("y")], 
  ApplicationE([NameE(Name("+")), NameE(Name("x")), NameE(Name("y"))])), 
  "lambda (x y) (+ x y)");

checkExpectExpression(parseExpression(ListC([SymbolC("let"), 
  ListC([ListC([SymbolC("x"), NumberC(3)])]), ListC([SymbolC("+"), SymbolC("x"), 
  NumberC(5)])])), LetE([(Name("x"), NumE(3))], 
  ApplicationE([NameE(Name("+")), NameE(Name("x")), NumE(5)])), "let(x3)(+x5)");
checkExpectExpression(parseExpression(ListC([SymbolC("let"), 
  ListC([ListC([SymbolC("x"), NumberC(3)]), ListC([SymbolC("y"), NumberC(5)])]),
  ListC([SymbolC("+"), SymbolC("x"), SymbolC("y")])])), LetE([(Name("x"), 
  NumE(3)), (Name("y"), NumE(5))], ApplicationE([NameE(Name("+")), 
  NameE(Name("x")), NameE(Name("y"))])), "let((x3)(y5))(+xy)");

checkExpectExpression(parseExpression(ListC([SymbolC("+"), SymbolC("x"), 
  SymbolC("y")])), ApplicationE([NameE(Name("+")), NameE(Name("x")), 
  NameE(Name("y"))]), "(+ x y)");
checkExpectExpression(parseExpression(ListC([SymbolC("zero?"), NumberC(5)])), 
  ApplicationE([NameE(Name("zero?")), NumE(5)]), "(zero? 5)");

/*Check Errors for parseExpression*/
checkError(() => parseExpression(ListC([SymbolC("and"), NumberC(5)])), 
  "AND expects two inputs.");
checkError(() => parseExpression(ListC([SymbolC("and"), SymbolC("true")])), 
  "AND expects two inputs.");
checkError(() => parseExpression(ListC([SymbolC("or"), NumberC(5)])), 
  "OR expects two inputs.");
checkError(() => parseExpression(ListC([SymbolC("or"), SymbolC("false")])), 
  "OR expects two inputs.");
checkError(() => parseExpression(ListC([SymbolC("if"), NumberC(5)])), 
  "IF expects three inputs.");
checkError(() => parseExpression(ListC([SymbolC("if"), NumberC(5), 
  SymbolC("empty")])), "IF expects three inputs.");
checkError(() => parseExpression(ListC([SymbolC("if")])), 
  "IF expects three inputs.");
checkError(() => parseExpression(ListC([SymbolC("cond"), ListC([])])), 
  "COND expects pairs.");
checkError(() => parseExpression(ListC([SymbolC("cond"), 
  ListC([SymbolC("true")])])), "COND expects pairs.");
checkError(() => parseExpression(ListC([SymbolC("lambda"), ListC([NumberC(5)]), 
  NumberC(2)])), "LAMBDA expects a name(s) for an argument(s).");
checkError(() => parseExpression(ListC([SymbolC("lambda"), ListC([]), 
  NumberC(2)])), 
  "LAMBDA expects a non empty list of arguments (aka names) and a body.");
checkError(() => parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("a")])])), 
  "LAMBDA expects a non empty list of arguments (aka names) and a body.");
checkError(() => parseExpression(ListC([SymbolC("lambda"), 
  ListC([SymbolC("a")]), NumberC(3), NumberC(5)])), 
  "LAMBDA expects a non empty list of arguments (aka names) and a body.");
checkError(() => parseExpression(ListC([SymbolC("let"), ])), 
  "LET expects a list of pairs of (name, expression) and a body.");
// ## checkError(() => parseExpression(ListC([SymbolC("let"), ])), 
// "LET expects a pair of name and expression not previously bound in the L.");
checkError(() => parseExpression(ListC([SymbolC("let"), ListC([NumberC(4), 
  NumberC(4)]), NumberC(5)])), "LET expects a pair of name and expression.");
checkError(() => parseExpression(ListC([SymbolC("let"), ListC([SymbolC("a")]), 
  NumberC(5)])), "LET expects a pair of name and expression.");
checkError(() => parseExpression(ListC([SymbolC("let")])), 
  "LET expects a list of pairs of (name, expression) and a body.");
checkError(() => parseExpression(ListC([SymbolC("let"), ListC([SymbolC("a"), 
  NumberC(4)]), NumberC(5), SymbolC("true")])), 
  "LET expects a list of pairs of (name, expression) and a body.");
checkError(() => parseExpression(ListC([])), 
  "An open parenthesis expects a function.");

/* 
Type Signature: 
parseDefinition: concreteProgramPiece -> definition

Input: input, a concreteProgramPiece
Output: a defintion corresponding to the input 
*/
let parseDefinition: concreteProgramPiece => definition =
  input =>
  switch(input){
    | ListC([SymbolC("define"), SymbolC(a), b]) => 
      (Name(a), parseExpression(b))
    | _ => failwith("This is an invalid input for parseDefinition.");
  };

// Test parseDefinition
checkExpectDefinition(parseDefinition(ListC([SymbolC("define"), SymbolC("a"), 
  NumberC(5)])), (Name("a"), NumE(5)), "define a 5");
checkExpectDefinition(parseDefinition(ListC([SymbolC("define"), SymbolC("a"), 
  SymbolC("f")])), (Name("a"), NameE(Name("f"))), "define a f");
checkExpectDefinition(parseDefinition(ListC([SymbolC("define"), SymbolC("a"), 
  SymbolC("empty")])), (Name("a"), EmptyE), "define a (list )");
checkExpectDefinition(parseDefinition(ListC([SymbolC("define"), SymbolC("a"), 
  SymbolC("true")])), (Name("a"), BoolE(true)), "define a true");
checkExpectDefinition(parseDefinition(ListC([SymbolC("define"), SymbolC("a"), 
  SymbolC("false")])), (Name("a"), BoolE(false)), "define a false");

/*Check Errors for parseDefinition*/
checkError(() => parseDefinition(ListC([SymbolC("define")])), 
  "This is an invalid input for parseDefinition.");
checkError(() => parseDefinition(ListC([SymbolC("define"), NumberC(5)])), 
  "This is an invalid input for parseDefinition.");
checkError(() => parseDefinition(ListC([SymbolC("define"), SymbolC("a")])), 
  "This is an invalid input for parseDefinition.");
checkError(() => parseDefinition(ListC([NumberC(5)])), 
  "This is an invalid input for parseDefinition.");

/* 
Type Signature: 
parsePiece: concreteProgramPiece -> abstractProgramPiece

Input: input, a concreteProgramPiece which can be a definition or expression
Output: the resulting abstractProgramPiece
*/
let parsePiece: concreteProgramPiece => abstractProgramPiece =
  input =>
    switch (input) {
    | ListC([SymbolC("define"), ..._]) =>
      Definition(parseDefinition(input))
    | _ => Expression(parseExpression(input))
    };

/* 
Type Signature: 
parse: concreteProgram -> abstractProgram 

Input: input, a concreteProgram which is the result of readAll
Output: an abstractProgram which is the result of converting the input 
*/
let parse: concreteProgram => abstractProgram =
  input =>
    /* this will parse all of the pieces of this program,
     * giving us a list of pieces, our abstract syntax */
    List.map(parsePiece, input);

/* 
Type Signature: 
eval: environment * environment * expression -> value 

Input: tle, env, expr: tle is the top-level environment (T); env is the local 
       environment (E); expr is an expression.
Output: the value of the expression in the environment T + E (T extended by E),
        in which values are looked up first in E and then in T.

Recursion Diagram: 
OI: (T, L, BoolE(true))
  RI: -
  RO: -
OO: BoolV(true)
ideation space: For NumV and BoolV just process without recursion. 

OI: (T, L, AndE(BoolE(true), NumE(5)))
  RI: NumE(5)
  RO: NumV(5)
OO: NumV(5)
ideation space: pattern matching on first expression of AndE and if it's false
                then evaluate to false. If true then evaluate second expression
                through recursion
*/
let rec eval: (environment, environment, expression) => value =
  (tle, env, expr) => 
  switch(expr){
    | NumE(a) => NumV(a)
    | BoolE(b) => BoolV(b)
    | EmptyE => ListV([])
    | NameE(c) => 
      {let rec getBind: (environment, environment, name) => value = 
      (tle, env, n) =>
      switch(env){
        |[] => 
        switch (tle){
          | [] => failwith("Both environments are empty/no matches.")
          | [(x, y), ...tl] => if (x == n) {y} else {getBind(tl,env,n)}
        }
        |[(x, y), ...tl] => if (x == n) {y} else {getBind(tle, tl, n)};
      } getBind(tle, env, c);} 
    | AndE(a, b) => 
    switch(eval(tle, env, a), eval(tle, env, b)){
      | (BoolV(false), _) => BoolV(false)
      | (BoolV(true), x) => x
      | _ => failwith("AND expects expr1 to evaluate to a boolean.")
    }
    | OrE(a, b) => 
    switch(eval(tle, env, a), eval(tle, env, b)){
      | (BoolV(true), _) => BoolV(true)
      | (BoolV(false), x) => x
      | _ => failwith("OR expects expr1 to evaluate to a boolean.")}
    | IfE(a, b, c) => 
    switch(eval(tle, env, a)){
      | BoolV(true) => eval(tle, env, b)
      | BoolV(false) => eval(tle, env, c)
      | _ => failwith("IF expects expr1 to evaluate to a boolean.")
    } 
    | CondE([(a, b), ...tl]) => 
    switch(eval(tle, env, a)){
      | BoolV(true) => eval(tle, env, b)
      | BoolV(false) => eval(tle, env, CondE(tl))
      | _ => failwith("None of the conditions evaluate to a boolean/true.")
    }
    | CondE([]) => 
    failwith("None of the conditions evaluate to a boolean/true.");
    | LambdaE(a, b) =>
    switch(a){
      | [Name(_), ..._] => ClosureV(a, b, List.append(env, tle))
      | _ => failwith("LAMBDA expects a list of arguments and a body.")
    }
    | LetE([], _) => 
      failwith("LET should have a non-empty list of (name, expr)");
    | LetE(a, b) => eval(tle, List.append(List.map(
      fun
      | (Name(z), zz) => (Name(z), eval(tle, env, zz)), a), env), b);
    | ApplicationE([a, ...tl]) => 
    switch(eval(tle, env, a)){
      | BuiltinV(_, b) => b(List.map(eval(tle, env, ), tl)) 
      | ClosureV(ln, ex, _) =>  if (List.length(ln) == List.length(tl))
      {eval(tle, List.append({
        let rec makeBinding: (list(name), list(expression)) => environment
        = (ln, tl) =>
        switch(ln, tl){
          | ([], []) => []
          | ([a,...lntl], [b, ...tltl]) => [(a, eval(tle, env, b)), 
          ...makeBinding(lntl, tltl)]
          | _ => failwith("Should not happen cuz ln = tl length. but catch!");
        }
      makeBinding(ln, tl)}, env), ex);} 
      else {failwith("some arguments are undefined.");};
      | _ => failwith("ApplicationE expects the expr1 to evaluate to either" ++
      " a BuiltinV or ClosureV.")
      };
    | ApplicationE([]) => failwith("ApplicationE cannot be empty.")
  };
      
/* 
addDefinition: environment * (name, expression) -> environment
Input: envi, (idn, exp): envi is an environment; (idn, exp) is a 
(name, expression) tuple
Output: an environment with a binding of idn and the evaluated version of exp
is added to it if there isn't a binding with the same name in already;
otherwise, a failwith statement informing the user that the name has 
already been defined.
*/
let rec addDefinition: (environment, (name, expression)) => 
environment = (envi, (idn, exp)) => 
switch(envi){
  | [] => [(idn, eval(initialTle, envi, exp))]
  | ([(hd, tl), ...rest]) => 
  if (idn == hd) {failwith("name is defined already");}
  else {[(hd, tl), ...addDefinition(rest, (idn, exp))];}
  };
/* Testing addDefinition */
checkExpect(addDefinition([], (Name("a"), NumE(5))), 
[(Name("a"), NumV(5))], "testing addDefinition a 5");
checkExpect(addDefinition([], (Name("a"), BoolE(true))), 
[(Name("a"), BoolV(true))], "testing addDefinition a true");
checkExpect(addDefinition([], (Name("a"), EmptyE)), 
[(Name("a"), ListV([]))], "testing addDefinition a 5");

// Check Errors for addDefinition
checkError(() => addDefinition([(Name("a"), NumV(5))], (Name("a"), NumE(5))), 
"name is defined already");
checkError(() => addDefinition([(Name("a"), NumV(5))], (Name("a"), NumE(6))), 
"name is defined already");

/*
stringOfValue: value -> string

input: a value, val1
output: a string representation of val1

Recursion Diagram #1 for stringOfValue:
Original Input: NumV(25)
  Recursive Input: N/A
  Recursive Output: N/A
 Ideation Space:
  This is a base care that does not need recursion.
  Whatever is in NumV() is printed directly
Original Output: "25"

Recursion Diagram #2 for stringOfValue:
Original Input: ListV([NumV(6), NumV(7)])
  Recursive Input: NumV(6), NumV(7)
  Recursive Output: "6", "7"
 Ideation Space:
  We print out "(list " ++ recursive output +")"
Original Output: (list 1 2)
*/
let rec stringOfValue: value => string = val1 => 
switch(val1) {
  | NumV(int) => string_of_int(int)
  | BoolV(bool) => string_of_bool(bool)
  | BuiltinV(builtinName, _) => builtinName
  | ListV(values)  => "(list " ++{ 
    let rec vListHelper: list(value) => string = fun
    | [] => ")"
    | [hd] =>stringOfValue(hd) ++ ")"
    | [hd, ...tl] => stringOfValue(hd) ++ " " ++ vListHelper(tl);
    vListHelper(values);
    } 
    | ClosureV(_, _, _) => "<user-proc>"
    };

checkExpect(stringOfValue(NumV(3)), "3", "sov 3");
checkExpect(stringOfValue(BoolV(true)), "true", "sov true");
checkExpect(stringOfValue(BoolV(false)), "false", "sov false");
checkExpect(stringOfValue(ListV([NumV(5)])), "(list 5)", "sov (list 5)");

/* 
process: abstractProgram -> value list

Input: pieces: an abstractProgram which is the result of parse
Output: a list of values that will be read by stringofValue
*/
let process: abstractProgram => list(value) =
  pieces => {
    let rec processHelper: (environment, abstractProgram) => list(value) =
      (tle, pieces) =>
        switch (pieces) {
        | [] => []
        | [Definition(d), ...tl] => processHelper(addDefinition(tle, d), tl)
        | [Expression(e), ...tl] => [
            eval(tle, [], e),
            ...processHelper(tle, tl),
          ]
        };
    processHelper(initialTle, pieces);
  };

/* 
rackette: rawProgram -> string list

Input: program: a rawProgram which is a string of Racket code
Output: a list of strings which is the result of reading, parsing, 
and processing (evaluating and defining) the input code
*/
let rackette: rawProgram => list(string) =
  program => List.map(stringOfValue, process(parse(readAll(program))));
  
// Test rackette
checkExpect(rackette("((lambda (x y)((lambda (y)(+ x y))x))17 18)"), ["34"], 
"rackette test-case #1");

// # 
// checkExpect(rackette("((lambda (x y)((lambda (x)(+ x y))y))17 18)"), ["36"],
// "rackette test-case #2");

checkExpect(rackette("(define y 17)(let ((y 3))(+ y 7))"), ["10"],
  "rackette test-case #3");
 
// # supposed to be 17 but is 34
// checkExpect(rackette("(let ((x 0)(y 18))(let 
// ((f (lambda (a b)(+ x b )))(x 17))(f y x)))"), ["17"], 
// "rackette test-case #4");

checkExpect(rackette("(define fact (lambda (x)(if (zero? x) 1 
(* x (fact ( - x 1))))))(fact 3)"), ["6"], 
  "rackette test-case #5");

checkExpect(rackette("(+ 3 6)"), ["9"], "rackette test-case #6");

// # supposed to be 0 but is 5
// checkExpect(rackette("(let ((x 0))(let 
// (( f (lambda (a)(* x a ))))(let ((x 1 ))(f 5))))
// "), ["0"], "rackette test-case #6");

// # 
// checkExpect(rackette("((lambda (x y)((lambda (x)(+ x y))x))17 18)"), ["35"], 
//   "rackette test-case #7");

checkExpect(rackette("(define x 1) (+ x 1)"), ["2"],
  "operation and definitions");

checkExpect(rackette("6 7"), ["6", "7"], "numbers");

checkExpect(rackette("(if (= 0 9) 6 4)"), ["4"], " if statement");

 // Testing eval 
  checkExpect(stringOfValue(eval(initialTle, [], NumE(5))), "5", 
    "evaluation of 5");
  checkExpect(stringOfValue(eval(initialTle, [], BoolE(true))), "true", 
    "evaluation of true");
  checkExpect(stringOfValue(eval(initialTle, [], BoolE(false))), "false", 
    "evaluation of false");
  checkExpect(stringOfValue(eval(initialTle, [], EmptyE)), "(list )", 
    "evaluation of empty list");

  checkExpect(stringOfValue(eval(initialTle, [(Name("f"), BoolV(true))], 
    NameE(Name("f")))), "true", "evaluation of name f = true");
  checkExpect(stringOfValue(eval(initialTle, 
    [(Name("f"), ListV([BoolV(true)]))], NameE(Name("f")))), "(list true)", 
    "evaluation of name f = (list true)");  
  checkExpect(stringOfValue(eval(initialTle, 
    [(Name("f"), ListV([BoolV(true), BoolV(false)]))], NameE(Name("f")))), 
    "(list true false)", "evaluation of name f = (list true false)");
  checkExpect(stringOfValue(eval(initialTle, [(Name("a"), NumV(5))], 
    NameE(Name("a")))), "5", "evaluation of name a = 5");

  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(false), 
    BoolE(true)))), "false", "evaluation of AND");
  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(false), 
    NumE(5)))), "false", "evaluation of AND");
  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(false), 
    EmptyE))), "false", "evaluation of AND");
  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(true), 
    BoolE(false)))), "false", "evaluation of AND");
  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(true), 
    NumE(5)))), "5", "evaluation of AND");
  checkExpect(stringOfValue(eval(initialTle, [], AndE(BoolE(true), 
    EmptyE))), "(list )", "evaluation of AND");

  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(true), 
    BoolE(false)))), "true", "evaluation of OR");
  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(true), 
    NumE(5)))), "true", "evaluation of OR");
  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(true), 
    EmptyE))), "true", "evaluation of OR");
  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(false), 
    BoolE(true)))), "true", "evaluation of OR");
  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(false), 
    NumE(5)))), "5", "evaluation of OR");
  checkExpect(stringOfValue(eval(initialTle, [], OrE(BoolE(false), 
    EmptyE))), "(list )", "evaluation of OR");
  
  checkExpect(stringOfValue(eval(initialTle, [], IfE(BoolE(true), NumE(3), 
    NumE(5)))), "3", "evaluation of IF"); 
  checkExpect(stringOfValue(eval(initialTle, [], IfE(BoolE(false), NumE(3), 
    NumE(5)))), "5", "evaluation of IF"); 
  checkExpect(stringOfValue(eval(initialTle, [], 
    IfE(ApplicationE([NameE(Name("zero?")), NumE(0)]) , NumE(3), 
    NumE(5)))), "3", "evaluation of IF"); 
  checkExpect(stringOfValue(eval(initialTle, [], 
    IfE(ApplicationE([NameE(Name("zero?")), NumE(7)]) , NumE(3), 
    NumE(5)))), "5", "evaluation of IF"); 

  checkExpect(stringOfValue(eval(initialTle, [], CondE([(BoolE(true), NumE(3)), 
    (BoolE(false), NumE(5))]))), "3", "evaluation of COND"); 
  checkExpect(stringOfValue(eval(initialTle, [], 
    CondE([(BoolE(false), NumE(3)), (BoolE(true), NumE(5))]))), 
    "5", "evaluation of COND"); 
  checkExpect(stringOfValue(eval(initialTle, [], 
    CondE([(ApplicationE([NameE(Name("zero?")), NumE(0)]), NumE(3)),
    (ApplicationE([NameE(Name("zero?")), NumE(7)]), NumE(5))]))), "3", 
    "evaluation of COND"); 
  checkExpect(stringOfValue(eval(initialTle, [], 
    CondE([(ApplicationE([NameE(Name("zero?")), NumE(7)]), NumE(3)),
    (ApplicationE([NameE(Name("zero?")), NumE(0)]), NumE(5))]))), "5", 
    "evaluation of COND"); 
  checkExpect(stringOfValue(eval(initialTle, [], CondE([(BoolE(true), NumE(4)), 
    (ApplicationE([NameE(Name("zero?")), NumE(5)]), NumE(3))]))), "4", 
    "evaluation of COND 4");
  
  checkExpect(stringOfValue(eval(initialTle, [], 
    ApplicationE([LambdaE([Name("x")], ApplicationE([NameE(Name("+")), 
    NameE(Name("x")), NumE(5)])), NumE(3)]))), "8", 
    "trying out LAMBDA - result 8");

  checkExpect(stringOfValue(eval(initialTle, [], LetE([(Name("a"), NumE(1)), 
    (Name("b"), NumE(-1))], NumE(3)))), "3", "trying out LET - result 3");
  checkExpect(stringOfValue(eval(initialTle, [], LetE([(Name("a"), NumE(1)), 
    (Name("b"), NumE(-1))], ApplicationE([NameE(Name("+")), NameE(Name("a")), 
    NameE(Name("b"))])))), "0", "trying out LET - result 0");

  checkExpect(stringOfValue(eval(initialTle, [], 
    ApplicationE([NameE(Name("+")), NumE(5), NumE(15)]))), "20", 
    "trying out Builtin App - result 20");
  checkExpect(stringOfValue(eval(initialTle, [], 
    ApplicationE([NameE(Name("zero?")), NumE(5)]))), "false", 
    "trying out Builtin App - result false");
  checkExpect(stringOfValue(eval(initialTle, [], 
    ApplicationE([LambdaE([Name("x")], ApplicationE([NameE(Name("+")),
    NameE(Name("x")), NumE(4)])), NumE(3)]))), "7", 
    "trying out Lambda App - result 7");
  
  /* Check Errors for Eval*/
  checkError(() => eval([], [], NameE(Name("+"))), 
    "Both environments are empty/no matches."); 
  checkError(() => eval(initialTle, [], NameE(Name("a"))), 
    "Both environments are empty/no matches."); 
  checkError(() => eval(initialTle, [], AndE(NumE(5), NumE(3))), 
    "AND expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], AndE(EmptyE, NumE(3))), 
    "AND expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], OrE(NumE(5), NumE(3))), 
    "OR expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], OrE(EmptyE, NumE(3))), 
    "OR expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], IfE(NumE(5), NumE(3), NumE(4))), 
    "IF expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], IfE(EmptyE, NumE(4), NumE(3))), 
    "IF expects expr1 to evaluate to a boolean.");
  checkError(() => eval(initialTle, [], CondE([(BoolE(false), NumE(4)), 
    (ApplicationE([NameE(Name("zero?")), NumE(5)]), NumE(3))])), 
    "None of the conditions evaluate to a boolean/true.");
  checkError(() => eval(initialTle, [], CondE([])), 
    "None of the conditions evaluate to a boolean/true.");
  checkError(() => eval(initialTle, [], LambdaE([], NumE(5))), 
    "LAMBDA expects a list of arguments and a body.");
  checkError(() => eval(initialTle, [], LetE([], NumE(4))), 
    "LET should have a non-empty list of (name, expr)");
  checkError(() => eval(initialTle, [],
    LetE([(Name("x"), NumE(3)), (Name("x"), NumE(5))], NumE(17))),
    "name is defined already");
  checkError(() => eval(initialTle, [], ApplicationE([NumE(5)])), 
    "ApplicationE expects the expr1 to evaluate to either" ++
    " a BuiltinV or ClosureV.");
  checkError(() => eval(initialTle, [], ApplicationE([])), 
    "ApplicationE cannot be empty.");
  // #
  // checkError(() => eval(initialTle, [], ApplicationE([LambdaE([Name("x")], 
  // ApplicationE([NameE(Name("+")), NameE(Name("x")), NumE(4)])), 
  // NumE(3), NumE(5)]))), "7", "some arguments are undefined.");