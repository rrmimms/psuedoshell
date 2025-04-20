#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "command.h"

void listDir()
{

    long size = pathconf(".", _PC_PATH_MAX);
    char *buff = (char *)malloc(size);
    const char *wdName = getcwd(buff, size);
    char *newLine = "\n";
    char *space = " ";
    char *fileName;
    if (wdName != NULL)
    {
        int count = 0;
        DIR *wdir = opendir(wdName);
        if (wdir == NULL)
        {
            free(buff);
            perror("directory is NULL\n");
            return;
        }
        struct dirent *entry;
        while ((entry = readdir(wdir)) != NULL)
        {
            fileName = entry->d_name;
            if (fileName[0] != '.')
            {
                if (count != 0)
                    write(STDOUT_FILENO, space, strlen(space));
                write(STDOUT_FILENO, fileName, strlen(fileName));
                write(STDOUT_FILENO, space, strlen(space));
                count++;
                if (count % 6 == 0)
                {
                    write(STDOUT_FILENO, newLine, strlen(newLine));
                }
            }
        }
        closedir(wdir);

        write(STDOUT_FILENO, newLine, strlen(newLine));
    } /*for the ls command*/
    free(buff);
}
void showCurrentDir()
{

    long size = pathconf(".", _PC_PATH_MAX);
    char *buff = (char *)malloc(size);
    char *cwd = getcwd(buff, size);
    strcat(cwd, "\n");
    write(STDOUT_FILENO, cwd, strlen(cwd));
    free(buff);
} /*for the pwd command*/

void makeDir(char *dirName)
{

    long size = pathconf(".", _PC_PATH_MAX);
    char *buff = (char *)malloc(size);
    char *cwd = getcwd(buff, size);
    strcat(cwd, "/");
    strcat(cwd, dirName);
    if (mkdir(cwd, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        perror("mkdir failed");
        free(buff);
        return;
    }
    free(buff);
}

void changeDir(char *dirName)
{

    long size = pathconf(".", _PC_PATH_MAX);
    char *dir = (char *)malloc(size);
    dir = getcwd(dir, size);
    strcat(dir, "/");
    strcat(dir, dirName);

    int ret = chdir(dir);

    if (ret == 0)
    {
        free(dir);
        return;
    }
    else
    {
        perror("chdir failed: \n");
        free(dir);
        return;
    }
    free(dir);
}

void copyFile(char *sourcePath, char *destinationPath)
{

    long len = 1024;
    char *curDir = (char *)malloc(len);
    if (curDir == NULL)
    {
        perror("malloc failed");
        return;
    }
    if (getcwd(curDir, len) == NULL)
    {
        perror("getcwd failed");
        free(curDir);
        return;
    }
    if (curDir == NULL)
    {
        perror("malloc failed - cp\n");
        return;
    }
    int inFile = open(sourcePath, O_RDONLY, S_IRWXU);
    if (inFile < 0)
    {
        perror("Couldn't open file -cp\n");
        free(curDir);
        return;
    }

    int outFile = 0;

    if (strcmp(destinationPath, ".") == 0 || strcmp(destinationPath, "..") == 0)
    {
        if (strcmp(destinationPath, "..") == 0)
        {
            changeDir("..");
        }

        char *endPath = strrchr(sourcePath, '/');
        char *fileName = NULL;
        if (endPath == NULL)
        {
            fileName = sourcePath;
        }
        else
        {
            fileName = endPath;
        }
        if (fileName + 1 == "/0")
        {
            fileName++;
        }
        getcwd(curDir, sizeof(curDir));
        strcat(curDir, fileName);
        outFile = open(curDir, O_CREAT | O_WRONLY, S_IRWXU);
    }
    else
    {
        outFile = open(destinationPath, O_CREAT | O_WRONLY, S_IRWXU);
    }
    if (outFile < 0)
    {
        perror("Couldn't open file: ");
        free(curDir);
        return;
    }

    struct stat fileStat;
    if (fstat(inFile, &fileStat) < 0)
    {
        perror("fstat failed -cp\n");
        close(inFile);
        close(outFile);
        free(curDir);
        return;
    }
    int size = fileStat.st_size;
    char *buff = (char *)malloc(size);
    if (buff == NULL)
    {
        perror("malloc failed -cp\n");
        close(inFile);
        close(outFile);
        free(curDir);
        return;
    }

    if (read(inFile, buff, size) < 0)
    {
        perror("error reading source file -cp\n");
        close(inFile);
        close(outFile);
        free(curDir);
        return;
    }

    int ret = write(outFile, buff, size);

    if (ret != -1)
    {
        free(buff);
        close(inFile);
        close(outFile);
        free(curDir);
        return;
    }
    else
    {
        perror("copy failed: ");
        close(inFile);
        close(outFile);
        free(buff);
        free(curDir);

        return;
    }

    free(buff);
    close(inFile);
    close(outFile);
    free(curDir);
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
