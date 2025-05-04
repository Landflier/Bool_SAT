#include "sat.h"

// Check if an assignment satisfies a formula
bool is_satisfied(Formula* formula, Assignment* assignment) {
    // A formula is satisfied if all clauses are satisfied
    for (int i = 0; i < formula->size; i++) {
        Clause* clause = &formula->clauses[i];
        bool clause_satisfied = false;
        
        // A clause is satisfied if at least one literal is satisfied
        for (int j = 0; j < clause->size; j++) {
            Literal lit = clause->literals[j];
            int var = abs(lit);
            bool is_positive = (lit > 0);
            
            if (assignment->assigned[var - 1]) {
                bool var_value = assignment->values[var - 1];
                if ((is_positive && var_value) || (!is_positive && !var_value)) {
                    clause_satisfied = true;
                    break;
                }
            }
        }
        
        if (!clause_satisfied) {
            return false;
        }
    }
    
    return true;
}

// Entry point for solving
bool solve(Formula* formula, Assignment* assignment) {
    return solve_dpll(formula, assignment);
}

// Check if a clause is unit (has only one unassigned literal)
bool is_unit_clause(Clause* clause, Assignment* assignment, Literal* unit_lit) {
    int unassigned_count = 0;
    Literal last_unassigned = 0;
    
    for (int i = 0; i < clause->size; i++) {
        Literal lit = clause->literals[i];
        int var = abs(lit);
        bool is_positive = (lit > 0);
        
        if (!assignment->assigned[var - 1]) {
            unassigned_count++;
            last_unassigned = lit;
        } else {
            bool var_value = assignment->values[var - 1];
            if ((is_positive && var_value) || (!is_positive && !var_value)) {
                // Clause is already satisfied
                return false;
            }
        }
    }
    
    if (unassigned_count == 1) {
        *unit_lit = last_unassigned;
        return true;
    }
    
    return false;
}

// Unit propagation: find and assign all unit clauses
bool unit_propagation(Formula* formula, Assignment* assignment) {
    bool changes_made;
    
    do {
        changes_made = false;
        
        for (int i = 0; i < formula->size; i++) {
            Clause* clause = &formula->clauses[i];
            Literal unit_lit = 0;
            
            if (is_unit_clause(clause, assignment, &unit_lit)) {
                int var = abs(unit_lit);
                bool value = (unit_lit > 0);
                
                // Assign the variable
                assignment->values[var - 1] = value;
                assignment->assigned[var - 1] = true;
                changes_made = true;
                
                // Check if this assignment creates an empty clause
                for (int j = 0; j < formula->size; j++) {
                    Clause* check_clause = &formula->clauses[j];
                    bool clause_satisfied = false;
                    bool all_assigned = true;
                    
                    for (int k = 0; k < check_clause->size; k++) {
                        Literal check_lit = check_clause->literals[k];
                        int check_var = abs(check_lit);
                        bool is_positive = (check_lit > 0);
                        
                        if (!assignment->assigned[check_var - 1]) {
                            all_assigned = false;
                            break;
                        }
                        
                        bool check_val = assignment->values[check_var - 1];
                        if ((is_positive && check_val) || (!is_positive && !check_val)) {
                            clause_satisfied = true;
                            break;
                        }
                    }
                    
                    if (all_assigned && !clause_satisfied) {
                        // Found an empty clause
                        return false;
                    }
                }
            }
        }
    } while (changes_made);
    
    return true;
}

// Choose an unassigned variable for branching
int choose_variable(Formula* formula, Assignment* assignment) {
    for (int i = 0; i < formula->num_variables; i++) {
        if (!assignment->assigned[i]) {
            return i + 1;
        }
    }
    return 0; // All variables are assigned
}

// DPLL algorithm implementation
bool solve_dpll(Formula* formula, Assignment* assignment) {
    // Check if the formula is already satisfied
    if (is_satisfied(formula, assignment)) {
        return true;
    }
    
    // Apply unit propagation
    if (!unit_propagation(formula, assignment)) {
        return false;
    }
    
    // Choose an unassigned variable
    int var = choose_variable(formula, assignment);
    if (var == 0) {
        // All variables are assigned, but formula is not satisfied
        return false;
    }
    
    // Try setting the variable to true
    Assignment* true_assignment = create_assignment(formula->num_variables);
    memcpy(true_assignment->values, assignment->values, formula->num_variables * sizeof(bool));
    memcpy(true_assignment->assigned, assignment->assigned, formula->num_variables * sizeof(bool));
    
    true_assignment->values[var - 1] = true;
    true_assignment->assigned[var - 1] = true;
    
    if (solve_dpll(formula, true_assignment)) {
        // Copy the successful assignment back
        memcpy(assignment->values, true_assignment->values, formula->num_variables * sizeof(bool));
        memcpy(assignment->assigned, true_assignment->assigned, formula->num_variables * sizeof(bool));
        free_assignment(true_assignment);
        return true;
    }
    
    free_assignment(true_assignment);
    
    // Try setting the variable to false
    Assignment* false_assignment = create_assignment(formula->num_variables);
    memcpy(false_assignment->values, assignment->values, formula->num_variables * sizeof(bool));
    memcpy(false_assignment->assigned, assignment->assigned, formula->num_variables * sizeof(bool));
    
    false_assignment->values[var - 1] = false;
    false_assignment->assigned[var - 1] = true;
    
    if (solve_dpll(formula, false_assignment)) {
        // Copy the successful assignment back
        memcpy(assignment->values, false_assignment->values, formula->num_variables * sizeof(bool));
        memcpy(assignment->assigned, false_assignment->assigned, formula->num_variables * sizeof(bool));
        free_assignment(false_assignment);
        return true;
    }
    
    free_assignment(false_assignment);
    
    // No solution found
    return false;
}

// Create a new assignment
Assignment* create_assignment(int num_variables) {
    Assignment* assignment = (Assignment*)malloc(sizeof(Assignment));
    if (!assignment) {
        perror("Failed to allocate memory for assignment");
        exit(EXIT_FAILURE);
    }
    
    assignment->values = (bool*)calloc(num_variables, sizeof(bool));
    assignment->assigned = (bool*)calloc(num_variables, sizeof(bool));
    
    if (!assignment->values || !assignment->assigned) {
        perror("Failed to allocate memory for assignment arrays");
        exit(EXIT_FAILURE);
    }
    
    assignment->size = num_variables;
    return assignment;
}

// Free an assignment
void free_assignment(Assignment* assignment) {
    if (!assignment) return;
    free(assignment->values);
    free(assignment->assigned);
    free(assignment);
}

// Print the assignment
void print_assignment(Assignment* assignment) {
    printf("Assignment:\n");
    for (int i = 0; i < assignment->size; i++) {
        if (assignment->assigned[i]) {
            printf("Variable %d = %s\n", i + 1, assignment->values[i] ? "true" : "false");
        } else {
            printf("Variable %d = unassigned\n", i + 1);
        }
    }
} 