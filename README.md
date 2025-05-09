# Boolean SAT Solver

A simple Boolean Satisfiability (SAT) solver implemented in C. This solver uses the DPLL (Davis-Putnam-Logemann-Loveland) algorithm to find a satisfying assignment for a Boolean formula in Conjunctive Normal Form (CNF).

## Features

- Parses CNF formulas in DIMACS format
- Implements the DPLL algorithm with unit propagation
- Detects satisfiability and provides a satisfying assignment
- Verification of the found solution
- Conflict analysis for improved backtracking

## Building

To build the SAT solver:

```bash
make
```

This will create the executable `bin/sat_solver`.

## Usage

Run the solver with a CNF formula file as input:

```bash
./bin/sat_solver path/to/cnf/file.cnf
```

Example:

```bash
./bin/sat_solver examples/simple.cnf
```

## CNF Format (DIMACS)

The solver accepts CNF formulas in the standard DIMACS format:

- Lines starting with 'c' are comments
- The problem line (starting with 'p') has the format: `p cnf <num_variables> <num_clauses>`
- Each clause is represented as a space-separated list of integers followed by a 0
- Positive integers represent positive literals, negative integers represent negative literals
- The trailing 0 marks the end of a clause

Example:

```
c Simple SAT example
c (x1 OR x2) AND (NOT x1 OR x3) AND (NOT x2 OR NOT x3 OR x4)
p cnf 4 3
1 2 0
-1 3 0
-2 -3 4 0
```

## Project Structure

- `src/` - Source code
  - `sat.h` - Main header file with data structures and function declarations
  - `formula.c` - Implementation of formula data structures
  - `parser.c` - DIMACS format parser
  - `solver.c` - DPLL algorithm implementation
  - `main.c` - Program entry point
- `examples/` - Example CNF formulas
- `bin/` - Compiled binary (created by make)
- `obj/` - Object files (created by make)

## Data Structures

The solver uses the following data structures defined in `sat.h`:

- **Literal**: Integer representation where positive values represent positive literals and negative values represent negated literals.
  
- **Clause**: A disjunction (OR) of literals
  ```c
  typedef struct {
      Literal* literals;   // Array of literals in the clause
      int size;            // Current number of literals
      int capacity;        // Maximum capacity of the literals array
  } Clause;
  ```

- **Formula**: A conjunction (AND) of clauses in CNF
  ```c
  typedef struct {
      Clause* clauses;     // Array of clauses
      int size;            // Current number of clauses
      int capacity;        // Maximum capacity of the clauses array
      int num_variables;   // Total number of variables in the formula
  } Formula;
  ```

- **Assignment**: Tracks the truth assignment and decision information
  ```c
  typedef struct {
      bool* values;           // Truth values for each variable (true/false)
      bool* assigned;         // Tracks which variables have been assigned
      int size;               // Number of variables
      int current_depth_level; // Current decision level
      int* depth;             // Decision level at which each variable was assigned
      int* antecedent_clause; // Clause that led to each assignment (for conflict analysis)
  } Assignment;
  ```

## Algorithm

The solver implements the DPLL (Davis-Putnam-Logemann-Loveland) algorithm with conflict-driven learning:

1. **Unit Propagation**: If a clause has only one unassigned literal, assign a value that makes the clause true
2. **Conflict Analysis**: When a conflict is detected, analyze the conflict to determine the cause and learn from it
3. **Backtracking**: When conflicts occur, backtrack to an appropriate decision level determined by conflict analysis
4. **Decision**: Choose an unassigned variable and recursively search with a value assignment

The solver uses various clause statuses to track resolution progress:
- `CLAUSE_SAT`: The clause is satisfied
- `CLAUSE_UNSAT`: The clause is unsatisfied
- `CLAUSE_UNIT`: The clause has only one unassigned literal
- `CLAUSE_UNRESOLVED`: The clause has multiple unassigned literals

## License

This project is open source and available under the MIT License. 