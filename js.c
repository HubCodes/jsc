#include <stdio.h>

struct MapEntry {
    void* value;
    struct MapEntry* next;
};
typedef struct MapEntry MapEntry;
struct Map {
    MapEntry** bucket;
};
typedef struct Map Map;
Map* Map_new(void);
void Map_set(Map* this, char* key, void* value);
void* Map_get(Map* this, char* key);
void Map_remove(Map* this, char* key);

struct Object {
    struct Object* prototype;
};
typedef struct Object Object;

struct Env {
    struct Env* parent;
    struct Env** children;
    int children_length;
};
typedef struct Env Env;

int main(int argc, char** argv) {
    return 0;
}
