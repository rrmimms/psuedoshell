#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include "command.h"

void listDir()
{
    int flag = 0;
    size_t size = 1000000;
    char *buff = (char *)malloc(size);
    char *wdName = getcwd(NULL, 0);
    char *newLine = "\n";
    char *space = " ";
    char *fileName = NULL;
    do
    {
        if (wdName != NULL)
        {
            int count = 0;
            DIR *wdir = opendir(wdName);
            if (wdir == NULL)
            {
                perror("directory is NULL\n");
                break;
            }
            struct dirent *entry;
            while ((entry = readdir(wdir)) != NULL)
            {
                fileName = entry->d_name;
                if (strcmp(fileName, "...") == 0)
                    break;

                if (fileName[0] != '.')
                {
                    if (count != 0)
                        write(STDOUT_FILENO, space, strlen(space));
                    write(STDOUT_FILENO, fileName, strlen(fileName));
                    count++;
                }
                else if (fileName[0] == '.' && fileName[1] == '.' && fileName[2] == '\0')
                {
                    write(STDOUT_FILENO, fileName, strlen(fileName));
                    write(STDOUT_FILENO, space, strlen(space));
                }
                else if (fileName[0] == '.' && fileName[1] == '\0')
                {
                    write(STDOUT_FILENO, fileName, strlen(fileName));
                    write(STDOUT_FILENO, space, strlen(space));
                }
            }
            closedir(wdir);
        }
    } while (flag);
    write(STDOUT_FILENO, newLine, strlen(newLine));
    if (buff)
    {
        free(buff);
    }
    if (wdName)
    {
        free(wdName);
    }
    return;
}
void showCurrentDir()
{

    size_t size = 10000000;
    char *buff = (char *)malloc(size * 2);
    char *cwd = NULL;
    char *newLine = "\n";
    cwd = getcwd(NULL, 0);
    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, newLine, strlen(newLine));
    if (cwd)
    {
        free(cwd);
    }
    free(buff);
} /*for the pwd command*/

void makeDir(char *dirName)
{

    size_t size = 100000000;
    char *buff = (char *)malloc((size * 2) + 50);
    char *cwd = getcwd(NULL, 0);
    do
    {
        strcpy(buff, cwd);
        strcat(buff, "/");
        strcat(buff, dirName);
        if (mkdir(buff, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
        {
            perror("mkdir failed");
            break;
        }

    } while (0);
    if (cwd)
    {
        free(cwd);
    }
    if (buff)
    {
        free(buff);
    }
}

void changeDir(char *dirName)
{

    int ret = chdir(dirName);

    if (ret == 0)
    {
        // free(dir);
        return;
    }
    else
    {
        perror("chdir failed: \n");
        // free(dir);
        return;
    }
    // free(dir);
}

void copyFile(char *sourcePath, char *destinationPath)
{

    char *curDir = getcwd(NULL, 0);

    char *finalPath = (char *)malloc(100000);
    strcpy(finalPath, destinationPath);
    const size_t buffSize = 64 * 1024 * 1024;
    struct stat dirSt;
    int outFile = 0;
    struct stat fileStat;
    char *buff = (char *)malloc(buffSize);
    int inFile = -1;
    ssize_t readSize = 0;

    char *baseSourceBuff = strdup(sourcePath);
    char *baseSourcePath = basename(baseSourceBuff);

    do
    {
        if (curDir == NULL)
        {
            perror("getcwd was bad");
            break;
        }
        inFile = open(sourcePath, O_RDONLY);
        if (inFile < 0)
        {
            perror("Couldn't open input file");
            break;
        }

        if (baseSourceBuff == NULL)
        {
            perror("strdup failed");
            break;
        }

                if (stat(destinationPath, &dirSt) == 0 && S_ISDIR(dirSt.st_mode))
                {
                    if (finalPath[strlen(finalPath) - 1] != '/')
                    {
                        strcat(finalPath, "/");
                    }
                    strcat(finalPath, baseSourcePath);
                }

        outFile = open(finalPath, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);

        if (outFile < 0)
        {
            perror("Couldn't open file: ");
            break;
        }

        if (fstat(inFile, &fileStat) < 0)
        {
            perror("fstat failed -cp\n");
            break;
        }

        if (buff == NULL)
        {
            perror("malloc failed -cp\n");
            break;
        }
        for (size_t fileSize = fileStat.st_size; fileSize > 0; fileSize -= readSize)
        {
            if ((readSize = read(inFile, buff, buffSize)) < 0)
            {
                perror("error reading source file -cp\n");
                break;
            }
            if (readSize == 0)
            {
                break;
            }
            if (write(outFile, buff, readSize) < 0)
            {

                perror("write failed: ");
                break;
            }
        }
        break;
    } while (0);
    if (buff)
    {
        free(buff);
    }
    if (inFile >= 0)
    {
        close(inFile);
    }
    if (outFile >= 0)
    {
        close(outFile);
    }
    if (curDir)
    {
        free(curDir);
    }
    if (finalPath)
    {
        free(finalPath);
    }
    if (baseSourceBuff)
    {
        free(baseSourceBuff);
    }

    return;
}

void moveFile(char *sourcePath, char *destinationPath)
{
    copyFile(sourcePath, destinationPath);
    deleteFile(sourcePath);

} /*for the mv command*/

void deleteFile(char *filename)
{

    int ret = unlink(filename);
    if (ret != -1)
    {
        return;
    }
    else
    {
        perror("Delete failed.\n");
        return;
    }

} /*for the rm command*/

void displayFile(char *filename)
{
    int inFile = open(filename, O_RDONLY, S_IRWXU);

    struct stat fileStat;
    if (fstat(inFile, &fileStat) < 0)
    {
        return;
    }
    int size = fileStat.st_size;
    char *buff = (char *)malloc(size);

    read(inFile, buff, size);
    int ret = write(STDOUT_FILENO, buff, size);

    if (ret != -1)
    {
        free(buff);
        close(inFile);
        return;
    }
    else
    {
        perror("CAT failed.\n");
        free(buff);
        close(inFile);
        return;
    }
    free(buff);
    close(inFile);
}
/*for the cat command*/
