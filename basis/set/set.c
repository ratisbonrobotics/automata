#include "set.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>

static const unsigned int default_table_size = 100;
typedef struct hash_table_ *hash_table;

struct hash_table_
{
    void **table;
    unsigned int table_size;
    unsigned int full_size;
    unsigned int traversal_index;
};

static hash_table HashTable(unsigned int table_size)
{
    hash_table newTable = (hash_table)malloc(sizeof(struct hash_table_));
    newTable->table = (void **)calloc(table_size, sizeof(void *));
    newTable->table_size = table_size;
    newTable->full_size = 0;
    newTable->traversal_index = 0;
    return newTable;
}

void freeHashTable(hash_table ht)
{
    free(ht->table);
    free(ht);
}

void *traverse(hash_table ht)
{
    void *temp = ht->table[ht->traversal_index];

    for (unsigned int i = 0; temp == NULL; i++)
    {
        if (i == ht->table_size)
        {
            return NULL;
        }
        ht->traversal_index = (ht->traversal_index + 1) % ht->table_size;
        temp = ht->table[ht->traversal_index];
    }

    ht->traversal_index = (ht->traversal_index + 1) % ht->table_size;
    return temp;
}

static unsigned int hash(void *data, unsigned int table_size)
{
    unsigned int golden_ratio = 0x9E3779B9;
    unsigned long int value = (unsigned long int)data * golden_ratio;
    value = value >> 32;
    return (unsigned int)value % table_size;
}

void insert(hash_table ht, void *data)
{
    unsigned int idx = hash(data, ht->table_size);
    void *temp = ht->table[idx];

    for (unsigned int i = 0; temp != NULL; i++)
    {
        if (i == ht->table_size)
        {
            unsigned int old_table_size = ht->table_size;
            unsigned int number_of_previous_resizes = old_table_size / default_table_size;
            unsigned long int new_table_size_long_int = (unsigned long int)old_table_size + (unsigned long int)default_table_size * number_of_previous_resizes;
            assert(new_table_size_long_int <= UINT_MAX);
            hash_table temp_ht = HashTable((unsigned int)new_table_size_long_int);

            for (unsigned int j = 0; j < old_table_size; j++)
            {
                insert(temp_ht, traverse(ht));
            }
            void **temp_table = ht->table;
            ht->table = temp_ht->table;
            ht->table_size = temp_ht->table_size;
            ht->full_size = temp_ht->full_size;
            ht->traversal_index = temp_ht->traversal_index;

            temp_ht->table = temp_table;

            freeHashTable(temp_ht);
            insert(ht, data);
            return;
        }
        idx = (idx + 1) % ht->table_size;
        temp = ht->table[idx];
    }

    ht->table[idx] = data;
    ht->full_size++;
}

bool find(hash_table ht, void *data)
{
    unsigned int idx = hash(data, ht->table_size);
    void *temp = ht->table[idx];

    for (unsigned int i = 0; temp != data; i++)
    {
        if (i == ht->table_size)
        {
            return false;
        }
        idx = (idx + 1) % ht->table_size;
        temp = ht->table[idx];
    }

    return true;
}

bool take_out(hash_table ht, void *data)
{
    unsigned int idx = hash(data, ht->table_size);
    void *temp = ht->table[idx];

    for (unsigned int i = 0; temp != data; i++)
    {
        if (i == ht->table_size)
        {
            return false;
        }
        idx = (idx + 1) % ht->table_size;
        temp = ht->table[idx];
    }
    ht->table[idx] = NULL;
    ht->full_size -= 1;
    return true;
}

unsigned int getSize(hash_table ht)
{
    return ht->full_size;
}

enum
{
    default_size_of_the_universe = 100000
};

set universe_of_discourse[default_size_of_the_universe] = {NULL};

static bool isSetInUniverse(set s)
{
    assert(s != NULL);

    for (unsigned int i = 0; i < default_size_of_the_universe; i++)
    {
        if (universe_of_discourse[i] == s)
        {
            return true;
        }
    }

    return false;
}

bool isObjectASet(void *p)
{
    return isSetInUniverse(p);
}

set Set()
{
    if (universe_of_discourse[0] == NULL)
    {
        universe_of_discourse[0] = HashTable(default_table_size);
        return universe_of_discourse[0];
    }
    else
    {
        return universe_of_discourse[0];
    }
}

set addToSet(set s, void *object)
{
    assert(isSetInUniverse(s));

    if (find(s, object))
        return s;

    set new_set = HashTable(default_table_size);
    for (unsigned int i = 0; i < getSize(s); i++)
    {
        insert(new_set, traverse(s));
    }
    insert(new_set, object);

    for (unsigned int i = 0; i < default_size_of_the_universe; i++)
    {
        set u_s = universe_of_discourse[i];

        if (u_s == NULL)
        {
            universe_of_discourse[i] = new_set;
            return new_set;
        }
        else if (getSize(u_s) == getSize(new_set))
        {
            bool identical = true;

            for (unsigned int j = 0; j < getSize(u_s); j++)
            {
                if (!find(u_s, traverse(new_set)))
                {
                    identical = false;
                    break;
                }
            }
            if (identical)
            {
                freeHashTable(new_set);
                return u_s;
            }
        }
    }

    assert(false && "Universe is full.");
}

bool isElementOf(set s, void *object)
{
    assert(isSetInUniverse(s));

    return find(s, object);
}

set removeFromSet(set s, void *object)
{
    assert(isSetInUniverse(s));

    if (!find(s, object))
        return s;

    set new_set = HashTable(default_table_size);
    for (unsigned int i = 0; i < getSize(s); i++)
    {
        void *o = traverse(s);
        if (o != object)
            insert(new_set, o);
    }

    for (unsigned int i = 0; i < default_size_of_the_universe; i++)
    {
        set u_s = universe_of_discourse[i];

        if (u_s == NULL)
        {
            universe_of_discourse[i] = new_set;
            return new_set;
        }
        else if (getSize(u_s) == getSize(new_set))
        {
            bool identical = true;

            for (unsigned int j = 0; j < getSize(u_s); j++)
            {
                if (!find(u_s, traverse(new_set)))
                {
                    identical = false;
                    break;
                }
            }
            if (identical)
            {
                freeHashTable(new_set);
                return u_s;
            }
        }
    }

    assert(false && "Universe is full.");
}

void *drawFromSet(set s)
{
    assert(isSetInUniverse(s));
    return traverse(s);
}

unsigned int getCardinality(set s)
{
    assert(isSetInUniverse(s));
    return getSize(s);
}

set unionSet(set s, set t)
{
    assert(isSetInUniverse(s));
    assert(isSetInUniverse(t));

    set new_set = HashTable(default_table_size);
    for (unsigned int i = 0; i < getSize(s); i++)
    {
        insert(new_set, traverse(s));
    }
    for (unsigned int i = 0; i < getSize(t); i++)
    {
        insert(new_set, traverse(t));
    }

    for (unsigned int i = 0; i < default_size_of_the_universe; i++)
    {
        set u_s = universe_of_discourse[i];

        if (u_s == NULL)
        {
            universe_of_discourse[i] = new_set;
            return new_set;
        }
        else if (getSize(u_s) == getSize(new_set))
        {
            bool identical = true;

            for (unsigned int j = 0; j < getSize(u_s); j++)
            {
                if (!find(u_s, traverse(new_set)))
                {
                    identical = false;
                    break;
                }
            }
            if (identical)
            {
                freeHashTable(new_set);
                return u_s;
            }
        }
    }

    assert(false && "Universe is full.");
}