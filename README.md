# The Wick Programming Language

## About This Project
Wick is a novel programming language created for the purpose of gaining
experience in programming language and interpreter design; two subjects that
have become increasingly neglected in computer science programs. Additionally,
two subjects that were captivating to our team and seemed like fun subjects for
a project. 

Wick is presently a simple, dynamically-typed, and multi-paradigm language,
supporting. First class functions (here called subroutines) are supported as
well as object-oriented programming via prototypes. A manual for writing Wick
programs and some additional details of the project can be found on the
interpreter's GitHub repository
(https://github.com/Happy-Hour-Capstone-Team/wick-interpreter).

Also provided is an online IDE (currently not hosted on a server, but can be run
locally) seen on our teams GitHub page
(https://github.com/Happy-Hour-Capstone-Team/wick-ide).

## About Us
This project was developed by The Happy Hour team from Arkansas Tech University
through Capstone I and Capstone II from 2023 to 2024. 

Our members are:
- Braden Pierce
- Lisset Luna
- Evelin Cerros-Patricio
- Evan Keathley

## How The Wick Interpreter Works
The interpreter is formed out of three main components: the scanner, parser,
and, well, *interpreter*. 

When the interpreter is executed, it is passed a Wick file name (just a text
file with a .wick extension). This file is read, and its contents read into a
string before being passed into the scanner. The scanner's job is to break the
contents of the file into different tokens (similar to words in natural
language). These tokens contain a lexeme representing the raw text of the token,
a type (whether it's a number, string, keyword, or something else), and some
information for error reporting like its line and column number. Additionally,
whitespace, comments, and other unnecessary symbols are disregarded at this
stage. The scanner essentially breaks the program into a list of words depending
on some regular grammar. 

When the scanner is done, the list of tokens is passed into the parser. The
parser breaks the tokens into grammatical concepts like subroutine definitions,
prototype definitions, expressions, and so on. This is presently accomplished
using a recursive-descent parser that translates the context-free grammar for
Wick directly into class methods. These methods return nodes and together form a
parse tree. These nodes can either be expression nodes (implying that they can
be evaluated to return some value) or statement nodes (implying they impart some
state in the program, like change a variables value). Both these nodes define an
interface for visitors to perform operations on them. Some basic semantic
analysis is also performed here (like making sure return is only used within an
appropriate context or similar). 

If an error is found during either of these stages, the error reporter passed
along is informed. Scanning/parsing will continue as long as possible to allow
the accruement of all errors in the program at once, but execution will be
preempted. 

Finally, the program is run via the tree-traversal interpreter. This does as it
sounds, iterating through the statement nodes produced by the parser and
traversing downward, applying the expected behavior associated with each node.
For instance, the interpreter may come along a statement variable node implying
a variable is being declared. If an initializer exists, it is evaluated and its
value associated with the variable token in the working environment. The
interpreter also initializes the global environment with all native subroutines
and constants when started, in the same manner as if a user had defined them. 

## Additional Notes
Wick is a strongly-typed language in that operations between inappropriate data
types will result in an error. This is in contrast to weakly-typed languages
that will attempt "something" or another to fulfill the operation, regardless of
how unintuitive the behavior is.

Wick is a dynamically-typed language in that the data types of operands are
checked for validity at runtime rather than at compile time. This is in contrast
to statically-typed languages where such issues can be caught before the program
is run. Wick does not necessarily *have* to be a dynamically-typed language, but
was made such for simplicity-sake. 

There is some ambiguity with the terms "compiler" and "interpreter." Simply put,
compiler usually implies an additional degree of translation from the
programming language (usually to an intermediary language or machine code with
optimizations). Because this interpreter does essentially the bare minimum for
translation, "interpreter" is usually the preferred language though "compiler"
is occasionally used. 

## Future Work
While the core functionality for both the language, interpreter, IDE, and
documentation are present, there is still much that could be done to make Wick
and its associated technologies more complete. This section breaks up the
most-critical missing features into the separate categories of the project.
Technically, some of these changes may break previously existing Wick programs:
a concern that will be fully reckoned with if Wick ever experiences legitimate
use. 

### The Language
- Experiments with static-typing.
- Native support for collections like lists. 
- Native support for enumerated values. 
- Switch statements.
- File I/O.
- Exception and error-handling.
- Multi-file Wick projects or a package manager.
- A standard library. 
- Formatted printing, inheriting constructors, assignment operators (++, +=, -=,
  and so on), parallel programming, and more...

### The Interpreter
- Greatly improve error-reporting throughout the project.
- Formalize much of the testing for the components of the interpreter.
- Ensure the robustness of garbage collection (currently accomplished naively
  with reference-counting pointers). 
- Add a semantic analysis step to check for errors and potential issues (also
  would be required for static typing). 
- Add an optimization step to interpreter to modify user programs to be more
  performative.
- Transition away from tree-traversal interpretation.
- Also, maybe come up with a name for this part of the project besides "Wick
  Interpreter" would be nice...

### The IDE
- Allow input from the terminal. 
- Allow multi-file projects.
- Add the ability to save and upload Wick files.
- Keyword text highlighting for improved readability. 
- A formatting button.
- Some portal to a repository of resources for using Wick and the IDE.
  - As well as those resources being spruced up and in text and video formats. 
- And, of course, actually having it be hosted...
- Also, maybe come up with a name for this part of the project besides "Wick
  IDE" would be nice...