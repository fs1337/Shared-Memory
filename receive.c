#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>

#define SIZE_MEMORY_SIZE 1024
sem_t *sem_id;

char* receive(char *line, const char *code)
{
    if(strstr(line, code) != NULL)
    {
        return line;
    }
    else
    {
        return NULL;
    }
}

FILE* read_file(char *p)
{
    //check whether the file exists or not
    if(p == NULL)
    {
        return NULL;
    }
    
    //open file
    FILE *fp;
    fp = fopen(p, "r");
    if (fp == NULL) {
        printf("Sorry, the file can not open!\n");
        return NULL;
    }
    
    return fp;
}

int main(void)
{
    int id;
    int key = 9999;
    
    //create shared memory
    //id: the id for shared memory
    id = shmget(key, SIZE_MEMORY_SIZE, IPC_CREAT | 0666);
    if (id == -1) {
        perror("Failed to create shared memory segment!");
        exit(1);
    }
    
    //attach the segment to our data space.
    char* flagShm;
    if ((flagShm = shmat(id, NULL, 0)) == (char *) -1) {
        perror("shmat error!");
        exit(1);
    }
    
    //open a semaphore
    //if not exist, create one.
    sem_id = sem_open("/mysem", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_post(sem_id);
    
    //if the line contains the secret code "C00L".
    //sends this information to the the shared memory
    while(1)
    {
        //get alpha numeric strings as input from the user
        //one line at a time
        sleep(2);
        
        //read file
        char line[128];
        FILE *fp;
        if((fp = read_file("receive.txt"))!=NULL)
        {
            
            while(fgets(line, 1024, fp) != NULL)
            {
                //printf ("line is %s", line);
                sleep(1);
                if(receive(line, "C00L")!= NULL)
                {
                    //printf ("flagShm is: %s", line);
                    //printf("waiting\n");
                    sem_wait(sem_id);//wait for semaphore
                    sprintf (flagShm, "%s", line);
                    sem_post(sem_id);//release semaphore
                    //printf("releasing\n");
                    //wait until the line has read
                    //'#' meanst that the line has read
                    while(*flagShm != '#')
                    {
                        sleep(1);
                    }
                }
                
            }
            fclose(fp);
        }
    }
    
    //Detach the shared memory segment.
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
    
}
