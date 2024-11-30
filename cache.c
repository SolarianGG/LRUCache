#include "cache.h"
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

typedef struct node_t {
    unsigned data;
    struct node_t *prev, *next;
} node;

typedef struct hash_node_t {
    node* data;
    struct hash_node_t *next;
} hash_node;

typedef struct lru_cache_t {
    unsigned size;
    unsigned len;
    struct node_t *begin, *end;
    hash_node **hash_table;
} cache;

static void destroy_hash_list(hash_node **hash_list)
{
    assert(hash_list);
    while (*hash_list)
    {
        hash_node *tmp = *hash_list;
        *hash_list = (*hash_list)->next;
        free(tmp);
    }
    hash_list = NULL;
}

static void destroy_hash_table(hash_node **hash_table, unsigned size)
{
    assert(hash_table);
    for (unsigned i = 0; i < size; ++i) {
        if (hash_table[i]) {
            destroy_hash_list(&hash_table[i]);
        }
    }
    free(hash_table);
}

struct lru_cache_t *create_cache(unsigned cache_size)
{
    cache *lrucache = (cache*)malloc(sizeof(cache));
    assert(lrucache);
    lrucache->size = cache_size;
    lrucache->len = 0;
    lrucache->begin = NULL;
    lrucache->end = NULL;
    lrucache->hash_table = (hash_node**)calloc(cache_size, sizeof(hash_node*));
    assert(lrucache->hash_table);
    return lrucache;
}

static void delete_list(node **list)
{
    assert(list);
    assert(*list);
    if (!list || !(*list)) return;

    while (*list)
    {
        node *tmp = *list;
        *list = (*list)->next;
        free(tmp);
    }
    *list = NULL;
}

void delete_cache(struct lru_cache_t **c)
{
    assert(c);
    assert(*c);
    if (!c || !(*c)) return;
    delete_list(&((*c)->begin));
    destroy_hash_table((*c)->hash_table, (*c)->size);
    free(*c);
    *c = NULL;
}

static void moveToHead(cache *cache, node *n)
{
    if (cache->begin == n) return;
    if (n->prev) n->prev->next = n->next;
    if (n->next) n->next->prev = n->prev;
    if (n == cache->end) cache->end = n->prev;

    n->prev = NULL;
    n->next = cache->begin;
    if (cache->begin) cache->begin->prev = n;
    cache->begin = n;
    if (!cache->end) cache->end = n;
}

static node *get_from_hash(cache *c, unsigned data)
{
    hash_node *start = c->hash_table[data % c->size];
    while(start && start->data->data != data)
        start = start->next;
    return start ? start->data : NULL;
}

static void put_to_hash(cache *c, node *n)
{
    hash_node *tmp = (hash_node*)malloc(sizeof(hash_node));
    assert(tmp);
    tmp->data = n;
    tmp->next = NULL;

    hash_node *start = c->hash_table[n->data % c->size];
    if (!start)
    {
        c->hash_table[n->data % c->size] = tmp;
        return;
    }
    while(start->next)
        start = start->next;
    start->next = tmp;
}

static void remove_from_hash(cache *c, node *n)
{
    hash_node **start = &(c->hash_table[n->data % c->size]);
    while (*start)
    {
        if ((*start)->data == n)
        {
            hash_node *tmp = *start;
            *start = (*start)->next;
            free(tmp);
        }
        else
            start = &(*start)->next;
    }
}

int put(cache *c, unsigned data)
{
    node* n = get_from_hash(c, data);
    if (n) {
        moveToHead(c, n);
        return 1;
    }
    node* newNode = (node*)malloc(sizeof(node));
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = c->begin;

    if (c->begin) c->begin->prev = newNode;
    c->begin = newNode;
    if (!c->end) c->end = newNode;

    put_to_hash(c, newNode);

    if (c->len < c->size) { 
        c->len++;
    } else {
        node* tail = c->end;
        remove_from_hash(c, tail);
        c->end = tail->prev;
        if (c->end) c->end->next = NULL;
        free(tail);
    }
    return 0;
}