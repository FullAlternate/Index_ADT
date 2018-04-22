#include "index.h"
#include "map.h"
#include "set.h"

#include <stdlib.h>

struct index {
    map_t *map;
};

/*
 * Creates a new, empty index.
 */
index_t *index_create(){
    index_t *index = malloc(sizeof(index_t));
    
    if (index == 0){
        fatal_error("out of memory");
    }

    index->map = map_create(compare_strings, hash_string);

    return index;
}

/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index){
    map_destroy(index->map, NULL, NULL);
    free(index);
}

/*
 * Adds the given path to the given index, and index the given
 * list of words under that path.
 * NOTE: It is the responsibility of index_addpath() to deallocate (free)
 *       'path' and the contents of the 'words' list.
 */
void index_addpath(index_t *index, char *path, list_t *words){
    set_t *tempset;
    list_iter_t *words_iter = list_createiter(words);
    void *tempword;
    while(list_hasnext(words_iter) == 1){
        tempword = list_next(words_iter);

        if(map_haskey(index->map, tempword)){
            tempset = map_get(index->map, tempword);       
        } else {
            tempset = set_create(compare_strings); 
        }

        map_put(index->map, tempword, tempset);
        set_add(tempset, path);
    }
    //printf("%s", (char*)tempword);
    //printf("%s", path);

}

/*
 * Performs the given query on the given index.  If the query
 * succeeds, the return value will be a list of paths.  If there
 * is an error (e.g. a syntax error in the query), an error message
 * is assigned to the given errmsg pointer and the return value
 * will be NULL.
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg){
       /* list_t *alist = list_create(compare_strings);
        void *banan = "banan";
        list_addlast(alist, banan);
        printf("%d", list_poplast(alist));
        list_addlast(alist, banan);
        return alist;*/
}