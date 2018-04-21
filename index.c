#include "index.h"
#include "map.h"
#include "set.h"

#include <stdlib.h>

struct index {
    map_t *map;
    set_t *set;
};

/*
 * Creates a new, empty index.
 */
index_t *index_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc){
    index_t *index = malloc(sizeof(index_t));
    if (index == 0){
        fatal_error("out of memory");
    }

    index->map = map_create(cmpfunc, hashfunc);
    index->set = set_create(cmpfunc);
}

/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index){
    map_destroy(index->map, NULL, NULL);
    set_destroy(index->set);
    free(index);
}

/*
 * Adds the given path to the given index, and index the given
 * list of words under that path.
 * NOTE: It is the responsibility of index_addpath() to deallocate (free)
 *       'path' and the contents of the 'words' list.
 */
void index_addpath(index_t *index, char *path, list_t *words){
    set_add(index->set, words);
    map_put(index->map, path, index->set->words);

}

/*
 * Performs the given query on the given index.  If the query
 * succeeds, the return value will be a list of paths.  If there
 * is an error (e.g. a syntax error in the query), an error message
 * is assigned to the given errmsg pointer and the return value
 * will be NULL.
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg);