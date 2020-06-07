#include "js.h"

/*
 * Map: HashMap implementation
 *
 * Separate chaining with linked list
 */

#define MAP_SIZE 128

Map *Map_new(void)
{
	Map *self = calloc(sizeof(Map), 1);
	self->bucket = calloc(sizeof(MapEntry *), MAP_SIZE);
	self->size = MAP_SIZE;
	self->entries = 0;
	return self;
}

static unsigned int string_hash(char *str)
{
	unsigned int value = 37;
	while (*str)
	{
		value = (value * 54059) ^ (*str * 76963);
		str++;
	}
	return value;
}

static void Map_rehash(Map *self)
{
	double load_factor = (double)self->entries / (double)self->size;
	if (load_factor < 0.75)
		return;
	int old_size = self->size;
	int new_size = old_size * 2;
	MapEntry **old_bucket = self->bucket;
	MapEntry **new_bucket = calloc(sizeof(MapEntry *), new_size);
	self->bucket = new_bucket;
	self->entries = 0;
	self->size = new_size;
	for (int i = 0; i < old_size; i++)
	{
		MapEntry *entry = old_bucket[i];
		MapEntry *prev = NULL;
		while (entry)
		{
			Map_set(self, entry->key, entry->value);
			prev = entry;
			entry = entry->next;
			free(prev);
		}
	}
	free(old_bucket);
}

void Map_set(Map *self, char *key, void *value)
{
	Map_rehash(self);
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry *table_entry = self->bucket[key_index];
	MapEntry *new_entry = calloc(sizeof(MapEntry), 1);
	MapEntry *prev_entry = NULL;
	new_entry->key = key;
	new_entry->value = value;
	new_entry->next = NULL;
	if (!table_entry)
	{
		self->bucket[key_index] = new_entry;
		self->entries++;
		return;
	}
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
		if (is_same_key && prev_entry)
		{
			new_entry->next = table_entry->next;
			prev_entry->next = new_entry;
			free(table_entry);
			return;
		}
		else if (is_same_key)
		{
			self->bucket[key_index] = new_entry;
			free(table_entry);
			return;
		}
		prev_entry = table_entry;
		table_entry = table_entry->next;
	}
	prev_entry->next = new_entry;
	self->entries++;
}

void *Map_get(Map *self, char *key)
{
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry *table_entry = self->bucket[key_index];
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
		if (is_same_key)
			return table_entry->value;
		table_entry = table_entry->next;
	}
	return NULL;
}

void Map_remove(Map *self, char *key)
{
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry *table_entry = self->bucket[key_index];
	MapEntry *prev_entry = NULL;
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
		if (is_same_key && prev_entry)
		{
			self->entries--;
			prev_entry->next = table_entry->next;
			free(table_entry);
			return;
		}
		else if (is_same_key)
		{
			self->entries--;
			self->bucket[key_index] = table_entry->next;
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

static void Vector_extend(Vector *self);

Vector *Vector_new(void)
{
	Vector *self = calloc(sizeof(Vector), 1);
	self->buf = calloc(sizeof(void *), VECTOR_CAPACITY);
	self->size = 0;
	self->capacity = VECTOR_CAPACITY;
	return self;
}

void Vector_push(Vector *self, void *item)
{
	if (self->size >= self->capacity)
		Vector_extend(self);
	self->buf[self->size] = item;
	self->size++;
}

void *Vector_get(Vector *self, int index)
{
	if (index >= self->size)
		return NULL;
	return self->buf[index];
}

void Vector_set(Vector *self, int index, void *item)
{
	if (index >= self->size && index < 0)
		return;
	self->buf[index] = item;
}

void *Vector_pop(Vector *self)
{
	int last_index = self->size - 1;
	void *item = Vector_get(self, last_index);
	Vector_set(self, last_index, NULL);
	self->size--;
	return item;
}

static void Vector_extend(Vector *self)
{
	int old_capacity = self->capacity;
	int new_capacity = old_capacity * 2;
	void **old_buf = self->buf;
	self->buf = calloc(sizeof(void *), new_capacity);
	self->capacity = new_capacity;
	memcpy((void *)self->buf, old_buf, old_capacity * sizeof(void *));
}

#undef VECTOR_CAPACITY

/*
 * String: Variable length character sequence
 */

#define STRING_CAPACITY 8

static void String_extend(String *this);

String *String_new(void)
{
	String *this = calloc(sizeof(String), 1);
	this->buf = calloc(sizeof(char), STRING_CAPACITY);
	this->capacity = STRING_CAPACITY;
	this->size = 0;
	return this;
}

void String_delete(String *this)
{
	free(this->buf);
	free(this);
}

static void String_extend(String *this)
{
	int old_capacity = this->capacity;
	int new_capacity = old_capacity + STRING_CAPACITY;
	char *old_buf = this->buf;
	this->buf = calloc(sizeof(char), new_capacity);
	this->capacity = new_capacity;
	strncpy(this->buf, old_buf, old_capacity * sizeof(char));
}

void String_push(String *this, char item)
{
	if (this->size >= this->capacity)
		String_extend(this);
	this->buf[this->size] = item;
	this->size++;
}

void String_append(String *this, String *that)
{
	if (this->capacity - this->size > that->size)
	{
		strncpy(this->buf + this->size, that->buf, that->size * sizeof(char));
		return;
	}
	char *new_buf = calloc(sizeof(char), this->capacity + that->capacity);
	strncpy(new_buf, this->buf, sizeof(char) * this->size);
	strncpy(new_buf + this->size, that->buf, sizeof(char) * that->size);
	free(this->buf);
	this->size += that->size;
	this->capacity += that->capacity;
	this->buf = new_buf;
}

#undef STRING_CAPACITY

/*
 * Loc: Source code location
 */

Loc Loc_make(Pos start, Pos end)
{
	return (Loc){.start = start, .end = end};
}
