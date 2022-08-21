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
#define BUF_SIZE 480
#define PORT 8080
#define TRUE 1
#define FALSE 0

typedef char element;

typedef union {
	unsigned value:10;
	char data[2];
} ECG;

typedef union {
	unsigned value:8;
	char data[1];
} TIMESTAMP;

typedef struct {
	int idx;
	unsigned value:10;
	unsigned hours:8;
	unsigned minutes:8;
	unsigned seconds:8;
	unsigned millis:8;
} E_DATA;


int push_csv(char*, E_DATA*);

/* main */
int main(int argc, char *argv[]) {

	char filename[100];
	printf("filename : ");
	scanf("%s", filename);
	strcat(filename, ".csv");

	int sockfd;
	element buf[BUF_SIZE];
	struct sockaddr_in servaddr, cliaddr;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("Socket Creation Failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}



	memset(buf, '\0', sizeof(buf));
	printf("Wait for connection request\n");

	int len, n;
	while(TRUE) {
		len = sizeof(cliaddr);
		int value_idx=0;
		while(TRUE) {
			int status, fd[2];
			pipe(fd);
			pid_t pid;
			pid = fork();

			if ( pid == 0 ) {
				n = recvfrom(sockfd, (char *)buf, BUF_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
				write(fd[1], buf, sizeof(buf));
				exit(0);
			} else {
				waitpid(pid, &status, 0);
				read(fd[0], buf, sizeof(buf));
				printf("Buf received data, prosseing.\n");

				int i; ECG e; TIMESTAMP thr, tmn, tsc, tmm; E_DATA ed; E_DATA eda[BUF_SIZE/6];
				for ( i=0; i<BUF_SIZE/6; i++ ) {
					thr.data[0] = buf[i*6];
					tmn.data[0] = buf[i*6+1];
					tsc.data[0] = buf[i*6+2];
					tmm.data[0] = buf[i*6+3];
					e.data[0] = buf[i*6+4];
					e.data[1] = buf[i*6+5];

					ed.idx = value_idx;
					ed.value = e.value;
					ed.hours = thr.value;
					ed.minutes = tmn.value;
					ed.seconds = tsc.value;
					ed.millis = tmm.value;
					eda[i] = ed;
					printf("[%2d:%2d:%2d.%2d] : %5d", eda[i].hours, eda[i].minutes, eda[i].seconds, eda[i].millis, eda[i].value);
					if ( (i+1)%5 == 0 ) {
						printf("\n");
					}
					value_idx++;
				}
				push_csv(filename, eda);
			}

		}
	}
	return 0;
}

int push_csv(char* filename, E_DATA* eda) {
	FILE *fp;
	if ( eda[0].idx == 0 ) {
		fp = fopen(filename, "w+");
		fprintf(fp, "Index,TimeStamp,ECG Value\n");
	} else {
		fp = fopen(filename, "a");
	}

	int i;
	for ( i=0; i<BUF_SIZE/6; i++ ){
		fprintf(fp, "%d,%d:%d:%d.%d,%d\n", eda[i].idx, eda[i].hours, eda[i].minutes, eda[i].seconds, eda[i].millis, eda[i].value);
	}
	fclose(fp);
	return 0;
}
