#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>                                     //wait을 사용하기 위한 라이브러리

int main(int argc, char **argv)
{
	int pipe_fds[2];                                  //파이프의 입력용,출력용을 지정해주기 위해 fd값 2개 생성
        pid_t pid;                                        //pid(프로세스 id)를 넣어 줄 구조체 타입의 변수 선언

	char buf[1024];                                   //문자열을 저장하기 위해 문자형 배열 1024의 길이를 가지는 변수 선언 이름 -buf
	char buf2[1024];

	int wstatus;                                      // wait를 사용하기 위해 선언함

	printf("[%d] start of function\n", getpid());        //프린트문 (출력문) pid에 저장된 값을 가져와서 값을 출력해준다  그런데 pid는 따로 초기화를 해준적 없는데 왜 바로 사용이 가능한지(값이 나오는지 모르겠습니다. 
	memset(buf, 0, sizeof(buf));      //memset을 해주는 이유는 초기화를 하지 않으면 안에 쓰레기값이 들어있을 수 있기 때문임. (사용전에 반드시 초기화해줘야하는것 같음)

	if (pipe(pipe_fds)) {                             //pipe(pipe_fds)에 대해 조건 검사 1일 경우 error발생 0 일경우 패스
	    perror("pipe()");				  // pipe에 사용할 fd를 생성하는데 성공하면 0 실패하면-1이 리턴되므로 그걸 이용해서 조건문으로 실패하면 바로 -1이 리턴되도록 유도
	    return -1;
	}
	
	pid = fork();                                       // fork() 함수를 사용하여 자식 프로세스를 생성하는 코드
	printf("[%d] 는 pid의 값입니다\n", getpid());       // pid의 값이 궁금해서 확인차 출력문을 사용했으나 확인해본 결과 2번 실행됨과 동시에 한번은 위에서 나온 pid값이 나오고 한번은 아래에서 나오는 p                                                                id값이 나오는데 2번이나 출력되는 이유를 모르겠습니다.
	if (pid == 0) {						// 조건문. 만약에 생성된 pid가 0 일경우 자식 프로세스로 정의한다. 자식 프로세스는 읽기만 사용할 것이기 때문에 fd의 1번은 닫아준다.
		/* child process */
		close(pipe_fds[1]);
		read(pipe_fds[0], buf, sizeof(buf));			//read()함수 사용 읽기전용 fd를 사용하여 buf에 있는 자료를 buf의 사이즈만큼 가져와서 읽는다.
		printf("[%d] parent said... %s\n", getpid(), buf);      // 출력문 함수를 이용하여 pid를 출력하고 출력하고자 하는 문장... 마지막으로 buf에 저장되어있는 문장을 읽어 출력한다.

	       	close(pipe_fds[0]);					// 마지막으로 읽기 전용인 fd[0]을 닫아준다.
									// 혹시 몰라서 close()를 주석 처리하고 돌려봤는데 주석 처리를 해도 잘 돌아갑니다. 잘 돌아간다면 왜 굳이 close()를 해주는건지 이유가     									 궁금합니다.
	} else if (pid > 0) {
		/* parent process */
		close(pipe_fds[0]);
		strncpy(buf, "hello childs~\n", sizeof(buf)-1 ); 	// 마지막에 -1을 넣는 이유는 보통 문자열의 끝을알리는 \0이 있기 때문에 그 부분을 날리기 위해서 -1을 써준다.
		strncpy(buf2, "Second, hello! this is second message!", sizeof(buf2) -1);	// 하지만 buf의 사이즈가 저장하고자 하는 문자열보다 훨씬 큰데 굳이 -1을 하는 이유가 있나? 생각이 든다.
	


 		write(pipe_fds[1], buf, strlen(buf));			// 쓰기 전용 fd에 "hello childs~" 가 복사된 buf를 넣어준다. 얼마나? 문자열의 길이만큼. 
		write(pipe_fds[1], buf2, strlen(buf2));			// 생각한게 맞다면 sizeof 는 buf가 가지고 있는 크기이고 (1024) strlen은 글자의 길이이므로 13이 된다. 즉 13자리의 글자를 fd에 넣음.
	     	close(pipe_fds[1]);					// 마지막으로 사용이 끝난 fd를 닫아준다.
	
		pid = wait(&wstatus);                           //자식 프로세스를 생성(Fork())후 반드시 써줘야하는것 부모 프로세스를 기다리게 하는용.
                                                                // 사용하는 이유는 좀비 프로세스와 고아 프로세스를 방지하기 위함.
                                                                // 여기서 좀비 프로세스란 프로세스가 종료되면 모든 메모리가 회수되지 않고 일부가 남아있는 좀비 프로세스가 되어버림.
								// 부모 프로세스에서 wait을 사용해주지 않으면 계속 좀비 프로세스로 남아있게됨.
								// 고아 프로세스는 자식보다 부모가 먼저 종료되어버릴 경우 고아 프로세스가 되어버림. 따라서 반드시 자식프로세스 생성후에는 wait을 써줘야한


	} else {
		/* error case */
		perror("fork()");				// 에러 케이스일 경우 즉 pid가 0이 아니면서 0보다 크지 않은 상태일 경우 err:로 가서 파이프를 닫고 끝낸다.
		goto err;
	} 

	return 0;

err:
      close(pipe_fds[0]);
      close(pipe_fds[1]); 


      return -1;
}

