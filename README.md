# shared_mutex

Minimal example for creating a pthread_mutex that is shared between two processes.
A creates the shared memory and initializes the mutex. 
B opens and locks the mutex for acces to the data.

```
make
./a

#then in another teminal 
./b

```
