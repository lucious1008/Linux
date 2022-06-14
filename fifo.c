#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FIFO_FILENAME  		"./testfifo"				// fifo파일이 생성될 경로를 지정하는 부분

static void print_usage(char *progname)      				// main함수 쪽에서 argv값을 넘겨주고 그 값을 프린트 해준다.. 프로그램의 이름을 넘겨 받기 때문에 프로그램 이름을 출력함.
{
	printf("%s (r|w)\n", progname);					// %s로 progname으로 전달받은 값을 출력하고 (r|w)를 출력하면서 리턴해준다.

	return;
}

static int do_reader(void)						// do_reader()함수 정의부분
{
    int fd;								// fd를 지정해주기 위해 변수선언
    char buf[128];							// 문자열을 읽어들이기 위해 do_writer에 선언된 buf와 같은 크기의 배열 선언
	
	printf("call open()\n");					// call open 문자열 출력
        fd = open(FIFO_FILENAME, O_RDONLY);				// fd 를 open하는데 이때 읽기 전용으로 Open한다.
        if (fd < 0) {							// 만약 fd가 0보다 작으면 즉 제대로 생성되지 않고 실패했을 경우 에러 메시지를 출력하고 프로그램을 종료시킨다.
            perror("reader_side_open()");				// writer side에도 에러가 났을 때 open()으로 똑같은 메시지를 출력하기 때문에 임의로 reader_side를 붙여 어느쪽 에러인지 명확하게 함
            return -1;
        }
        read(fd, buf, sizeof(buf));					// fd를 이용하여 buf에 저장되어있는 문자열을 buf의 크기만큼 읽어낸다.
        printf("writer said...%s\n", buf);				// 읽어온 buf를 printf()함수를 이용하여 출력한다.
	close(fd); 							// 작업이 끝난 fd는 자원관리를 위하여 닫아준다.
return 0;
}

static int do_writer(void)
{
   int fd; 								// fd를 설정할 int형 데이터변수
   char buf[128];							// 문자열 입력을 위한 buf라는 이름의 문자형 배열 변수

     printf("make fifo\n");
     unlink(FIFO_FILENAME); 						// 파이프 이름의 프로그램이 만들어져있을 수도있기 때문에 제거를 해준다.
     if (mkfifo(FIFO_FILENAME, 0644)) {					// FIFO_FILENAME에 정의되어있는 경로에 0644의 권한을 가지는 파이프를 생성한다.
	perror("mkfifo()");						// 생성하지못하고 0이외의 값을 받았을 때 에러 메시지 출력. 의미는 mkfifo()에서 에러가 발생했다.
	return -1;							// 프로그램 종료
     }
	printf("call open()\n");
	fd = open(FIFO_FILENAME, O_WRONLY);				// fd(파일디스크립터) 어느쪽을 사용할 것인지 설정 0이면 리드 1이면 쓰기
									// open API를 사용할때 파일의 경로, 어느쪽을 사용할 것인지 인자로 넘겨줘야함. O_WRONLY이기 때문에 쓰기전용으로 fd를 열겠다라는 뜻
	if (fd < 0) {							// 만약 fd가 0보다 작을때 즉 fd가 제대로 생성되지 않고 실패했을 때 에러 메시지를 출력하고 프로그램을 종료시킨다.
	    perror("writer_side_open()");				// reader side에도 에러가 났을 때 open()으로 똑같은 메시지를 출력하기 때문에 임의로 writer_side를 붙여 어느쪽 에러인지 명확하게 함
	    return -1;
	}
	strncpy(buf, "hello", sizeof(buf));				// buf에 hello라는 문자열을 buf의 사이즈만큼 복사한다.
	write(fd, buf, strlen(buf));					// fd에 buf에 저장되어있는 문자열을 write한다.
        close(fd);
	return 0;
}


/* fifo (s|r) */
int main(int argc, char **argv)
{
	if (argc <2 ){							// 만약에 argc값이 2보다 작으면 즉 2보다 작다는 것은 아무것도 입력하지 않은 상태를 말한다. 왜냐하면 아무것도 입력하지 않으면       
									// argv에는 경로만 들어가고 argc는 1이 되기 때문이다.	

		print_usage(argv[0]);					// print_usage함수 실행하면서 argv[0]값을 던져줌.
		return -1;						// 프로그램 종료
	}

	if (!strcmp(argv[1], "r")) {					// r을 입력했을 경우 읽기쪽 이므로 읽기 전용 함수 실행
		/* reader */						// argv[0] 에는 이미 경로가 들어가있음. 그 뒤에 문자 값을 써주는데 그 값이 r이면
		do_reader();						// 읽기 함수 호출
	} else if (!strcmp(argv[1], "w")) {				// w를 입력했을 경우 쓰기 쪽 이므로 쓰기 전용 함수 실행
		/* write */
		do_writer();						// 쓰기 함수 호출
	} else {							// 파라미터 값으로 아무것도 안들어왔을 때, r 또는 w가 아닐때 print_usage함수를 호출하고 값으로 경로(프로그램 명)을 던져줌
		print_usage(argv[0]);					// r,w 둘중 어느쪽도 아닌 값을 입력했을 시 프로그램 종료
		return -1;
	}								// 이와 같이 코드를 바로 작성할 수도 있지만 함수로 뺄수도 있다.
									// 알아보기 더 쉬움

	return 0;
}
