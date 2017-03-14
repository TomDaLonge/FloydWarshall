# FloydWarshall
Parallel Implementation of pathfinding algorithm using openMP

# Course
Intelligent Systems Design / 7. Semester

# Purpose
This project was done to measure the speedup of a parallel implementation of Floyd-Warshall

# Description
Implementation of the pathfinding algorithm Floyd-Warshall. Can be build as Single Core and MultiCore.
The OpenMP pragmas in the ProcedureControl.cpp are ignored by gcc, if the -fopenmp flag is not set, 
so you can easily create two files (parallel / not parallel).

# API used
OpenMP

# Build
Build it yourself with the RebuildAll.bat. Of course you need gcc to do so.

# Execute
1. Parameter: Input Text File / Demanded
2. Parameter: LogLevel (0-2) / Optional
3. Parameter: DumpToFile (bool) / Optional
