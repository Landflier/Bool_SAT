#include "sat.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <cnf_file>\n", program_name);
    printf("  <cnf_file>: Path to a CNF formula file in DIMACS format\n");
}

void print_formula(Formula* formula) {
    printf("CNF Formula with %d variables and %d clauses:\n", formula->num_variables, formula->size);
    
    for (int i = 0; i < formula->size; i++) {
        Clause* clause = &formula->clauses[i];
        printf("Clause %d: (", i + 1);
        
        for (int j = 0; j < clause->size; j++) {
            printf("%d", clause->literals[j]);
            if (j < clause->size - 1) {
                printf(" ∨ ");
            }
        }
        
        printf(")\n");
    }
}

int main(int argc, char* argv[]) {
    // Check arguments
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Parse the CNF file
    Formula* formula = parse_cnf_file(argv[1]);
    if (!formula) {
        fprintf(stderr, "Failed to parse the CNF file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    // Print the formula
    print_formula(formula);
    
    // Create an empty assignment
    Assignment* assignment = create_assignment(formula->num_variables);
    
    // Solve the formula
    printf("\nSolving...\n");
    bool result = solve(formula, assignment);
    // Print the result
    if (result) {
        printf("\nRESULT: SAT\n");
        print_assignment(assignment);
        
        // Verify the solution
        if (is_satisfied(formula, assignment)) {
            printf("\nVerification: The assignment satisfies all clauses.\n");
        } else {
            printf("\nVerification Error: The assignment does not satisfy all clauses!\n");
        }
    } else {
        printf("\nRESULT: UNSAT\n");
    }
    
    // Free memory
    free_assignment(assignment);
    free_formula(formula);
    
    return EXIT_SUCCESS;
} 