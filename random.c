#include <time.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]){
    int n_of_cores = 1;
    char cmd[100];
    for(n_of_cores = 1; n_of_cores <= 6; n_of_cores++) {
        sprintf(cmd, "OMP_NUM_THREADS=%d", n_of_cores);

        printf("ENV: %s.\n", cmd);
        putenv(cmd);
        printf("Running with %d threads (4 threads per core).\n", n_of_cores);
        system("./odijkstra");
    }
}


