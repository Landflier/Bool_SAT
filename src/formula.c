#include "sat.h"

#define INITIAL_CAPACITY 32

Formula* create_formula() {
    Formula* formula = (Formula*)malloc(sizeof(Formula));
    if (!formula) {
        perror("Failed to allocate memory for formula");
        exit(EXIT_FAILURE);
    }
    
    formula->clauses = (Clause*)malloc(INITIAL_CAPACITY * sizeof(Clause));
    if (!formula->clauses) {
        perror("Failed to allocate memory for clauses");
        free(formula);
        exit(EXIT_FAILURE);
    }
    
    formula->size = 0;
    formula->capacity = INITIAL_CAPACITY;
    formula->num_variables = 0;
    
    return formula;
}

void free_formula(Formula* formula) {
    if (!formula) return;
    
    for (int i = 0; i < formula->size; i++) {
        free_clause(&formula->clauses[i]);
    }
    
    free(formula->clauses);
    free(formula);
}

void add_clause(Formula* formula, Clause* clause) {
    if (formula->size >= formula->capacity) {
        formula->capacity *= 2;
        formula->clauses = (Clause*)realloc(formula->clauses, formula->capacity * sizeof(Clause));
        if (!formula->clauses) {
            perror("Failed to reallocate memory for clauses");
            exit(EXIT_FAILURE);
        }
    }
    
    formula->clauses[formula->size] = *clause;
    formula->size++;
    
    // Update the number of variables if needed
    for (int i = 0; i < clause->size; i++) {
        int var = abs(clause->literals[i]);
        if (var > formula->num_variables) {
            formula->num_variables = var;
        }
    }
}

Clause* create_clause() {
    Clause* clause = (Clause*)malloc(sizeof(Clause));
    if (!clause) {
        perror("Failed to allocate memory for clause");
        free(clause);
        exit(EXIT_FAILURE);
    }
    
    clause->literals = (Literal*)malloc(INITIAL_CAPACITY * sizeof(Literal));
    if (!clause->literals) {
        perror("Failed to allocate memory for literals");
        free(clause);
        exit(EXIT_FAILURE);
    }
    clause->size = 0;
    clause->capacity = INITIAL_CAPACITY;
    
    return clause;
}

void add_literal(Clause* clause, Literal lit) {
    if (clause->size >= clause->capacity) {
        clause->capacity *= 2;
        clause->literals = (Literal*)realloc(clause->literals, clause->capacity * sizeof(Literal));
        if (!clause->literals) {
            perror("Failed to reallocate memory for literals");
            exit(EXIT_FAILURE);
        }
    }
    
    clause->literals[clause->size] = lit;
    clause->size++;
}

void free_clause(Clause* clause) {
    if (!clause) return;
    free(clause->literals);
    // Note: We don't free the clause itself because it's part of the formula array
} 

//sorts descending
int comp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int getSecondLargest(int* array, int n) {
    
    // Sort the array in non-decreasing order
    qsort(array, n, sizeof(int), comp);
  
    // start from second last element as last element is the largest
    for (int i = n - 2; i >= 0; i--) {
        
        // return the first element which is not equal to the 
        // largest element
        if (array[i] != array[n - 1]) {
            return array[i];
        }
    }
  
    // If no second largest element was found, return the largest element
    return array[n-1];
}