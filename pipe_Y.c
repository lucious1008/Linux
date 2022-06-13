#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){

	int pipe_fds[2];
	pid_t pid;

	char buf[1024];

	int wstatus;


	printf("여기서 부터 시작합니다\n");

	memset(buf, 0, sizeof(buf));


	if (pipe(pipe_fds)) {
		perror("pipe()에서 에러발생");
		return -1;
	}

	pid=fork();

	if(pid==0) {
	/* pid가 0이라면 자식 프로세스이기 때문에 이곳에는 자식프로세스를 정의한다.*/	
		close(pipe_fds[1]);

		read(pipe_fds[0], buf, sizeof(buf));
		printf("Parent say... %s" , buf);
		close(pipe_fds[0]);		// 다 끝났으므로 닫아줌.

	} else if (pid > 0){
		/*0보다 크다는 것은 부모프로세스를 의미하므로 이곳은 부모 프로세스를 정의한다.*/
		
		close(pipe_fds[0]);		// 쓰기만 할거니까 읽기전용부분을 닫아줌
		
		strncpy(buf, "처음으로 내가 직접 만들어봄 (최대한 안보고)", sizeof(buf)-1);

		write(pipe_fds[1], buf, strlen(buf));

		close(pipe_fds[1]);

		pid = wait(&wstatus);



	} else {
		/*둘다 해당되지 않으면 에러이므로 에러케이스를 작성한다.*/
		perror("fork()");
		goto err;

	}	

return 0;


err:
	close(pipe_fds[0]);
	close(pipe_fds[1]);

	return -1;

}
