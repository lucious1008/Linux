#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){

	int pipe_fds[2];			// 인트형 배열 입력 출력의 파일 디스크립터로 사용할 fd 생성
	pid_t pid;				// pid를 저장하기 위해 변수선언
	char buf[1024];				// 문자를 저장하기 위한 buf 선언 크기는 1024임. 

	int wstatus;				// wait를 사용하기 위해 변수를 선언해준다.


	printf("여기서 부터 시작합니다\n");	// 여기서부터 시작한다는 것을 알리기위해 출력문 사용

	memset(buf, 0, sizeof(buf));		// memset을 이용하여 초기화를 시켜준다 buf를 0으로 buf의 size만큼 초기화시킴.


	if (pipe(pipe_fds)) {				//pipe_fds를 생성하는데 0이면 성공 -1이면 실패. 이를 이용하여 실패하면 에러발생을 띄운다.
		perror("pipe()에서 에러발생");
		return -1;
	}

	pid=fork();					// fork를 이용하여 자식 프로세스를 생성한다. 0이면 자식프로세스 0보다크면 부모 프로세스 아무것도 해당되지 않으면 에러케이스.

	if(pid==0) {					// pid가 0이면 자식프로세스 이기때문에 이곳에 자식프로세스를 정의한다.
	/* pid가 0이라면 자식 프로세스이기 때문에 이곳에는 자식프로세스를 정의한다.*/	
		close(pipe_fds[1]);			// 자식 프로세스는 쓰기를 할 필요가 없기때문에 바로 닫아준다.

		read(pipe_fds[0], buf, sizeof(buf));	// fd0을 통해 buf에 저장되어있는 문자를 buf 사이즈 만큼 읽어온다.
		printf("Parent say... %s" , buf);	// printf함수를 통해 저장되어있는 buf를 출력한다.
		close(pipe_fds[0]);		// 다 끝났으므로 닫아줌.

	} else if (pid > 0){				// 0보다 크다는 것은 부모프로세스를 의미. 그러므로 부모프로세스를 정의한다.
		/*0보다 크다는 것은 부모프로세스를 의미하므로 이곳은 부모 프로세스를 정의한다.*/
		
		close(pipe_fds[0]);		// 쓰기만 할거니까 읽기전용부분을 닫아줌
		
		strncpy(buf, "처음으로 내가 직접 만들어봄 (최대한 안보고)", sizeof(buf)-1);   		// 문자를 카피한다. buf에 ""안의 문장을 복사함. 크기는 buf의 -1사이즈만큼.                

		write(pipe_fds[1], buf, strlen(buf));							// fd1에 buf의 내용을 문자열 길이만큼 쓴다.

		close(pipe_fds[1]);					// 사용이 끝난 부분은 닫아준다.

		pid = wait(&wstatus);					// 좀비프로세스,고아프로세스가 되는 것을 막기위해 부모프로세스를 조금 기다리게해준다.



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
