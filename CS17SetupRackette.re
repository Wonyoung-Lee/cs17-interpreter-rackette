open Types;

/* take in a string, s, and print it with green color */
let printGreen: string => unit =
  s => print_endline("\027[32m" ++ s ++ "\027[0m");

/* take in a string, s, and print it with red color */
let printRed: string => unit =
  s => print_endline("\027[31m" ++ s ++ "\027[0m");

/* checkExpect
 * Inputs: actual and expected, two 'a and message, a string. You can use this
 *         procedure on any type, but it will only print nice if it's an atomic
 *         data type like string, int, float, bool.
 * Output: nothing
 */
let checkExpect: ('a, 'a, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ceSuccess: " ++ message);
    } else {
      printRed("ceFail: " ++ message);
      printRed("expected output: ");
      Js.log(expected);
      printRed("actual output: ");
      Js.log(actual);
    };

/* takes in a list, lst, and returns its string representation */
let stringOfAList: (list('a), 'a => string) => string =
  (lst, strOf) => {
    let rec listHelper: list('a) => string =
      lst =>
        switch (lst) {
        | [] => ""
        | [hd] => strOf(hd)
        | [hd, ...tl] => strOf(hd) ++ ", " ++ listHelper(tl)
        };
    "[" ++ listHelper(lst) ++ "]";
  };

/* takes in a name, namex, and returns its string representation */
let stringOfName: name => string =
  namex =>
    switch (namex) {
    | Name(stringx) => "Name(" ++ stringx ++ ")"
    };

/* takes in a list of names, nameList, and returns its string representation */
let stringOfNameList: list(name) => string =
  nameList => stringOfAList(nameList, stringOfName);

/* takes in an Expression, expr, and returns its string representation */
let rec stringOfExpression: expression => string =
  expr =>
    switch (expr) {
    | NumE(intx) => "NumE(" ++ string_of_int(intx) ++ ")"
    | BoolE(boolx) => "BoolE(" ++ string_of_bool(boolx) ++ ")"
    | EmptyE => "EmptyE"
    | NameE(namex) => "NameE(" ++ stringOfName(namex) ++ ")"
    | AndE(expr1, expr2) =>
      "AndE("
      ++ stringOfExpression(expr1)
      ++ ", "
      ++ stringOfExpression(expr2)
      ++ ")"
    | OrE(expr1, expr2) =>
      "OrE("
      ++ stringOfExpression(expr1)
      ++ ", "
      ++ stringOfExpression(expr2)
      ++ ")"
    | IfE(expr1, expr2, expr3) =>
      "IfE("
      ++ stringOfExpression(expr1) ++ ", "
      ++ stringOfExpression(expr2) ++ ", "
      ++ stringOfExpression(expr3)
      ++ ")"
    | CondE((exprpairlist: list((expression, expression)))) =>
      let stringOfCondExpressionPair: ((expression, expression)) => string = (
        ((expr1, expr2)) =>
          "("
          ++ stringOfExpression(expr1)
          ++ ", "
          ++ stringOfExpression(expr2)
          ++ ")"
      );
      "CondE("
      ++ stringOfAList(exprpairlist, stringOfCondExpressionPair)
      ++ ")";

    | LambdaE(namelist, expr1) =>
      "LambdaE("
      ++ stringOfNameList(namelist)
      ++ ", "
      ++ stringOfExpression(expr1)
      ++ ")"
    | LetE(letexprpairlist, expr1) =>
      let stringOfLetExpressionPair: ((name, expression)) => string = (
        ((name1, expr2)) =>
          "("
          ++ stringOfName(name1)
          ++ ", "
          ++ stringOfExpression(expr2)
          ++ ")"
      );
      "LetE("
      ++ stringOfAList(letexprpairlist, stringOfLetExpressionPair)
      ++ ", "
      ++ stringOfExpression(expr1)
      ++ ")";
    | ApplicationE(expressionlist) =>
      "ApplicationE("
      ++ stringOfAList(expressionlist, stringOfExpression)
      ++ ")"
    };

/* takes in a Definition, def, and returns its string representation */
let stringOfDefinition: definition => string =
  def => {
    let (name1, expr1) = def;
    "(" ++ stringOfName(name1) ++ ", " ++ stringOfExpression(expr1) ++ ")";
  };

/* takes in an abstractProgramPiece, piece, and returns its string representation */
let stringOfAbstractProgramPiece: abstractProgramPiece => string =
  piece =>
    switch (piece) {
    | Expression(expr) => "Expression(" ++ stringOfExpression(expr) ++ ")"
    | Definition(def) => "Definition(" ++ stringOfDefinition(def) ++ ")"
    };

/* takes in an abstractProgram, abstr, and returns its string representation */
let stringOfAbstractProgram: abstractProgram => string =
  abstr => stringOfAList(abstr, stringOfAbstractProgramPiece);

/* takes in a concreteProgramPiece, concrpiece, and returns its string representation */
let rec stringOfConcreteProgramPiece: concreteProgramPiece => string =
  concrpiece =>
    switch (concrpiece) {
    | NumberC(intx) => "NumberC(" ++ string_of_int(intx) ++ ")"
    | SymbolC(stringx) => "SymbolC(" ++ stringx ++ ")"
    | ListC(concreteProgramPieceList) =>
      "ListC("
      ++ stringOfConcreteProgram(concreteProgramPieceList: concreteProgram)
      ++ ")"
    }
/* takes in a concreteProgram, concrprog, and returns its string representation */
and stringOfConcreteProgram: concreteProgram => string =
  concrprog => stringOfAList(concrprog, stringOfConcreteProgramPiece);

/* checkExpectExpression
   Inputs: two expressions, actual and expected, and a string, message
   Output: nothing */
let checkExpectExpression: (expression, expression, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfExpression(expected));
      printRed("actual output: ");
      printRed(stringOfExpression(actual));
    };

/* checkExpectConcreteProgram
   Inputs: two concretePrograms, actual and expected, and a string, message
   Output: nothing */
let checkExpectConcreteProgram:
  (concreteProgram, concreteProgram, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfConcreteProgram(expected));
      printRed("actual output: ");
      printRed(stringOfConcreteProgram(actual));
    };

/* checkExpectConcreteProgramPiece
   Inputs: two concreteProgramPieces, actual and expected, and a string, message
   Output: nothing */
let checkExpectConcreteProgramPiece:
  (concreteProgramPiece, concreteProgramPiece, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfConcreteProgramPiece(expected));
      printRed("actual output: ");
      printRed(stringOfConcreteProgramPiece(actual));
    };

/* checkExpectAbstractProgram
   Inputs: two abstractPrograms, actual and expected, and a string, message
   Output: nothing */
let checkExpectAbstractProgram:
  (abstractProgram, abstractProgram, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfAbstractProgram(expected));
      printRed("actual output: ");
      printRed(stringOfAbstractProgram(actual));
    };

/* checkExpectAbstractProgramPiece
   Inputs: two abstractProgramPieces, actual and expected, and a string, message
   Output: nothing */
let checkExpectAbstractProgramPiece:
  (abstractProgramPiece, abstractProgramPiece, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfAbstractProgramPiece(expected));
      printRed("actual output: ");
      printRed(stringOfAbstractProgramPiece(actual));
    };

/* checkExpectDefinition
   Inputs: two definitions, actual and expected, and a string, message
   Output: nothing */
let checkExpectDefinition: (definition, definition, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfDefinition(expected));
      printRed("actual output: ");
      printRed(stringOfDefinition(actual));
    };

/* checkExpectName
   Inputs: two names, actual and expected, and a string, message
   Output: nothing */
let checkExpectName: (name, name, string) => unit =
  (actual, expected, message) =>
    if (actual == expected) {
      printGreen("ce_Success: " ++ message);
    } else {
      printRed("ce_Fail: " ++ message);
      printRed("expected output: ");
      printRed(stringOfName(expected));
      printRed("actual output: ");
      printRed(stringOfName(actual));
    };

/* checkWithin
   Input: Three floats, input, expected, and within. Input is a given value to be checked.
   Output: nothing */
let checkWithin: (float, float, float) => unit =
  (input, expected, within) =>
    if (abs_float(input -. expected) <= abs_float(within)) {
      printGreen("cwSuccess ");
    } else {
      printRed("cwFail ");
    };

/* checkError
 * Input: a one-argument procedure 'thunk' that returns the thing you want to
 *       test when it's applied to an int
 *       and a string of the error message of the 'failwith' clause in the procedure
 * Output: nothing */
let checkError: (unit => 'a, string) => unit =
  (input, expect) =>
    try (
      {
        ignore(input());
        failwith("Error did not occur");
      }
    ) {
    | Failure(err) when err == expect => printGreen("checkErrorSuccess ")
    | Failure(err) when err == "Error did not occur" =>
      printRed("Error did not occur")
    | Failure(err) =>
      printRed(
        "checkErrorFail. Expected error: "
        ++ expect
        ++ "; Actual error: "
        ++ err,
      )
    };