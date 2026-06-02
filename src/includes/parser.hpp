#pragma once
#include <iostream>
#include "lexer.hpp"
#include "ast.hpp"

class Parser{
    private:
        vector<Token> tokens;
        size_t pos = 0;

        Token peek();
        Token peekNext();
        Token consume();
        Token expect(TokenType t);
        bool check(TokenType t);
        bool isAtEnd();
        void error(string message);

        ProgramNode* parseProgram();
        ASTNode* parseDeclaration();

        ASTNode* parseStatement();
        ASTNode* parseVarDecl();
        ASTNode* parseAssignment();
        ASTNode* parsePrintStmt();
        ASTNode* parseInputStmt();
        ASTNode* parseIfStmt();
        ASTNode* parseWhileStmt();
        ASTNode* parseForStmt();
        ASTNode* parseSwitchStmt();
        ASTNode* parseCaseClause();
        ASTNode* parseReturnStmt();
        ASTNode* parseExitStmt();
        ASTNode* parseBreakStmt();
        ASTNode* parseContinueStmt();
        BlockNode* parseBlock();

        ASTNode* parseFuncDecl();
        ASTNode* parseMainDecl();
        vector<ParamNode*> parseParamList(); 
        ParamNode* parseParam();
        FuncCallNode* parseFuncCall(string name);
        vector<ASTNode*> parseArgList();

        ASTNode* parseExpr();
        ASTNode* parseBinaryExpr(int minPrecedence);
        ASTNode* parseUnaryExpr();
        ASTNode* parsePostFixExpr(ASTNode* left);
        ASTNode* parsePrimaryExpr();
        ASTNode* parseArrayLiteral();
        ASTNode* parseArrayAccess();
        int getOperatorPrecedence(TokenType t);

        ASTNode* parseNewExpr();
        ASTNode* parseFreeStmt();

    public:
        Parser(vector<Token> t) : tokens(t), pos(0) {}

        ProgramNode* parse() {
            return parseProgram();
        }
};