#ifndef CACHE_H_SENTRY
#define CACHE_H_SENTRY

struct node_t;

struct lru_cache_t;

struct lru_cache_t *create_cache(unsigned cache_size);

void delete_cache(struct lru_cache_t **cache);

int put(struct lru_cache_t *cache, unsigned data);

#endif