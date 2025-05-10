
/*
 * License:
 *
 * This file has been released under "unlicense" license
 * (https://unlicense.org).
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * For more information about this license, please visit - https://unlicense.org
 */

/*
 * Author: Amit Choudhary
 * Email: amitchoudhary0523 AT gmail DOT com
 */

/*
 * Description of this program:
 *
 *  This program reads a file line by line and checks whether the length of the
 *  lines is at most 80 characters or not. If some line's length is more than
 *  80 characters then the line number is printed on the output screen. Also,
 *  the total number of lines having more than 80 characters is also printed.
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NEW_LINE '\n'

#define NO_ERROR 0 // no error happened.
#define INVALID_FD -1 // fd is less than 0.
#define NO_MEMORY -2 // memory not available.
#define FILE_LSEEK_ERROR -3 // error in seeking file

#define BUF_SIZE_INCREMENT 1024

// function prototypes for gcc flag -Werror-implicit-function-declaration
static char *get_line_from_file(int fd, int *error_num);

static char *get_line_from_file(int fd, int *error_num)
{

    char *buf = NULL;
    char *buf_temp = NULL;

    long curr_buf_size = 0;
    long curr_data_len = 0;

    ssize_t bytes_read = -1;
    int end_of_file_reached_or_error = 0;
    int new_line_found = 0;
    long i = 0;

    if (error_num == NULL) {
        printf("\nError: Argument 'error_num' is NULL.\n");
        return NULL;
    }

    *error_num = NO_ERROR;

    if (fd < 0) {
        printf("\nError: Argument 'fd' is less than zero.\n");
        *error_num = INVALID_FD;
        return NULL;
    }

    while (1) {

        buf_temp = buf;

        buf = realloc(buf, (size_t)(curr_buf_size + BUF_SIZE_INCREMENT));

        if (!buf) {
            free(buf_temp);
            *error_num = NO_MEMORY;
            return NULL;
        }

        curr_buf_size = curr_buf_size + BUF_SIZE_INCREMENT;

        // read data from file
        while (curr_data_len != curr_buf_size) {

            bytes_read = read(fd, buf + curr_data_len,
                              (size_t)(curr_buf_size - curr_data_len));

            if (bytes_read == 0) { // end of file reached
                // Check if no bytes were read earlier also in this call to
                // get_line_from_file(). If yes, then this means that end of
                // test file was reached (but not actually read) in the last
                // call to get_line_from_file(). So, this time end of file was
                // read and 0 was returned by read. So, since there are no bytes
                // to process, free the buffer and return NULL.
                if (curr_data_len == 0) {
                    free(buf);
                    return NULL;
                }
            } // end of if bytes_read == 0

            if (bytes_read <= 0) {
                end_of_file_reached_or_error = 1;
                break;
            } // end of if bytes_read <= 0

            curr_data_len = curr_data_len + bytes_read;

        } // end of inner while (1) loop

        new_line_found = 0;

        for (i = 0; i < curr_data_len; i++) {
            if (buf[i] == NEW_LINE) {
                buf[i] = 0;
                new_line_found = 1;
                break;
            }
        } // end of for loop

        if (new_line_found) {

            // realloc and seek
            buf_temp = buf;

            buf = realloc(buf, (size_t)(i + 1));

            if (!buf) {
                free(buf_temp);
                *error_num = NO_MEMORY;
                return NULL;
            }

            if (lseek(fd, (i + 1) - curr_data_len, SEEK_CUR) < 0) {
                *error_num = FILE_LSEEK_ERROR;
                free(buf);
                return NULL;
            }

            return buf;

        } else { // new line not found

            // NEW_LINE not found and end of file has been reached or some
            // error happened. So, allocate one extra byte for terminating
            // null byte and return.
            if (end_of_file_reached_or_error) {

                buf_temp = buf;

                buf = realloc(buf, (size_t)(curr_data_len + 1));

                if (!buf) {
                    free(buf_temp);
                    *error_num = NO_MEMORY;
                    return NULL;
                }

                buf[curr_data_len] = 0;

                return buf;

            } // end of if end_of_file_reached_or_error

        } // end of if - else (new_line_found)

    } // end of outer while (1) loop

} // end of function get_line_from_file()

int main(int argc, char *argv[])
{

    int fd = -1;
    char *line = NULL;
    long line_num = 0;
    long printed = 0;
    int error_num = 0;

    if (argc != 2) {
        printf("\n");
        printf("Error: Incorrect usage.\n");
        printf("\n");
        printf("This program takes only one argument which is the name of a"
               " file whose line lengths have to be checked.\n");
        printf("\n");
        printf("Usage: %s file_name\n", argv[0]);
        printf("\n");
        printf("Please try again. Exiting..\n");
        printf("\n");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        printf("\n%s: File \"%s\" could not be opened. Exiting..\n\n",
               __FILE__, argv[1]);
        exit(1);
    }

    printf("\n");

    while ((line = get_line_from_file(fd, &error_num)) != NULL) {

        line_num = line_num + 1;

        if (strnlen(line, 81) == 81) {
            printed = printed + 1;
            printf("The length of the line at line number %ld is greater than"
                   " 80 characters.\n", line_num);
        }

        free(line);

    } // end of while loop

    if (printed) {
        printf("\nTotal %ld lines have a length of more than 80"
               " characters.\n\n", printed);
    } else {
        printf("No lines have a length of more than 80 characters.\n\n");
    }

    if (error_num != 0) {
        printf("\n\nError happened: Error number = %d\n\n", error_num);
    }

    close(fd);

} // end of function main()

