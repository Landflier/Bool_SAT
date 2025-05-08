#ifndef SAT_H
#define SAT_H


#define UIP_UNRESOLVED 0
#define UIP_CONFLICT 1

#define ANTECEDENT_CLAUSE_NONE -1

#define CLAUSE_UNSAT 0
#define CLAUSE_SAT 1
#define CLAUSE_UNIT 2
#define CLAUSE_UNRESOLVED 3

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
    bool* values; //default value is false
    bool* assigned;
    int size;
    int current_depth_level; //global variable to process. Could also use a stack for depth, but maybe in the future
    int* depth; //depth at which the literal was assigned
    int* antecedent_clause; //clause that implied the assignment
} Assignment;

// Function prototypes

// Formula operations
Formula* create_formula();
void free_formula(Formula* formula);
void add_clause(Formula* formula, Clause* clause);
Clause* create_clause();
void add_literal(Clause* clause, Literal lit);
void free_clause(Clause* clause);
int clause_status(Clause* clause, Assignment* assignment);
int unit_propagation(Formula* formula, Assignment* assignment, Clause** conflict_clause);

// Parser
Formula* parse_cnf_file(const char* filename);
Formula* parse_cnf_dimacs(FILE* file);

// Solver
bool solve(Formula* formula, Assignment* assignment);
bool solve_dpll(Formula* formula, Assignment* assignment);

// Assignment operations
Assignment* create_assignment(int num_variables);
void backtrack_assignment(Assignment* assignment, int backtrack_level);
bool is_satisfied(Formula* formula, Assignment* assignment);
bool all_variables_assigned(Assignment* assignment, Formula* formula);
void print_assignment(Assignment* assignment);
int choose_variable(Formula* formula, Assignment* assignment);
void free_assignment(Assignment* assignment);

// Clause operations
void print_clause(Clause* clause);

// Conflict analysis
int conflict_analysis(Formula* formula, Clause* clause, Assignment* assignment);
Clause* resolve_clauses(Clause* clause_a, Clause* clause_b, Literal literal);
// Test functions
Formula* generate_random_formula(int num_vars, int num_clauses, int clause_size);
bool test_random_formula(int num_vars, int num_clauses, int clause_size);
bool run_performance_test();
bool run_benchmark_test(const char* filename);
int run_all_tests();


//helper function to sort the depth levels. used for backtracking in analyze_conflict
int comp(const void *a, const void *b);
int getSecondLargest(int* array, int n);


#endif // SAT_H 