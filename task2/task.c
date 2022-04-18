#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

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

static int takenLocks = 0;

void sigintHandler(int signum)
{ 
	int fd = open_with_mode_safe("stats", O_WRONLY | O_APPEND | O_CREAT, 0400 | 0200);
	int strLength = snprintf(NULL, 0, "Количество успешных блокировок: %d\n", takenLocks);
	char* statStr = malloc(strLength + 1);
	if (statStr == NULL)
	{
		printf("Не удалось выделить память с помощью 'malloc'\n");
		exit(-1);
	}
	snprintf(statStr, strLength + 1, "Количество успешных блокировок: %d\n", takenLocks);
	write_safe(fd, statStr, strLength);
	exit(0);
}


int main(int argc, char *argv[])
{
	if (signal(SIGINT, sigintHandler) == SIG_ERR)
	{
		printf("Не удалось установить обработчик SIGINT\n");
		return -1;
	}

	int result;
	char* lockFileName = "somefile.lck";
	while (1)
	{
		int fd = -1;
		while (fd == -1)
		{
			fd = open(lockFileName, O_CREAT | O_EXCL | O_RDWR, 0400 | 0200);
		}
		
		takenLocks++;
		
		int pid = getpid();
		int pidLength = snprintf(NULL, 0, "%d", pid);
		char* pidStr = malloc(pidLength + 1);
		if (pidStr == NULL)
		{
			printf("Не удалось выделить память с помощью 'malloc'\n");
			return -1;
		}
		snprintf(pidStr, pidLength + 1, "%d", pid);
		write_safe(fd, pidStr, pidLength);


		int mainFd = open_safe("somefile", O_RDWR);		
		sleep(1);
		close_safe(mainFd);

		result = access(lockFileName, F_OK);
		if (result == -1)
		{
			printf("Отсутсвует файл блокировки процесса %d\n", pid);
			free(pidStr);
			return -1;
		}
		
		lseek(fd, 0, SEEK_SET);
		result = read(fd, pidStr, pidLength + 1);
		if (result == -1)
		{
			printf("Не удалось прочитать файл блокировки процесса %d. Ошибка: %s\n", pid, strerror(errno));
			free(pidStr);
			return -1;
		}
		
		int newPid = atoi(pidStr);
		free(pidStr);
		if (newPid != pid)
		{
			printf("В файле блокировки записан pid %d, а ожидалcя %d\n", newPid, pid);
			return -1;
		}
		
		close_safe(fd);
		result = unlink(lockFileName);
		if (result == -1)
		{
			printf("Не удалось удалить файл блокировки процесса %d. Ошибка: %s", pid, strerror(errno));
			return -1;
		}
	}
	return 0;
}
