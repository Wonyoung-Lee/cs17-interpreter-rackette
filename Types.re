type rawProgram = string;
// "(define a 3)"
// "(+ -1 17)"

type concreteProgramPiece =
  | NumberC(int)
  // NumberC(3)
  // NumberC(-1)
  | SymbolC(string)
  // SymbolC("a")
  // SymbolC("define")
  // SymbolC("+")
  | ListC(list(concreteProgramPiece));
  // ListC([NumberC(3), NumberC(4), NumberC(5)])
  // ListC([SymbolC("a"), SymbolC("b"), SymbolC("c")])
  // ListC([SymbolC("define"), SymbolC("a"), NumberC(3)])
  // ListC([SymbolC("+"), NumberC(-1), NumberC(17)])

type concreteProgram = list(concreteProgramPiece);
// [SymbolC("define"), SymbolC("a"), NumberC(3)]
// [SymbolC("+"), NumberC(-1), NumberC(17)]

/* a Rackette name */
type name =
  | Name(string);
// Name("fact")
// Name("alod")

/* a Rackette expression */
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
  | AndE(expression, expression)
  // AndE(BoolE(true), BoolE(false))
  // AndE(BoolE(false), NumE(5))
  | OrE(expression, expression)
  // OrE(BoolE(true), BoolE(false))
  // OrE(BoolE(true), NumV(6))
  | IfE(expression, expression, expression)
  // IfE(BoolE(true), NumE(3), NumE(0))
  // IfE(BoolE(false), NumE(-3), NumE(3))
  | CondE(list((expression, expression)))
  // CondE([(BoolE(true), NumE(0)), (BoolE(false), NumE(3)))])
  // CondE([(BoolE(true), NameE(Name("alod"))), 
  //         (BoolE(false), NameE(Name("alod2")))])
  | LambdaE(list(name), expression)
  // LambdaE([Name("x"), Name("y")], NumE(17))
  // LambdaE([Name("a")], BoolE(true))
  // LambdaE([Name("x")], 
  //  ApplicationE([NameE(Name("+")), NameE(Name("x")), NameE(Name("13"))]))))
  | LetE(list((name, expression)), expression)
  // LetE([(Name("a"), NumE(1)), (Name("b"), NumE(-1))], NumE(3))
  // LetE([(Name("a"), NumE(1)), (Name("b"), NumE(-1))], 
  //   ApplicationE([NameE(Name("+")), NameE(Name("a")), NameE(Name("b"))]))
  | ApplicationE(list(expression));
  // ApplicationE([NameE(Name("+")), NameE(Name("a")), NameE(Name("b"))]))
  // ApplicationE([NameE(Name("zero?")), NameE(Name("a"))])

/* a Rackette definition */
type definition = (name, expression);
// (Name("a"), NumE("3"))
// (Name("alod"), EmptyE)

/* a piece of Rackette that can be processed:
 * either a definition or an expression */
type abstractProgramPiece =
  | Definition(definition)
  // Definition((Name("friend"), BoolE("true")))
  // Definition((Name("b"), NumE(0))) 
  | Expression(expression);
  // Expression(ApplicationE([NameE(Name("zero?")), NameE(Name("b"))]))
  // Expression(NumE(17))

/* a representation of a Rackette program -
 * any number of pieces */
type abstractProgram = list(abstractProgramPiece);
// [Definition((Name("b"), NumE(0)))]
// [Definition((Name("b"), NumE(0))), 
//         Expression(ApplicationE([NameE(Name("zero?")), NameE(Name("b"))]))]
// [Expression(NumE(17))]

/* a Rackette value: the result of evaluating a Rackette expression */
type value =
  | NumV(int)
  // NumV(34)
  // NumV(-34)
  | BoolV(bool)
  // BoolV(true)
  // BoolV(false)
  | ListV(list(value))
  // ListV([NumV(1), NumV(2)])
  // ListV([BoolV(false), BoolV(false)])
  | BuiltinV(string, list(value) => value)
  // BuiltinV("+", [NumV(34), NumV(-34)] => NumV("0"))
  // BuiltinV("*", [NumV(1), Num(17)] => NumV(17))
  | ClosureV(list(name), expression, environment)
  // ClosureV([Name("x"), Name("y")], 
  //  ApplicationE([NameE(Name("+")), NameE(Name("x")), NameE(Name("y"))]))),
  //  [(Name("x"), NumV("17")), (Name("y"), NumV("18"))])
  // ClosureV([Name("a")], 
  // ApplicationE([NameE(Name("-")), NameE(Name("a")), NameE(Name("13"))]))),
  // [(Name("a"), NumV("17"))]) 
  and environment = (list(binding))
  // [(Name("a"), NumV(3)), (Name("b"), BoolV(true))]
  // [(Name("list1"), ListV([NumV(-3), NumV(3)]))]
  and binding = (name, value);
  // (Name("a"), NumV(3))
  // (Name("list1"), ListV([NumV(-3), NumV(3)]))