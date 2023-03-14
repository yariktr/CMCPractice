#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

void Sig(int s) {
	remove("file");
	printf("Producing of modul is stopped\n");
	exit(0);
}

int main(int argc, char **argv) {				
	struct sembuf detail_params[3];
	key_t key; int fd;
    fd = creat("file", 0666);
    if(fd < 0) exit(1);
    key = ftok("file", 's');
    int semid = semget(key, 4, 0666 | IPC_CREAT);                               
    if(semid == -1) {
    	perror("id");
        exit(1);
    }
    signal(SIGINT, Sig);

    detail_params[0].sem_num = 0;     
    detail_params[0].sem_flg = 0;
    detail_params[1].sem_num = 1;     
    detail_params[1].sem_flg = 0;
    detail_params[2].sem_num = 3;
    detail_params[2].sem_flg = 0; 

	while(1) {
		detail_params[0].sem_op = -1;
		detail_params[1].sem_op = -1;
		detail_params[2].sem_op = 1;
		if(semop(semid, &detail_params[0], 1) == -1) exit(1);
		if(semop(semid, &detail_params[1], 1) == -1) exit(1);
		printf("modul is produced\n");
		if(semop(semid, &detail_params[2], 1) == -1) exit(1);
	}
}