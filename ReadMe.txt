Directions on how a user would interact with the application:

The user simply needs to input a Racket program. They'll call rackette on a rawProgram, which is a string holding a valid Racket program. In turn, they could anticipate as an output a list containing strings that would be the outcome of different singular portions of input code being accurately assessed, supporting the syntactical terms and commands of evaluation of Racket.


An overview of how all the pieces of your program fit together:

The user will enter a string of a logical Racket code, which is defined as a rawProgram. This will be used by the rackette procedure, which works with read on each section of the rawProgram figures to produce a concreteProgram( a list of concreteProgramPieces).

Next, parse maps the parsePiece function across the concreteProgram; this implies that parsePiece is implemented to each concreteProgramPiece.
For each individual concreteProgramPieces, it either goes to parseExpression or parseDefinition contingent upon if the concreteProgramPieces start with Symbol("define") or not. parseExpression and parseDefinition are employed to each of the concreteProgramPieces, which promptly changes our concreteProgram into an abstractProgram containing abstractProgramPieces. Then, the abstractProgram is passed by the process function, which calls the addDefinition and eval procedures. Process produces in fact a definition, addDefinition is called, adding a binding to the top-level environment unless a binding with an identical title is defined already. If the abstractProgramPiece is an expression, the process will call the eval function, turning the expression into a value.

Lastly, rackette maps the stringOfValue function throughout the list of values, which is the output of running all the items in rawProgram through readAll, parse, and process.


A list of potential bugs or obstacles with your program:

- When a user attempts to define a name more than once in the same let 
expression, our program let's it through when it is supposed to throw an error. 
- The letE evaluates the expression in the wrong order of closures, meaning, 
it will associate the values outside first instead of overriding it with the
inside value. 

There are likely behaviors that could break our code: 
- dividing by zero (ReasonML would give a proper error) or by entering data that is too big to manipulate, but these don't appear to be avoidable.
- cond statements with different output types  - our rackette program does not check for this but Dr.Racket does. 
- and similar cases like these can cause erroneous answers but as mentioned in the handout, we deal with many cases and tried our best to fulfill the failWith cases! 


List of collaborators: 
Wonyoung Lee 
Marie Bordelon


Extra Features: 
We did not implement any extra features on our Rackette project
