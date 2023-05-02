#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define partA 0
#define partB 1
#define partC 2
#define modul 3

int semid;

void Sig(int s) {
    printf("Production is stopped\n");
    remove("file");
    semctl(semid, 0, IPC_RMID, (int) 0);
    semctl(semid, 1, IPC_RMID, (int) 0);
    semctl(semid, 2, IPC_RMID, (int) 0);
    semctl(semid, 3, IPC_RMID, (int) 0);
    exit(0);
}

int main(int argc, char **argv) {
    struct sembuf product[4];
    key_t key; int cnt = 0, fd;
    fd = creat("file", 0666);
    if(fd < 0) exit(1);
    key = ftok("file", 's');
    semid = semget(key, 4, 0666 | IPC_CREAT);                            
    if(semid == -1) {
        perror("id");
        exit(1);
    }
    signal(SIGINT, Sig);

    product[partA].sem_num = 0;    
    product[partA].sem_flg = 0;
    product[partB].sem_num = 1;    
    product[partB].sem_flg = 0;
    product[partC].sem_num = 2;     
    product[partC].sem_flg = 0;     
    product[modul].sem_num = 3;   
    product[modul].sem_flg = 0;

    semctl(semid, partA, SETVAL, (int) 0);                        
    semctl(semid, partB, SETVAL, (int) 0);
    semctl(semid, partC, SETVAL, (int) 0);
    semctl(semid, modul, SETVAL, (int) 0);

    printf("Ctrl-C to stop \nProduction line:\n");

    while(1) {
        product[partC].sem_op = -1;             
        product[modul].sem_op = -1;

        semop(semid, &product[partC], 1);
        semop(semid, &product[modul], 1);
        cnt++;
        printf("Num of device: %d\n", cnt);
    }

    return 0;
}