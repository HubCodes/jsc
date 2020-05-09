#include "js.hpp"

static bool is_id_start(int ch);
static bool is_id_content(int ch);
static bool is_number_start(int ch);
static bool is_string_literal_start(int ch);
static bool is_punct(int ch);
static bool is_op_start(int ch);

Lexer::Lexer(std::stringstream&& code): code(move(code)) {}

unique<Token> Lexer::next() {
    skip_whitespace();
    int ch = code.peek();
    if (is_id_start(ch)) {
        return get_id();
    } else if (is_number_start(ch)) {
        return get_number();
    } else if (is_string_literal_start(ch)) {
        return get_string_literal();
    } else if (is_punct(ch)) {
        return get_punct();
    } else if (is_op_start(ch)) {
        return get_op();
    } else if (code.eof()) {
        unique<Token> eot = make_unique<Token>();
        eot->kind = TokenKind::EOT;
        eot->data = 0;
        return eot;
    }
    unique<Token> bad = make_unique<Token>();
    bad->kind = TokenKind::BAD;
    bad->data = 0;
    return bad;
}

unique<Token> Lexer::peek() {
    auto pos = code.tellg();
    auto result = next();
    code.seekg(pos);
    return result;
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
        id->kind = TokenKind::BINOP;
        id->data = binary_ops[id_str];
    } else if (unary_ops.find(id_str) != unary_ops.end()) {
        id->kind = TokenKind::UNOP;
        id->data = unary_ops[id_str];
    } else if (booleans.find(id_str) != booleans.end()) {
        id->kind = TokenKind::BOOLEAN;
        id->data = booleans[id_str];
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
    punct->data = static_cast<char>(code.get());
    punct->kind = TokenKind::PUNCT;
    return punct;
}

unique<Token> Lexer::get_op() {
    unique<Token> op = make_unique<Token>();
    string op_str = "";
    int ch = code.get();
    op_str.push_back(ch);
    switch (ch) {
    case '=':
    case '!':
        if (code.peek() == '=') {
            op_str.push_back(code.get());
            if (code.peek() == '=') {
                op_str.push_back(code.get());
            }
        }
        break;
    case '+':
    case '-':
    case '/':
    case '%':
    case '&':
    case '^':
    case '|':
        if (code.peek() == '=') {
            op_str.push_back(code.get());
        } else if (ch == '&' && code.peek() == '&') {
            code.get();
            op_str += "&&";
        } else if (ch == '|' && code.peek() == '|') {
            code.get();
            op_str += "||";
        } else if (ch == '+' && code.peek() == '+') {
            code.get();
            op_str += "++";
        } else if (ch == '-' && code.peek() == '-') {
            code.get();
            op_str += "--";
        }
        break;
    case '*':
        if (code.peek() == '*') {
            op_str.push_back(code.get());
        }
        if (code.peek() == '=') {
            op_str.push_back(code.get());
        }
        break;
    case '<':
        if (code.peek() == '<') {
            op_str.push_back(code.get());
        }
        if (code.peek() == '=') {
            op_str.push_back(code.get());
        }
        break;
    case '>':
        if (code.peek() == '>') {
            op_str.push_back(code.get());
            if (code.peek() == '>') {
                op_str.push_back(code.get());
            }
        }
        if (code.peek() == '=') {
            op_str.push_back(code.get());
        }
        break;
    }
    if (binary_ops.find(op_str) != binary_ops.end()) {
        op->kind = TokenKind::BINOP;
        op->data = binary_ops[op_str];
    } else if (unary_ops.find(op_str) != unary_ops.end()) {
        op->kind = TokenKind::UNOP;
        op->data = unary_ops[op_str];
    } else {
        std::cerr << "Don't know how to handle operator [" << op_str << "]\n";
    }
    return op;
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

static bool is_number_start(int ch) {
    return std::isdigit(ch) || ch == '.';
}

static bool is_string_literal_start(int ch) {
    return ch == '\'' || ch == '\"';
}

static bool is_punct(int ch) {
    static string punct_chars = "{}[]();:.";
    return punct_chars.find(ch) != string::npos;
};

static bool is_op_start(int ch) {
    static string op_start_chars = "=+-*/%<>&^|!~";
    return op_start_chars.find(ch) != string::npos;
}
