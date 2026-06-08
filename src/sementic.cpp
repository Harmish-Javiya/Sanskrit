#include "includes/sementic.hpp"


//-- Symbol Table --

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    if(!scopes.empty())
        scopes.pop_back();
}

bool SymbolTable::existsInCurrentScope(string name) { 
    if (scopes.empty()) return false;
    return scopes.back().count(name) > 0;
}

bool SymbolTable::declare(SymbolInfo info) {
    if (scopes.empty()) return false;
    if(existsInCurrentScope(info.name)) return false; 
    scopes.back()[info.name] = info;
    return true;
}

SymbolInfo* SymbolTable::lookup(string name) { 

    for (int i = scopes.size() - 1; i >= 0; i--) { 
        auto it = scopes[i].find(name);
        if(it != scopes[i].end())
            return &it->second;
    }
    return nullptr; 
}


//-- Helpers --

SansType SemanticAnalyzer::typeFromString(string type) { 
    
    if(type == "purnank") return SansType::INT;
    if(type == "dashamlav") return SansType::FLOAT;
    if(type == "aksarmala") return SansType::STRING;
    if(type == "aksar") return SansType::CHAR;
    if(type == "tark") return SansType::BOOL;
    if(type == "sunya") return SansType::VOID;
    if(type.substr(0,6) == "sarani") return SansType::ARRAY;

    return SansType::UNKNOWN; 
}

string SemanticAnalyzer::typeToString(SansType type) {
    switch (type) {
        case SansType::INT:      return "purnank";
        case SansType::FLOAT:    return "dashamlav";
        case SansType::STRING:   return "aksarmala";
        case SansType::CHAR:     return "aksar";
        case SansType::BOOL:     return "tark";
        case SansType::VOID:     return "sunya";
        case SansType::ARRAY:    return "sarani";
        case SansType::FUNCTION: return "karma";
        default:                 return "unknown";
    }
}

bool SemanticAnalyzer::typesCompatible(SansType a, SansType b) { 
    if(a == b) return true;
    
    if((a == SansType::INT && b == SansType::FLOAT) || 
    (a == SansType::FLOAT && b == SansType::INT))
       return true;

    return false; 
}

void SemanticAnalyzer::reportError(string message) {
    errors.push_back(message);
}

void SemanticAnalyzer::analyze(ProgramNode* node) {
    visitProgram(node);
}

void SemanticAnalyzer::visitProgram(ProgramNode* node){
    SymbolTable.enterScope();
    for(auto* decl : node->declarations) {
        visitStatement(decl);
    }
    SymbolTable.exitScope();
}

//-- Dispatchers -- 

void SemanticAnalyzer::visitStatement(ASTNode* node) {
    if (!node) return;

    if (auto* n = dynamic_cast<VarDeclNode*>(node)) visitVarDecl(n);
    else if (auto* n = dynamic_cast<AssignNode*>(node)) visitAssign(n);
    else if (auto* n = dynamic_cast<PrintNode*>(node)) visitPrint(n);
    else if (auto* n = dynamic_cast<InputNode*>(node)) visitInput(n);
    else if (auto* n = dynamic_cast<IfNode*>(node)) visitIf(n);
    else if (auto* n = dynamic_cast<WhileNode*>(node)) visitWhile(n);
    else if (auto* n = dynamic_cast<ForNode*>(node)) visitFor(n);
    else if (auto* n = dynamic_cast<SwitchNode*>(node)) visitSwitch(n);
    else if (auto* n = dynamic_cast<ReturnNode*>(node)) visitReturn(n);
    else if (auto* n = dynamic_cast<BreakNode*>(node)) visitBreak(n);
    else if (auto* n = dynamic_cast<ContinueNode*>(node)) visitContinue(n);
    else if (auto* n = dynamic_cast<ExitNode*>(node)) visitExit(n);
    else if (auto* n = dynamic_cast<FuncDeclNode*>(node)) visitFuncDecl(n);
    else if (auto* n = dynamic_cast<FuncCallNode*>(node)) visitFuncCall(n);
}

void SemanticAnalyzer::visitBlock(BlockNode* node){
    if(!node) return;
    SymbolTable.enterScope();
    for(auto* stmt : node->statements)
        visitStatement(stmt);
    SymbolTable.exitScope();
}

SansType SemanticAnalyzer::visitExpr(ASTNode* node) { 
    if (!node) return SansType::VOID;

    if (auto* n = dynamic_cast<LiteralNode*>(node)) return visitLiteral(n);
    if (auto* n = dynamic_cast<IdentifierNode*>(node)) return visitIdentifier(n);
    if (auto* n = dynamic_cast<BinaryOpNode*>(node)) return visitBinaryOp(n);
    if (auto* n = dynamic_cast<UnaryOpNode*>(node)) return visitUnaryOp(n);
    if (auto* n = dynamic_cast<FuncCallNode*>(node)) { visitFuncCall(n); return SansType::UNKNOWN; }
    if (auto* n = dynamic_cast<ArrayLiteralNode*>(node)) return visitArrayLiteral(n);
    if (auto* n = dynamic_cast<ArrayAccessNode*>(node)) return visitArrayAccess(n);

    return SansType::UNKNOWN;
}

//-- STATEMENT VISITORS --
void SemanticAnalyzer::visitVarDecl(VarDeclNode* node){
    SansType declaredType = typeFromString(node->type);

    if(node->initializer) {
        SansType initType = visitExpr(node->initializer);
        if(!typesCompatible(declaredType, initType)){
            reportError("Type mismatch in declaration of ' " + node->varName + "' : expected " + typeToString(declaredType) + " but got " + typeToString(initType));
        }
    }

    if (SymbolTable.existsInCurrentScope(node->varName)) {
        reportError("Variable already declared: " + node->varName);
        return;
    }

    SymbolInfo info;
    info.name = node->varName;
    info.type = declaredType;
    info.isConst = node->isConst;
    info.isFunction = false;
    SymbolTable.declare(info);
}

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
SansType SemanticAnalyzer::visitBinaryOp(BinaryOpNode* node) { 
    SansType left = visitExpr(node->left);
    SansType right = visitExpr(node->right);

    if (node->op == TokenType::adhik || 
        node->op == TokenType::nyun || 
        node->op == TokenType::sam || 
        node->op == TokenType::asam || 
        node->op == TokenType::adhik_sam || 
        node->op == TokenType::nyun_sam || 
        node->op == TokenType::sym_adhik || 
        node->op == TokenType::sym_nyun || 
        node->op == TokenType::sym_sam || 
        node->op == TokenType::sym_asam || 
        node->op == TokenType::sym_adhik_sam || 
        node->op == TokenType::sym_nyun_sam ) {
        if (!typesCompatible(left,right))    
            reportError("Type mismatch in comparison");
        return SansType::BOOL;
    }
    
    if (node->op == TokenType::tatha ||
        node->op == TokenType::va) {
        
            if(left != SansType::BOOL)
                reportError("Left side of logical operator must be tark");
            if(right != SansType::BOOL)
                reportError("Left side of logical operator must be tark");
            return SansType::BOOL;
    }

    if(!typesCompatible(left, right)) {
        reportError("Type mismatch in arithmetic operation: " + typeToString(left) + " and " + typeToString(right));
        return SansType::UNKNOWN;
    }

    if (left == SansType::FLOAT || right == SansType::FLOAT)
        return SansType::FLOAT;
    
    return SansType::UNKNOWN;
}

SansType SemanticAnalyzer::visitUnaryOp(UnaryOpNode* node) {
    SansType operandType = visitExpr(node->operand);

    if (node->op == TokenType::na) {
        if (operandType != SansType::BOOL)
            reportError(" '!' operator requires tark(bool) operand");
        return operandType;
    }

    if(node->op == TokenType::viyoga) {
        if (operandType != SansType::INT && operandType != SansType::FLOAT)
            reportError(" '-' operator requires numeric operand");
        return operandType;    
    }

    if(node->op == TokenType::vriddhi || node->op == TokenType::kshaya) {
        if (operandType != SansType::INT)
            reportError(" '++/--' operator requires purnank (int) operand");
        return SansType::INT;
    }

    return operandType;
}

SansType SemanticAnalyzer::visitLiteral(LiteralNode* node) { 
    
    switch (node->type)
    {
        case TokenType::purnank_lit: return SansType::INT;
        case TokenType::dashamlav_lit: return SansType::FLOAT;
        case TokenType::aksarmala_lit: return SansType::STRING;
        case TokenType::aksar_lit: return SansType::CHAR;
        case TokenType::satya:
        case TokenType::asatya: return SansType::BOOL;
        default: return SansType::UNKNOWN;
    } 
}

SansType SemanticAnalyzer::visitIdentifier(IdentifierNode* node) { 
    SymbolInfo* info = SymbolTable.lookup(node->name);
    if(!info){
        reportError("Undeclared variable: " + node->name);
        return SansType::UNKNOWN;
    }
    return info->type;
}

SansType SemanticAnalyzer::visitArrayLiteral(ArrayLiteralNode* node) { return SansType::UNKNOWN; }

SansType SemanticAnalyzer::visitArrayAccess(ArrayAccessNode* node) { return SansType::UNKNOWN; }


bool SemanticAnalyzer::hasErrors() { return !errors.empty(); }

void SemanticAnalyzer::printErrors() {
    for (auto& e : errors)
        cerr << "[Semantic Error]" << e << endl;
}
