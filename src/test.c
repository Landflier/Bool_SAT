#include "sat.h"
#include <time.h>

// Generate a random satisfiable formula
Formula* generate_random_formula(int num_vars, int num_clauses, int clause_size) {
    Formula* formula = create_formula();
    formula->num_variables = num_vars;
    
    // Create a satisfying assignment to ensure the formula is satisfiable
    bool* solution = (bool*)malloc(num_vars * sizeof(bool));
    if (!solution) {
        perror("Failed to allocate memory for solution");
        exit(EXIT_FAILURE);
    }
    
    // Generate a random assignment
    srand(time(NULL));
    for (int i = 0; i < num_vars; i++) {
        solution[i] = (rand() % 2 == 0);
    }
    
    // Generate clauses that are satisfied by the assignment
    for (int i = 0; i < num_clauses; i++) {
        Clause* clause = create_clause();
        
        // Ensure the clause is satisfied
        int satisfied_lit_pos = rand() % clause_size;
        
        for (int j = 0; j < clause_size; j++) {
            int var = (rand() % num_vars) + 1;
            bool is_positive = (rand() % 2 == 0);
            
            // If this is the position we chose to satisfy, ensure it's consistent with solution
            if (j == satisfied_lit_pos) {
                is_positive = solution[var - 1];
            } else {
                // For other positions, random polarity but ensure not already in clause
                for (int k = 0; k < clause->size; k++) {
                    if (abs(clause->literals[k]) == var) {
                        // Variable already in clause, choose another
                        var = (rand() % num_vars) + 1;
                        k = -1; // restart check
                    }
                }
            }
            
            // Add the literal with appropriate polarity
            Literal lit = is_positive ? var : -var;
            add_literal(clause, lit);
        }
        
        add_clause(formula, clause);
        free(clause);
    }
    
    free(solution);
    return formula;
}

bool test_random_formula(int num_vars, int num_clauses, int clause_size) {
    printf("Testing random formula with %d variables, %d clauses, clause size %d\n", 
           num_vars, num_clauses, clause_size);
    
    Formula* formula = generate_random_formula(num_vars, num_clauses, clause_size);
    Assignment* assignment = create_assignment(formula->num_variables);
    
    clock_t start = clock();
    bool result = solve(formula, assignment);
    clock_t end = clock();
    
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    if (result) {
        printf("SAT: Formula is satisfiable!\n");
        
        if (is_satisfied(formula, assignment)) {
            printf("Verification: The assignment satisfies all clauses.\n");
        } else {
            printf("Verification Error: The assignment does not satisfy all clauses!\n");
            return false;
        }
    } else {
        printf("UNSAT: Formula is unsatisfiable (unexpected for randomly generated formula).\n");
        return false;
    }
    
    printf("Solved in %.6f seconds\n", cpu_time_used);
    
    free_assignment(assignment);
    free_formula(formula);
    
    return true;
}

bool run_performance_test() {
    printf("\n=== Performance Test ===\n");
    
    int vars_array[] = {10, 20, 30};
    int clauses_array[] = {20, 50, 100};
    int sizes = sizeof(vars_array) / sizeof(vars_array[0]);
    
    for (int i = 0; i < sizes; i++) {
        int num_vars = vars_array[i];
        int num_clauses = clauses_array[i];
        
        if (!test_random_formula(num_vars, num_clauses, 3)) {
            return false;
        }
    }
    
    return true;
}

bool run_benchmark_test(const char* filename) {
    printf("\n=== Benchmark Test: %s ===\n", filename);
    
    Formula* formula = parse_cnf_file(filename);
    if (!formula) {
        fprintf(stderr, "Failed to parse the benchmark file: %s\n", filename);
        return false;
    }
    
    Assignment* assignment = create_assignment(formula->num_variables);
    
    clock_t start = clock();
    bool result = solve(formula, assignment);
    clock_t end = clock();
    
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    if (result) {
        printf("SAT: Formula is satisfiable!\n");
        
        if (is_satisfied(formula, assignment)) {
            printf("Verification: The assignment satisfies all clauses.\n");
        } else {
            printf("Verification Error: The assignment does not satisfy all clauses!\n");
            free_assignment(assignment);
            free_formula(formula);
            return false;
        }
    } else {
        printf("UNSAT: Formula is unsatisfiable.\n");
    }
    
    printf("Solved in %.6f seconds\n", cpu_time_used);
    
    free_assignment(assignment);
    free_formula(formula);
    
    return true;
}

int run_all_tests() {
    int tests_failed = 0;
    
    printf("Running SAT solver tests...\n");
    
    // Test 1: Simple satisfiable formula
    printf("\n=== Test 1: Simple satisfiable formula ===\n");
    if (!run_benchmark_test("examples/simple.cnf")) {
        tests_failed++;
    }
    
    // Test 2: Unsatisfiable formula
    printf("\n=== Test 2: Unsatisfiable formula ===\n");
    if (!run_benchmark_test("examples/unsatisfiable.cnf")) {
        tests_failed++;
    }
    
    // Test 3: 3-SAT formula
    printf("\n=== Test 3: 3-SAT formula ===\n");
    if (!run_benchmark_test("examples/3sat.cnf")) {
        tests_failed++;
    }
    
    // Test 4: Pigeonhole principle (unsatisfiable)
    printf("\n=== Test 4: Pigeonhole principle (unsatisfiable) ===\n");
    if (!run_benchmark_test("examples/pigeonhole.cnf")) {
        tests_failed++;
    }
    
    // Test 5: Performance test with random formulas
    if (!run_performance_test()) {
        tests_failed++;
    }
    
    // Summary
    printf("\n=== Test Summary ===\n");
    if (tests_failed == 0) {
        printf("All tests PASSED!\n");
    } else {
        printf("%d test(s) FAILED!\n", tests_failed);
    }
    
    return tests_failed;
} 