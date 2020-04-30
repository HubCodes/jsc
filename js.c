#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MapEntry {
    char* key;
    void* value;
    struct MapEntry* next;
};
typedef struct MapEntry MapEntry;
struct Map {
    MapEntry** bucket;
    int size;
    int entries;
};
typedef struct Map Map;
Map* Map_new(void);
void Map_set(Map* this, char* key, void* value);
void* Map_get(Map* this, char* key);
void Map_remove(Map* this, char* key);

struct Loc {
    int start_line;
    int start_col;
    int end_line;
    int end_col;
};
typedef struct Loc Loc;
enum ASTKind {
    AST_LIT_BOOLEAN,
    AST_LIT_INTEGER,
    AST_LIT_DOUBLE,
    AST_LIT_STRING,
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
};
typedef enum ASTKind ASTKind;
enum BinOpKind {
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
};
typedef enum BinOpKind BinOpKind;
enum UnOpKind {
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
};
typedef enum UnOpKind UnOpKind;
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
struct AST {
    ASTKind kind;
    Loc loc;
    union {
        int boolean;
        int integer;
        double doubl;
        char* string;
        char* id;
        struct ArrayLit {
            int size;
            struct AST** items;
        } arr_lit;
        struct ObjectLit {
            Map* data;
        } obj_lit;
        struct UnOp {
            UnOpKind op;
            struct AST* expr;
        } unary;
        struct BinOp {
            BinOpKind op;
            struct AST* left;
            struct AST* right;
        } binary;
        struct IfElse {
            struct AST* cond;
            struct AST* then;
            struct AST* els;
        } if_else;
        struct For {
            struct AST* init;
            struct AST* cond;
            struct AST* mod;
            struct AST* body;
        } for_loop;
        struct While {
            struct AST* cond;
            struct AST* body;
        } while_loop;
        struct Return {
            struct AST* value;
        } return_stmt;
        struct Args {
            struct AST** args;
            int size;
        } args;
        struct Function {
            char* name;
            struct Args args;
            int args_size;
            struct AST* body;
        } function;
        struct Block {
            struct AST** stmts;
            int size;
        } block;
        struct VarDec {
            struct AST* id;
            struct AST* init;
            int type;
        } var_dec;
    };
};
typedef struct AST AST;

struct Object {
    struct Object* prototype;
    Map* properties;
    char* type_str;
};
typedef struct Object Object;

struct Env {
    struct Env* parent;
    struct Env** children;
    int children_size;
};
typedef struct Env Env;

int main(int argc, char** argv) {
    return 0;
}

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
