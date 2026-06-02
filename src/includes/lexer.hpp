#pragma once

#include <string>
#include <vector>
#include <optional>

using namespace std;

enum class TokenType {

    // I/O verbs (verb-at-end style: "x mudraya;")
    mudraya,        // print
    grahan,         // input

    // Assignment & compound assignment
    sthapna,            // =
    yoga_sthapana,      // +=
    viyoga_sthapana,    // -=
    gunan_sthapana,     // *=
    bhaga_sthapana,     // /=
    shesh_sthapana,     // %=

    // Arithmetic operators
    yoga,       // +
    viyoga,     // -
    gunan,      // *
    bhaga,      // /
    shesh,      // %
    vriddhi,    // ++
    kshaya,     // --
    sandarbha,  // &
    bindu,      // .

    // Logical operators
    tatha,  // &&
    va,     // ||
    na,     // !

    // Word-based comparison operators (used in yadi/yavat conditions)
    sam,        // ==   (also keyword: "sam")
    asam,       // !=   (also keyword: "asam")
    adhik,      // >    (also keyword: "adhik")
    nyun,       // <    (also keyword: "nyun")
    adhik_sam,  // >=   (also keyword: "adhik_sam")
    nyun_sam,   // <=   (also keyword: "nyun_sam")

    // Symbol-based comparisons (still supported for C-familiar users)
    sym_sam,        // ==
    sym_asam,       // !=
    sym_adhik,      // >
    sym_nyun,       // <
    sym_adhik_sam,  // >=
    sym_nyun_sam,   // <=

    // Control flow keywords
    mukhya,     // main
    nivarta,    // return
    nirgacch,   //exit
    yadi,       // if
    anyatha,    // else
    yavat,      // while
    krute,      // for  — natural style: "krute i : 0 se 10"
    se,         // from — used in for range: "0 se 10"
    viram,      // break
    anuvarta,   // continue
    vikalp,     // switch
    prakaran,   // case
    adya,       // default
    karma,      // function keyword  
    
    // Literals
    purnank_lit,    // integer:  42
    dashamlav_lit,  // float:    3.14
    aksarmala_lit,  // string:   "hello"
    aksar_lit,      // char:     'a'
    tark_lit,       // bool:     satya / asatya

    // Data type keywords (written AFTER variable name: "x purnank = 10")
    purnank,    // int
    dashamlav,  // float
    aksarmala,  // string
    aksar,      // char
    tark,       // bool
    sarani,     // array
    sunya,      // void / null
    sthir,      // const

    // Delimiters
    vam_koshthak,       // (
    dakshin_koshthak,   // )
    vam_akar,           // {
    dakshin_akar,       // }
    vam_varga,          // [
    dakshin_varga,      // ]
    alpviram,           // ,
    dviviram,           // :
    ardhviram,          // ;

    // Boolean literals
    satya,   // true
    asatya,  // false

    // Memory
    nava,   // new
    mukta,  // free

    // Special
    dosha,      // lexer error
    anta,       // EOF
    identifier, // variable/function names
};

struct Token {
    TokenType type;
    std::optional<string> value{};
};

vector<Token> tokenize(const string& src);
string tokenTypeToString(TokenType type);
