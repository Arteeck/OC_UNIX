#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

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

int read_safe(int fd, char *buf, int count)
{
    int result = read(fd, buf, count);
    if (result == -1)
    {
		printf("Операция 'read' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
    }
    return result;
}

int lseek_safe(int fd, int offset, int whence)
{
    int result = lseek(fd, offset, whence);
    if (result == -1)
    {
		printf("Операция 'lseek' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
    }
    return result;
}

int close_safe(int fd)
{
    int result = close(fd);
    if (result == -1)
    {
		printf("Операция 'close' завершилась с ошибкой: %s\n", strerror(errno));
    }
    return result;
}

int open_with_mode_safe(char *filePath, int flags, int mode)
{
    int result = open(filePath, flags, mode);
    if (result == -1)
    {
		printf("Операция 'open' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
    }
    return result;
}

int open_safe(char *filePath, int flags)
{
    int result = open(filePath, flags);
    if (result == -1)
    {
		printf("Операция 'open' завершилась с ошибкой: %s\n", strerror(errno));
		exit(-1);
    }
    return result;
}

int main(int argc, char *argv[])
{
	int opt;
	int blockSize = 4096;
	while ((opt = getopt(argc, argv, ":b:")) != -1) 
	{
	 switch (opt) 
	 {
	  case 'b':
		blockSize = atoi(optarg);
		break;
	  case '?':
		printf("Unknown option: %c\n", optopt);
		break;
	  case ':':
		printf("Missing arg for %c\n", optopt);
		break;
	 }
	}

	char *firstFilePath;
	char *secondFilePath = NULL;
	int inputFileFd;
	int outputFileFd;
	
	if (optind < argc) 
	{
	  firstFilePath = argv[optind++];
	}
	else
	{
		printf("Должен быть указан хотя бы один путь до файла.\n");
		return -1;
	}
	if (optind < argc) 
	{
	  secondFilePath = argv[optind++];
	}
	
	if (secondFilePath == NULL)
	{
		inputFileFd = 0;
		outputFileFd = open_with_mode_safe(firstFilePath, O_WRONLY|O_TRUNC|O_CREAT, 0400 | 0200);
	}
	else
	{
		inputFileFd = open_safe(firstFilePath, O_RDONLY);
		outputFileFd = open_with_mode_safe(secondFilePath, O_WRONLY|O_TRUNC|O_CREAT, 0400 | 0200);
	}
	
	char isEmpty;
	int emptyBytesCount = 0;
	int readCount;
	char *readBytes = (char*) alloca(blockSize);
	if (readBytes == NULL)
    {
		printf("Не удалось выделить память с помощью 'alloca'\n");
		exit(-1);
    }
	while((readCount = read_safe(inputFileFd, readBytes, blockSize)) != 0)
	{
		isEmpty = 1;
		for (int i = 0; i < readCount; i++) 
		{
            if (readBytes[i] != 0)
			{
				isEmpty = 0;
				break;
			}
        }
		if (isEmpty == 0)
		{
			if (emptyBytesCount != 0)
			{
				lseek_safe(outputFileFd, emptyBytesCount, SEEK_CUR);
				emptyBytesCount = 0;
			}
			write_safe(outputFileFd, readBytes, readCount);
		}
		else
		{
			emptyBytesCount += readCount;
		}
	}
	
	if (emptyBytesCount != 0)
	{
		lseek_safe(outputFileFd, emptyBytesCount, SEEK_CUR);
	}
	
	if (inputFileFd != 0)
	{
		close_safe(inputFileFd);
	}
	close_safe(outputFileFd);

	return 0;
}
