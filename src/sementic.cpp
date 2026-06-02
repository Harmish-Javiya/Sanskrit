#include "includes/sementic.hpp"


//-- Symbol Table --

void SymbolTable::enterScope() {}

void SymbolTable::exitScope() {}

bool SymbolTable::declare(SymbolInfo info) {return true;}

SymbolInfo* SymbolTable::lookup(string name) { return nullptr; }

bool SymbolTable::existsInCurrentScope(string name) { return false; }

//-- Helpers --

SansType SemanticAnalyzer::typeFromString(string type) { return SansType::UNKNOWN; }

string SemanticAnalyzer::typeToString(SansType type) {return "unknown"; }

bool SemanticAnalyzer::typesCompatible(SansType a, SansType b) { return true; }

void SemanticAnalyzer::reportError(string message) {}

//-- Dispatchers -- 

void SemanticAnalyzer::visitStatement(ASTNode* node) {}

SansType SemanticAnalyzer::visitExpr(ASTNode* node) { return SansType::UNKNOWN; }

//-- STATEMENT VISITORS --

void SemanticAnalyzer::visitProgram(ProgramNode* node){}
void SemanticAnalyzer::visitBlock(BlockNode* node){}
void SemanticAnalyzer::visitVarBlock(VarDeclNode* node){}
void SemanticAnalyzer::visitAssign(AssignNode* node){}
void SemanticAnalyzer::visitPrint(PrintNode* node){}
void SemanticAnalyzer::visitInput(InputNode* node){}
void SemanticAnalyzer::visitIf(IfNode* node){}
void SemanticAnalyzer::visitWhile(WhileNode* node){}
void SemanticAnalyzer::visitFor(ForNode* node){}
void SemanticAnalyzer::visitSwitch(SwitchNode* node){}
void SemanticAnalyzer::visitReturn(ReturnNode* node){}
void SemanticAnalyzer::visitBreak(BreakNode* node) {}
void SemanticAnalyzer::visitContinue(ContinueNode* node){}
void SemanticAnalyzer::visitExit(ExitNode* node) {}
void SemanticAnalyzer::visitFuncDecl(FuncDeclNode* node) {}
void SemanticAnalyzer::visitFuncCall(FuncCallNode* node) {}

//-- Expression Visitors --

