#pragma once
#include "lexer.hpp"
#include <string>
#include <vector>

using namespace std;

struct ASTNode{
    virtual ~ASTNode() = default;
};

struct LiteralNode : public ASTNode{
    string value;
    TokenType type;
};

struct IdentifierNode: public ASTNode{
    string name;
};

struct BinaryOpNode: public ASTNode{
    TokenType op;
    ASTNode* left;
    ASTNode* right;
};

struct UnaryOpNode: public ASTNode{
    TokenType op;
    ASTNode* operand;
    bool postfix;
};

struct FuncCallNode: public ASTNode{
    string funcName;
    vector<ASTNode*> args;
};


struct BlockNode: public ASTNode{
  vector<ASTNode*> statements; 
};

struct VarDeclNode: public ASTNode{
    string varName;
    string type;
    bool isConst;
    ASTNode* initializer; //for Null or not given
};

struct AssignNode: public ASTNode{
    string name;
    TokenType op;
    ASTNode* value;
};

struct PrintNode: public ASTNode{
    ASTNode* value;
};

struct InputNode: public ASTNode{
    string name;
};

struct ArrayLiteralNode: public ASTNode{
    vector<ASTNode*> elements;
};

struct ArrayAccessNode: public ASTNode{
    string name;
    ASTNode* index;
    ASTNode* value; 
};

struct IfNode: public ASTNode{
    ASTNode* condition;
    BlockNode* thenBlock;
    BlockNode* elseBlock;
};

struct CaseNode: public ASTNode{
    ASTNode* value;
    BlockNode* body;
};


struct SwitchNode: public ASTNode{
    ASTNode* expression;
    vector<CaseNode*> cases;
    BlockNode* defaultBlock;
};

struct WhileNode: public ASTNode{
    ASTNode* condition;
    BlockNode* body;
};

struct ForNode: public ASTNode{
    string var;
    ASTNode* from;
    ASTNode* to;
    BlockNode* body;
    ASTNode* update;    // ← add this  i++
    bool isRange; 
};

struct ReturnNode: public ASTNode{
    ASTNode* value;
};

struct NewNode: public ASTNode{
    string type;
};

struct FreeNode: public ASTNode{
    string name;
};

struct ExitNode: public ASTNode{
    ASTNode* code;
};

struct BreakNode: public ASTNode{};
struct ContinueNode: public ASTNode{};

struct ParamNode: public ASTNode{
    string name;
    string type;
};

struct FuncDeclNode: public ASTNode{
    string name;
    vector<ParamNode*> params;
    string returnType;
    BlockNode* body;
};

struct ProgramNode: public ASTNode{
    vector<ASTNode*> declarations;
};








