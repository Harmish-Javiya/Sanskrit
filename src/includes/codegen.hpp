#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include "ast.hpp"

using namespace std;

class CodeGenerator {
private:
    stringstream output;
    stringstream dataSection;
    unordered_map<string, int> variables;
    unordered_map<string, string> variableTypes;
    unordered_map<string, bool> isFloatVar;
    int calculateStackSize(BlockNode* body, vector<ParamNode*>& params);
    int stackOffset = 0;
    int labelCount = 0;
    string currentFunction;
    int stringCount = 0;


    //Utilities
    void emit(string line);
    void emitLabel(string label);
    string newLabel();
    string newStringLabel();
    int typeSize(string type);

    //Generators
    void genProgram(ProgramNode* node);
    void genFuncDecl(FuncDeclNode* node);
    void genBlock(BlockNode* node);
    void genStatement(ASTNode* node);
    void genVarDecl(VarDeclNode* node);
    void genAssign(AssignNode* node);
    void genPrint(PrintNode* node);
    void genInput(InputNode* node);
    void genIf(IfNode* node);
    void genWhile(WhileNode* node);
    void genFor(ForNode* node);
    void genReturn(ReturnNode* node);
    void genBreak(BreakNode* node);
    void genContinue(ContinueNode* node);
    void genExit(ExitNode* node);
    void genFuncCall(FuncCallNode* node);
    void genArrayLiteral(ArrayLiteralNode* node);
    void genArrayAccess(ArrayAccessNode* node);
    void genSwitch(SwitchNode* node);

    //Expressions
    void genExpr(ASTNode* node);
    void genBinaryOp(BinaryOpNode* node);
    void genUnaryOp(UnaryOpNode* node);
    void genLiteral(LiteralNode* node);
    void genIdentifier(IdentifierNode* node);
    void genNewExpr(NewNode* node);
    void genFreeStmt(FreeNode* node);

    //Libraries
    void emitMathLibrary();
    
    // helper
    bool isFloatNode(ASTNode* node);
public:
    void generate(ProgramNode* node);
    string getOutput();
    void writeToFile(string filename);
};
