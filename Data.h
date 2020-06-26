
#include <pthread.h>
#define SHM_KEY 0x0000162e
#define SHM_PERM 0666

// Data to be placed in the shared memory
// might be good to provide in init function
typedef struct Data {
  int counter;
  pthread_mutex_t  m;
} Data;