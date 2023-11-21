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
   
    char outputBuffer[BUFFER_SIZE];
	uint32_t bytesWrote;
	int32_t returnCode, fd;

    /* Create named fifo. No action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) != -1 )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* Install signal handler for handling signals from console */
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a reader--type some stuff\n");

    /* Loop forever */
	while (1)
	{
        /* Get some text from console */
		fgets(outputBuffer, BUFFER_SIZE, stdin);
        
        /* Check if the input is a newline character (ENTER key pressed) */
        if(outputBuffer[0]== '\n')
        {
            //If the user pressed ENTER, format the message and write it to the named fifo
            char dataMessage[BUFFER_SIZE];
            snprintf(dataMessage, BUFFER_SIZE, "DATA:%s", outputBuffer + 1); //skip the leading newline

            if((bytesWrote = write(fd, dataMessage, strlen(dataMessage))) == -1)
            {
                perror("write");
            } else {
                printf("writer: wrote %d bytes\n", bytesWrote);
            }
        }
	}
	return 0;
}


void signalHandler(int signal)
{
    char signalMessage[BUFFER_SIZE];
    snprintf(signalMessage, BUFFER_SIZE, "SIGN:%d", signal - SIGUSR1 + 1);
    write(fd, signalMessage, strlen(signalMessage));
    printf("writer: Sent signal message: %s\n", signalMessage);
}