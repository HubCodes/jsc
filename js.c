#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MapEntry {
    char* key;
    void* value;
    struct MapEntry* next;
} MapEntry;
typedef struct {
    MapEntry** bucket;
    int size;
    int entries;
} Map;
Map* Map_new(void);
void Map_set(Map* this, char* key, void* value);
void* Map_get(Map* this, char* key);
void Map_remove(Map* this, char* key);

typedef struct {
    void** buf;
    int size;
    int capacity;
} Vector;
Vector* Vector_new(void);
void Vector_push(Vector* this, void* item);
void* Vector_get(Vector* this, int index);
void Vector_set(Vector* this, int index, void* item);
void* Vector_pop(Vector* this);

typedef struct {
    int start_line;
    int start_col;
    int end_line;
    int end_col;
} Loc;

typedef enum {
    TOK_ID,
    TOK_KEYWORD,
    TOK_INTEGER,
    TOK_DOUBLE,
    TOK_STRING,
    TOK_BOOLEAN,
    TOK_SPREAD,
    TOK_EOT,
    TOK_BAD,
} TokenKind;
typedef struct {
    TokenKind kind;
    Loc loc;
    union {
        int integer;
        double doubl;
        int boolean;
        char punct;
        struct {
            char* str;
            int size;
        };
    };
} Token;
typedef struct {
    char* code;
    int code_size;
    int pos;
    int line;
    int col;
} Lexer;
Lexer* Lexer_new(const char* code, int code_size);
Token* Lexer_next(Lexer* this);
Token* Lexer_peek(Lexer* this);

typedef enum {
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
typedef enum {
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
typedef enum {
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
const char* keywords[] = {
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
typedef struct AST {
    ASTKind kind;
    Loc loc;
    union {
        int boolean;
        int integer;
        double doubl;
        char* id;
        struct {
            char* str;
            int size;
        } string;
        struct {
            int size;
            struct AST** items;
        } arr_lit;
        struct {
            Map* data;
        } obj_lit;
        struct {
            UnOpKind op;
            struct AST* expr;
        } unary;
        struct {
            BinOpKind op;
            struct AST* left;
            struct AST* right;
        } binary;
        struct {
            struct AST* cond;
            struct AST* then;
            struct AST* els;
        } if_else;
        struct {
            struct AST* init;
            struct AST* cond;
            struct AST* mod;
            struct AST* body;
        } for_loop;
        struct {
            struct AST* cond;
            struct AST* body;
        } while_loop;
        struct {
            struct AST* value;
        } return_stmt;
        struct Args {
            struct AST** args;
            int size;
        } args;
        struct {
            char* name;
            struct Args args;
            int args_size;
            struct AST* body;
        } function;
        struct {
            struct AST** stmts;
            int size;
        } block;
        struct {
            struct AST* id;
            struct AST* init;
            int type;
        } var_dec;
    };
} AST;

typedef struct Object {
    struct Object* prototype;
    Map* properties;
    char* type_str;
} Object;

typedef struct Env {
    struct Env* parent;
    struct Env** children;
    int children_size;
} Env;

/*
 * THIS IS MAIN!
 *
 * int main(int argc, char** argv)
 */

int main(int argc, char** argv) {
    return 0;
}

/*
 * Map: HashMap implementation
 *
 * Separate chaining with linked list
 */

#define MAP_SIZE 128

Map* Map_new(void) {
    Map* this = calloc(sizeof(Map), 1);
    this->bucket = calloc(sizeof(MapEntry*), MAP_SIZE);
    this->size = MAP_SIZE;
    this->entries = 0;
    return this;
}

static unsigned int string_hash(char* str) {
    unsigned int value = 37;
    while (*str) {
        value = (value * 54059) ^ (*str * 76963);
        str++;
    }
    return value;
}

static void Map_rehash(Map* this) {
    double load_factor = (double) this->entries / (double) this->size;
    if (load_factor < 0.75) return;
    int old_size = this->size;
    int new_size = old_size * 2;
    MapEntry** old_bucket = this->bucket;
    MapEntry** new_bucket = calloc(sizeof(MapEntry*), new_size);
    this->bucket = new_bucket;
    this->entries = 0;
    this->size = new_size;
    for (int i = 0; i < old_size; i++) {
        MapEntry* entry = old_bucket[i];
        MapEntry* prev = NULL;
        while (entry) {
            Map_set(this, entry->key, entry->value);
            prev = entry;
            entry = entry->next;
            free(prev);
        }
    }
    free(old_bucket);
}

void Map_set(Map* this, char* key, void* value) {
    Map_rehash(this);
    unsigned int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    MapEntry* new_entry = calloc(sizeof(MapEntry), 1);
    MapEntry* prev_entry = NULL;
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = NULL;
    if (!table_entry) {
        this->bucket[key_index] = new_entry;
        this->entries++;
        return;
    }
    while (table_entry) {
        int is_same_key = strcmp(key, table_entry->key) == 0;
        if (is_same_key && prev_entry) {
            new_entry->next = table_entry->next;
            prev_entry->next = new_entry;
            free(table_entry);
            return;
        } else if (is_same_key) {
            this->bucket[key_index] = new_entry;
            free(table_entry);
            return;
        }
        prev_entry = table_entry;
        table_entry = table_entry->next;
    }
    prev_entry->next = new_entry;
    this->entries++;
}

void* Map_get(Map* this, char* key) {
    unsigned int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    while (table_entry) {
        int is_same_key = strcmp(key, table_entry->key) == 0;
        if (is_same_key) return table_entry->value;
        table_entry = table_entry->next;
    }
    return NULL;
}

void Map_remove(Map* this, char* key) {
    unsigned int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    MapEntry* prev_entry = NULL;
    while (table_entry) {
        int is_same_key = strcmp(key, table_entry->key) == 0;
        if (is_same_key && prev_entry) {
            this->entries--;
            prev_entry->next = table_entry->next;
            free(table_entry);
            return;
        } else if (is_same_key) {
            this->entries--;
            this->bucket[key_index] = table_entry->next;
            free(table_entry);
            return;
        }
        prev_entry = table_entry;
        table_entry = table_entry->next;
    }
}

#undef MAP_SIZE

/*
 * Vector: Variable length array
 */

#define VECTOR_CAPACITY 8

static void Vector_extend(Vector* this);

Vector* Vector_new(void) {
    Vector* this = calloc(sizeof(Vector), 1);
    this->buf = calloc(sizeof(void*), VECTOR_CAPACITY);
    this->size = 0;
    this->capacity = VECTOR_CAPACITY;
    return this;
}

void Vector_push(Vector* this, void* item) {
    if (this->size >= this->capacity) Vector_extend(this);
    this->buf[this->size] = item;
    this->size++;
}

void* Vector_get(Vector* this, int index) {
    if (index >= this->size) return NULL;
    return this->buf[index];
}

void Vector_set(Vector* this, int index, void* item) {
    if (index >= this->size && index < 0) return;
    this->buf[index] = item;
}

void* Vector_pop(Vector* this) {
    int last_index = this->size - 1;
    void* item = Vector_get(this, last_index);
    Vector_set(this, last_index, NULL);
    this->size--;
    return item;
}

static void Vector_extend(Vector* this) {
    int old_capacity = this->capacity;
    int new_capacity = old_capacity * 2;
    void** old_buf = this->buf;
    this->buf = calloc(sizeof(void*), new_capacity);
    this->capacity = new_capacity;
    memcpy((void*) this->buf, old_buf, old_capacity * sizeof(void*));
}

#undef VECTOR_CAPACITY

/*
 * Lexer: Tokenize raw source code into Token
 */
/*
Lexer* Lexer_new(const char* code, int code_size) {
    Lexer* this = calloc(sizeof(Lexer), 1);
    this->code = calloc(sizeof(char), code_size);
    this->code_size = code_size;
    this->pos = 0;
    this->line = 0;
    this->col = 0;
    strncpy(this->code, code, code_size);
    return this;
}

static int Lexer_get_char(Lexer* this);
static int Lexer_peek_char(Lexer* this);
static void Lexer_skip_whitespace(Lexer* this);
static Token* Lexer_get_id(Lexer* this);

Token* Lexer_next(Lexer* this) {
    Lexer_skip_whitespace(this);
    // int ch = Lexer_peek_char(this);
    return NULL;
}

static void Lexer_skip_whitespace(Lexer* this) {
    int ch;
    while (isspace(ch = Lexer_peek_char(this))) Lexer_get_char(this);
}

static Token* Lexer_get_id(Lexer* this) {
    return NULL;
}
*/
