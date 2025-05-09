#!/usr/bin/env python3
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import glob
import re


# Update rcParams to set the plot styles globally
plt.rcParams.update({
    'figure.figsize': (8.27, 5.83),            # Figure size
    'font.size': 12,                           # Global font size
    'axes.labelsize': 12,                      # X and Y axis label font size
    'xtick.labelsize': 12,                     # X-axis tick label font size
    'ytick.labelsize': 12,                     # Y-axis tick label font size
    'lines.linewidth': 1,                      # Line width
    'grid.color': 'grey',                      # Grid line color
    'grid.linestyle': '--',                    # Grid line style
    'grid.linewidth': 0.8,                     # Grid line width
    'axes.grid': True,                         # Enable grid
    'legend.fontsize': 10,                     # Legend font size
    'legend.loc': 'upper right',               # Legend location
    'axes.spines.top': True,                   # Show top spine
    'axes.spines.right': True,                 # Show right spine
    'savefig.dpi': 200,                        # DPI for savefigure
})

# Create directory for figures if it doesn't exist
if not os.path.exists('figures'):
    os.makedirs('figures')

# Get all CSV files in the current directory
csv_files = glob.glob('*.csv')

# Process each CSV file
for csv_file in csv_files:
    print(f"Processing {csv_file}...")
    
    # Extract benchmark name from filename
    benchmark_name = os.path.splitext(csv_file)[0]
    if "_timing_analysis" in benchmark_name:
        benchmark_name = benchmark_name.replace("_timing_analysis", "")
    elif "_timing_results" in benchmark_name:
        benchmark_name = benchmark_name.replace("_timing_results", "")
    
    # Load the data
    df = pd.read_csv(csv_file)
    
    # Check if there's data in the file
    if df.empty or len(df) < 2:
        print(f"  Warning: Not enough data in {csv_file} to generate meaningful plots")
        continue
    
    # Rename columns if needed for consistency
    if 'Execution Time (seconds)' in df.columns:
        df.rename(columns={'Execution Time (seconds)': 'Time'}, inplace=True)
    
    # Basic statistics
    avg_time = df['Time'].mean()
    median_time = df['Time'].median()
    min_time = df['Time'].min()
    max_time = df['Time'].max()
    std_time = df['Time'].std()
    
    print(f"  Statistics for {benchmark_name}:")
    print(f"    Average time: {avg_time:.6f} seconds")
    print(f"    Median time: {median_time:.6f} seconds")
    print(f"    Min time: {min_time:.6f} seconds")
    print(f"    Max time: {max_time:.6f} seconds")
    print(f"    Standard deviation: {std_time:.6f} seconds")
    
    # Save statistics to a text file
    with open(f"figures/{benchmark_name}_stats.txt", "w") as f:
        f.write(f"Statistics for {benchmark_name}:\n")
        f.write(f"  Number of instances: {len(df)}\n")
        f.write(f"  Average time: {avg_time:.6f} seconds\n")
        f.write(f"  Median time: {median_time:.6f} seconds\n")
        f.write(f"  Min time: {min_time:.6f} seconds\n")
        f.write(f"  Max time: {max_time:.6f} seconds\n")
        f.write(f"  Standard deviation: {std_time:.6f} seconds\n")
    
    # Extract instance numbers from filenames if available
    instance_nums = []
    for filename in df['Filename']:
        # Try to find a number at the end of the filename (before .cnf)
        match = re.search(r'(\d+)\.cnf$', filename)
        if match:
            instance_nums.append(int(match.group(1)))
        else:
            # If no number is found, use index position
            instance_nums.append(0)
    
    # Add instance numbers to dataframe if they were extracted
    if any(instance_nums) and not all(x == 0 for x in instance_nums):
        df['Instance'] = instance_nums
        
        # Sort by instance number for better visualization
        df.sort_values('Instance', inplace=True)
        
        # Plot execution time vs instance number
        plt.figure(figsize=(10, 6))
        plt.plot(df['Instance'], df['Time'], 'o-', alpha=0.7)
        plt.title(f'Execution Time by Instance Number - {benchmark_name}')
        plt.xlabel('Instance Number')
        plt.ylabel('Execution Time (seconds)')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(f'figures/{benchmark_name}_time_by_instance.png')
        plt.close()
    
    # Histogram of execution times
    plt.figure(figsize=(10, 6))
    plt.hist(df['Time'], bins=min(30, len(df)//2), alpha=0.7, color='teal')
    plt.axvline(avg_time, color='red', linestyle='dashed', linewidth=1, label=f'Mean: {avg_time:.4f}s')
    plt.axvline(median_time, color='green', linestyle='dashed', linewidth=1, label=f'Median: {median_time:.4f}s')
    plt.title(f'Distribution of Execution Times - {benchmark_name}')
    plt.xlabel('Execution Time (seconds)')
    plt.ylabel('Frequency')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'figures/{benchmark_name}_time_histogram.png')
    plt.close()
    
    # Box plot of execution times
    plt.figure(figsize=(8, 6))
    plt.boxplot(df['Time'], vert=True, patch_artist=True, boxprops=dict(facecolor='lightblue'))
    plt.title(f'Execution Time Distribution - {benchmark_name}')
    plt.ylabel('Execution Time (seconds)')
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'figures/{benchmark_name}_time_boxplot.png')
    plt.close()
    
    # Cumulative distribution function (CDF)
    plt.figure(figsize=(10, 6))
    sorted_data = np.sort(df['Time'])
    yvals = np.arange(1, len(sorted_data)+1) / len(sorted_data)
    plt.plot(sorted_data, yvals, '-', linewidth=2)
    plt.title(f'Cumulative Distribution of Execution Times - {benchmark_name}')
    plt.xlabel('Execution Time (seconds)')
    plt.ylabel('Proportion of Instances')
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'figures/{benchmark_name}_time_cdf.png')
    plt.close()

print("\nAll plots saved in the 'figures' directory!")

# Create a comparison plot if multiple CSV files exist
if len(csv_files) > 1:
    print("\nCreating comparison plots...")
    
    # Prepare data for comparison
    stats_data = []
    for csv_file in csv_files:
        benchmark_name = os.path.splitext(csv_file)[0]
        if "_timing_analysis" in benchmark_name:
            benchmark_name = benchmark_name.replace("_timing_analysis", "")
        elif "_timing_results" in benchmark_name:
            benchmark_name = benchmark_name.replace("_timing_results", "")
        
        df = pd.read_csv(csv_file)
        
        # Skip empty dataframes
        if df.empty:
            continue
            
        # Rename columns if needed for consistency
        if 'Execution Time (seconds)' in df.columns:
            df.rename(columns={'Execution Time (seconds)': 'Time'}, inplace=True)
        
        stats_data.append({
            'Benchmark': benchmark_name,
            'Mean': df['Time'].mean(),
            'Median': df['Time'].median(),
            'Min': df['Time'].min(),
            'Max': df['Time'].max(),
            'StdDev': df['Time'].std(),
            'Count': len(df)
        })
    
    # Create comparison dataframe
    stats_df = pd.DataFrame(stats_data)
    
    # Bar chart comparing mean execution times
    plt.figure(figsize=(12, 6))
    bars = plt.bar(stats_df['Benchmark'], stats_df['Mean'], yerr=stats_df['StdDev'], 
           alpha=0.7, capsize=10, color='skyblue', ecolor='black')
    
    # Add value labels on top of bars
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                f'{height:.4f}s', ha='center', va='bottom', rotation=0)
    
    plt.title('Comparison of Mean Execution Times Across Benchmarks')
    plt.xlabel('Benchmark')
    plt.ylabel('Mean Execution Time (seconds)')
    plt.xticks(rotation=45, ha='right')
    plt.grid(True, axis='y', alpha=0.3)
    plt.tight_layout()
    plt.savefig('figures/comparison_mean_times.png')
    plt.close()
    
    # Box plot comparison
    data_for_boxplot = []
    labels = []
    
    for csv_file in csv_files:
        benchmark_name = os.path.splitext(csv_file)[0]
        if "_timing_analysis" in benchmark_name:
            benchmark_name = benchmark_name.replace("_timing_analysis", "")
        elif "_timing_results" in benchmark_name:
            benchmark_name = benchmark_name.replace("_timing_results", "")
        
        df = pd.read_csv(csv_file)
        
        # Skip empty dataframes
        if df.empty:
            continue
            
        # Rename columns if needed for consistency
        if 'Execution Time (seconds)' in df.columns:
            df.rename(columns={'Execution Time (seconds)': 'Time'}, inplace=True)
        
        data_for_boxplot.append(df['Time'].values)
        labels.append(benchmark_name)
    
    plt.figure(figsize=(12, 6))
    plt.boxplot(data_for_boxplot, labels=labels, patch_artist=True, 
               boxprops=dict(facecolor='lightblue'))
    plt.title('Comparison of Execution Time Distributions')
    plt.ylabel('Execution Time (seconds)')
    plt.xticks(rotation=45, ha='right')
    plt.grid(True, axis='y', alpha=0.3)
    plt.tight_layout()
    plt.savefig('figures/comparison_boxplots.png')
    plt.close()
    
    # Save comparison statistics to a file
    stats_df.to_csv('figures/benchmark_comparison_stats.csv', index=False)
    
    print("Comparison plots saved in the 'figures' directory!")
