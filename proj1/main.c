#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "string_parser.h"
#include "command.h"

void fileMode(char *sourcePath, int argc)
{

    int comFile = open(sourcePath, O_RDONLY);
    if (comFile < 0)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    FILE *inFile = fdopen(comFile, "r");
    if (inFile == NULL)
    {
        perror("Error converting file descriptor to FILE*");
        close(comFile);
        exit(EXIT_FAILURE);
    }

    size_t size = 128;
    char *buff = (char *)malloc(size);
    if (buff == NULL)
    {
        perror("malloc failed");
        fclose(inFile);
        exit(EXIT_FAILURE);
    }
    command_line largeCL;
    command_line smallCL;

    while (getline(&buff, &size, inFile) != -1)
    {
        largeCL = str_filler(buff, ";");

        for (int i = 0; largeCL.command_list[i] != NULL; i++)
        {

            smallCL = str_filler(largeCL.command_list[i], " ");

            for (int j = 0; j < smallCL.num_token; j++)
            {
                // fprintf(stdout, "%s\n" ,smallCL.command_list[j]);
                char *com = smallCL.command_list[j];

                if (strcmp(com, "cp") == 0)
                {

                    copyFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    j += 2;
                }

                else if (strcmp(com, "mkdir") == 0)
                {

                    fprintf(stdout, "%s\n", smallCL.command_list[j + 1]);
                    makeDir(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "ls") == 0 || strcmp(com, " ls") == 0 || strcmp(com, "ls ") == 0 || strcmp(com, " ls ") == 0)
                {
                    listDir();
                }

                else if (strcmp(com, "pwd") == 0)
                {
                    showCurrentDir();
                }

                else if (strcmp(com, "cd") == 0)
                {
                    changeDir(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "mv") == 0)
                {
                    moveFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    j += 2;
                }

                else if (strcmp(com, "rm") == 0)
                {
                    deleteFile(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "cat") == 0)
                {
                    displayFile(smallCL.command_list[j + 1]);
                }

                else
                {
                    fprintf(stderr, "Unrecognized command\n");
                }
                if (j > smallCL.num_token)
                    break;
            }
            // showCurrentDir();
            // listDir();
            memset(&smallCL, 0, 0);
            free_command_line(&smallCL);
        }

        free_command_line(&largeCL);
        memset(&largeCL, 0, 0);
    }
    free(buff);
    fclose(inFile);
}

void interactiveMode()
{
    FILE *line = fdopen(STDIN_FILENO, "r");

    size_t size = 128;
    char *buff = (char *)malloc(size);
    if (buff == NULL)
    {
        perror("malloc failed");
        fclose(line);
        exit(EXIT_FAILURE);
    }
    command_line largeCL;
    command_line smallCL;
    while (getline(&buff, &size, line))
    {
        largeCL = str_filler(buff, ";");

        for (int i = 0; largeCL.command_list[i] != NULL; i++)
        {

            smallCL = str_filler(largeCL.command_list[i], " ");

            for (int j = 0; j < smallCL.num_token; j++)
            {
                // fprintf(stdout, "%s\n" ,smallCL.command_list[j]);
                char *com = smallCL.command_list[j];

                if (strcmp(com, "cp") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL || smallCL.command_list[j + 2] == NULL)
                    {
                        fprintf(stderr, "One of the file arguments was NULL\nUsage: cp <filepath> <filepath>");
                        break;
                    }
                    copyFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    j += 2;
                }

                else if (strcmp(com, "mkdir") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stderr, "Directory argument was NULL\nUsage: mkdir <directory name>");
                        break;
                    }
                    makeDir(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "ls") == 0 || strcmp(com, " ls") == 0 || strcmp(com, "ls ") == 0 || strcmp(com, " ls ") == 0)
                {
                    listDir();
                }

                else if (strcmp(com, "pwd") == 0)
                {
                    showCurrentDir();
                }

                else if (strcmp(com, "cd") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stderr, "Directory argument was NULL\nUsage: cd <filepath>");
                        break;
                    }
                    changeDir(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "mv") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL || smallCL.command_list[j + 2] == NULL)
                    {
                        fprintf(stderr, "One of the arguments was NULL\nUsage: mv <filepath> <filepath>");
                        break;
                    }
                    moveFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    j += 2;
                }

                else if (strcmp(com, "rm") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stderr, "File argument was NULL\nUsage: rm <filepath>");
                        break;
                    }
                    deleteFile(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "cat") == 0)
                {
                    if(smallCL.command_list[j+1] == NULL)
                    {
                        fprintf(stderr, "File argument was NULL\nUsage: cat <filepath>");
                        break;
                    }
                    displayFile(smallCL.command_list[j + 1]);
                }

                else if (strcmp(com, "exit") == 0)
                {
                    exit(EXIT_SUCCESS);
                }

                else if (strcmp(com, "\0") != 0 || strcmp(com, "\n") != 0)
                {
                    fprintf(stderr, "Unrecognized command\n");
                }

                if (j > smallCL.num_token)
                    break;
            }
            // showCurrentDir();
            // listDir();
            memset(&smallCL, 0, 0);
            free_command_line(&smallCL);
        }

        free_command_line(&largeCL);
        memset(&largeCL, 0, 0);
    }
    free(buff);
    fclose(line);
}

int main(int argc, char *argv[])
{
    // showCurrentDir();
    // listDir();
    // makeDir(argv[1]);
    // changeDir(argv[1]);
    // copyFile("/home/robert/testFile.txt", "/home/robert/CS415/testFile.txt");
    // moveFile("/home/robert/testFile.txt", "/home/robert/CS415/out/testFile.txt");
    // listDir();
    // displayFile("/home/robert/CS415/out/testFile.txt");
    // deleteFile("/home/robert/CS415/out/testFile.txt");

    if (argc != 1)
    {
        fileMode(argv[2], argc);
    }
    else
        interactiveMode();
    // listDir();
    //  for (int i =0; i < argc; i++)
    //  {
    //      printf("%s\n", argv[i]);
    //  }
    //  else
    //      interactiveMode();
}