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
	uint32_t bytesRead;
	int32_t returnCode, fd;
    FILE *textLogFile, *signalLogFile;
    
    /* Create named fifo. No action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) != -1  )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }
    
    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ( (fd = open(FIFO_NAME, O_RDONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* Install signal handler for handling signals from console*/
    signal(SIGINT, signalHandler);

    /* Open log  files for text and signals */
    textLogFile = fopen("log.txt", "a");
    signalLogFile = fopen("signal.txt", "a");

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
			inputBuffer[bytesRead] = '\0';
			
            if(strncmp(inputBuffer, "DATA:", 5) == 0)
            {
                //log text to file
                fprintf(textLogFile, "%s\n", inputBuffer + 5); // skip the "DATA:" prefix
            } else if (strncmp(inputBuffer, "SIGN:", 5) == 0) 
            {
                //log signal to file
                fprintf(signalLogFile, "%s\n", inputBuffer +5); //skip the "SIGN:" prefix
            }
            printf("reader: read %d bytes: \"%s\"\n", bytesRead, inputBuffer);
		}
	}
	while (bytesRead > 0);

    //Close log files
    fclose(textLogFile);
    fclose(signalLogFile);

	return 0;
}

void signalHandler(int signal) {
    printf("reader: Received signal %d\n", signal);
    exit(0);
}
