#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int pipe_fds[2];
        pid_t pid;	//이 구조체는 무엇일까요? 그냥 숫자 자료형을 안쓰고 왜 구조체를 사용할까요?
	char buf[1024];
	int wstatus;

	printf("[%d] start of function\n", getpid());
	memset(buf, 0, sizeof(buf));
	
        // pipe_fds는 무엇일까요? 힌트: 파일디스크립터
	if (pipe(pipe_fds)) {
	    perror("pipe()");
	    return -1;
	}
	
	// fork는 무엇일까요? exec과의 차이는?
	pid = fork();
	// 자식프로세스의 PID는 누가 갖고있을까요?
	if (pid == 0) {
		/* child process */
		// 파이프를 닫아주는 이유는?
		close(pipe_fds[1]);
		read(pipe_fds[0], buf, sizeof(buf));
		printf("[%d] parent said... %s\n", getpid(), buf);

		close(pipe_fds[0]);

	} else if (pid > 0) {
		/* parent process */
		// 파이프를 닫아주는 이유는?
		close(pipe_fds[0]);
		strncpy(buf, "hello child~", sizeof(buf) - 1);
		write(pipe_fds[1], buf, strlen(buf));
		close(pipe_fds[1]);
	
		pid = wait(&wstatus);

	} else {
		/* error case */
		perror("fork()");
		goto err;
	} 

	return 0;

err:
      close(pipe_fds[0]);
      close(pipe_fds[1]); 


      return -1;
}

