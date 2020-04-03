/*fifo_write_block*/ 
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

int main()
{
	const char *fifo_name = "/tmp/my_fifo";
	int pipe_fd = -1;
	int data_fd = -1;
	int res = 0;
	int bytes_sent = 0;
	char buffer[PIPE_BUF + 1];    //where is PIPE_BUF? Do you known?
 
	if(access(fifo_name, F_OK) == -1) //F_OK is used to determine whether the FIFO exists?
	{
		//step 1. Create the FIFO if it does not exist!
		res = mkfifo(fifo_name, 0777);
		if(res != 0)
		{
			fprintf(stderr, "Could not create fifo %s\n", fifo_name);
			exit(1);
		}
	}

	//step 2. Open the FIFO with O_WRONLY
	pipe_fd = open(fifo_name, O_WRONLY);
	printf("Process %d opening FIFO %d with O_WRONLY\n", getpid(),pipe_fd);

	data_fd = open("Data.txt", O_RDONLY);
	float data_size = lseek( data_fd, 0, SEEK_END);
	lseek( data_fd, 0, SEEK_SET);
	printf("total data size : %.2f \n", data_size);

	if(pipe_fd != -1)
	{
		int bytes_read = 0;
		//read data from "Data.txt", and save in buffer
		bytes_read = read(data_fd, buffer, PIPE_BUF);
		buffer[bytes_read] = '\0';          //why assigning it with '\0'.
		while(bytes_read > 0)
		{
			//step 3. Write data form the buffer to FIFO
			res = write(pipe_fd, buffer, bytes_read);
			if(res == -1)
			{
				fprintf(stderr, "Write error on pipe\n");
				exit(1);
			}
			bytes_sent += res;
			bytes_read = read(data_fd, buffer, PIPE_BUF);
			buffer[bytes_read] = '\0';  //why assigning it with '\0'.

			//transfer, please wait for a moment
			//Completion Rate: complete = bytes_sent/data_size
			float bytes_sent_temp = bytes_sent;
			float rate = 100*(bytes_sent_temp/data_size);
			printf("send complete %.2f%% \n",rate);
			
			sleep(1);
		}

		//step 4. colse the FIFO
		close(pipe_fd);

		close(data_fd);
	}
	else
		exit(1);

	printf("Process %d finished, %d bytes send\n", getpid(),bytes_sent);
	exit(0);
}
