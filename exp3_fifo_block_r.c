/*fifo_block_r.c*/ 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

int main()
{
	const char *fifo_name = "/tmp/my_fifo";
	int pipe_fd = -1;
	int data_fd = -1;
	int res = 0;
	char buffer[PIPE_BUF + 1];    //where is PIPE_BUF? Do you known?
	memset(buffer, '\0', sizeof(buffer));
	int bytes_read = 0;
	int bytes_write = 0;

	//step 1. Open the FIFO with O_RDONLY
	pipe_fd = open(fifo_name, O_RDONLY);
	printf("Process %d opening FIFO %d with O_RDONLY\n", getpid(), pipe_fd);

	data_fd = open("DataFormFIFO.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);

	if(pipe_fd != -1)
	{	
		do
		{
			//step 2. read the data from FIFO
			res = read(pipe_fd, buffer, PIPE_BUF);
			//save the data on "DataFormFIFO.txt"
			bytes_write = write(data_fd, buffer, res);
			bytes_read += res;
			
			printf("receiving now ... \n");
			
		}while(res > 0);
		
		//step 3. colse the FIFO
		close(pipe_fd);

		close(data_fd);
	}
	else
		exit(1);

	printf("Process %d finished, %d bytes read\n", getpid(), bytes_read);
	exit(0);
}
