# SAT Solver Analysis Tool

This directory contains tools for analyzing the performance of the SAT solver on benchmark problems.

## Benchmark Analysis

The `run_cbs_analysis` program runs the SAT solver on all CNF files in a specified benchmark directory and measures execution time for each instance.

### Building

```bash
make
```

### Running

Make sure the main SAT solver has been built first:

```bash
# In the project root directory
make
```

Then run the analysis tool with a directory name as an argument:

```bash
# In the analysis directory
./run_cbs_analysis <directory_name>
```

For example:

```bash
./run_cbs_analysis CBS_k3_n100_m423_b50
```

This will run the solver on all .cnf files found in `examples/CBS_k3_n100_m423_b50/`.

### Output

The program produces a CSV file named `<directory_name>_timing_analysis.csv` with the following format:

```
Filename,Execution Time (seconds)
CBS_k3_n100_m423_b50_900.cnf,0.123456
CBS_k3_n100_m423_b50_901.cnf,0.234567
...
```

You can import this CSV file into any spreadsheet software for further analysis.

### Cleaning Up

```bash
make clean
``` 