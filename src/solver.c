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

bool all_variables_assigned(Assignment* assignment, Formula* formula){
    for (int i = 0; i < formula->num_variables; i++){
        if (!assignment->assigned[i]){
            printf("Variable %d is not assigned\n", i+1);
            return false;
        }
    }
    return true;
}

// Entry point for solving
bool solve(Formula* formula, Assignment* assignment) {
    return solve_dpll(formula, assignment);
}


// Unit propagation: find and assign all unit clauses
int unit_propagation(Formula* formula, Assignment* assignment, Clause* conflict_clause) {
    bool end_propagation = false;

    // loop finishes, after no more unit/conflict clauses are found
    do {
        end_propagation = true;
        
        for (int i = 0; i < formula->size; i++) {
            Clause* clause = &formula->clauses[i];
            int status = clause_status(clause, assignment);
            if(status == CLAUSE_SAT || status == CLAUSE_UNRESOLVED){
                printf("Clause %d is satisfied or unresolved\n", i+1);
                // goes to next clause in formula
                continue;
            }else if(status == CLAUSE_UNIT){
                printf("Clause %d is unit\n", i+1);
                // find unit literal and assign it
                for (int j = 0; j < clause->size; j++){
                    printf("Checking literal %d\n", clause->literals[j]);
                    Literal lit = clause->literals[j];
                    int var = abs(lit);
                    if(!assignment->assigned[var-1]){
                        printf("Literal %d is not assigned and is unit\n", var);
                        printf("Assigning value %b\n", (lit>0));
                        assignment->assigned[var-1] = true;
                        assignment->values[var-1] = (lit > 0);
                        assignment->depth[var-1] = assignment->current_depth_level;
                        assignment->antecedent_clause[var-1] = i;
                        end_propagation = false;
                        break;
                    }   
                }
            }else{
                printf("Clause %d is conflict\n", i);
                // this is CLAUSE_CONFLICT
                conflict_clause = clause;
                return UIP_CONFLICT;
            }
        }
    } while (!end_propagation);
    
    return UIP_UNRESOLVED;
}

// Choose an unassigned variable for branching
int choose_variable(Formula* formula, Assignment* assignment) {
    // i is the index, care not to subtract 1
    for (int i = 0; i < formula->num_variables; i++) {
        if (!assignment->assigned[i]) {
            printf("Choosing variable %d, assigning true\n", i+1);
            assignment->assigned[i] = true;
            assignment->values[i] = true; // assign true to the variable
            return i + 1;
        }
    }
    return 0; // All variables are assigned
}

// DPLL algorithm implementation
bool solve_dpll(Formula* formula, Assignment* assignment) {
    Clause* conflict_clause = create_clause();
    // Apply unit propagation
    if (unit_propagation(formula, assignment, &conflict_clause) == UIP_CONFLICT) {
        printf("Unit propagation failed on first run\n");
        return false;
    }
    
    while (!all_variables_assigned(assignment, formula)){
        // Choose an unassigned variable
        int var = choose_variable(formula, assignment);
        assignment->current_depth_level++;
        assignment->depth[var-1] = assignment->current_depth_level;

            while (true){
                int reason = unit_propagation(formula, assignment, &conflict_clause);
                print_assignment(assignment);
                if (reason != UIP_CONFLICT){
                    // no conflict after UIP, so return to branching
                    break;
                }

                int b = conflict_analysis(formula, &conflict_clause, assignment);
                // if conflict was 'backpropagated' to the root,
                // then we have no solution
                if ( b < 0 ){
                    return false;
                }

                backtrack_assignment(assignment, b);
                assignment->current_depth_level = b;
            }
    // No solution found
    return false;
    }
}

Clause* resolve_clauses(Clause* clause_a, Clause* clause_b, Literal* literal){

    // since we are using add_literal, the resolution clause will have a meaningful size
    printf("entered Resolving clauses\n");
    Clause* resolution = malloc(sizeof(Clause) * (clause_a->size + clause_b->size));
    for (int i = 0; i < clause_a->size; i++){
        if (clause_a->literals[i] != *literal && clause_a->literals[i] != -*literal){
            add_literal(resolution, clause_a->literals[i]);
        }
    }
    for (int i = 0; i < clause_b->size; i++){
        if (clause_b->literals[i] != *literal && clause_b->literals[i] != -*literal){
            add_literal(resolution, clause_b->literals[i]);
        }
    }
    return resolution;
}

int conflict_analysis(Formula* formula, Clause* clause, Assignment* assignment){
    printf("entered conflict_analysis\n");
    if (assignment->current_depth_level == 0){
        return (-1);
    }

    // find the literals with assignment on this decision level
    Literal* literals_at_current_depth = (Literal*)malloc(sizeof(Literal) * clause->size);
    int count=0;
    // collect the literals that are assigned at the current depth
    for (int i = 0; i < clause->size; i++){
        if (assignment->depth[abs(literals_at_current_depth[i])-1] == assignment->current_depth_level){
            literals_at_current_depth[count] = clause->literals[i];
            count++;
        }
    }


    // select any literal on the current depth level
    Clause* learned_clause = create_clause(formula->num_variables);
    while (count > 1) {
        // Filter for implied literals
        int i;
        for (i = 0; i < count; i++) {
            if (assignment->antecedent_clause[abs(literals_at_current_depth[i])-1] != ANTECEDENT_CLAUSE_NONE)
                break;
        }


        Literal* literal = literals_at_current_depth[i];
        Clause* antecedent = &formula->clauses[assignment->antecedent_clause[abs(literal)-1]];

        learned_clause = resolve_clauses(clause, antecedent, literal);

        // Rebuild temp with new clause
        count = 0;
        for (int i = 0; i < learned_clause->size; i++) {
            if (assignment->depth[abs(learned_clause->literals[i])-1] == assignment->current_depth_level) {
                literals_at_current_depth[count++] = learned_clause->literals[i];
            }
        }
    }
    int* deision_levels = (int*)malloc(sizeof(int) * learned_clause->size);
    if (learned_clause->size <=1){
        return 0;
    } else{
        qsort(deision_levels, learned_clause->size, sizeof(int), comp);
        // return second largest decision level
        return deision_levels[1];
    }
}


//check if clause is unit, sat, unsat or unresolved
int clause_status(Clause* clause, Assignment* assignment){
    int total_assigned =0;
    for (int i = 0; i < clause->size; i++){
        int literal = clause->literals[i];
        bool is_assigned = assignment->assigned[abs(literal)-1];
        bool literal_value = assignment->values[abs(literal)-1];
        total_assigned = total_assigned + is_assigned;

        // check if the literal in the clause is assigned and is set to True
        if(is_assigned == true && \
        ((literal > 0 && literal_value == true) || \
        (literal < 0 && literal_value == false)) ){
            return CLAUSE_SAT;
        }
    }
    if(total_assigned == clause->size){
        return CLAUSE_UNSAT;
    }else if(total_assigned == clause->size-1){
        return CLAUSE_UNIT;
    }else{
        return CLAUSE_UNRESOLVED;
    }
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
    assignment->depth = (int*)calloc(num_variables, sizeof(int));
    assignment->antecedent_clause = (int*)calloc(num_variables, sizeof(int));
    assignment->current_depth_level = (int)calloc(1, sizeof(int));

    if (!assignment->values || !assignment->assigned || !assignment->depth || !assignment->antecedent_clause) {
        perror("Failed to allocate memory for assignment arrays");
        exit(EXIT_FAILURE);
    }
    
    assignment->size = num_variables;
    for(int i = 0; i < num_variables; i++){
        assignment->depth[i] = 0;
        assignment->antecedent_clause[i] = ANTECEDENT_CLAUSE_NONE;
    }
    assignment->current_depth_level = 0;
    return assignment;
}

// backtrack an assignment to decision level
void backtrack_assignment(Assignment* assignment, int backtrack_level){
    // reset all assignments after backtrack level
    for(int i = 0; i < assignment->size; i++){
        if(assignment->depth[i] >= backtrack_level){
            assignment->values[i] = false;
            assignment->assigned[i] = false;
            assignment->antecedent_clause[i] = ANTECEDENT_CLAUSE_NONE;
            assignment->depth[i] = 0;
        }
    }
}
// Free an assignment
void free_assignment(Assignment* assignment) {
    if (!assignment) return;
    free(assignment->values);
    free(assignment->assigned);
    free(assignment->depth);
    free(assignment->antecedent_clause);
    free(assignment->current_depth_level);
    free(assignment);
}


// Print the assignment
void print_assignment(Assignment* assignment) {
    printf("ASSIGNMENT: ");
    for (int i = 0; i < assignment->size; i++) {
        if (assignment->assigned[i]) {
            printf("%d=%s ", i + 1, assignment->values[i] ? "1" : "0");
        } else {
            printf("%d=0 " , i + 1);
        }
        printf("depth: %d, antecedent_clause: %d\n", assignment->depth[i], assignment->antecedent_clause[i]);
    }
} 