/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#ifndef INDEX_H
#define INDEX_H

#include "list.h"

struct index;
typedef struct index index_t;

typedef
struct query_result
{
	char *path;    /* Document path */
	double score;  /* Document to query score */
}query_result_t;

/*
 * Creates a new, empty index.
 */
index_t *index_create();

/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index);

/*
 * Adds the given path to the given index, and index the given
 * list of words under that path.
 * NOTE: It is the responsibility of index_addpath() to deallocate (free)
 *       'path' and the contents of the 'words' list.
 */
void index_addpath(index_t *index, char *path, list_t *words);

/*
 * Performs the given query on the given index.  If the query
 * succeeds, the return value will be a list of paths.  If there
 * is an error (e.g. a syntax error in the query), an error message
 * is assigned to the given errmsg pointer and the return value
 * will be NULL.
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg);


// Parsing functions

typedef enum {
	TERM, OR, AND, ANDNOT
} operator_type;

struct leafnode;
typedef struct leafnode leafnode_t;

struct leafnode 
{
    operator_type type;
    char *elem;
    leafnode_t *left;
    leafnode_t *right;
};

struct context;
typedef struct context context_t;

struct context
{
	char *current_elem;
	list_iter_t *current_iter;
};

#define AMOUNT_OF_FILES 3204


static leafnode_t *newnode(operator_type type, void *elem, leafnode_t *left, leafnode_t *right);


static leafnode_t *parse_query(context_t *context);


static leafnode_t *parse_andterm(context_t *context);


static leafnode_t *parse_orterm(context_t *context);


static leafnode_t *parse_term(context_t *context);


static leafnode_t *parse(list_t *query);


void index_dump(index_t *index); // Pelle

void dump_query(char *query); // Pelle

#endif


