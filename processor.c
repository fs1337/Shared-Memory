#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <semaphore.h>

#define SIZE_MEMORY_SIZE 1024
sem_t *sem_id;

//create a new output file
FILE* create_file()
{
    FILE *fp;
    fp = fopen("secret.txt", "w+");
    if (fp == NULL) {
        printf("Sorry, the file can not open!\n");
        return NULL;
    }
    
    fprintf(fp, "aaaaa");
    
    return fp;
}

int main()
{
    //create shared memory
    //id: the id for shared memory
    int key = 9999;
    int id;
    
    //create a new output file
    //FILE *fp = create_file();
    
    //
    setbuf(stdout, NULL);
    FILE *fp = fopen("secret.txt", "w+");
    if(fp==NULL)
    {
        printf("Sorry, secret.txt can not be generated!");
        return 1;
    }
    
    //located the shared memory
    id = shmget(key, SIZE_MEMORY_SIZE, IPC_CREAT | 0666);
    if (id == -1) {
        perror("Failed to create shared memory segment!");
        exit(1);
    }
    
    //open a semaphore
    //if not exist, create one
    sem_id = sem_open("/mysem", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_post(sem_id);
    
    //attach the segment to our data space.
    char* flagShm;
    if ((flagShm = shmat(id, NULL, 0)) == (char *) -1) {
        perror("shmat error!");
        exit(1);
    }
    
    while(1)
    {
        sleep(2);
        //printf("waiting\n");
        sem_wait(sem_id);
        
        int length = strlen(flagShm);
        //printf("Length is:%d\n", length);
        //there is a line in the shared memory and
        //this line has not read
        if(length > 0 && *flagShm != '#')
        {
            //printf("%d,%s",length-1, flagShm);
            fprintf(fp, "%d,%s",length-1, flagShm);//write into secret.out
            fflush(fp);
            *flagShm = '#';//change the first character of the line to '#', means it has read
        }
        sem_post(sem_id);
        //printf("releasing\n");
        sleep(2);
    }
    // Detach the shared memory segment.
    shmdt(flagShm);
    //Deallocate the shared memory segment.
    shmctl (id, IPC_RMID, 0);
    
    //Close the semaphore
    if ( sem_close(sem_id) < 0 )
    {
        perror("sem_close");
    }
    
    //Remove a named semaphore  from the system.
    if (sem_unlink("/mysem") < 0 )
    {
        perror("sem_unlink!");
    }
    fclose(fp);
}
