/* include library */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* define macro */
#define DEFAULT_PROTOCOL 0
#define BUF_SIZE 1024
#define TRUE 1
#define FALSE 0

typedef char element;

typedef union {
	unsigned value:10;
	char data[2];
} ECG;

typedef struct {
	int idx;
	unsigned value:10;
} E_DATA;

int push_csv(char*, E_DATA*);

element buf[BUF_SIZE];

/* main */
int main(int argc, char *argv[]) {

	char filename[100];
	printf("filename : ");
	scanf("%s", filename);
	strcat(filename, ".csv");

	int clen, sfd, cfd, n;
	struct sockaddr_in saddr, caddr;
	char *haddrp;

	if ( argc != 2 ) {
		printf("input port err\n");
		exit(0);
	}

	if ( (sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL)) == -1 ) {
		printf("sfd socket err\n");
		exit(0);
	}

	memset(&saddr, 0x00, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(atoi(argv[1]));

	if ( bind(sfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0 ) {
		printf("sfd bind err\n");
		exit(0);
	}

	if ( listen(sfd, 5) < 0 ) {
		printf("sfd listen err\n");
		exit(0);
	}

	memset(buf, '\0', sizeof(buf));
	printf("Wait for connection request\n");

	while(TRUE) {
		clen = sizeof(caddr);
		cfd = accept(sfd, (struct sockaddr *) &caddr, (socklen_t *) &clen);

		if ( cfd < 0 ) {
			printf("cfd accept err\n");
			exit(0);
		}

		haddrp = inet_ntoa(caddr.sin_addr);
		printf("    >> CLIENT : %s(%d) connected\n", haddrp, caddr.sin_port);

		int value_idx=0;
		while(TRUE) {
			int status, fd[2], val_idx=0;
			pipe(fd);
			pid_t pid;
			pid = fork();

			if ( pid == 0 ) {
				n = read(cfd, buf, sizeof(buf));
				write(fd[1], buf, sizeof(buf));
				exit(0);
			} else {
				waitpid(pid, &status, 0);
				read(fd[0], buf, sizeof(buf));
				printf("Buf received data, prosseing.\n");

				int i; ECG e; E_DATA ed; E_DATA eda[BUF_SIZE/2];
				for ( i=0; i<BUF_SIZE/2; i++ ) {
					e.data[0] = buf[i*2];
					e.data[1] = buf[i*2+1];

					ed.idx = value_idx;
					ed.value = e.value;
					eda[i] = ed;

					printf("[%5d]:%5d ", eda[i].idx, eda[i].value);
					if ( (i+1)%8 == 0 ) {
						printf("\n");
					}
					value_idx++;
				}
				push_csv(filename, eda); 
			}

		}

		printf("exit\n");
                close(cfd);

	}
	close(sfd);
	return 0;
}

int push_csv(char* filename, E_DATA* eda) {
	FILE *fp;
	//printf("[%s.csv] file open", filename);
	if ( eda[0].idx == 0 ) {
		fp = fopen(filename, "w+");
		fprintf(fp, "Index, ECG Value\n");
	} else {
		fp = fopen(filename, "a");
	}

	int i;
	for ( i=0; i<BUF_SIZE/2; i++ ){
		fprintf(fp, "%d,%d\n", eda[i].idx, eda[i].value);
	}
	fclose(fp);
	return 0;
}
