#include "index.h"
#include "map.h"
#include "set.h"

#include <stdlib.h>
#include <string.h>

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
    char *temppath = malloc(sizeof(char) * strlen(path));
    printf("index_addpath start\n");
    list_iter_t *words_iter = list_createiter(words);

    
    while(list_hasnext(words_iter) == 1){
        
        tempword = list_next(words_iter);
        strcpy(temppath, path);
        
        //printf("temp: %s|%s\n", temppath, path);
        if(map_haskey(index->map, tempword) == 1){
            tempset = map_get(index->map, tempword);      
            set_add(tempset, temppath);

        } else {
            set_t *newset = set_create(compare_strings);
            set_add(newset, temppath);
            map_put(index->map, tempword, newset);
            
        }
        
        //free(tempword);
        
        //free(path);
    }
    //printf("%s", (char*)tempword);
    //printf("%p\n", path);
    //list_destroy(words);
    //free(path);
    printf("index_addpath end\n");
}

typedef enum {
	TERM, OR, AND, ANDNOT
} operator_type;

static parsenode_t *parse_operator(char *term) {
	parsenode_t *t2, *t = parse_term(context);
	if (context->p[0] == '+') {
		context->p++;
		t2 = parse_expr(context);
		return newnode(ADD, 0, t, t2);
	}
	else if (context->p[0] == '-') {
		context->p++;
		t2 = parse_expr(context);
		return newnode(SUB, 0, t, t2);
	}
	return t;
}

static parsenode_t *parse_factor(char *term) {
	if (context->p[0] == '(') {
		parsenode_t *e;
		context->p++;
		e = parse_expr(context);
		if (context->p[0] == ')')
			context->p++;
		else
			fatal_error("Missing )");
		return e;
	}
	else {
		return parse_number(context);
	}
}

/*
 * Performs the given query on the given index.  If the query
 * succeeds, the return value will be a list of paths.  If there
 * is an error (e.g. a syntax error in the query), an error message
 * is assigned to the given errmsg pointer and the return value
 * will be NULL.
 */
/*list_t *index_query(index_t *index, list_t *query, char **errmsg){
    *index_term(index, )
}*/



list_t *index_query(index_t *index, list_t *query, char **errmsg){
    
        // Bruk set isteden så add alt i return listen tilslutt
    
    char *tempquery; 
    printf("index_query start\n");
    list_iter_t *query_iter = list_createiter(query);
    set_iter_t *set_iter;
    set_t *tempset;
    int operator;

    
    //list_t *setlist = list_create(compare_strings);
    list_t *returnlist = list_create(compare_strings);

    while(list_hasnext(query_iter) == 1){
        tempquery = list_next(query_iter);
        printf("%s\n\n", tempquery);
        
        if(compare_strings(tempquery, "ANDNOT") == 0){
            operator = ANDNOT;
            printf("ANDNOT RUN\n\n");

        } else if(compare_strings(tempquery, "AND") == 0){
            opeator = AND;
            printf("AND RUN\n\n");

        } else if(compare_strings(tempquery, "OR") == 0){
            operator = OR;
            printf("OR RUN\n\n");

        } else if(compare_strings(tempquery, "(" || compare_strings(tempquery, ")") == 0){
            printf("() RUN\n\n");

        } else {
            operator = TERM;
            printf("word RUN\n\n");
            if(map_haskey(index->map, tempquery) == 1){       
                tempset = map_get(index->map, tempquery);
                //list_addlast(setlist, tempset);
            } else {
                if(errmsg != NULL){
                *errmsg = "No such word in files\n";
                }
                return NULL;
            }

            set_iter = set_createiter(tempset);

        while(set_hasnext(set_iter) == 1){
            query_result_t *result = malloc(sizeof(query_result_t));
            result->path = set_next(set_iter);
            result->score = 1;
            printf("result->path: %s\n\n", result->path);

            list_addlast(returnlist, result);
        }
        set_destroyiter(set_iter);

    return returnlist;
        }
    }

    
        
    /*list_t *alist = list_create(compare_strings);
    char *banan = "banan";
    char *eple = "eple";
    list_addlast(alist, (void*)banan);
    printf("%d", list_poplast(alist));
    list_addlast(alist, (void*)banan);
    list_addlast(alist, (void*)eple);
    return alist;*/
    printf("index_query end\n");
}

// Pelle
void index_dump(index_t *index){ 
    map_dump(index->map);
}