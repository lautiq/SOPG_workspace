#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define FIFO_NAME "fifotp"
#define BUFFER_SIZE 300

void signalHandler(int signal);

int main(void)
{
	uint8_t inputBuffer[BUFFER_SIZE];
	int32_t bytesRead;
	int32_t bytesWrote;
	int32_t returnCode, fd, fd_log, fd_sign;
	
    FILE *textLogFile, *signalLogFile;

    /*Create log.txt */
	if ((fd_log = open("Log.txt", O_CREAT | O_APPEND | O_WRONLY, 0666)) == -1)
	{ 
		printf("Error creating log file: %d\n", returnCode);
		exit(1);
	}
    /*Create Sign.txt */
	if( (fd_sign = open("Sign.txt", O_CREAT | O_APPEND | O_WRONLY, 0666)) == -1 )
	{
		printf("Error creating sign file: %d\n", returnCode);
		exit(1);
	}

	
    /* Create named fifo. No action if already exists */
	if ((returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0)) != -1)
	{
	printf("Error creating named fifo: %d\n", returnCode);
	exit(1);
	}
    
    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ((fd = open(FIFO_NAME, O_RDONLY)) < 0)
	{
	printf("Error opening named fifo file: %d\n", fd);
	exit(1);
	}
    

    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a writer\n");

    /* Loop until read syscall returns a value <= 0 */
	do
	{
        /* read data into local buffer */
		if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
        {
			perror("read");
        }
        else
		{
			if(inputBuffer[0] == 'D')
			{
				inputBuffer[bytesRead] = '\0';
				fprintf(stdout, "Reader process received %d bytes %s \n", bytesRead, inputBuffer);
				
				inputBuffer[bytesRead] = '\n';
				if((bytesWrote = write(fd_log, inputBuffer, bytesRead+1)) ==-1)
				{
					perror("write");
				}
				else
				{
					fprintf(stdout,"Log.txt: write %d bytes \n", bytesWrote);
				}
			}

			if(inputBuffer[0] == 'S')
			{
				inputBuffer[bytesRead] = '\0';
				fprintf(stdout, "Reader process received %d bytes %s \n", bytesRead, inputBuffer);

				inputBuffer[bytesRead] = '\n';
				if((bytesWrote = write(fd_sign, inputBuffer, bytesRead+1)) ==-1)
				{
					perror("write");
				}
				else
				{
					fprintf(stdout,"Sign.txt: write %d bytes \n", bytesWrote);
				}
			}
		}
	}
			
			
	while (bytesRead > 0);

    //Close log files
	close(fd_log);
	close(fd_sign);
	close(fd);

	return 0;
}

