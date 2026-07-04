#include "includes/parser.hpp"
#include <stdexcept>


Token Parser::peek(){
    return tokens[pos];
}

Token Parser::peekNext(){
    if(pos + 1 < tokens.size()){
        return tokens[pos + 1];
    }
    return tokens[pos];
}

Token Parser::consume(){
    return tokens[pos++];
}

Token Parser::expect(TokenType t){
    if(peek().type == t)
        return consume();
    error("Unexpected token: " + tokenTypeToString(peek().type));
    throw runtime_error("parse error");
}

bool Parser::check(TokenType t){
    return peek().type == t;
}

bool Parser::isAtEnd(){
    return peek().type == TokenType::anta;
}

void Parser::error(string message){
    cerr << "[Parser Error]" << message << endl;
}

ProgramNode* Parser::parseProgram() {
    ProgramNode* program = new ProgramNode();
    while (!isAtEnd()) {
        program->declarations.push_back(parseDeclaration());
    }
    return program;
}

ASTNode* Parser::parseDeclaration() {
    if (check(TokenType::karma))  return parseFuncDecl();
    if (check(TokenType::mukhya)) return parseMainDecl();
    return parseStatement();
}

ASTNode* Parser::parseArrayLiteral() { 
    expect(TokenType::vam_varga);
    ArrayLiteralNode* node = new ArrayLiteralNode();

    while(!check(TokenType::dakshin_varga) && !isAtEnd()){
        node->elements.push_back(parseExpr());
        if(check(TokenType::alpviram)){
            consume();
        }
    }

    expect(TokenType::dakshin_varga);
    return node;
}

ASTNode* Parser::parseArrayAccess() { 
    string name = consume().value.value();
    expect(TokenType::vam_varga);
    ASTNode* index = parseExpr();
    expect(TokenType::dakshin_varga);
    ASTNode* value = nullptr;

    if (check(TokenType::sthapna)) {
        consume();                           
        value = parseExpr();                 
        expect(TokenType::ardhviram);        
    }

    ArrayAccessNode* node = new ArrayAccessNode();
    node->index = index;
    node->name = name;
    node->value = value;

    return node;   
}

ASTNode* Parser::parseStatement() {
    if(check(TokenType::yadi)) return parseIfStmt();

    if(check(TokenType::yavat)) return parseWhileStmt();
    
    if(check(TokenType::krute)) return parseForStmt();

    if(check(TokenType::vikalp)) return parseSwitchStmt();

    if(check(TokenType::nivarta)) return parseReturnStmt();
    
    if(check(TokenType::viram)) return parseBreakStmt();

    if(check(TokenType::anuvarta)) return parseContinueStmt();
    
    if(check(TokenType::nirgacch)) return parseExitStmt();

    if(check(TokenType::mukta)) return parseFreeStmt();

    if(check(TokenType::identifier)){

        if(peekNext().type == TokenType::purnank ||
           peekNext().type == TokenType::dashamlav ||
           peekNext().type == TokenType::aksarmala ||
           peekNext().type == TokenType::aksar ||
           peekNext().type == TokenType::tark ||
           peekNext().type == TokenType::sarani ||
           peekNext().type == TokenType::sthir) 
           return parseVarDecl();

        if(peekNext().type == TokenType::sthapna ||
           peekNext().type == TokenType::yoga_sthapana ||
           peekNext().type == TokenType::viyoga_sthapana ||
           peekNext().type == TokenType::gunan_sthapana ||
           peekNext().type == TokenType::bhaga_sthapana ||
           peekNext().type == TokenType::shesh_sthapana)
           return parseAssignment();
           
        if (peekNext().type == TokenType::vam_varga){
            ASTNode* arrNode =  parseArrayAccess();

            if(check(TokenType::mudraya)){
                consume();
                expect(TokenType::ardhviram);
                PrintNode* node = new PrintNode();
                node->value = arrNode;
                return node;
            }
            return arrNode;
        }   

        if (peekNext().type == TokenType::mudraya)
            return parsePrintStmt();

        if(peekNext().type == TokenType::grahan)
            return parseInputStmt();
    }

    if ((check(TokenType::purnank_lit)   ||
        check(TokenType::dashamlav_lit) ||
        check(TokenType::aksarmala_lit) ||
        check(TokenType::satya)         ||
        check(TokenType::asatya))       &&
        peekNext().type == TokenType::mudraya)
        return parsePrintStmt();

    if(check(TokenType::aksarmala_lit) && peekNext().type == TokenType::mudraya)
        return parsePrintStmt();

    error("Unknown statement starting with: " + tokenTypeToString(peek().type));
    consume();

    return nullptr;

}

ASTNode* Parser::parseExpr(){
    return parseBinaryExpr(0);
}

BlockNode* Parser::parseBlock() { 
    expect(TokenType::vam_akar);
    BlockNode* block = new BlockNode();

    while (!check(TokenType::dakshin_akar) && !isAtEnd())
    {
        block->statements.push_back(parseStatement());
    }
    
    expect(TokenType::dakshin_akar);
    return block;
}

ASTNode* Parser::parsePrimaryExpr(){
    Token t = peek();

    if (check(TokenType::vam_varga))
        return parseArrayLiteral();

    if (t.type == TokenType::purnank_lit ||
        t.type == TokenType::dashamlav_lit ||
        t.type == TokenType::aksarmala_lit ||
        t.type == TokenType::aksar_lit ||
        t.type == TokenType::satya ||
        t.type == TokenType::asatya){
            consume();
            LiteralNode* node = new LiteralNode();
            node->value = t.value.value_or("");
            node->type = t.type;
            return node;
    }

    if(t.type == TokenType::identifier){
        consume();

        if(check(TokenType::vam_koshthak)){
            return parseFuncCall(t.value.value());
        }

        if(check(TokenType::vam_varga)){
            consume();
            ASTNode* index = parseExpr();
            expect(TokenType::dakshin_varga);

            ArrayAccessNode* node = new ArrayAccessNode();
            node->name = t.value.value();
            node->index = index;

            return node;
        }

        IdentifierNode* node = new IdentifierNode();
        node->name = t.value.value();
        return node;
    }

    if (check(TokenType::nava)) {
        consume();                                    
        string type = tokenTypeToString(consume().type); 
        NewNode* node = new NewNode();
        node->type = type;
        return node;
    }

    error("Expected expressions, got: " + tokenTypeToString(t.type));
    return nullptr;
}

ASTNode* Parser::parseVarDecl(){
    string name = consume().value.value();

    bool isConst = false;
    if(check(TokenType::sthir)){
        isConst = true;
        consume();
    }

    string type = tokenTypeToString(consume().type);

    if (type == "sarani" && !check(TokenType::sthapna) && !isAtEnd()) {
        string elementType = tokenTypeToString(consume().type);
        type = type + "_" + elementType;   // "sarani_purnank"
    }

    ASTNode* initializer = nullptr;
    if(check(TokenType::sthapna)){
        consume();
        initializer = parseExpr();
    }

    expect(TokenType::ardhviram);

    VarDeclNode* node = new VarDeclNode();
    node->varName = name;
    node->type = type;
    node->isConst = isConst;
    node->initializer = initializer;

    return node;
}

ASTNode* Parser::parsePrintStmt() { 
    ASTNode* value = parseExpr();
    expect(TokenType::mudraya);
    expect(TokenType::ardhviram);
    
    PrintNode* node = new PrintNode();
    node->value = value;
    return node; 
}

ASTNode* Parser::parseAssignment() { 
    
    string name = consume().value.value();
    Token op = consume();

    ASTNode* value = parseExpr();
    expect(TokenType::ardhviram);

    AssignNode* node = new AssignNode();
    node->name = name;
    node->op = op.type;
    node->value = value;
    return node; 
}

ASTNode* Parser::parseInputStmt() { 
    string name = consume().value.value();
    expect(TokenType::grahan);
    expect(TokenType::ardhviram);

    InputNode* node = new InputNode();
    node->name = name;
    return node; 
}

ASTNode* Parser::parseIfStmt() {
    expect(TokenType::yadi);
    ASTNode* condition = parseExpr();
    BlockNode* thenBlock = parseBlock();

    BlockNode* elseBlock = nullptr;
    if(check(TokenType::anyatha)){
        consume();
        elseBlock = parseBlock();
    }

    IfNode* node = new IfNode();
    node->condition = condition;
    node->thenBlock = thenBlock;
    node->elseBlock = elseBlock;
    
    return node;
}

ASTNode* Parser::parseWhileStmt() { 
    expect(TokenType::yavat);
    ASTNode* condition = parseExpr();
    BlockNode* body = parseBlock();

    WhileNode* node = new WhileNode();
    node->condition = condition;
    node->body = body;

    return node;
}

ASTNode* Parser::parseForStmt() { 
    expect(TokenType::krute);

    if(check(TokenType::vam_koshthak)){
        consume();
        
        ASTNode* init = parseVarDecl();

        ASTNode* condition = parseExpr();
        expect(TokenType::ardhviram);

        ASTNode* update = parseExpr();
        expect(TokenType::dakshin_koshthak);

        BlockNode* body = parseBlock();

        ForNode* node = new ForNode();
        node->var = "";
        node->from = init;
        node->to = condition;
        node->body = body;
        node->update = update;
        node->isRange = false;
        return node;
    }

    string var = consume().value.value();
    expect(TokenType::dviviram);
    ASTNode* from = parseExpr();
    expect(TokenType::se);
    ASTNode* to = parseExpr();
    BlockNode* body = parseBlock();

    ForNode* node = new ForNode();
    node->var = var;
    node->from = from;
    node->to = to;
    node->update = nullptr;
    node->body = body;
    node->isRange = true;

    return node;
}

ASTNode* Parser::parseCaseClause() { 
    expect(TokenType::prakaran);
    ASTNode* value = parseExpr();

    expect(TokenType::dviviram);
    BlockNode* body = new BlockNode();

    while  (!check(TokenType::viram) && 
            !check(TokenType::prakaran) &&
            !check(TokenType::adya) &&
            !check(TokenType::dakshin_akar) &&
            !isAtEnd()){
        body->statements.push_back(parseStatement());        
    }

    if (check(TokenType::viram)) {
        consume();                    
        expect(TokenType::ardhviram); 
    }

    CaseNode* node = new CaseNode();
    node->value = value;
    node->body = body;

    return node;
}

ASTNode* Parser::parseSwitchStmt() { 
    expect(TokenType::vikalp);
    ASTNode* expression = parseExpr();
    expect(TokenType::vam_akar);
    
    SwitchNode* node = new SwitchNode();
    node->expression = expression;

    while(!check(TokenType::dakshin_akar) && !isAtEnd()){
        if(check(TokenType::prakaran)){
            node->cases.push_back(
                dynamic_cast<CaseNode*>(parseCaseClause())
            );
        }else if(check(TokenType::adya)){
            consume();
            expect(TokenType::dviviram);
            BlockNode* body = new BlockNode();
            while(!check(TokenType::dakshin_akar) && !isAtEnd()){
                body->statements.push_back(parseStatement());
            }
            CaseNode* defaultCase = new CaseNode();
            defaultCase->value = nullptr;
            defaultCase->body = body;
            node->cases.push_back(defaultCase);
        }else{
            error("Expected prakaran or adya in vikalp");
            consume();
        }
    }
    expect(TokenType::dakshin_akar);
    return node;
}


int Parser::getOperatorPrecedence(TokenType t) { 
    switch(t){
        case TokenType::va: return 1;
        case TokenType::tatha: return 2;

        case TokenType::sam:
        case TokenType::asam:
        case TokenType::sym_sam:
        case TokenType::sym_asam: return 3;

        case TokenType::adhik:
        case TokenType::nyun:
        case TokenType::adhik_sam:
        case TokenType::nyun_sam:
        case TokenType::sym_adhik:
        case TokenType::sym_nyun:
        case TokenType::sym_adhik_sam:
        case TokenType::sym_nyun_sam: return 4;

        case TokenType::yoga:
        case TokenType::viyoga: return 5;

        case TokenType::gunan:
        case TokenType::bhaga:
        case TokenType::shesh: return 6;
        
        default: return 0;
    }
}

ASTNode* Parser::parseUnaryExpr() { 
    if (check(TokenType::na) ||
        check(TokenType::viyoga) ||
        check(TokenType::vriddhi) ||
        check(TokenType::kshaya)){
            Token op = consume();
            ASTNode* operand = parseUnaryExpr();
            UnaryOpNode* node = new UnaryOpNode();
            node->op = op.type;
            node->operand = operand;
            node->postfix = false;
            return node;
    }

    ASTNode* left = parsePrimaryExpr();
    return parsePostFixExpr(left);
}


ASTNode* Parser::parseBinaryExpr(int minPrecedence) { 
    ASTNode* left = parseUnaryExpr();

    while(true){
        int prec = getOperatorPrecedence(peek().type);
        if(prec <= minPrecedence) break;

        Token op = consume();
        ASTNode* right = parseBinaryExpr(prec);

        BinaryOpNode* node = new BinaryOpNode();
        node->left = left;
        node->op = op.type;
        node->right = right;
        left = node;
    }

    return left;
}

ASTNode* Parser::parseReturnStmt() { 
    expect(TokenType::nivarta);
    ASTNode* value = nullptr;

    if(!check(TokenType::ardhviram)){
        value = parseExpr();
    }

    expect(TokenType::ardhviram);

    ReturnNode* node = new ReturnNode();
    node->value = value;

    return node;
}

ASTNode* Parser::parseExitStmt() { 
    expect(TokenType::nirgacch);
    ASTNode* code = nullptr;

    expect(TokenType::vam_koshthak);
    if(!check(TokenType::dakshin_koshthak)){
        code = parseExpr();
    }
    expect(TokenType::dakshin_koshthak);
    expect(TokenType::ardhviram);

    ExitNode* node = new ExitNode();
    node->code = code;

    return node;
}

ASTNode* Parser::parseBreakStmt() {
    expect(TokenType::viram);
    expect(TokenType::ardhviram);

    BreakNode* node = new BreakNode();
    return node;
}

ASTNode* Parser::parseContinueStmt() {
    expect(TokenType::anuvarta);
    expect(TokenType::ardhviram);

    ContinueNode* node = new ContinueNode();
    return node;
}

ASTNode* Parser::parseNewExpr() { 
    expect(TokenType::nava);
    string type = tokenTypeToString(consume().type);
    expect(TokenType::ardhviram);

    NewNode* node = new NewNode();
    node->type = type;
    return node;
}

ASTNode* Parser::parseFreeStmt() { 
    expect(TokenType::mukta);
    string name = consume().value.value();
    expect(TokenType::ardhviram);
    
    FreeNode* node = new FreeNode();
    node->name = name;
    return node;
}

ASTNode* Parser::parsePostFixExpr(ASTNode* left) { 
    if(check(TokenType::vriddhi) || check(TokenType::kshaya)){
        Token op = consume();
        UnaryOpNode* node = new UnaryOpNode();
        node->op = op.type;
        node->postfix = true;
        node->operand = left;
        return node;
    }

    return left;
}

ParamNode* Parser::parseParam() { 
    string name = consume().value.value();
    string type = tokenTypeToString(consume().type);

    ParamNode* node = new ParamNode();
    node->name = name;
    node->type = type;

    return node;
}

vector<ParamNode*> Parser::parseParamList() { 
    expect(TokenType::vam_koshthak);
    vector<ParamNode*> params;

    while(!check(TokenType::dakshin_koshthak) && !isAtEnd()){
        params.push_back(parseParam());
        if(check(TokenType::alpviram)){
            consume();
        }
    }
     
    expect(TokenType::dakshin_koshthak);
    return params;
}

ASTNode* Parser::parseFuncDecl() { 
    expect(TokenType::karma);
    string name = consume().value.value();
    vector<ParamNode*> params = parseParamList();
    string returnType = tokenTypeToString(consume().type);
    BlockNode* body = parseBlock();

    FuncDeclNode* node = new FuncDeclNode();
    node->name = name;
    node->params = params;
    node->returnType = returnType;
    node->body = body;

    return node;
}

vector<ASTNode*> Parser::parseArgList() { 
    expect(TokenType::vam_koshthak);
    vector<ASTNode*> args;

    while(!check(TokenType::dakshin_koshthak) && !isAtEnd()){
        args.push_back(parseExpr());
        if(check(TokenType::alpviram))
            consume();
    }

    expect(TokenType::dakshin_koshthak);
    return args;
}

FuncCallNode* Parser::parseFuncCall(string name) { 
    vector<ASTNode*> args = parseArgList();

    FuncCallNode* node = new FuncCallNode();
    node->args = args;
    node->funcName = name;
    return node;
}

ASTNode* Parser::parseMainDecl() { 
    expect(TokenType::mukhya);
    vector<ParamNode*> params = parseParamList();
    BlockNode* body = parseBlock();

    FuncDeclNode* node = new FuncDeclNode();
    node->name = "mukhya";
    node->params = params;
    node->returnType = "sunya";
    node->body = body;

    return node;
}


