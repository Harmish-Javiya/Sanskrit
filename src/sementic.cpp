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

        if(auto* f = dynamic_cast<FuncDeclNode*>(decl)){
            visitFuncDecl(f);
        }else{
            visitStatement(decl);
        }
    }
    SymbolTable.exitScope();
}

//-- Dispatchers -- 

void SemanticAnalyzer::visitStatement(ASTNode* node) {
    if (!node) {return;}

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
    else if (auto* n = dynamic_cast<SwitchNode*>(node))    visitSwitch(n);
    else if (auto* n = dynamic_cast<NewNode*>(node)) visitNewExpr(n);
    else if (auto* n = dynamic_cast<FreeNode*>(node)) visitFreeStmt(n);
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
    if (auto* n = dynamic_cast<FuncCallNode*>(node)) { 
        visitFuncCall(n); 
        SymbolInfo* info = SymbolTable.lookup(n->funcName);

        if(info) {
            return info->returnType;
        }
        return SansType::UNKNOWN; 
    }
    if (auto* n = dynamic_cast<ArrayLiteralNode*>(node)) return visitArrayLiteral(n);
    if (auto* n = dynamic_cast<ArrayAccessNode*>(node)) return visitArrayAccess(n);
    if (auto* n = dynamic_cast<NewNode*>(node)) return visitNewExpr(n);
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

void SemanticAnalyzer::visitAssign(AssignNode* node){
    
    SymbolInfo* info = SymbolTable.lookup(node->name);

    if(!info){
        reportError("Undeclared variable: " + node->name);
        return;
    }

    if(info->isConst){
        reportError("Cannot assign to const variable: " + node->name);
        return;
    }

    SansType valueType = visitExpr(node->value);
    if(!typesCompatible(info->type, valueType)){
        reportError("Type mismatch in assignment to '" + node->name +"': expected" + typeToString(info->type) + " but got " + typeToString(valueType));
    }
}

void SemanticAnalyzer::visitPrint(PrintNode* node){
    visitExpr(node->value);
}

void SemanticAnalyzer::visitInput(InputNode* node){
    SymbolInfo* info = SymbolTable.lookup(node->name);
    if (!info) {
        reportError("Undeclared variable: " + node->name);
        return;
    }


    if (info->isConst) {
        reportError("Cannot take input into const variable: " + node->name);
        return;
    }
}

void SemanticAnalyzer::visitIf(IfNode* node){
    SansType condType = visitExpr(node->condition);

    if(condType != SansType::BOOL && condType != SansType::UNKNOWN){
        reportError("yadi condition must be tark (bool)");
    }

    visitBlock(node->thenBlock);
    if(node->elseBlock){
        visitBlock(node->thenBlock);
    }
}

void SemanticAnalyzer::visitWhile(WhileNode* node){

    SansType condType = visitExpr(node->condition);

    if (condType != SansType::BOOL && condType != SansType::UNKNOWN){
        reportError("yadi condition must be tark (bool)");
    }

    visitBlock(node->body);
}

void SemanticAnalyzer::visitFor(ForNode* node){
    SymbolTable.enterScope();

    if(node->isRange){
        SansType fromType = visitExpr(node->from);
        SansType toType = visitExpr(node->to);

        if(fromType != SansType::INT)
            reportError("For loop range start must be purnank");
        if(toType != SansType::INT)
            reportError("For loop range start must be purnank");
        
        SymbolInfo info;
        info.name = node->var;
        info.type = SansType::INT;
        info.isConst = false;
        info.isFunction = false;

        SymbolTable.declare(info);
    } else {

        if (node->from) visitStatement(node->from);
        if (node->to) visitExpr(node->to);
        if (node->update) visitExpr(node->update);
    }

    visitBlock(node->body);
    SymbolTable.exitScope();
}

void SemanticAnalyzer::visitSwitch(SwitchNode* node){
    SansType exprType = visitExpr(node->expression);

    for(auto* caseNode: node->cases){
        if(caseNode->value != nullptr){

            SansType caseType = visitExpr(caseNode->value);
            if (!typesCompatible(exprType, caseType)){
                reportError("Case Type are diffrent in vikalp: expected : " + typeToString(exprType) + " but got " + typeToString(caseType));
            }    
        }
        
        visitBlock(caseNode->body);
    }

}

void SemanticAnalyzer::visitReturn(ReturnNode* node){
    SansType expectedType = typeFromString(currentFunctionReturn);

    if(node->value == nullptr){
        if(expectedType != SansType::VOID){
            reportError("Expected return value of type: " + currentFunctionReturn );
        }
        return;
    }

    SansType returnType = visitExpr(node->value);

    if(!typesCompatible(expectedType, returnType)) {
        reportError("Return Type mismatch: expected " + typeToString(expectedType) + " but got " + typeToString(returnType));
    }
}

void SemanticAnalyzer::visitBreak(BreakNode* node) {
    // nothing to check — just valid inside loops/switch
    // loop validation can be added later
}

void SemanticAnalyzer::visitContinue(ContinueNode* node){
    // nothing to check — just valid inside loops
}


void SemanticAnalyzer::visitExit(ExitNode* node) {
    if (node->code != nullptr){
        SansType codeType = visitExpr(node->code);
        if(codeType != SansType::INT){
            reportError(" nirgachh exit code must be purnank (int) ");
        }
    }
}

void SemanticAnalyzer::visitFuncDecl(FuncDeclNode* node) {
    SymbolInfo funcInfo;
    funcInfo.name = node->name;
    funcInfo.type = typeFromString(node->returnType);
    funcInfo.isFunction = true;
    funcInfo.returnType = typeFromString(node->returnType);

    for (auto* param: node->params){
        funcInfo.paramTypes.push_back(typeFromString(param->type));
    }
    SymbolTable.declare(funcInfo);

    SymbolTable.enterScope();
    for(auto* param: node->params){
        SymbolInfo info;
        info.name = param->name;
        info.type = typeFromString(param->type);
        info.isConst = false;
        info.isFunction = false;
        SymbolTable.declare(info);
    }

    string previousReturn = currentFunctionReturn;
    currentFunctionReturn = node->returnType;

    for(auto* stmt : node->body->statements)
        visitStatement(stmt);

    currentFunctionReturn = previousReturn; 
    SymbolTable.exitScope();
}

void SemanticAnalyzer::visitFuncCall(FuncCallNode* node) {
    SymbolInfo* info = SymbolTable.lookup(node->funcName);
    
    if (!info){
        reportError("Undeclared function: " + node->funcName);
        return;
    }

    if (!info->isFunction) {
        reportError(node->funcName + " is not a function ");
        return;
    }

    if (node->args.size() != info->paramTypes.size()) {
        reportError("Function '" + node->funcName + "' expects " + to_string(info->paramTypes.size()) + " arguments but got " + to_string(node->args.size()));
        return;
    }

    for (int i = 0; i < node->args.size() ; i++){
        SansType argType = visitExpr(node->args[i]);

        if(!typesCompatible(argType, info->paramTypes[i])){
            reportError("Argumets " + to_string(i+1) + " of '" + node->funcName + "': expected " + typeToString(info->paramTypes[i]) + " but got " + typeToString(argType));
        }
    }
}

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
    } else {
        return left;
    }

    if (left == SansType::FLOAT || right == SansType::FLOAT)
        return SansType::FLOAT;

    return left;

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

SansType SemanticAnalyzer::visitArrayLiteral(ArrayLiteralNode* node) { 
    if(node->elements.empty())
        return SansType::ARRAY;

    SansType firstType = visitExpr(node->elements[0]);

    for (int i = 0; i < node->elements.size(); i++){
        SansType elemType = visitExpr(node->elements[i]);
        if (!typesCompatible(firstType, elemType)){
            reportError("Array elements must be same type: element " +
                to_string(i) + " expected " + typeToString(firstType) +
                " but got " + typeToString(elemType));
        }
    }

    return SansType::ARRAY; 
}

SansType SemanticAnalyzer::visitArrayAccess(ArrayAccessNode* node) { 
    SymbolInfo* info = SymbolTable.lookup(node->name);
    if(!info){
        reportError("Undeclared array: " + node->name);
        return SansType::UNKNOWN;
    }

    if (info->type != SansType::ARRAY){
        reportError(node->name + " is not an array");
        return SansType::UNKNOWN;
    }

    SansType indexType = visitExpr(node->index);
    if (indexType != SansType::INT) {
        reportError("Array index must be purnank (int)");
    }

    if (node->value != nullptr) {
        visitExpr(node->value);
    }

    return SansType::INT; // element type — improve later with sarani_purnank
}


SansType SemanticAnalyzer::visitNewExpr(NewNode* node){
    SansType type = typeFromString(node->type);

    if(type == SansType::UNKNOWN){
        reportError("Undeclared variable in nava: " + node->type);
    }
    return type;
}

void SemanticAnalyzer::visitFreeStmt(FreeNode* node){
    SymbolInfo* info = SymbolTable.lookup(node->name);

    if (!info){
        reportError("Undeclared variable in mukta: " + node->name);
        return;
    }

    if(info->isConst){
        reportError("Cannot free const variable: " + node->name);
    }
}

bool SemanticAnalyzer::hasErrors() { return !errors.empty(); }

void SemanticAnalyzer::printErrors() {
    for (auto& e : errors)
        cerr << "[Semantic Error]" << e << endl;
}
