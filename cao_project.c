#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

// Define the path to the sysfs interface for CPU frequency scaling
#define CPU_FREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq/"

// Define thresholds for CPU utilization and I/O activity
#define IDLE_THRESHOLD 10
#define IO_THRESHOLD 5

// Function to set CPU frequency using DVFS based on CPU utilization and I/O activity
void set_cpu_frequency(int cpu_utilization, int io_activity) {
    // Define the path to the scaling_setspeed file
    char freq_file_path[100];
    snprintf(freq_file_path, sizeof(freq_file_path), "%s%s", CPU_FREQ_PATH, "scaling_setspeed");

    FILE* freq_file = fopen(freq_file_path, "w");
    if (freq_file == NULL) {
        perror("Error opening scaling_setspeed file");
        return; // Error
    }

    int freq_kHz;

    if (cpu_utilization < IDLE_THRESHOLD || io_activity <= IO_THRESHOLD) {
        // If CPU is idle or there's low I/O activity, set a lower frequency (e.g., 1 GHz)
        freq_kHz = 1000000;
    } else {
        // Otherwise, set a higher frequency (e.g., 2 GHz)
        freq_kHz = 2000000;
    }

    // Set the frequency
    fprintf(freq_file, "%d", freq_kHz);

    fclose(freq_file);
}

int main(int argc, char* argv[]) {
    int cpu_utilization = 5; // Simulated CPU utilization (0-100%)
    int io_activity = 3;     // Simulated I/O activity (0-100%)

    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Distribute the simulation to all MPI processes
    MPI_Bcast(&cpu_utilization, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&io_activity, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform CPU frequency adjustment
    set_cpu_frequency(cpu_utilization, io_activity);

    // Synchronize all MPI processes
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("CPU frequency adjusted based on CPU utilization: %d%% and I/O activity: %d%%\n", cpu_utilization, io_activity);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}

