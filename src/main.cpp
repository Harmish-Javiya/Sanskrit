#include "includes/lexer.hpp"
#include "includes/parser.hpp"
#include "includes/sementic.hpp"
#include "includes/codegen.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// void printAST(ASTNode* node, int indent = 0) {
//     string pad(indent * 2, ' ');

//     if (!node) {
//         cout << pad << "nullptr" << endl;
//         return;
//     }

//     if (auto* p = dynamic_cast<ProgramNode*>(node)) {
//         cout << pad << "[ProgramNode]" << endl;
//         for (auto* decl : p->declarations)
//             printAST(decl, indent + 1);
//     }
//     else if (auto* v = dynamic_cast<VarDeclNode*>(node)) {
//         cout << pad << "[VarDecl] " << v->varName 
//              << " : " << v->type 
//              << (v->isConst ? " (const)" : "") << endl;
//         if (v->initializer) {
//             cout << pad << "  initializer:" << endl;
//             printAST(v->initializer, indent + 2);
//         }
//     }
//     else if (auto* l = dynamic_cast<LiteralNode*>(node)) {
//         cout << pad << "[Literal] " << l->value << endl;
//     }
//     else if (auto* i = dynamic_cast<IdentifierNode*>(node)) {
//         cout << pad << "[Identifier] " << i->name << endl;
//     }
//     else if (auto* p = dynamic_cast<PrintNode*>(node)) {
//         cout << pad << "[PrintNode]" << endl;
//         printAST(p->value, indent + 1);
//     }
//     else if (auto* a = dynamic_cast<AssignNode*>(node)) {
//         cout << pad << "[AssignNode] " << a->name 
//              << " " << tokenTypeToString(a->op) << endl;
//         printAST(a->value, indent + 1);
//     }
//     else if (auto* i = dynamic_cast<InputNode*>(node)) {
//         cout << pad << "[InputNode] " << i->name << endl;
//     }else if (auto* b = dynamic_cast<BlockNode*>(node)) {
//         cout << pad << "[BlockNode]" << endl;
//         for (auto* stmt : b->statements)
//             printAST(stmt, indent + 1);
//     }
//     else if (auto* i = dynamic_cast<IfNode*>(node)) {
//         cout << pad << "[IfNode]" << endl;
//         cout << pad << "  condition:" << endl;
//         printAST(i->condition, indent + 2);
//         cout << pad << "  then:" << endl;
//         printAST(i->thenBlock, indent + 2);
//         if (i->elseBlock) {
//             cout << pad << "  else:" << endl;
//             printAST(i->elseBlock, indent + 2);
//         }
//     }else if (auto* b = dynamic_cast<BinaryOpNode*>(node)) {
//         cout << pad << "[BinaryOp] " << tokenTypeToString(b->op) << endl;
//         printAST(b->left,  indent + 1);
//         printAST(b->right, indent + 1);
//     }
//     else if (auto* u = dynamic_cast<UnaryOpNode*>(node)) {
//         cout << pad << "[UnaryOp] " << tokenTypeToString(u->op)
//              << (u->postfix ? " (postfix)" : " (prefix)") << endl;
//         printAST(u->operand, indent + 1);
//     } else if (auto* i = dynamic_cast<WhileNode*>(node)) {
//         cout << pad << "[WhileNode]" << endl;
//         cout << pad << "  condition:" << endl;
//         printAST(i->condition, indent + 2);
//         cout << pad << "  body:" << endl;
//         printAST(i->body, indent + 2);
//     }else if (auto* f = dynamic_cast<ForNode*>(node)) {
//         if (f->isRange) {
//             cout << pad << "[ForNode - Range] " << f->var << endl;
//             cout << pad << "  from:" << endl;
//             printAST(f->from, indent + 2);
//             cout << pad << "  to:" << endl;
//             printAST(f->to, indent + 2);
//         } else {
//             cout << pad << "[ForNode - Native]" << endl;
//             cout << pad << "  init:" << endl;
//             printAST(f->from, indent + 2);
//             cout << pad << "  condition:" << endl;
//             printAST(f->to, indent + 2);
//             cout << pad << "  update:" << endl;
//             printAST(f->update, indent + 2);
//         }
//         cout << pad << "  body:" << endl;
//         printAST(f->body, indent + 2);
//     }else if (auto* s = dynamic_cast<SwitchNode*>(node)) {
//         cout << pad << "[SwitchNode]" << endl;
//         cout << pad << "  expr:" << endl;
//         printAST(s->expression, indent + 2);
//         for (auto* c : s->cases)
//             printAST(c, indent + 1);
//     }
//     else if (auto* c = dynamic_cast<CaseNode*>(node)) {
//         if (c->value == nullptr) {
//             cout << pad << "[DefaultCase]" << endl;
//         } else {
//             cout << pad << "[CaseNode]" << endl;
//             cout << pad << "  value:" << endl;
//             printAST(c->value, indent + 2);
//         }
//         cout << pad << "  body:" << endl;
//         printAST(c->body, indent + 2);
//     }else if (auto* s = dynamic_cast<ReturnNode*>(node)) {
//         cout << pad << "[ReturnNode]" << endl;
//         cout << pad << "  value:" << endl;
//         printAST(s->value, indent + 2);
//     }else if (auto* s = dynamic_cast<ExitNode*>(node)) {
//         cout << pad << "[ExitNode]" << endl;
//         cout << pad << "  code:" << endl;
//         printAST(s->code, indent + 2);
//     }else if (auto* a = dynamic_cast<ArrayLiteralNode*>(node)) {
//         cout << pad << "[ArrayLiteral]" << endl;
//         for (auto* e : a->elements)
//             printAST(e, indent + 1);
//     }
//     else if (auto* a = dynamic_cast<ArrayAccessNode*>(node)) {
//         cout << pad << "[ArrayAccess] " << a->name << endl;
//         cout << pad << "  index:" << endl;
//         printAST(a->index, indent + 2);
//         if (a->value) {
//             cout << pad << "  value:" << endl;
//             printAST(a->value, indent + 2);
//         }
//     }else if (auto* f = dynamic_cast<FuncDeclNode*>(node)) {
//         cout << pad << "[FuncDecl] " << f->name 
//              << " → " << f->returnType << endl;
//         for (auto* p : f->params)
//             cout << pad << "  param: " << p->name 
//                  << " : " << p->type << endl;
//         printAST(f->body, indent + 1);
//     }
//     else if (auto* f = dynamic_cast<FuncCallNode*>(node)) {
//         cout << pad << "[FuncCall] " << f->funcName << endl;
//         for (auto* a : f->args)
//             printAST(a, indent + 1);
//     }
//     else {
//         cout << pad << "[Unknown Node]" << endl;
//     }
// }

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Incorrect Usage" << endl;
        cerr << "Usage: ./compiler <input.sans>" << endl;
        return EXIT_FAILURE;
    }
 
    string content;
    {
        fstream input(argv[1], ios::in);
        stringstream ss;
        ss << input.rdbuf();
        content = ss.str();
    }

    // lex
    vector<Token> tokens = tokenize(content);

    // parse
    Parser parser(tokens);
    ProgramNode* ast = parser.parse();

    // print AST
    // printAST(ast);

    SemanticAnalyzer analyzer;
    analyzer.analyze(ast);

    if(analyzer.hasErrors()){
        analyzer.printErrors();
        return EXIT_FAILURE;
    }
    
    CodeGenerator codegen;
    
    codegen.generate(ast);
    codegen.writeToFile("output.asm");
    cout << "Generated output.asm" << endl;

    cout << "No Errors" << endl;

    return EXIT_SUCCESS;
}