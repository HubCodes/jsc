#include "js.hpp"

static bool is_id_start(int ch);
static bool is_id_content(int ch);

Lexer::Lexer(std::stringstream&& code): code(move(code)) {}

unique<Token> Lexer::next() {
    skip_whitespace();
    int ch = code.peek();
    if (is_id_start(ch)) {
        return get_id();
    }
    return NULL;
}

unique<Token> Lexer::get_id() {
    unique<Token> id = make_unique<Token>();
    string id_str = "";
    while (is_id_content(code.peek())) {
        id_str.push_back(code.get());
    }
    if (keywords.find(id_str) != keywords.end()) {
        id->kind = TokenKind::KEYWORD;
        id->data = keywords[id_str];
    } else if (binary_ops.find(id_str) != binary_ops.end()) {
        id->kind = TokenKind::OP;
        id->data = binary_ops[id_str];
    } else if (unary_ops.find(id_str) != unary_ops.end()) {
        id->kind = TokenKind::OP;
        id->data = unary_ops[id_str];
    } else {
        id->kind = TokenKind::ID;
        id->data = id_str;
    }
    return id;
}

unique<Token> Lexer::get_number() {
    unique<Token> num = make_unique<Token>();
    string num_str = "";
    int ch = code.peek();
    bool is_exp = false;
    TokenKind kind = ch == '.' ? TokenKind::DOUBLE : TokenKind::INTEGER;
    for (;;) {
        ch = code.get();
        if (std::toupper(ch) == 'E') {
            num_str += get_exp();
            is_exp = true;
            goto make_token;
        } else if (kind == TokenKind::INTEGER && ch == '.') {
            num_str.push_back(ch);
            kind = TokenKind::DOUBLE;
        } else if (std::isdigit(ch)) {
            num_str.push_back(ch);
        } else {
            code.unget();
            break;
        }
    }
make_token:
    if (kind == TokenKind::DOUBLE || is_exp) {
        num->kind = TokenKind::DOUBLE;
        num->data = std::stod(num_str);
    } else {
        num->kind = TokenKind::INTEGER;
        num->data = std::stoi(num_str);
    }
    return num;
}

unique<Token> Lexer::get_string_literal() {
    unique<Token> string_literal = make_unique<Token>();
    string literal = "";
    int closer = code.get();
    int ch = code.get();
    static string escapers = "\'\"\\nrtbfv0";
    while (ch != closer) {
        if (ch == '\\' && escapers.find(code.peek()) != string::npos) {
            literal += "\\";
            literal.push_back(code.get());
            ch = code.get();
            continue;
        }
        literal.push_back(ch);
    }
    string_literal->kind = TokenKind::STRING;
    string_literal->data = literal;
    return string_literal;
}

unique<Token> Lexer::get_punct() {
    unique<Token> punct = make_unique<Token>();
    static auto is_punct = [](int ch) -> bool {
        static string punct_chars = "{}[]();:";
        return punct_chars.find(ch) != string::npos;
    };
    int ch = code.get();
    if (is_punct(ch)) {

    }
    return punct;
}

void Lexer::skip_whitespace() {
    while (std::isspace(code.peek())) code.get();
}

string Lexer::get_exp() {
    string exp = "e";
    int ch = code.peek();
    if (ch == '+' || ch == '-') {
        exp.push_back(code.get());
        ch = code.peek();
    }
    while (std::isdigit(ch)) {
        exp.push_back(code.get());
        ch = code.peek();
    }
    return exp;
}

static bool is_id_start(int ch) {
    return (
        ('a' <= ch && ch <= 'z')
        || ('A' <= ch && ch <= 'Z')
        || ch == '$'
        || ch == '_');
}

static bool is_id_content(int ch) {
    return is_id_start(ch) || ('0' <= ch && ch <= '9');
}
