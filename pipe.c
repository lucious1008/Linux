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
        		//시스템에 따라 프로세스의 번호가 int형이 아닐수도 있기 때문에 이식성 면에서 더 낫고, PID_MAX_DEFAULT가 0~32767까지만 가능하기에 그 값을 벗어난 값도 사용할 수 있도록 하기위해 숫자 자료형을 쓰지 않습니다.
	char buf[1024];
	int wstatus;

	printf("[%d] start of function\n", getpid());
	memset(buf, 0, sizeof(buf));
	
        // pipe_fds는 무엇일까요? 힌트: 파일디스크립터
        // 파일 디스트립터 이며 파이프,FIFO,소켓,디바이스,일반파일 등 종류에 상관없이 모든 열려있는 파일을 참조하기 위해 사용합니다.
        // 0 - 표준입력  1 - 표준 출력  2 - 표준 에러
	if (pipe(pipe_fds)) {
	    perror("pipe()");
	    return -1;
	}
	
	// fork는 무엇일까요? exec과의 차이는?
	// fork,exec둘다 자식프로세스를 생성하기 위해 사용하지만 fork는 부모프로세스를 그대로 사용할 수 있고 exec의 경우는 덮어쓰여져 생성되는게 큰 차이입니다.
	// 따라서 부모 프로세스를 그대로 사용할 시에는 fork, 다른 프로세스로 변경해서 사용하고자 한다면 exec를 사용해야합니다. 이해한게 맞을까요?
	pid = fork();
	// 자식프로세스의 PID는 누가 갖고있을까요?
	// PID는 고유의 식별자이므로 자식프로세스가 가지고 있습니다.
	if (pid == 0) {
		/* child process */
		// 파이프를 닫아주는 이유는?
		// 파이프를 닫아주는 이유는 부모 프로세스가 쓰고 자식 프로세스가 읽는데 자식이 아닌 부모가 읽어버리는 경우가 있기 때문에 사용하지 않는 fd를 닫아주게되면 이러한 문제를 방지할 수 있습니다. 그렇기 때문에 반드시 닫아주어야합니다.
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

