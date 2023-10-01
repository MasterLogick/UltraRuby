cmake_minimum_required(VERSION 3.26)
project(ultra_ruby)

find_package(LLVM REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(CMAKE_CXX_STANDARD 20)

add_executable(ultra_ruby src/main.cpp
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/lexer/TokenType.h
        src/lexer/LexerInput.h
        src/ast/Statement.h
        src/ast/Variable.h
        src/ast/BinaryOperation.h
        src/ast/Call.h
        src/ast/FunctionDef.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/ir/CodeGenerator.cpp
        src/ir/CodeGenerator.h
        src/ast/OperationType.h
        src/lexer/input/StringLexerInput.cpp
        src/lexer/input/StringLexerInput.h
        src/ast/StatementType.h
        src/lexer/input/FileLexerInput.cpp
        src/lexer/input/FileLexerInput.h
        src/lexer/TokenType.cpp
        src/ast/ClassDef.h
        src/ast/UnaryOperation.h
        src/ast/Assignment.h
        src/ast/ModuleDef.h
        src/ast/Block.h
        src/ast/If.h
        src/ast/While.h
        src/ast/String.h
        src/ast/BoolConst.h
        src/ast/Return.h
        src/ast/Next.h
        src/ast/Break.h
        src/ast/Redo.h
        src/ast/Retry.h
        src/ast/CallArgs.h
        src/ast/Yield.h
        src/ast/YieldSelf.h
        src/ast/Case.h
        src/ast/For.h
        src/ast/ClassInstanceDef.h
        src/ast/Array.h
        src/ast/FuncDefArgs.h
        src/ast/IntegerConst.h
        src/ast/Hash.h
        src/lexer/TokenQueue.h
        src/lexer/Token.h
        src/lexer/TokenQueue.cpp
        src/ast/Symbol.h
        src/ast/ExceptionalBlock.h
        src/ir/Scope.cpp
        src/ir/Scope.h
        src/lang/Class.cpp
        src/lang/Class.h
        src/lang/HashInternal.cpp
        src/lang/HashInternal.h
        src/lang/Object.cpp
        src/lang/Object.h
        src/lang/FunctionInfo.cpp
        src/lang/FunctionInfo.h
        src/lang/String.cpp
        src/lang/String.h
        src/lang/Heap.cpp
        src/lang/Heap.h
        src/lang/BasicClasses.cpp
        src/lang/BasicClasses.h
        src/lang/Symbol.cpp
        src/lang/Symbol.h
        src/ast/AST.h
        src/ast/ASTClassDecls.h
        src/lang/Array.cpp
        src/lang/Array.h
        src/lang/ObjectVector.cpp
        src/lang/ObjectVector.h
        src/ast/FuncDefArgs.cpp
        src/lang/Hash.cpp
        src/lang/Hash.h
)
message(${LLVM_AVAILABLE_LIBS})
llvm_map_components_to_libnames(llvm_libs core orcjit native)
target_link_libraries(ultra_ruby ${llvm_libs})