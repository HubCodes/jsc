#include <cassert>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

template <typename T>
using unique = std::unique_ptr<T>;
template <typename T>
using shared = std::shared_ptr<T>;
template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;
using std::make_unique;
using std::move;
using std::string;
using std::vector;

enum class BinOpKind {
    ASSIGN,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    EXP_ASSIGN,
    SHL_ASSIGN,
    SHR_ASSIGN,
    SHR_UNSIGNED_ASSIGN,
    BITAND_ASSIGN,
    BITXOR_ASSIGN,
    BITOR_ASSIGN,
    EQ,
    NEQ,
    EQ_SAME,
    NEQ_SAME,
    GT,
    LT,
    GTE,
    LTE,
    BITAND,
    BITOR,
    BITXOR,
    SHL,
    SHR,
    SHR_UNSIGNED,
    LAND,
    LOR,
    IN,
    INSTANCEOF,
    CALL,
    SUBSCRIPT,
    ARROW,
    BINOP_ILLEGAL,
};

hash_map<string, BinOpKind> binary_ops = {
    {"=", BinOpKind::ASSIGN},
    {"+", BinOpKind::ADD},
    {"-", BinOpKind::SUB},
    {"*", BinOpKind::MUL},
    {"/", BinOpKind::DIV},
    {"%", BinOpKind::MOD},
    {"+=", BinOpKind::PLUS_ASSIGN},
    {"-=", BinOpKind::MINUS_ASSIGN},
    {"*=", BinOpKind::MUL_ASSIGN},
    {"/=", BinOpKind::DIV_ASSIGN},
    {"%=", BinOpKind::MOD_ASSIGN},
    {"**=", BinOpKind::EXP_ASSIGN},
    {"<<=", BinOpKind::SHL_ASSIGN},
    {">>=", BinOpKind::SHR_ASSIGN},
    {">>>=", BinOpKind::SHR_UNSIGNED_ASSIGN},
    {"&=", BinOpKind::BITAND_ASSIGN},
    {"^=", BinOpKind::BITXOR_ASSIGN},
    {"|=", BinOpKind::BITOR_ASSIGN},
    {"==", BinOpKind::EQ},
    {"!=", BinOpKind::NEQ},
    {"===", BinOpKind::EQ_SAME},
    {"!==", BinOpKind::NEQ_SAME},
    {">", BinOpKind::GT},
    {"<", BinOpKind::LT},
    {">=", BinOpKind::GTE},
    {"<=", BinOpKind::LTE},
    {"&", BinOpKind::BITAND},
    {"|", BinOpKind::BITOR},
    {"^", BinOpKind::BITXOR},
    {"<<", BinOpKind::SHL},
    {">>", BinOpKind::SHR},
    {">>>", BinOpKind::SHR_UNSIGNED},
    {"&&", BinOpKind::LAND},
    {"||", BinOpKind::LOR},
    {"in", BinOpKind::IN},
    {"instanceof", BinOpKind::INSTANCEOF},
    {"=>", BinOpKind::ARROW}};

enum class UnOpKind {
    INC,
    DEC,
    NEG,
    PLUS,
    BITNOT,
    DELETE,
    TYPEOF,
    VOID,
    NEW,
    SUPER,
    SPREAD,
    UNOP_ILLEGAL,
};

hash_map<string, UnOpKind> unary_ops = {
    {"++", UnOpKind::INC},
    {"--", UnOpKind::DEC},
    {"-", UnOpKind::NEG},
    {"+", UnOpKind::PLUS},
    {"~", UnOpKind::BITNOT},
    {"delete", UnOpKind::DELETE},
    {"typeof", UnOpKind::TYPEOF},
    {"void", UnOpKind::VOID},
    {"new", UnOpKind::NEW},
    {"super", UnOpKind::SUPER},
    {"...", UnOpKind::SPREAD}};

enum class TokenKind {
    ID,
    KEYWORD,
    INTEGER,
    DOUBLE,
    STRING,
    PUNCT,
    OP,
    BOOLEAN,
    SPREAD,
    EOT,
    BAD,
};

enum class KeywordKind {
    AWAIT,
    BREAK,
    CASE,
    CATCH,
    CLASS,
    CONST,
    CONTINUE,
    DEFAULT,
    DO,
    ELSE,
    EVAL,
    EXPORT,
    EXTENDS,
    FINALLY,
    FOR,
    IF,
    IMPLEMENTS,
    LET,
    NULL_,
    RETURN,
    STATIC,
    SUPER,
    SWITCH,
    THIS,
    THROW,
    TRY,
    VAR,
    WHILE,
    WITH,
    YIELD,
};

hash_map<string, KeywordKind> keywords = {
    {"await", KeywordKind::AWAIT},
    {"break", KeywordKind::BREAK},
    {"case", KeywordKind::CASE},
    {"catch", KeywordKind::CATCH},
    {"class", KeywordKind::CLASS},
    {"const", KeywordKind::CONST},
    {"continue", KeywordKind::CONTINUE},
    {"default", KeywordKind::DEFAULT},
    {"do", KeywordKind::DO},
    {"else", KeywordKind::ELSE},
    {"eval", KeywordKind::EVAL},
    {"export", KeywordKind::EXPORT},
    {"extends", KeywordKind::EXTENDS},
    {"finally", KeywordKind::FINALLY},
    {"for", KeywordKind::FOR},
    {"if", KeywordKind::IF},
    {"implements", KeywordKind::IMPLEMENTS},
    {"let", KeywordKind::LET},
    {"null", KeywordKind::NULL_},
    {"return", KeywordKind::RETURN},
    {"static", KeywordKind::STATIC},
    {"super", KeywordKind::SUPER},
    {"switch", KeywordKind::SWITCH},
    {"this", KeywordKind::THIS},
    {"throw", KeywordKind::THROW},
    {"try", KeywordKind::TRY},
    {"var", KeywordKind::VAR},
    {"while", KeywordKind::WHILE},
    {"with", KeywordKind::WITH},
    {"yield", KeywordKind::YIELD}};

using TokenData = std::variant<
    int,
    double,
    bool,
    char,
    string,
    BinOpKind,
    UnOpKind,
    KeywordKind>;

struct Token {
    TokenKind kind;
    TokenData data;
};

class Lexer {
public:
    Lexer(std::stringstream&& code);
    unique<Token> next();
    unique<Token> peek();
private:
    unique<Token> get_id();
    unique<Token> get_number();
    unique<Token> get_string_literal();
    unique<Token> get_punct();
    unique<Token> get_op();
    void skip_whitespace();
    string get_exp();

    std::stringstream code;
};

class Statement;
class Expression;

class ASTNode {
public:
    virtual ~ASTNode() = 0;
};

class Statement : public ASTNode {
public:
    virtual ~Statement() = 0;
    virtual bool is_if_else() { return false; }
    virtual bool is_for() { return false; }
    virtual bool is_while() { return false; }
    virtual bool is_return() { return false; }
    virtual bool is_break() { return false; }
    virtual bool is_continue() { return false; }
    virtual bool is_try_catch() { return false; }
    virtual bool is_breakable() { return false; }
    virtual bool is_block() { return true; }
};

class IfElse final : public Statement {
public:
    IfElse(unique<Expression> cond, unique<Statement> then, unique<Statement> els);
    virtual ~IfElse();
    virtual bool is_if_else() override { return true; }
private:
    unique<Expression> cond;
    unique<Statement> then;
    unique<Statement> els;
};

class For final : public Statement {
public:
    For(
        unique<Statement> init,
        unique<Expression> cond,
        unique<Expression> inc,
        unique<Statement> body);
    virtual ~For();
    virtual bool is_for() override { return true; }
    virtual bool is_breakable() override { return true; }
private:
    unique<Statement> init;
    unique<Expression> cond;
    unique<Expression> inc;
    unique<Statement> body;
};

class While final : public Statement {
public:
    While(unique<Expression> cond, unique<Statement> body);
    virtual ~While();
    virtual bool is_while() override { return true; }
    virtual bool is_breakable() override { return true; }
private:
    unique<Expression> cond;
    unique<Statement> body;
};

class Return final : public Statement {
public:
    Return(unique<Expression> value);
    virtual ~Return();
    virtual bool is_return() override { return true; }
private:
    unique<Expression> value;
};

class Break final : public Statement {
public:
    Break();
    virtual ~Break();
    virtual bool is_break() override { return true; }
};

class Continue final : public Statement {
public:
    Continue();
    virtual ~Continue();
    virtual bool is_continue() override { return true; }
};

class ID;

class Catch final : public Statement {
public:
    Catch(unique<ID> exn, unique<Statement> catching);
    virtual ~Catch();
private:
    unique<ID> exn;
    unique<Statement> catching;
};

class TryCatch final : public Statement {
public:
    TryCatch(
        unique<Statement> trying,
        unique<Catch> catching,
        unique<Statement> finally);
    virtual ~TryCatch();
    virtual bool is_try_catch() override { return true; }
private:
    unique<Statement> trying;
    unique<Catch> catching;
    unique<Statement> finally;
};

class Block final : public Statement {
public:
    Block(unique<vector<Statement>> body);
    virtual ~Block();
    virtual bool is_block() override { return true; }
private:
    unique<vector<Statement>> body;
};

class Expression : public ASTNode {
public:
    virtual ~Expression() = 0;
    virtual bool is_id() { return false; }
    virtual bool is_literal() { return false; }
    virtual bool is_unary() { return false; }
    virtual bool is_binary() { return false; }
    virtual bool is_function() { return false; }
    virtual bool is_args() { return false; }
};

class ID final : public Expression {
public:
    ID(const string& identifier);
    virtual ~ID();
    virtual bool is_id() override { return true; }
private:
    unique<string> identifier;
};

class Literal : public Expression {
public:
    using ArrayValue = vector<unique<Expression>>;
    using ObjectValue = hash_map<string, unique<Expression>>;
    using LiteralValue = std::variant<
        bool,
        int,
        double,
        string,
        ArrayValue,
        ObjectValue>;
    Literal(bool value);
    Literal(int value);
    Literal(double value);
    Literal(string value);
    Literal(ArrayValue value);
    Literal(ObjectValue value);
    virtual ~Literal() = 0;
    bool is_bool() const;
    bool get_bool() const;
    bool is_integer() const;
    int get_integer() const;
    bool is_double() const;
    double get_double() const;
    bool is_string() const;
    string get_string() const;
    bool is_array() const;
    vector<unique<Expression>> get_array() const;
    bool is_object() const;
    virtual bool is_literal() override { return true; }
private:
    LiteralValue value;
};

class BinOp final : public Expression {
public:
    BinOp(BinOpKind op, unique<Expression> left, unique<Expression> right);
    virtual ~BinOp();
    virtual bool is_binary() override { return true; }
private:
    BinOpKind op;
    unique<Expression> left;
    unique<Expression> right;
};

class UnOp final : public Expression {
public:
    UnOp(UnOpKind op, unique<Expression> expr);
    virtual ~UnOp();
    virtual bool is_unary() override { return true; }
private:
    UnOpKind op;
    unique<Expression> expr;
};

class Args final : public Expression {
public:
    Args(unique<vector<Expression>> args);
    virtual ~Args();
    virtual bool is_args() override { return true; }
private:
    unique<vector<Expression>> args;
};

class Function final : public Expression {
public:
    Function(unique<ID> name, unique<Args> args, unique<Block> body);
    virtual ~Function();
    virtual bool is_function() { return true; }
private:
    unique<ID> name;
    unique<Args> args;
    unique<Block> body;
};

int main(int argc, char** argv) {
    return 0;
}

/* Lexer */

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
