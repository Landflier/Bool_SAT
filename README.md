# Boolean SAT Solver

A simple Boolean Satisfiability (SAT) solver implemented in C. This solver uses the DPLL (Davis-Putnam-Logemann-Loveland) algorithm to find a satisfying assignment for a Boolean formula in Conjunctive Normal Form (CNF).

## Features

- Parses CNF formulas in DIMACS format
- Implements the DPLL algorithm with unit propagation
- Detects satisfiability and provides a satisfying assignment
- Verification of the found solution

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

## Algorithm

The solver implements the DPLL (Davis-Putnam-Logemann-Loveland) algorithm, which includes:

1. **Unit Propagation**: If a clause has only one unassigned literal, assign a value that makes the clause true
2. **Pure Literal Elimination**: If a variable appears only with one polarity, assign it to make those literals true
3. **Backtracking Search**: Choose an unassigned variable and recursively search with both possible assignments

## License

This project is open source and available under the MIT License. 