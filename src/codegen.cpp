#include "includes/codegen.hpp"

void CodeGenerator::emit(string line){
    output << "    " << line <<  "\n";
}

void CodeGenerator::emitLabel(string label) {
    output << label << ":\n";
}

string CodeGenerator::newLabel() {
    return ("label_" + to_string(labelCount++));
}

string CodeGenerator::newStringLabel(){
    return "str_" + to_string(stringCount++);
}

int CodeGenerator::typeSize(string type){
    if(type == "purnank") return 8;
    if(type == "dashamlav") return 8;
    if(type == "aksar") return 1;
    if(type == "tark") return 1;
    return 0;
}

void CodeGenerator::generate(ProgramNode* node) {
    genProgram(node);
}

string CodeGenerator::getOutput(){
    stringstream final;
    final << "global _main\n\n";
    final << "section .data\n";
    final << dataSection.str();
    final << "\nsection .text\n";
    final << output.str();
    return final.str();
}

void CodeGenerator::writeToFile(string filename) {
    ofstream file(filename);
    file << getOutput();
    file.close();
}

void CodeGenerator::genProgram(ProgramNode* node) {
    for (auto* decl : node->declarations){
        if (auto* f = dynamic_cast<FuncDeclNode*>(decl))
            genFuncDecl(f);
    }
}

void CodeGenerator::genFuncDecl(FuncDeclNode* node) {
    variables.clear();
    stackOffset = 0;

    string funcName = (node->name == "mukhya") ? "_main" : node->name;

    emit("push rbp"); //save caller's base pointer
    emit("mov rbp, rsp"); // set our base pointer
    
    // reserve space for local variables
    // we use 128 bytes for now - improve later
    emit("sub rsp, 128"); // move stack pointer down
    
    // handle fucntion parameters
    // System V AMD64 calling convetion:
    // first 6 args come in: rdi,rsi,rdx,rcx,r8,r9
    vector<string> argRegs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    for(int i = 0; i < node->params.size() && i < 6; i++){
        stackOffset += 8;
        variables[node->params[i]->name] = stackOffset;
        emit("mov [rbp-" + to_string(stackOffset) + "], " + argRegs[i]);
    }

    // generate function body
    genBlock(node->body);

    // function epilogue - only for non-main void functions
    if (node->name != "mukhya"){
        emit("mov rsp, rbp"); // restore stack pointer
        emit("pop rbp"); // restore caller's base pointer
        emit("ret");
    }else{
        // main - exit with syscall
        emit("mov rax, 0x2000001");
        emit("mov rdi, 0");
        emit("syscall");
    }

}
void CodeGenerator::genBlock(BlockNode* node) {
    if (!node) return;

    // generate each statement in the block
    for (auto* stmt : node->statements){
        genStatement(stmt);
    }
}

void CodeGenerator::genStatement(ASTNode* node) {
    if (!node) return;

    if (auto* n = dynamic_cast<VarDeclNode*>(node))         genVarDecl(n);
    else if (auto* n = dynamic_cast<AssignNode*>(node))     genAssign(n);
    else if (auto* n = dynamic_cast<PrintNode*>(node))      genPrint(n);
    else if (auto* n = dynamic_cast<InputNode*>(node))      genInput(n);
    else if (auto* n = dynamic_cast<IfNode*>(node))         genIf(n);
    else if (auto* n = dynamic_cast<WhileNode*>(node))      genWhile(n);
    else if (auto* n = dynamic_cast<ForNode*>(node))        genFor(n);
    else if (auto* n = dynamic_cast<ReturnNode*>(node))     genReturn(n);
    else if (auto* n = dynamic_cast<BreakNode*>(node))      genBreak(n);
    else if (auto* n = dynamic_cast<ContinueNode*>(node))   genContinue(n);
    else if (auto* n = dynamic_cast<ExitNode*>(node))       genExit(n);
    else if (auto* n = dynamic_cast<FuncCallNode*>(node))   genFuncCall(n);
}

void CodeGenerator::genExpr(ASTNode* node) {
    if (!node) return;

    if (auto* n = dynamic_cast<LiteralNode*>(node))
        genLiteral(n);
    else if (auto* n = dynamic_cast<IdentifierNode*>(node))
        genIdentifier(n);
    else if (auto* n = dynamic_cast<BinaryOpNode*>(node))
        genBinaryOp(n);
    else if (auto* n = dynamic_cast<UnaryOpNode*>(node))
        genUnaryOp(n);
    else if (auto* n = dynamic_cast<FuncCallNode*>(node))
        genFuncCall(n);
}

void CodeGenerator::genLiteral(LiteralNode* node) {
    if (node->type == TokenType::purnank_lit){
        // integer Literal
        emit("mov rax, " + node->value);
    }
    else if (node->type == TokenType::satya){
        //boolean true = 1
        emit("mov rax, 1");
    }
    else if (node->type == TokenType::asatya){
        //boolean true = 0
        emit("mov rax, 0");
    }
    else if (node->type == TokenType::aksarmala_lit){
        //string literal - need to store in .data and load address
        string label = newStringLabel();
        dataSection << "   " << label << " db \"" << node->value << "\", 0\n";
        emit("lea rax, [rel " + label + "]");
    }
}

void CodeGenerator::genIdentifier(IdentifierNode* node) {
    // look up where this variable lives on stack
    auto it = variables.find(node->name);
    if (it == variables.end()){
        cerr << "[CodeGen Error] Undefined variable: " << node->name << endl;
        return;
    }

    int offset = it->second;

    // load value from stack into rax
    emit("mov rax, [rbp-" + to_string(offset) + "]");
}

void CodeGenerator::genVarDecl(VarDeclNode* node) {
    // calculate stack space needed
    int size = typeSize(node->type);
    stackOffset += size;

    // remember where the variable lives
    variables[node->varName] = stackOffset;
    
    // if there's an initializer, evaluate and store it
    if (node->initializer) {
        // evaluate expression - result goes into rax
        genExpr(node->initializer);
        //store rax value onto stack at variable's location
        emit("mov [rbp-" + to_string(stackOffset) + "], rax");
    } else {
        // no initalizer - deafult to 0
        emit("mov qword [rbp-" + to_string(stackOffset) + "], 0");
    }
}

void CodeGenerator::genAssign(AssignNode* node) {}
void CodeGenerator::genPrint(PrintNode* node) {}
void CodeGenerator::genInput(InputNode* node) {}
void CodeGenerator::genIf(IfNode* node) {}
void CodeGenerator::genWhile(WhileNode* node) {}
void CodeGenerator::genFor(ForNode* node) {}
void CodeGenerator::genReturn(ReturnNode* node) {}
void CodeGenerator::genBreak(BreakNode* node) {}
void CodeGenerator::genContinue(ContinueNode* node) {}
void CodeGenerator::genExit(ExitNode* node) {}
void CodeGenerator::genFuncCall(FuncCallNode* node) {}
void CodeGenerator::genArrayLiteral(ArrayLiteralNode* node) {}
void CodeGenerator::genArrayAccess(ArrayAccessNode* node) {}
void CodeGenerator::genBinaryOp(BinaryOpNode* node) {}
void CodeGenerator::genUnaryOp(UnaryOpNode* node) {}