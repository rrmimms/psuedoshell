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
    FILE *inFile = freopen(sourcePath, "r", stdin);
    if (inFile == NULL)
    {
        perror("Error opening input file");
        fclose(inFile);
        exit(EXIT_FAILURE);
    }
    FILE *outFile = freopen("output.txt", "w", stdout);
    if (outFile == NULL)
    {
        perror("Error creating or opening output file.");
        fclose(outFile);
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
                char *com = smallCL.command_list[j];
                if (strcmp(com, "cp") == 0)
                {
                    copyFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    j += 2;
                }

                else if (strcmp(com, "mkdir") == 0)
                {           

                    //fprintf(stdout, "%s\n", smallCL.command_list[j + 1]);
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
                    fprintf(stdout, "Error: Unrecognized command\n");
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
    fprintf(stdout, "\n");
    free(buff);
    fclose(inFile);
}

void interactiveMode(int argc)  
{
    FILE *line = fdopen(STDIN_FILENO, "r");

    const size_t buffSize = 1024;
    char *buff = (char *)malloc(buffSize);

    if (buff == NULL)
    {
        perror("malloc failed");
        fclose(line);
        exit(EXIT_FAILURE);
    }
    command_line largeCL;
    command_line smallCL;

    printf(">>> ");
    fflush(stdout);
    int flag = 0;
    
    for (size_t size = buffSize; getline(&buff, &size, line); size = buffSize)
    {
        largeCL = str_filler(buff, ";");

        for (int i = 0; largeCL.command_list[i] != NULL; i++)
        {
            if (flag == 1)
            {
                flag = 0;
            }
            smallCL = str_filler(largeCL.command_list[i], " ");

            for (int j = 0; j < smallCL.num_token; j++)
            {
                if (flag == 1)
                {
                    j = smallCL.num_token;
                    break;
                }
                // if (smallCL.command_list[j] == ">>> ")
                //     continue;

                char *com = smallCL.command_list[j];
                if (com == NULL || strcmp(com, "") == 0 || strcmp(com, "\n") == 0)
                {
                    continue;
                }

                if (strcmp(com, "cp") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL || smallCL.command_list[j + 2] == NULL)
                    {
                        fprintf(stdout, "Error: One of the file arguments was NULL\nUsage: cp <filepath> <filepath>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 3] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    copyFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    break;
                }

                else if (strcmp(com, "mkdir") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stdout, "Error: Directory argument was NULL\nUsage: mkdir <directory name>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 2] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    makeDir(smallCL.command_list[j + 1]);
                    j++;
                }

                else if (strcmp(com, "ls") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        listDir();
                        break;
                    }
                    else
                    {
                        fprintf(stdout, "Error: ls doesn't take arguments\n");
                        flag = 1;
                        break;
                    }
                }

                else if (strcmp(com, "pwd") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        showCurrentDir();
                        break;
                    }
                    else
                    {
                        fprintf(stdout, "Error: pwd\npwd doesn't take arguments\n");
                        flag = 1;
                        break;
                    }
                }

                else if (strcmp(com, "cd") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stdout, "Error: Directory argument was NULL\nUsage: cd <filepath>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 2] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    changeDir(smallCL.command_list[j + 1]);
                    break;
                }

                else if (strcmp(com, "mv") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL || smallCL.command_list[j + 2] == NULL)
                    {
                        fprintf(stdout, "Error: One of the arguments was NULL\nUsage: mv <filepath> <filepath>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 3] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    moveFile(smallCL.command_list[j + 1], smallCL.command_list[j + 2]);
                    break;
                }

                else if (strcmp(com, "rm") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stdout, "Error: File argument was NULL\nUsage: rm <filepath>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 2] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    deleteFile(smallCL.command_list[j + 1]);
                    break;  
                }

                else if (strcmp(com, "cat") == 0)
                {
                    if (smallCL.command_list[j + 1] == NULL)
                    {
                        fprintf(stdout, "Error: File argument was NULL\nUsage: cat <filepath>");
                        flag = 1;
                        break;
                    }
                    if (smallCL.command_list[j + 2] != NULL)
                    {
                        fprintf(stdout, "Error: too many arguemnts!");
                        flag = 1;
                        break;
                    }
                    displayFile(smallCL.command_list[j + 1]);
                    break;
                }

                else if (strcmp(com, "exit") == 0)
                {
                    exit(EXIT_SUCCESS);
                }

                else if (strcmp(com, "\0") != 0 || strcmp(com, "\n") == 0)
                {
                    fprintf(stdout, "Error: Unrecognized command\n");
                    flag = 1;
                    break;
                }
            }
            // showCurrentDir();
            // listDir();
            free_command_line(&smallCL);
        }
        free_command_line(&largeCL);
        //  printf("\n");
        printf(">>> ");
        fflush(stdout);
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
        if (strcmp(argv[1], "-f") == 0)        
        fileMode(argv[2], argc);
        else{
            fprintf(stderr, "File mode usage: ./pseudo-shell -f <filename>\n");
            exit(EXIT_SUCCESS);
        }
    }
    else
        interactiveMode(argc);
    // listDir();
    //  for (int i =0; i < argc; i++)
    //  {
    //      printf("%s\n", argv[i]);
    //  }
    //  else
    //      interactiveMode();
}