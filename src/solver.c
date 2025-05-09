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
int unit_propagation(Formula* formula, Assignment* assignment, Clause** conflict_clause) {
    bool end_propagation = false;

    // loop finishes, after no more unit/conflict clauses are found
    do {
        end_propagation = true;
        
        for (int i = 0; i < formula->size; i++) {
            Clause* clause = &formula->clauses[i];
            int status = clause_status(clause, assignment);
            if(status == CLAUSE_SAT || status == CLAUSE_UNRESOLVED){
                // goes to next clause in formula
                continue;
            }else if(status == CLAUSE_UNIT){
                // find unit literal and assign it
                for (int j = 0; j < clause->size; j++){
                    Literal lit = clause->literals[j];
                    int var = abs(lit);
                    if(!assignment->assigned[var-1]){
                        assignment->assigned[var-1] = true;
                        assignment->values[var-1] = (lit > 0);
                        assignment->depth[var-1] = assignment->current_depth_level;
                        assignment->antecedent_clause[var-1] = i;
                        end_propagation = false;
                        break;
                    }   
                }
            }else{
                // this is CLAUSE_CONFLICT
                *conflict_clause = clause;
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
            assignment->assigned[i] = true;
            assignment->values[i] = true; // assign true to the variable
            assignment->depth[i] = assignment->current_depth_level;            
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
        // Choose an unassigned literal and assign it true
        assignment->current_depth_level++;
        int var = choose_variable(formula, assignment);

            while (true){
                int reason = unit_propagation(formula, assignment, &conflict_clause);
                if (reason != UIP_CONFLICT){
                    // no conflict after UIP, so return to branching
                    break;
                }
                int b = conflict_analysis(formula, conflict_clause, assignment);

                // if conflict was 'backpropagated' to the root,
                // then we have no solution
                if ( b < 0 ){
                    return false;
                }

                backtrack_assignment(assignment, b);
                assignment->current_depth_level = b;
            }
    } 
    // SAT found
    //print_assignment(assignment); -> this is in main() too
    return true;
    
}

Clause* resolve_clauses(Clause* clause_a, Clause* clause_b, Literal literal){

    // since we are using add_literal, the resolution clause will have a meaningful size
    Clause* resolution = create_clause();
    if(!resolution){
        perror("Failed to allocate memory for resolution");
        exit(EXIT_FAILURE);
    }
    int size_of_resolution = 0;
    for (int i = 0; i < clause_a->size; i++){
        if (clause_a->literals[i] != literal && clause_a->literals[i] != -literal){
            add_literal(resolution, clause_a->literals[i]);
            size_of_resolution++;
        }
    }
    for (int i = 0; i < clause_b->size; i++){
        if (clause_b->literals[i] != literal && clause_b->literals[i] != -literal && clause_b->literals[i] != 0){
            // check if the literal already exists in the resolution
            bool already_in_resolution = false;
            for (int j = 0; j < size_of_resolution; j++){
                if (resolution->literals[j] == clause_b->literals[i]){
                    already_in_resolution = true;
                    break;
                }
            }
            if (!already_in_resolution){
                add_literal(resolution, clause_b->literals[i]);
                // since we are not parsing size, we don't need to increment
                // size_of_resolution++;
            }
        }
    }
    return resolution;
}

int conflict_analysis(Formula* formula, Clause* clause, Assignment* assignment){
    if (assignment->current_depth_level == 0){
        return (-1);
    }
    // find the literals with assignment on this decision level
    Literal* literals_at_current_depth = (Literal*)malloc(sizeof(Literal) * clause->size);
    if(!literals_at_current_depth){
        perror("Failed to allocate memory for literals_at_current_depth");
        exit(EXIT_FAILURE);
    }
    int count=0;
    // collect the literals that are assigned at the current depth
    for (int i = 0; i < clause->size; i++){
        if (assignment->depth[abs(clause->literals[i])-1] == assignment->current_depth_level){
            literals_at_current_depth[count] = clause->literals[i];
            count++;
        }
    }

    // select any literal on the current depth level
    Clause* learned_clause = create_clause();
    learned_clause = clause;
    //printf("assignment when calculating backtrack level: ");
    //print_assignment(assignment);
    while (count > 1 ) {
        // Filter for implied literals
        int i;
        // pick any literal that is implied
        for (i = 0; i < count; i++) {
            if (assignment->antecedent_clause[abs(literals_at_current_depth[i])-1] != ANTECEDENT_CLAUSE_NONE){
                break;
            }
        }


        Literal literal = literals_at_current_depth[i];
        Clause antecedent = formula->clauses[assignment->antecedent_clause[abs(literal)-1]];

        learned_clause = resolve_clauses(learned_clause, &antecedent, literal);

        // Rebuild temp with new clause
        count = 0;
        for (int i = 0; i < learned_clause->size; i++) {
            if (assignment->depth[abs(learned_clause->literals[i])-1] == assignment->current_depth_level) {
                literals_at_current_depth[count] = learned_clause->literals[i];
                count++;
            }
        }
    }

    add_clause(formula, learned_clause);

    int* deision_levels = (int*)malloc(sizeof(int) * learned_clause->size);
    for (int i = 0; i < learned_clause->size; i++){
        deision_levels[i] = assignment->depth[abs(learned_clause->literals[i])-1];
    }
    if (learned_clause->size <=1){
        return 0;
    } else{
        // return second largest decision level
        return getSecondLargest(deision_levels, learned_clause->size);
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
    for(int i = 0; i < assignment->size; i++){
        // reset all assignments after backtrack_level
        if(assignment->depth[i] > backtrack_level){
            assignment->values[i] = false;
            assignment->assigned[i] = false;
            assignment->antecedent_clause[i] = ANTECEDENT_CLAUSE_NONE;
            assignment->depth[i] = 0;
            assignment->current_depth_level = backtrack_level;
        // for literals assigned at the backtrack_level, we assign the other value
        }else if(assignment->depth[i] == backtrack_level && assignment->antecedent_clause[i] == ANTECEDENT_CLAUSE_NONE){
            assignment->values[i] = !assignment->values[i];
        // free up the implied literals on backtrack_level
        }else if(assignment->depth[i] == backtrack_level && assignment->antecedent_clause[i] != ANTECEDENT_CLAUSE_NONE){
            assignment->values[i] = 0;
            assignment->assigned[i] = false;
            assignment->antecedent_clause[i] = ANTECEDENT_CLAUSE_NONE;
            assignment->depth[i] = 0;
        }
    }
}
// Free an assignment
void free_assignment(Assignment* assignment){
    free(assignment->values);
    free(assignment->assigned);
    free(assignment->depth);
    free(assignment->antecedent_clause);
    free(assignment);
}

// Print the clause
void print_clause(Clause* clause) {
    printf("\nsize: %d\n", clause->size);
    for (int i = 0; i < clause->size; i++) {
        printf("%d ", clause->literals[i]);
    }
    printf("\n");
}

// Print the assignment
void print_assignment(Assignment* assignment) {
    printf("ASSIGNMENT: ");
    for (int i = 0; i < assignment->size; i++) {
        if (assignment->assigned[i]) {
            printf("%d=%s ", i + 1, assignment->values[i] ? "1" : "0");
            //printf("assigned: %d, depth: %d, antecedent_clause: %d\n", assignment->assigned[i], assignment->depth[i], assignment->antecedent_clause[i]);
        } else {
            printf("%d=NOT ASSIGNED " , i + 1);
        }
    }
    printf("\n");
    //printf("current_depth_level: %d\n", assignment->current_depth_level);
} 