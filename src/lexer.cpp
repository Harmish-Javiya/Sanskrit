#include "includes/lexer.hpp"
#include <unordered_map>

const unordered_map<string, TokenType> keywords = {
    {"mukhya",    TokenType::mukhya},
    {"nivarta",   TokenType::nivarta},
    {"nirgacch",   TokenType::nirgacch},
    {"mudraya",   TokenType::mudraya},
    {"grahan",    TokenType::grahan},
    {"yadi",      TokenType::yadi},
    {"anyatha",   TokenType::anyatha},
    {"yavat",     TokenType::yavat},
    {"krute",     TokenType::krute},
    {"se",        TokenType::se},
    {"viram",     TokenType::viram},
    {"anuvarta",  TokenType::anuvarta},
    {"vikalp",    TokenType::vikalp},
    {"prakaran",  TokenType::prakaran},
    {"adya",      TokenType::adya},
    {"karma",     TokenType::karma},

    // Type keywords (post-fix: "x purnank = 10")
    {"purnank",   TokenType::purnank},
    {"dashamlav", TokenType::dashamlav},
    {"aksarmala", TokenType::aksarmala},
    {"aksar",     TokenType::aksar},
    {"tark",      TokenType::tark},
    {"sarani",    TokenType::sarani},
    {"sunya",     TokenType::sunya},
    {"sthir",     TokenType::sthir},

    // Word-based comparison operators
    {"adhik_sam", TokenType::adhik_sam},
    {"nyun_sam",  TokenType::nyun_sam},
    {"adhik",     TokenType::adhik},
    {"nyun",      TokenType::nyun},
    {"sam",       TokenType::sam},
    {"asam",      TokenType::asam},

    // Boolean literals
    {"satya",     TokenType::satya},
    {"asatya",    TokenType::asatya},

    // Memory
    {"nava",      TokenType::nava},
    {"mukta",     TokenType::mukta},
};

const unordered_map<string, TokenType> symbols = {
    // Arithmetic
    {"+", TokenType::yoga},
    {"-", TokenType::viyoga},
    {"*", TokenType::gunan},
    {"/", TokenType::bhaga},
    {"%", TokenType::shesh},
    {"&", TokenType::sandarbha},
    {".", TokenType::bindu},

    // Assignment
    {"=",  TokenType::sthapna},
    {"+=", TokenType::yoga_sthapana},
    {"-=", TokenType::viyoga_sthapana},
    {"*=", TokenType::gunan_sthapana},
    {"/=", TokenType::bhaga_sthapana},
    {"%=", TokenType::shesh_sthapana},

    // Increment / decrement
    {"++", TokenType::vriddhi},
    {"--", TokenType::kshaya},

    // Logical
    {"&&", TokenType::tatha},
    {"||", TokenType::va},
    {"!",  TokenType::na},

    // Symbol comparisons (still valid alongside word operators)
    {"==", TokenType::sym_sam},
    {"!=", TokenType::sym_asam},
    {">",  TokenType::sym_adhik},
    {"<",  TokenType::sym_nyun},
    {">=", TokenType::sym_adhik_sam},
    {"<=", TokenType::sym_nyun_sam},

    // Delimiters
    {"(", TokenType::vam_koshthak},
    {")", TokenType::dakshin_koshthak},
    {"{", TokenType::vam_akar},
    {"}", TokenType::dakshin_akar},
    {"[", TokenType::vam_varga},
    {"]", TokenType::dakshin_varga},
    {",", TokenType::alpviram},
    {":", TokenType::dviviram},
    {";", TokenType::ardhviram},
};

vector<Token> tokenize(const string& src) {
    vector<Token> tokens;
    int i = 0;
    int len = (int)src.size();

    while (i < len) {
        char c = src[i];

        // ── Skip whitespace ──────────────────────────────────────────────
        if (isspace(c)) { i++; continue; }

        // ── Skip single-line comments  // ... ────────────────────────────
        if (c == '/' && i + 1 < len && src[i + 1] == '/') {
            while (i < len && src[i] != '\n') i++;
            continue;
        }

        // ── String literal  "..." ────────────────────────────────────────
        if (c == '"') {
            string str;
            i++; // skip opening "
            while (i < len && src[i] != '"') {
                if (src[i] == '\\' && i + 1 < len) {
                    char esc = src[i + 1];
                    if      (esc == 'n')  str += '\n';
                    else if (esc == 't')  str += '\t';
                    else if (esc == '"')  str += '"';
                    else if (esc == '\\') str += '\\';
                    else                  { str += '\\'; str += esc; }
                    i += 2;
                } else {
                    str += src[i++];
                }
            }
            if (i < len) i++; // skip closing "
            tokens.push_back({TokenType::aksarmala_lit, str});
            continue;
        }

        // ── Char literal  'a' ────────────────────────────────────────────
        if (c == '\'') {
            i++; // skip opening '
            char ch = 0;
            if (i < len) {
                if (src[i] == '\\' && i + 1 < len) {
                    char esc = src[i + 1];
                    if      (esc == 'n')  ch = '\n';
                    else if (esc == 't')  ch = '\t';
                    else if (esc == '\'') ch = '\'';
                    else if (esc == '\\') ch = '\\';
                    i += 2;
                } else {
                    ch = src[i++];
                }
            }
            if (i < len && src[i] == '\'') i++; // skip closing '
            tokens.push_back({TokenType::aksar_lit, string(1, ch)});
            continue;
        }

        // ── Identifier or keyword ─────────────────────────────────────────
        // Identifiers: start with alpha or '_', continue with alnum or '_'
        if (isalpha(c) || c == '_') {
            string buf;
            while (i < len && (isalnum(src[i]) || src[i] == '_')) {
                buf += src[i++];
            }
            auto it = keywords.find(buf);
            if (it != keywords.end()) {
                tokens.push_back({it->second});
            } else {
                tokens.push_back({TokenType::identifier, buf});
            }
            continue;
        }

        // ── Number literal  42  or  3.14 ─────────────────────────────────
        if (isdigit(c)) {
            string buf;
            bool isFloat = false;
            while (i < len && isdigit(src[i])) buf += src[i++];
            if (i < len && src[i] == '.' && i + 1 < len && isdigit(src[i + 1])) {
                isFloat = true;
                buf += src[i++]; // consume '.'
                while (i < len && isdigit(src[i])) buf += src[i++];
            }
            tokens.push_back({isFloat ? TokenType::dashamlav_lit : TokenType::purnank_lit, buf});
            continue;
        }

        // ── Symbol (one or two character) ────────────────────────────────
        if (!isalnum(c) && !isspace(c)) {
            // Try two-char symbol first
            if (i + 1 < len) {
                string two; two += c; two += src[i + 1];
                auto it = symbols.find(two);
                if (it != symbols.end()) {
                    tokens.push_back({it->second});
                    i += 2;
                    continue;
                }
            }
            // Try one-char symbol
            string one; one += c;
            auto it = symbols.find(one);
            if (it != symbols.end()) {
                tokens.push_back({it->second});
            } else {
                tokens.push_back({TokenType::dosha, one});
            }
            i++;
            continue;
        }

        // ── Fallthrough (shouldn't happen) ───────────────────────────────
        string unknown; unknown += c;
        tokens.push_back({TokenType::dosha, unknown});
        i++;
    }

    tokens.push_back({TokenType::anta});
    return tokens;
}

string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::mudraya:            return "mudraya";
        case TokenType::grahan:             return "grahan";
        case TokenType::sthapna:            return "sthapna";
        case TokenType::yoga_sthapana:      return "yoga_sthapana";
        case TokenType::viyoga_sthapana:    return "viyoga_sthapana";
        case TokenType::gunan_sthapana:     return "gunan_sthapana";
        case TokenType::bhaga_sthapana:     return "bhaga_sthapana";
        case TokenType::shesh_sthapana:     return "shesh_sthapana";
        case TokenType::yoga:               return "yoga";
        case TokenType::viyoga:             return "viyoga";
        case TokenType::gunan:              return "gunan";
        case TokenType::bhaga:              return "bhaga";
        case TokenType::shesh:              return "shesh";
        case TokenType::vriddhi:            return "vriddhi";
        case TokenType::kshaya:             return "kshaya";
        case TokenType::sandarbha:          return "sandarbha";
        case TokenType::bindu:              return "bindu";
        case TokenType::tatha:              return "tatha";
        case TokenType::va:                 return "va";
        case TokenType::na:                 return "na";
        case TokenType::sam:                return "sam";
        case TokenType::asam:               return "asam";
        case TokenType::adhik:              return "adhik";
        case TokenType::nyun:               return "nyun";
        case TokenType::adhik_sam:          return "adhik_sam";
        case TokenType::nyun_sam:           return "nyun_sam";
        case TokenType::sym_sam:            return "sym_sam";
        case TokenType::sym_asam:           return "sym_asam";
        case TokenType::sym_adhik:          return "sym_adhik";
        case TokenType::sym_nyun:           return "sym_nyun";
        case TokenType::sym_adhik_sam:      return "sym_adhik_sam";
        case TokenType::sym_nyun_sam:       return "sym_nyun_sam";
        case TokenType::mukhya:             return "mukhya";
        case TokenType::nivarta:            return "nivarta";
        case TokenType::yadi:               return "yadi";
        case TokenType::anyatha:            return "anyatha";
        case TokenType::yavat:              return "yavat";
        case TokenType::krute:              return "krute";
        case TokenType::se:                 return "se";
        case TokenType::viram:              return "viram";
        case TokenType::anuvarta:           return "anuvarta";
        case TokenType::vikalp:             return "vikalp";
        case TokenType::prakaran:           return "prakaran";
        case TokenType::adya:               return "adya";
        case TokenType::karma:              return "karma";
        case TokenType::purnank_lit:        return "purnank_lit";
        case TokenType::dashamlav_lit:      return "dashamlav_lit";
        case TokenType::aksarmala_lit:      return "aksarmala_lit";
        case TokenType::aksar_lit:          return "aksar_lit";
        case TokenType::tark_lit:           return "tark_lit";
        case TokenType::purnank:            return "purnank";
        case TokenType::dashamlav:          return "dashamlav";
        case TokenType::aksarmala:          return "aksarmala";
        case TokenType::aksar:              return "aksar";
        case TokenType::tark:               return "tark";
        case TokenType::sarani:             return "sarani";
        case TokenType::sunya:              return "sunya";
        case TokenType::sthir:              return "sthir";
        case TokenType::vam_koshthak:       return "vam_koshthak";
        case TokenType::dakshin_koshthak:   return "dakshin_koshthak";
        case TokenType::vam_akar:           return "vam_akar";
        case TokenType::dakshin_akar:       return "dakshin_akar";
        case TokenType::vam_varga:          return "vam_varga";
        case TokenType::dakshin_varga:      return "dakshin_varga";
        case TokenType::alpviram:           return "alpviram";
        case TokenType::dviviram:           return "dviviram";
        case TokenType::ardhviram:          return "ardhviram";
        case TokenType::satya:              return "satya";
        case TokenType::asatya:             return "asatya";
        case TokenType::nava:               return "nava";
        case TokenType::mukta:              return "mukta";
        case TokenType::dosha:              return "dosha";
        case TokenType::anta:               return "anta";
        case TokenType::identifier:         return "identifier";
        default:                            return "unknown";
    }
}


