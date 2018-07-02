/*
 * tiny-cat.c - This file is part of the tiny-utils package for Linux & ELKS,
 * Copyright (C) 1995, 1996 Nat Friedman <ndf@linux.mit.edu>.
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define STDIN_FILENO   0  /* fileno(stdin)  */
#define STDOUT_FILENO  1  /* fileno(stdout) */

#define BUFFER_SIZE    100

static char buff[BUFFER_SIZE];

/* The name of the file currently being displayed, "-" indicates stdin. */

char *filename;


void DumpHex(unsigned int* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%04X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("     ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

int open_file(char *new_filename)
{
    int fd;

    filename = new_filename;

    if (*filename == '-' && *(filename + 1) == '\0')
	return (STDIN_FILENO);

    /*
     * If open() returns an error, the accepted behavior is for cat to
     * report the error and move on to the next file in the argument list.
     */
    if ((fd = open(filename, O_RDONLY)) < 0)
	perror(filename);

    return (fd);
}


/*
 * Output from the current fd until we reach EOF, and then return.
 */

int output_file(int fd)
{
    int bytes_read = 0;
    int bytes_to_read = 16;
    unsigned int buf[16];
	int n = 0;

    while(1) {
	while(bytes_to_read - bytes_read) {
           n = read(fd, buf, bytes_to_read - bytes_read);
           bytes_read += n;
	   if(!n) break;
        }
	
	DumpHex(buf, bytes_read);
        if(!n) break;
	bytes_read = 0;
    }

    return (1);
}

int main(int argc, char **argv)
{
    /* File descriptor for the input file */
    int curr_input_fd;
    int arg_num;

    if (argc == 1)
	arg_num = 0;
    else
	arg_num = 1;

    while (arg_num < argc) {
	if (argc == 1)
	    curr_input_fd = open_file("-");
	else
	    curr_input_fd = open_file(argv[arg_num]);

	if (curr_input_fd >= 0) {
	    output_file(curr_input_fd);
	    close(curr_input_fd);
	}
	arg_num++;
    }

    close(1);

    return (0);
}
