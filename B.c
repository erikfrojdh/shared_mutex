#include "Data.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

// B side - only opens shared memory and accsess the variables there

int main() {
    // Open shared memory
    int fd = shmget(SHM_KEY, sizeof(Data), SHM_PERM);
    if (fd == -1) {
        printf("ERROR when trying to open shared memory: [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Attach shared memory
    Data *shm = shmat(fd, NULL, 0);
    if (shm == (void *)-1) {
        printf("ERROR while attaching shared memory. [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Increment the counter in a loop, usleep to allow the other process to
    // get a hold of the mutex.
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&shm->m);
        shm->counter++;
        printf("Counter: %d\n", shm->counter);
        pthread_mutex_unlock(&shm->m);
        usleep(1000 * 10);
    }

    // Detach but don't destroy since A is in control of the shared memory
    if (shmdt(shm)) {
        printf("ERROR wile detaching shared memory. [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}