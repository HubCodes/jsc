#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

template <typename T>
using owned = std::unique_ptr<T>;
template <typename T>
using shared = std::shared_ptr<T>;
template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;
using std::move;
using std::string;
using std::vector;

struct Pos {
    int line;
    int col;
};

struct Loc {
    Pos start;
    Pos end;
};

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
    MOD,
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
    ARROW,
    BINOP_ILLEGAL,
};

enum class UnOpKind {
    PRE_INC,
    PRE_DEC,
    POST_INC,
    POST_DEC,
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

using TokenData = std::variant<
    int,
    double,
    bool,
    char,
    string,
    BinOpKind,
    UnOpKind>;

struct Token {
    TokenKind kind;
    Loc loc;
    TokenData data;
};

class Lexer {
public:
    Lexer(std::stringstream&& code);
    owned<Token> next();
    owned<Token> peek();
private:
    std::stringstream code;
    Pos current;
};

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
    IfElse(owned<Expression> cond, owned<Statement> then, owned<Statement> els);
    virtual ~IfElse();
    virtual bool is_if_else() override { return true; }
private:
    owned<Expression> cond;
    owned<Statement> then;
    owned<Statement> els;
};

class For final : public Statement {
public:
    For(
        owned<Statement> init,
        owned<Expression> cond,
        owned<Expression> inc,
        owned<Statement> body);
    virtual ~For();
    virtual bool is_for() override { return true; }
    virtual bool is_breakable() override { return true; }
private:
    owned<Statement> init;
    owned<Expression> cond;
    owned<Expression> inc;
    owned<Statement> body;
};

class While final : public Statement {
public:
    While(owned<Expression> cond, owned<Statement> body);
    virtual ~While();
    virtual bool is_while() override { return true; }
    virtual bool is_breakable() override { return true; }
private:
    owned<Expression> cond;
    owned<Statement> body;
};

class Return final : public Statement {
public:
    Return(owned<Expression> value);
    virtual ~Return();
    virtual bool is_return() override { return true; }
private:
    owned<Expression> value;
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

class TryCatch final : public Statement {
public:
    TryCatch(
        owned<Statement> trying,
        owned<Catch> catching,
        owned<Statement> finally);
    virtual ~TryCatch();
    virtual bool is_try_catch() override { return true; }
private:
    owned<Statement> trying;
    owned<Catch> catching;
    owned<Statement> finally;
};

class Catch final : public Statement {
public:
    Catch(owned<ID> exn, owned<Statement> catching);
    virtual ~Catch();
private:
    owned<ID> exn;
    owned<Statement> catching;
};

class Block final : public Statement {
public:
    Block(owned<vector<Statement>> body);
    virtual ~Block();
    virtual bool is_block() override { return true; }
private:
    owned<vector<Statement>> body;
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
    owned<string> identifier;
};

class Literal final : public Expression {
public:
    using ArrayValue = vector<owned<Expression>>;
    using ObjectValue = hash_map<string, owned<Expression>>;
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
    vector<owned<Expression>> get_array() const;
    bool is_object() const;
    virtual bool is_literal() override { return true; }
private:
    LiteralValue value;
};

class BinOp final : public Expression {
public:
    BinOp(BinOpKind op, owned<Expression> left, owned<Expression> right);
    virtual ~BinOp();
    virtual bool is_binary() override { return true; }
private:
    BinOpKind op;
    owned<Expression> left;
    owned<Expression> right;
};

class UnOp final : public Expression {
public:
    UnOp(UnOpKind op, owned<Expression> expr);
    virtual ~UnOp();
    virtual bool is_unary() override { return true; }
private:
    UnOpKind op;
    owned<Expression> expr;
};

class Function final : public Expression {
public:
    Function(owned<Args> args, owned<Statement> body);
    virtual ~Function();
    virtual bool is_function() { return true; }
};

class Args final : public Expression {
public:
    Args(owned<vector<Expression>> args);
    virtual ~Args();
    virtual bool is_args() override { return true; }
private:
    owned<vector<Expression>> args;
};

int main(int argc, char** argv) {
    return 0;
}
