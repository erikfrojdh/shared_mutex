
#include <errno.h> // errno
// #include <fcntl.h> // O_CREAT, O_TRUNC..
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // shared memory
#include <sys/stat.h> // fstat
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "Data.h"

// A side - creates and destroys the shared memory

int main() {

    // Create new shm, delete the old one if it exists
    int fd = shmget(SHM_KEY, sizeof(Data), IPC_CREAT | IPC_EXCL | SHM_PERM);
    if (fd == -1 && errno == EEXIST) {
        printf("WARNING: shm exists removing the old shm\n");

        // recommended way seems to be using ipcrm if size is not known
        char cmd[21];
        sprintf(cmd, "ipcrm -M 0x%08x", SHM_KEY); // remove only this shm
        system(cmd);
        fd = shmget(SHM_KEY, sizeof(Data), IPC_CREAT | IPC_EXCL | SHM_PERM);
    }
    if (fd == -1) {
        printf("ERROR: Create shared memory failed: [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Create mutex attributes to be able to set PTHREAD_PROCESS_SHARED
    pthread_mutexattr_t mta;
    pthread_mutexattr_init(&mta);
    pthread_mutexattr_setpshared(&mta, PTHREAD_PROCESS_SHARED);

    // Attach shm and initialize counter and mutex
    Data *shm = shmat(fd, NULL, 0);
    shm->counter = 0;
    if (pthread_mutex_init(&shm->m, &mta) != 0) {
        printf("ERROR Failed to initialize mutex in shm. [%s]\n",
               strerror(errno));
        return EXIT_FAILURE;
    }

    // Lock and sleep to make it visible that we hold the lock
    for (int i = 0; i < 100; ++i) {
        pthread_mutex_lock(&shm->m);
        printf("Shared mutex locked. Counter: %d", shm->counter);
        fflush(stdout);
        shm->counter += 10; // just for the sake of it
        usleep(1000 * 1000);
        pthread_mutex_unlock(&shm->m);
        printf(" Shared mutex unlocked\n");
        usleep(1000 * 1000);
    }


    if (shmdt(shm)) {
        printf("ERROR wile detaching shared memory. [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if(shmctl(fd, IPC_RMID, NULL) == -1){
        printf("ERROR while removing shared memory. [%s]\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pthread_mutexattr_destroy(&mta);
    printf("Bye!\n");
}