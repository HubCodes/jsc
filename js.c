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

#define MAP_SIZE 128

Map* Map_new(void) {
    Map* this = calloc(sizeof(Map), 1);
    this->bucket = calloc(sizeof(MapEntry*), MAP_SIZE);
    this->size = MAP_SIZE;
    this->entries = 0;
    return this;
}

static int string_hash(char* str) {
    int value = 37;
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
            int key_index = string_hash(entry->key) % new_size;
            MapEntry* table_entry = new_bucket[key_index];
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
    int key_size = strlen(key);
    int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    MapEntry* new_entry = calloc(sizeof(MapEntry), 1);
    MapEntry* prev_entry = NULL;
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = NULL;
    if (!table_entry) {
        this->bucket[key_index] = new_entry;
        return;
    }
    while (table_entry) {
        int is_same_key = strncmp(key, table_entry->key, key_size);
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
    int key_size = strlen(key);
    int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    while (table_entry) {
        int is_same_key = strncmp(key, table_entry->key, key_size);
        if (is_same_key) return table_entry->value;
        table_entry = table_entry->next;
    }
    if (!table_entry) return NULL;
}

void Map_remove(Map* this, char* key) {
    int key_size = strlen(key);
    int key_index = string_hash(key) % this->size;
    MapEntry* table_entry = this->bucket[key_index];
    MapEntry* prev_entry = NULL;
    while (table_entry) {
        int is_same_key = strncmp(key, table_entry->key, key_size);
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
