#ifndef _JS_H
#define _JS_H

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* lib */

typedef struct MapEntry
{
	char *key;
	void *value;
	struct MapEntry *next;
} MapEntry;
typedef struct Map
{
	struct MapEntry **bucket;
	int size;
	int entries;
} Map;
Map *Map_new(void);
void Map_set(Map *self, char *key, void *value);
void *Map_get(Map *self, char *key);
void Map_remove(Map *self, char *key);

typedef struct
{
	void **buf;
	int size;
	int capacity;
} Vector;
Vector *Vector_new(void);
void Vector_push(Vector *self, void *item);
void *Vector_get(Vector *self, int index);
void Vector_set(Vector *self, int index, void *item);
void *Vector_pop(Vector *self);

typedef struct
{
	char *buf;
	int size;
	int capacity;
} String;
String *String_new(void);
void String_delete(String *self);
void String_push(String *self, char item);
void String_append(String *self, String *that);

typedef struct
{
	int line;
	int col;
} Pos;
typedef struct
{
	Pos start;
	Pos end;
} Loc;
Loc Loc_make(Pos start, Pos end);

/* domain */

typedef enum
{
	TOK_ID,
	TOK_KEYWORD,
	TOK_INTEGER,
	TOK_DOUBLE,
	TOK_STRING,
	TOK_BOOLEAN,
	TOK_SPREAD,
	TOK_PUNCT,
	TOK_EOT,
	TOK_BAD,
} TokenKind;
typedef union TokenData {
	int integer;
	double doubl;
	int boolean;
	char punct;
	String *id;
} TokenData;
typedef struct
{
	TokenKind kind;
	Loc loc;
	TokenData data;
} Token;
Token *Token_new(TokenKind kind, Loc loc, TokenData data);
typedef struct
{
	char *code;
	int code_size;
	int pos;
	int line;
	int col;
} Lexer;
Lexer *Lexer_new(const char *code, int code_size);
Token *Lexer_next(Lexer *self);
Token *Lexer_peek(Lexer *self);

typedef enum
{
	AST_LIT_BOOLEAN,
	AST_LIT_INTEGER,
	AST_LIT_DOUBLE,
	AST_LIT_STRING,
	AST_LIT_UNDEFINED,
	AST_LIT_NULL,
	AST_LIT_ARRAY,
	AST_LIT_OBJECT,
	AST_ID,
	AST_UN_OP,
	AST_BIN_OP,
	AST_IF_ELSE,
	AST_FOR,
	AST_WHILE,
	AST_RETURN,
	AST_BREAK,
	AST_CONTINUE,
	AST_FUNCTION,
	AST_BLOCK,
	AST_VARDEC,
} ASTKind;
typedef enum
{
	OP_ASSIGN,
	OP_ADD_ASSIGN,
	OP_SUB_ASSIGN,
	OP_MUL_ASSIGN,
	OP_DIV_ASSIGN,
	OP_MOD_ASSIGN,
	OP_EXP_ASSIGN,
	OP_SHL_ASSIGN,
	OP_SHR_ASSIGN,
	OP_SHR_UNSIGNED_ASSIGN,
	OP_BITAND_ASSIGN,
	OP_BITXOR_ASSIGN,
	OP_BITOR_ASSIGN,
	OP_EQ,
	OP_NEQ,
	OP_EQ_SAME,
	OP_NEQ_SAME,
	OP_GT,
	OP_LT,
	OP_GTE,
	OP_LTE,
	OP_MOD,
	OP_BITAND,
	OP_BITOR,
	OP_BITXOR,
	OP_SHL,
	OP_SHR,
	OP_SHR_UNSIGNED,
	OP_LAND,
	OP_LOR,
	OP_IN,
	OP_INSTANCEOF,
	OP_CALL,
} BinOpKind;
typedef enum
{
	OP_PRE_INC,
	OP_PRE_DEC,
	OP_POST_INC,
	OP_POST_DEC,
	OP_NEG,
	OP_PLUS,
	OP_BITNOT,
	OP_DELETE,
	OP_TYPEOF,
	OP_VOID,
	OP_NEW,
	OP_SUPER,
	OP_SPREAD,
} UnOpKind;
static const char *keywords[] = {
    "await",
    "break",
    "byte",
    "case",
    "catch",
    "class",
    "const",
    "continue",
    "default",
    "delete",
    "do",
    "else",
    "eval",
    "export",
    "extends",
    "finally",
    "for",
    "false",
    "if",
    "implements",
    "in",
    "instanceof",
    "let",
    "new",
    "null",
    "return",
    "static",
    "super",
    "switch",
    "this",
    "throw",
    "true",
    "try",
    "typeof",
    "var",
    "void",
    "while",
    "with",
    "yield",
};
typedef struct AST
{
	ASTKind kind;
	Loc loc;
	union {
		int boolean;
		int integer;
		double doubl;
		String *id;
		String *string;
		struct
		{
			int size;
			struct AST **items;
		} arr_lit;
		struct
		{
			Map *data;
		} obj_lit;
		struct
		{
			UnOpKind op;
			struct AST *expr;
		} unary;
		struct
		{
			BinOpKind op;
			struct AST *left;
			struct AST *right;
		} binary;
		struct
		{
			struct AST *cond;
			struct AST *then;
			struct AST *els;
		} if_else;
		struct
		{
			struct AST *init;
			struct AST *cond;
			struct AST *mod;
			struct AST *body;
		} for_loop;
		struct
		{
			struct AST *cond;
			struct AST *body;
		} while_loop;
		struct
		{
			struct AST *value;
		} return_stmt;
		struct Args
		{
			struct AST **args;
			int size;
		} args;
		struct
		{
			char *name;
			struct Args args;
			int args_size;
			struct AST *body;
		} function;
		struct
		{
			struct AST **stmts;
			int size;
		} block;
		struct
		{
			struct AST *id;
			struct AST *init;
			int type;
		} var_dec;
	};
} AST;

typedef struct Object
{
	struct Object *prototype;
	Map *properties;
	String *type_str;
} Object;

typedef struct Env
{
	struct Env *parent;
	struct Env **children;
	int children_size;
} Env;

#endif
