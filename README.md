# SLISP-Interpreter
SLISP Interpreter is a C++ application designed to interpret and execute code written in the SLISP language, a minimalist dialect of Lisp. This project serves as an educational tool to understand the inner workings of a Lisp interpreter and the principles of language design.

Core Components:
1.The interpreter is structured into several key modules:
2.Expression Handling: Manages the representation and evaluation of SLISP expressions.
3.Environment Management: Implements the symbol table and scope management for variable bindings.
4.Interpreter Logic: Contains the main evaluation loop and dispatch mechanisms for different expression types.
5.Error Handling: Provides mechanisms for detecting and reporting semantic errors during interpretation.

File Structure - the repository includes the following files:
1.expression.cpp/hpp: Defines the data structures for SLISP expressions.
2.environment.cpp/hpp: Implements the environment model for variable storage and lookup.
3.interpreter.cpp/hpp: Contains the core logic for evaluating SLISP expressions.
4.interpreter_semantic_error.hpp: Defines custom error types for semantic issues.
5.main_window.cpp/hpp: Provides the graphical user interface components for user interaction.
6.canvas_widget.cpp/hpp: Implements the visual components for displaying SLISP code and results.
7.message_widget.cpp/hpp: Handles messaging and notifications within the interface.
8.catch.hpp: Includes the Catch2 testing framework for unit testing.

Ways to run the progra:
1. "./slisp -e (+ (2 (3)))"  through commands, the program would return 6. 
2. "./slisp file.slp" through .slp code file
3. "./sldraw" for QT GUI 

