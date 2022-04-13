#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#define defaultSize 4*1024*1024 + 1

int write_safe(int fd, char *buf, int count)
{
    int result = write(fd, buf, count);
    if (result == -1)
    {
		printf("Операция 'write' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
    }
    return result;
}

int main(int argc, char *argv[])
{
    int fd = open("fileA", O_WRONLY | O_CREAT | O_TRUNC, 0400 | 0200);
	if (fd == -1)
	{
		printf("Операция 'open' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
	}
    char* buffer;
    char buf1 = { 1 };
    buffer = (char*)calloc(defaultSize, 1);
    if (buffer == NULL)
    {
		printf("Не удалось выделить память с помощью 'calloc'\n");
		exit(-1);
    }
    write_safe(fd, &buf1, 1);
    write_safe(fd, buffer, 9999);
    write_safe(fd, &buf1, 1);
    write_safe(fd, buffer, defaultSize - 10002);
    write_safe(fd, &buf1, 1);
    int result = close(fd);
	if (result == -1)
	{
		printf("Операция 'close' завершилась с ошибкой: %s\n", strerror(errno));
	}
    free(buffer);
    return 0;
}
