#include "sat.h"

Formula* parse_cnf_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    Formula* formula = parse_cnf_dimacs(file);
    fclose(file);
    return formula;
}

Formula* parse_cnf_dimacs(FILE* file) {
    char line[1024];
    int num_variables = 0, num_clauses = 0;
    
    // Skip comments and find problem line
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c') {
            // Comment line - skip
            continue;
        } else if (line[0] == 'p') {
            // Problem line - parse number of variables and clauses
            if (sscanf(line, "p cnf %d %d", &num_variables, &num_clauses) != 2) {
                fprintf(stderr, "Invalid problem line format: %s\n", line);
                return NULL;
            }
            break;
        }
    }
    
    if (num_variables == 0 || num_clauses == 0) {
        fprintf(stderr, "Missing or invalid problem line\n");
        return NULL;
    }
    
    Formula* formula = create_formula();
    formula->num_variables = num_variables;
    
    // Parse clauses
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c') {
            // Comment line - skip
            continue;
        }
        
        // Parse literals in the clause
        Clause* clause = create_clause();
        char* token = strtok(line, " \t\n");
        
        while (token) {
            int lit = atoi(token);
            if (lit == 0) {
                // End of clause
                break;
            }
            
            add_literal(clause, lit);
            token = strtok(NULL, " \t\n");
        }
        
        // Add clause to formula if it's not empty
        if (clause->size > 0) {
            add_clause(formula, clause);
        }
        
        free(clause);
    }
    
    // Verify we read the expected number of clauses
    if (formula->size != num_clauses) {
        fprintf(stderr, "Warning: Expected %d clauses, but read %d\n", 
                num_clauses, formula->size);
    }
    
    return formula;
} 