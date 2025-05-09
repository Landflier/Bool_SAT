CC = gcc
CFLAGS = -Wall -Wextra -O2

all: run_cbs_analysis

run_cbs_analysis: run_cbs_analysis.c
	$(CC) $(CFLAGS) -o run_cbs_analysis run_cbs_analysis.c

clean:
	rm -f run_cbs_analysis *_timing_analysis.csv

.PHONY: all clean 