#include <stdio.h>
#include "cache.h"
#include <assert.h>

int main(int argc, char const *argv[])
{
    unsigned cell_count, number_of_requests;
    if (2 != scanf("%u%u", &cell_count, &number_of_requests))
    {
        fprintf(stderr, "Error: failed to initialize cache cell count and number of requests.\n");
        return 1;
    }
    
    unsigned coll = 0;
    struct lru_cache_t *cache = create_cache(cell_count);
    assert(cache);
    while (number_of_requests--)
    {
        unsigned n;
        if (1 != scanf("%u", &n))
            break;
        coll += put(cache, n);
        
    }
    delete_cache(&cache);
    printf("%u\n", coll);
    return 0;
}
