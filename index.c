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
    char *tempword;
    list_iter_t *words_iter = list_createiter(words);

    while(list_hasnext(words_iter) == 1){
        tempword = list_next(words_iter);

        if(map_haskey(index->map, tempword) == 1){
            tempset = map_get(index->map, tempword);       
        } else {
            tempset = set_create(compare_strings);
        }

        set_add(tempset, path);
        map_put(index->map, tempword, tempset);

        free(tempword);

        
    }
    //printf("%s", (char*)tempword);
    //printf("%s", path);

    free(path);
}

/*
 * Performs the given query on the given index.  If the query
 * succeeds, the return value will be a list of paths.  If there
 * is an error (e.g. a syntax error in the query), an error message
 * is assigned to the given errmsg pointer and the return value
 * will be NULL.
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg){
        
        // Bruk set isteden så add alt i return listen tilslutt
    char *tempquery; 
    query_result_t *result;
    list_iter_t *query_iter = list_createiter(query);


    
    set_t *tempset = set_create(compare_strings);

    list_t *returnlist = list_create(compare_strings);

    while(list_hasnext(query_iter) == 1){
        tempquery = list_next(query_iter);
        //printf("%s\n\n", tempquery);

        if(map_haskey(index->map, tempquery) == 1){       
            printf("%s\n\n", (char*)map_get(index->map, tempquery)); 
            tempset = (tempset, map_get(index->map, tempquery));

        } else {
            return NULL;
        }

    }
    set_iter_t *set_iter = set_createiter(tempset);

    while(set_hasnext(set_iter) == 1){
        result->path = (char*)set_next(set_iter);
        result->score = 1;

        //printf("%s\n\n", result->path);

        list_addlast(returnlist, result);
    }

    return returnlist;
        
    /*list_t *alist = list_create(compare_strings);
    char *banan = "banan";
    char *eple = "eple";
    list_addlast(alist, (void*)banan);
    printf("%d", list_poplast(alist));
    list_addlast(alist, (void*)banan);
    list_addlast(alist, (void*)eple);
    return alist;*/
}

// Pelle
void index_dump(index_t *index){ 
    map_dump(index->map);
}