#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <iostream>
#include "ast.hpp"

using namespace std;

enum class SansType{
    INT,
    FLOAT,
    STRING,
    CHAR,
    BOOL,
    VOID,
    ARRAY,
    FUNCTION,
    UNKNOWN
};

struct SymbolInfo {
    string name;
    SansType type;
    bool isConst;
    bool isFunction;
    vector<SansType> paramTypes;
    SansType returnType;
};

class SymbolTable{
    private:
        vector<unordered_map<string, SymbolInfo>> scopes;

    public:
        void enterScope();
        void exitScope();
        bool declare(SymbolInfo info);
        SymbolInfo* lookup(string name);
        bool existsInCurrentScope(string name);
};

class SemanticAnalyzer{
    private:
        SymbolTable SymbolTable;
        string currentFunctionReturn;
        vector<string> errors;

        SansType typeFromString(string type);
        string typeToString(SansType type);
        bool typesCompatible(SansType a, SansType b);
        void reportError(string message);

        void visitProgram(ProgramNode* node);
        void visitBlock(BlockNode* node);
        void visitVarDecl(VarDeclNode* node);
        void visitVarBlock(VarDeclNode* node);
        void visitAssign(AssignNode* node);
        void visitPrint(PrintNode* node);
        void visitInput(InputNode* node);
        void visitIf(IfNode* node);
        void visitWhile(WhileNode* node);
        void visitFor(ForNode* node);
        void visitSwitch(SwitchNode* node);
        void visitReturn(ReturnNode* node);
        void visitBreak(BreakNode* node);
        void visitContinue(ContinueNode* node);
        void visitExit(ExitNode* node);
        void visitFuncDecl(FuncDeclNode* node);
        void visitFuncCall(FuncCallNode* node);
        void visitFreeStmt(FreeNode* node);

        SansType visitExpr(ASTNode* node);
        SansType visitBinaryOp(BinaryOpNode* node);
        SansType visitUnaryOp(UnaryOpNode* node);
        SansType visitLiteral(LiteralNode* node);
        SansType visitIdentifier(IdentifierNode* node);
        SansType visitArrayLiteral(ArrayLiteralNode* node);
        SansType visitArrayAccess(ArrayAccessNode* node);
        SansType visitNewExpr(NewNode* node);
        
        void visitStatement(ASTNode* node);

    public:
        SemanticAnalyzer() : currentFunctionReturn("sunya") {}
        void analyze(ProgramNode* node);
        bool hasErrors();
        void printErrors();
};
