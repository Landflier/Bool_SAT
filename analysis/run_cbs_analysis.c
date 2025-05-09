#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PATH 1024
#define MAX_COMMAND 2048

// Function to run the solver on a given file and measure execution time
double run_solver(const char *solver_path, const char *cnf_file) {
    struct timeval start, end;
    double elapsed_time;
    pid_t pid;
    int status;
    
    gettimeofday(&start, NULL);
    
    // Fork a process to run the solver
    pid = fork();
    
    if (pid == -1) {
        perror("Fork failed");
        return -1;
    } else if (pid == 0) {
        // Child process: execute the solver
        execl(solver_path, solver_path, cnf_file, NULL);
        
        // If execl returns, there was an error
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to finish
        waitpid(pid, &status, 0);
        
        gettimeofday(&end, NULL);
        
        // Calculate elapsed time in seconds
        elapsed_time = (end.tv_sec - start.tv_sec) + 
                       (end.tv_usec - start.tv_usec) / 1000000.0;
        
        return elapsed_time;
    }
}

void print_usage(const char *program_name) {
    printf("Usage: %s <directory_name>\n", program_name);
    printf("Runs the SAT solver on all .cnf files in examples/<directory_name> and measures execution time.\n");
    printf("Results are saved to <directory_name>_timing_analysis.csv\n");
}

int main(int argc, char **argv) {
    DIR *dir;
    struct dirent *entry;
    char examples_dir[MAX_PATH] = "../examples";
    char target_dir_path[MAX_PATH];
    char solver_path[MAX_PATH] = "../bin/sat_solver";
    char output_file[MAX_PATH];
    char full_path[MAX_PATH];
    FILE *output;
    int file_count = 0;
    
    // Check for directory name argument
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Use the provided directory name
    const char *target_dir_name = argv[1];
    
    // Construct paths using the provided directory name
    snprintf(target_dir_path, MAX_PATH, "%s/%s", examples_dir, target_dir_name);
    snprintf(output_file, MAX_PATH, "%s_timing_analysis.csv", target_dir_name);
    
    printf("Target directory: %s\n", target_dir_path);
    printf("Output file: %s\n", output_file);
    
    // Check if solver exists
    if (access(solver_path, X_OK) != 0) {
        fprintf(stderr, "Error: Solver not found or not executable at %s\n", solver_path);
        fprintf(stderr, "Make sure to build the solver first with 'make' command in the project root\n");
        return EXIT_FAILURE;
    }
    
    // Check if target directory exists
    if (access(target_dir_path, F_OK) != 0) {
        fprintf(stderr, "Error: Directory %s does not exist\n", target_dir_path);
        return EXIT_FAILURE;
    }
    
    // Open the target directory
    dir = opendir(target_dir_path);
    if (!dir) {
        fprintf(stderr, "Error: Could not open directory %s\n", target_dir_path);
        return EXIT_FAILURE;
    }
    
    // Create output CSV file
    output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "Error: Could not create output file %s\n", output_file);
        closedir(dir);
        return EXIT_FAILURE;
    }
    
    // Write CSV header
    fprintf(output, "Filename,Execution Time (seconds)\n");
    
    printf("Running SAT solver on examples in %s...\n", target_dir_path);
    
    // Process each .cnf file in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Skip if not a .cnf file
        if (strstr(entry->d_name, ".cnf") == NULL) {
            continue;
        }
        
        // Construct full path to the .cnf file
        snprintf(full_path, MAX_PATH, "%s/%s", target_dir_path, entry->d_name);
        
        printf("Processing: %s\n", entry->d_name);
        
        // Run solver and measure time
        double elapsed_time = run_solver(solver_path, full_path);
        
        if (elapsed_time >= 0) {
            // Write result to CSV
            fprintf(output, "%s,%.6f\n", entry->d_name, elapsed_time);
            printf("  Time: %.6f seconds\n", elapsed_time);
            file_count++;
        } else {
            fprintf(output, "%s,ERROR\n", entry->d_name);
            printf("  Error running solver\n");
        }
        
        // Flush output to ensure results are written even if the program crashes
        fflush(output);
    }
    
    // Clean up
    fclose(output);
    closedir(dir);
    
    if (file_count > 0) {
        printf("Analysis complete. Processed %d files. Results written to %s\n", file_count, output_file);
    } else {
        printf("No .cnf files found in %s\n", target_dir_path);
    }
    
    return EXIT_SUCCESS;
} 