#ifndef SAT_H
#define SAT_H

#define ASSIGN_NONE 0
#define ASSIGN_BRANCH 1
#define ASSIGN_IMPLIED 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Literal representation: positive integers for uncomplemented literals,
// negative integers for complemented literals. 0 is not a valid literal.
typedef int Literal;

// A clause is a sum of literals
typedef struct {
    Literal* literals;
    int size;
    int capacity;
} Clause;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

// A formula in CNF is a product of clauses
typedef struct {
    Clause* clauses;
    int size;
    int capacity;
    int num_variables;
} Formula;

// Assignment of variables
typedef struct {
    bool* values;
    bool* assigned;
    int size;
    int* depth; //depth at which the literal was assigned
    int* decision; // ASSIGN_IMPLIED, ASSIGN_BRANCH, or ASSIGN_NONE
} Assignment;

// Function prototypes

// Formula operations
Formula* create_formula();
void free_formula(Formula* formula);
void add_clause(Formula* formula, Clause* clause);
Clause* create_clause();
void add_literal(Clause* clause, Literal lit);
void free_clause(Clause* clause);

// Parser
Formula* parse_cnf_file(const char* filename);
Formula* parse_cnf_dimacs(FILE* file);

// Solver
bool solve(Formula* formula, Assignment* assignment);
bool solve_dpll(Formula* formula, Assignment* assignment);

// Assignment operations
Assignment* create_assignment(int num_variables);
void free_assignment(Assignment* assignment);
bool is_satisfied(Formula* formula, Assignment* assignment);
void print_assignment(Assignment* assignment);
void copy_assignment(Assignment* assignment_from, Assignment* assignment_to);

// Test functions
Formula* generate_random_formula(int num_vars, int num_clauses, int clause_size);
bool test_random_formula(int num_vars, int num_clauses, int clause_size);
bool run_performance_test();
bool run_benchmark_test(const char* filename);
int run_all_tests();

#endif // SAT_H 