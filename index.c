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

static leafnode_t *newnode(operator_type type, void *elem, leafnode_t *left, leafnode_t *right) {
    printf("newnode start\n");
	leafnode_t *node = calloc(sizeof(leafnode_t), 1);
	if (node == NULL)
		fatal_error("out of memory");
        
	node->type = type;
	node->elem = elem;
	node->left = left;
	node->right = right;
	return node;
    printf("newnode end\n");
}


/*
query   ::= andterm

        | andterm "ANDNOT" query
*/
static leafnode_t *parse_query(context_t *context){
    printf("parse_query start | current = %s\n", context->current_elem);
    char *and_not = "ANDNOT"; 

	leafnode_t *a2, *a = parse_andterm(context);

	if (compare_strings(context->current_elem, and_not) == 0) {
		if(list_hasnext(context->current_iter) == 1){
            context->current_elem = list_next(context->current_iter);        
        }
        
        a2 = parse_query(context);
        return newnode(ANDNOT, NULL, a, a2);
	}
	
    printf("newnode end\n");
	return a;
}

/*

andterm  ::= orterm

        | orterm "AND" andterm
*/
static leafnode_t *parse_andterm(context_t *context) {
    printf("parse_andterm start | current = %s\n", context->current_elem);
    char *and = "AND"; 

	leafnode_t *o2, *o = parse_orterm(context); // her er context A

    printf("current_element: %s\n\n", context->current_elem);

	if (compare_strings(context->current_elem, and) == 0) {
        if(list_hasnext(context->current_iter) == 1){
            context->current_elem = list_next(context->current_iter);
            printf("context 3rd: %s\n\n\n", context->current_elem);

        }

        printf("AND with o: %s | o2: %s\n\n", o->elem, o2->elem);
		o2 = parse_andterm(context); // her er context B
        return newnode(AND, NULL, o, o2);
        //printf("context: %s | o: %s | o2: %s", context->current_elem, (char*)o->elem, (char*)o2->elem);

	}
	printf("parse_andterm end\n");
	return o;
}

/*
orterm  ::= term

        | term "OR" orterm
*/
static leafnode_t *parse_orterm(context_t *context) {
    printf("parse_orterm start | context = %s\n", context->current_elem);
	char *or = "OR"; 

	leafnode_t *t2, *t = parse_term(context);

	if (compare_strings(context->current_elem, or) == 0) {
        if(list_hasnext(context->current_iter) == 1){
            context->current_elem = list_next(context->current_iter);
        }

		t2 = parse_orterm(context);
        return newnode(OR, NULL, t, t2);
	}
	printf("parse_orterm end\n");
	return t;
}

/*
term    ::= "(" query ")"
        | <word>
*/
static leafnode_t *parse_term(context_t *context) {
    printf("parse_term start | context = %s\n", context->current_elem);
    char *left_bracket = "(";
    char *right_bracket = ")";	

    printf("First try A, Second B: %s\n\n", context->current_elem);
    if (compare_strings(context->current_elem, left_bracket) == 0) {
        //Brackets
		leafnode_t *q;
		if(list_hasnext(context->current_iter) == 1){
            context->current_elem = list_next(context->current_iter);
 
        }
		q = parse_term(context);

		if (compare_strings(context->current_elem, right_bracket) == 0){
			if(list_hasnext(context->current_iter) == 1){
                context->current_elem = list_next(context->current_iter);

            }
        }   
		else {
			fatal_error("Missing )");
        }
		return q;
	}
	else {
        //word
        printf("parse_term end\n");

        char *tmp = context->current_elem;

        if(list_hasnext(context->current_iter) == 1){
            context->current_elem = list_next(context->current_iter);
            printf("context 2nd: %s\n\n\n", context->current_elem);
        }

        printf("First try A, Second B: %s\n\n", context->current_elem);
		return newnode(TERM, tmp, NULL, NULL);
	}
}

static leafnode_t *parse(list_t *query) {
    printf("parse start\n");
	leafnode_t *result;
    context_t *context;
    context->current_iter = list_createiter(query);
    if(list_hasnext(context->current_iter) == 1){
        context->current_elem = list_next(context->current_iter);
        printf("context 1st: %s\n\n\n", context->current_elem);
        result = parse_query(context);
    } else {
        return NULL;
    }
	
    printf("parse end\n");
	return result;
}

static set_t *evaluate(index_t *index, leafnode_t *term, char **errmsg) {
    printf("evaluate start\n");
    set_t *result_set;
    printf("term: %s |index: %p\n\n\n", term->elem, index);

    printf("term->elem: %s\n", term->elem);
    if(term->left != NULL && term->right != NULL){
        printf("term->left->elem: %s | term->right->elem: %s\n", term->left->elem, term->right->elem);
    }
	switch(term->type) {
		case ANDNOT:
            printf("evaluate-andnot\n");
			return set_difference(evaluate(index, term->left, errmsg), evaluate(index, term->right, errmsg));
		case AND:
            printf("evaluate-and\n");
			return set_intersection(evaluate(index, term->left, errmsg), evaluate(index, term->right, errmsg));
		case OR:
            printf("evaluate-or\n");
			return set_union(evaluate(index, term->left, errmsg), evaluate(index, term->right, errmsg));		
		case TERM:
            printf("evaluate-term\n");
            if(map_haskey(index->map, term->elem) == 1){    
                printf("evaluate - map_haskey\n");   
                result_set = map_get(index->map, term->elem);
                printf("evaluate - map_get\n"); 
            } else {
                if(errmsg != NULL){
                    printf("Errormsg run\n");
                    *errmsg = "No such word in files\n";
                }
                return NULL;
            }
            printf("evaluate end\n");
			return result_set;

		default:
            printf("Unknown type\n");
            if(errmsg != NULL){
                    
                    *errmsg = "Unknown type\n";
                }
                return NULL;
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

    leafnode_t *rootnode = parse(query);
    set_t *result_set = evaluate(index, rootnode, errmsg); 
    //list_t *setlist = list_create(compare_strings);
    list_t *returnlist = list_create(compare_strings);

    /*while(list_hasnext(query_iter) == 1){
        tempquery = list_next(query_iter);
        printf("%s\n\n", tempquery);
        
        

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
        */
    set_iter = set_createiter(result_set);

    while(set_hasnext(set_iter) == 1){
        query_result_t *result = malloc(sizeof(query_result_t));
        result->path = set_next(set_iter);
        result->score = 1;
        //printf("result->path: %s\n\n", result->path);

        list_addlast(returnlist, result);
    }
    set_destroyiter(set_iter);

    printf("index_query end\n");
    return returnlist;
}

// Pelle
void index_dump(index_t *index){ 
    map_dump(index->map);
}