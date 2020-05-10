#include "js.hpp"

using std::get;

static bool is_function_start(const unique<Token>& token);
static bool is_variable_declaration_start(const unique<Token>& token);
static bool is_punct(const unique<Token>& token, char ch);
static bool is_array_open(const unique<Token>& token);
static bool is_object_open(const unique<Token>& token);
static bool is_assign_op(const unique<Token>& token);
static bool is_logical_or_op(const unique<Token>& token);
static bool is_logical_and_op(const unique<Token>& token);
static bool is_bitwise_or_op(const unique<Token>& token);
static bool is_bitwise_xor_op(const unique<Token>& token);
static bool is_bitwise_and_op(const unique<Token>& token);
static bool is_equality_op(const unique<Token>& token);
static bool is_comparison_op(const unique<Token>& token);
static bool is_bitshift_op(const unique<Token>& token);
static bool is_addition_op(const unique<Token>& token);
static bool is_multiplication_op(const unique<Token>& token);
static bool is_exponentiation_op(const unique<Token>& token);
static bool is_unary_op(const unique<Token>& token);
static bool is_postfix_unary_op(const unique<Token>& token);

Parser::Parser(unique<Lexer> lexer): lexer(move(lexer)) {}

unique<vector<unique<ASTNode>>> Parser::parse() {
    auto result = make_unique<vector<unique<ASTNode>>>();
    auto token = lexer->peek();
    while (token->kind != TokenKind::EOT) {
        if (is_function_start(token)) {
            result->emplace_back(get_function());
        } else if (is_variable_declaration_start(token)) {
            result->emplace_back(get_variable_declaration());
        } else if (token->kind == TokenKind::BAD) {
            return result;
        } else {
            result->emplace_back(get_expression());
        }
    }
    return result;
}

unique<Function> Parser::get_function() {
    return nullptr;
}

unique<VariableDeclaration> Parser::get_variable_declaration() {
    return nullptr;
}

unique<Expression> Parser::get_expression() {
    auto token = lexer->peek();
    if (is_array_open(token)) {

    } else if (is_object_open(token)) {

    } else if (is_function_start(token)) {
        return get_function();
    }
    return get_assign();
}

unique<Expression> Parser::get_assign() {
    auto lhs = get_logical_or();
    auto maybe_op = lexer->peek();
    while (is_assign_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_logical_or());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_logical_or() {
    auto lhs = get_logical_and();
    auto maybe_op = lexer->peek();
    while (is_logical_or_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_logical_and());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_logical_and() {
    auto lhs = get_bitwise_or();
    auto maybe_op = lexer->peek();
    while (is_logical_and_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_bitwise_or());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_bitwise_or() {
    auto lhs = get_bitwise_xor();
    auto maybe_op = lexer->peek();
    while (is_bitwise_or_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_bitwise_xor());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_bitwise_xor() {
    auto lhs = get_bitwise_and();
    auto maybe_op = lexer->peek();
    while (is_bitwise_xor_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_bitwise_and());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_bitwise_and() {
    auto lhs = get_equality();
    auto maybe_op = lexer->peek();
    while (is_bitwise_and_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_equality());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_equality() {
    auto lhs = get_comparison();
    auto maybe_op = lexer->peek();
    while (is_equality_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_comparison());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_comparison() {
    auto lhs = get_bitshift();
    auto maybe_op = lexer->peek();
    while (is_comparison_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_bitshift());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_bitshift() {
    auto lhs = get_addition();
    auto maybe_op = lexer->peek();
    while (is_bitshift_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_addition());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_addition() {
    auto lhs = get_multiplication();
    auto maybe_op = lexer->peek();
    while (is_addition_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_multiplication());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_multiplication() {
    auto lhs = get_exponentiation();
    auto maybe_op = lexer->peek();
    while (is_multiplication_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_multiplication());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_exponentiation() {
    auto lhs = get_unary();
    auto maybe_op = lexer->peek();
    while (is_exponentiation_op(maybe_op)) {
        lexer->next();
        auto op = get<BinOpKind>(maybe_op->data);
        lhs = make_unique<BinOp>(op, move(lhs), get_unary());
        maybe_op = lexer->peek();
    }
    return lhs;
}

unique<Expression> Parser::get_unary() {
    auto maybe_op = lexer->peek();
    if (is_unary_op(maybe_op)) {
        lexer->next();
        auto op = get<UnOpKind>(maybe_op->data);
        auto rhs = get_unary();
        return make_unique<UnOp>(op, move(rhs));
    }
    return get_postfix_unary();
}

unique<Expression> Parser::get_postfix_unary() {
    auto lhs = get_new_without_args();
    auto maybe_op = lexer->peek();
    if (is_postfix_unary_op(maybe_op)) {
        lexer->next();
        auto op = get<UnOpKind>(maybe_op->data);
        return make_unique<UnOp>(op, move(lhs));
    }
    return lhs;
}

static bool is_function_start(const unique<Token>& token) {
    return (
        token->kind == TokenKind::KEYWORD
        && get<KeywordKind>(token->data) == KeywordKind::FUNCTION);
}

static bool is_variable_declaration_start(const unique<Token>& token) {
    auto is_variable_start = [&token]() {
        auto keyword = get<KeywordKind>(token->data);
        bool is_var_let_const =
            keyword == KeywordKind::VAR
            || keyword == KeywordKind::LET
            || keyword == KeywordKind::CONST;
        return is_var_let_const;
    };
    return token->kind == TokenKind::KEYWORD && is_variable_start();
}

static bool is_punct(const unique<Token>& token, char ch) {
    return token->kind == TokenKind::PUNCT && get<char>(token->data) == ch;
}

static bool is_array_open(const unique<Token>& token) {
    return is_punct(token, '[');
}

static bool is_object_open(const unique<Token>& token) {
    return is_punct(token, '{');
}

static bool is_assign_op(const unique<Token>& token) {
    auto is_assign = [&token]() {
        auto op = get<BinOpKind>(token->data);
        bool is_assign_operator =
            op == BinOpKind::ASSIGN
            || op == BinOpKind::PLUS_ASSIGN
            || op == BinOpKind::MINUS_ASSIGN
            || op == BinOpKind::MUL_ASSIGN
            || op == BinOpKind::DIV_ASSIGN
            || op == BinOpKind::MOD_ASSIGN
            || op == BinOpKind::EXP_ASSIGN
            || op == BinOpKind::SHL_ASSIGN
            || op == BinOpKind::SHR_ASSIGN
            || op == BinOpKind::SHR_UNSIGNED_ASSIGN
            || op == BinOpKind::BITAND_ASSIGN
            || op == BinOpKind::BITXOR_ASSIGN
            || op == BinOpKind::BITOR_ASSIGN;
        return is_assign_operator;
    };
    return token->kind == TokenKind::BINOP && is_assign();
}

static bool is_logical_or_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::LOR);
}

static bool is_logical_and_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::LAND);
}

static bool is_bitwise_or_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::BITOR);
}

static bool is_bitwise_xor_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::BITXOR);
}

static bool is_bitwise_and_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::BITAND);
}

static bool is_equality_op(const unique<Token>& token) {
    auto is_equality = [&token]() {
        auto op_kind = get<BinOpKind>(token->data);
        return (
            op_kind == BinOpKind::EQ
            || op_kind == BinOpKind::EQ_SAME
            || op_kind == BinOpKind::NEQ
            || op_kind == BinOpKind::NEQ_SAME);
    };
    return token->kind == TokenKind::BINOP && is_equality();
}

static bool is_comparison_op(const unique<Token>& token) {
    auto is_comparison = [&token]() {
        auto op_kind = get<BinOpKind>(token->data);
        return (
            op_kind == BinOpKind::GT
            || op_kind == BinOpKind::GTE
            || op_kind == BinOpKind::LT
            || op_kind == BinOpKind::LTE
            || op_kind == BinOpKind::IN
            || op_kind == BinOpKind::INSTANCEOF);
    };
    return token->kind == TokenKind::BINOP && is_comparison();
}

static bool is_bitshift_op(const unique<Token>& token) {
    auto is_bitshift = [&token]() {
        auto op_kind = get<BinOpKind>(token->data);
        return (
            op_kind == BinOpKind::SHL
            || op_kind == BinOpKind::SHR
            || op_kind == BinOpKind::SHR_UNSIGNED);
    };
    return token->kind == TokenKind::BINOP && is_bitshift();
}

static bool is_addition_op(const unique<Token>& token) {
    auto is_addition = [&token]() {
        auto op_kind = get<BinOpKind>(token->data);
        return op_kind == BinOpKind::ADD || op_kind == BinOpKind::SUB;
    };
    return token->kind == TokenKind::BINOP && is_addition();
}

static bool is_multiplication_op(const unique<Token>& token) {
    auto is_multiplication = [&token]() {
        auto op_kind = get<BinOpKind>(token->data);
        return (
            op_kind == BinOpKind::MUL
            || op_kind == BinOpKind::DIV
            || op_kind == BinOpKind::MOD);
    };
    return token->kind == TokenKind::BINOP && is_multiplication();
}

static bool is_exponentiation_op(const unique<Token>& token) {
    return (
        token->kind == TokenKind::BINOP
        && get<BinOpKind>(token->data) == BinOpKind::EXP);
}

static bool is_unary_op(const unique<Token>& token) {
    if (std::holds_alternative<BinOpKind>(token->data)) {
        auto op = get<BinOpKind>(token->data);
        if (op == BinOpKind::ADD || op == BinOpKind::SUB) {
            auto actual_op =
                op == BinOpKind::ADD ? UnOpKind::PLUS : UnOpKind::MINUS;
            token->data = actual_op;
        }
    }
    auto is_unary = [&token]() {
        auto op_kind = get<UnOpKind>(token->data);
        return (
            op_kind == UnOpKind::NOT
            || op_kind == UnOpKind::BITNOT
            || op_kind == UnOpKind::PLUS
            || op_kind == UnOpKind::MINUS
            || op_kind == UnOpKind::INC
            || op_kind == UnOpKind::DEC
            || op_kind == UnOpKind::TYPEOF
            || op_kind == UnOpKind::VOID
            || op_kind == UnOpKind::DELETE);
    };
    return (
        (token->kind == TokenKind::BINOP || token->kind == TokenKind::UNOP)
        && is_unary());
}

static bool is_postfix_unary_op(const unique<Token>& token) {
    auto is_postfix_unary = [&token]() {
        auto op = get<UnOpKind>(token->data);
        return op == UnOpKind::INC || op == UnOpKind::DEC;
    };
    return token->kind == TokenKind::UNOP && is_postfix_unary();
}
