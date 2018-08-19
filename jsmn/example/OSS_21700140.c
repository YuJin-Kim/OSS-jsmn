#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

typedef struct { //구조체 선언
	char model[20]; //모델명
	char maker[20]; //제조사
	int year; //제조년도
	char gastype[20]; //연료타입(gasoline, diesel, lpg)
}car_t; //typedef car_t로 선언

char * readjsonfile() { //파일을 읽어 string으로 저장하는 함수
	char * json_string; //읽어온 string을 담을 변수
	char str[256]; //string을 한줄씩 읽어올 변수
	int size = 0; //string의 size 변수
	FILE *f; //읽어올 file을 담을 file 변수

	if ((f = fopen("cars.json","r")) == NULL) { //cars.json 파일을 열어 f에 담는데, 만약 파일이 존재하지 않으면,
		printf("파일이 존재하지 않습니다.\n"); //파일이 존재하지 않는다고 출력하고,
		return NULL; // NULL을 리턴
	}

	json_string = (char *)malloc(sizeof(str)); //string의 크기만큼 공간 할당

	while (1) { //무한하게 while loop를 돈다
		fgets(str, 256, f); //f에서 최대 256크기 만큼 string 한줄을 받아 str에 넣는다
		if (feof(f)) break; //만약 f가 NULL이면 while loop를 나간다
		size+= strlen(str)+1; //string size를 str+1만큼 더한다
		json_string = (char*)realloc(json_string, sizeof(char)*size); //string의 size를 더한 사이즈만큼 재할당해준다
		strcat(json_string, str); //한줄씩 읽어온 string을 뒤에 계속해서 붙여준다
	}

	fclose(f); //파일 f를 닫는다

	return json_string; //모두 읽어들인 json_string을 리턴
}

int * ObjectList(const char * jsonstr, jsmntok_t * t, int tokcount) { //각 Object의 첫 tokindex를 구해 담는 함수
	int i, count = 1; // 반복문을 위한 변수 i, object count를 위한 변수 count 생성 및 1로 초기화

	int * objectTokIndex = (int *)malloc(sizeof(int)*count); //object의 첫 인덱스를 담을 포인터 변수를 생성 및 크기만큼 할당해준다

	if (t[0].type == JSMN_OBJECT && t[2].type == JSMN_ARRAY) { //첫 token이 object type이고 세번째 token이 array type이면,
		for (i = 0; i < tokcount; i++) //token의 갯수만큼 반복한다
			if(t[i].parent == 2) { //각 token의 parent가 2이면
				*(objectTokIndex+count-1) = i; //object의 첫 tokindex를 넣는다
				count++; //count 1증가
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count)); //다음 object의 첫 tokindex를 담기위해 재할당해준다
			}
	}
	else if (t[0].type == JSMN_OBJECT) { //첫 token이 object type이면,
		for (i = 0; i < tokcount; i++) //token의 갯수만큼 반복한다
			if(t[i].parent == -1) { //각 token의 parent가 -1이면
				*(objectTokIndex+count-1) = i; //object의 첫 tokindex를 넣는다
				count++; //count 1증가
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count));//다음 object의 첫 tokindex를 담기위해 재할당해준다
			}
	}
	else { //그 외에,
		for (i = 0; i < tokcount; i++) //token의 갯수만큼 반복한다
			if(t[i].parent == 0) { //각 token의 parent가 0이면
				*(objectTokIndex+count-1) = i; //object의 첫 tokindex를 넣는다
				count++; //count 1증가
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count));//다음 object의 첫 tokindex를 담기위해 재할당해준다
			}
	}

	*(objectTokIndex+count-1) = 0; //object의 마지막을 알리기위해서 0을 넣어준다

	return objectTokIndex; //각 object의 첫 tokindex를 담은 포인터 objectTokeIndex를 리턴
}

int makecarlist(const char * json, jsmntok_t * t, int tokcount, car_t * list[]) {
	//json: json string, t: 파싱된 토큰 배열, tokcount: 토큰 개수, list: 구조체 포인터 배열

	int i = 0; //반복문을 위한 변수 i생성 및 0으로 초기화
	int index, year; //index 변수, 구조체에 year을 담기위한 year 변수
	char model[20], maker[20], str_year[6], gas[20]; // 각 구조체에 담기위한 변수들
	int * object = ObjectList(json, t, tokcount); //각 Object의 첫 tokindex를 알기위한 변수 및 설정

	do { // 반복한다
		list[i] = (car_t *)malloc(sizeof(car_t));
		index = *(object+i)+2; //각 object의 첫 token의 value index를 변수 index에 넣는다

		strncpy(model, &json[t[index].start], t[index].end-t[index].start); //model 값 읽어오기
		model[t[index].end-t[index].start] = '\0'; //마지막 문자에 널문자 넣기
		strcpy(list[i]->model, model); //구조체의 model member에 model값 넣기

		index += 2; //index값 2증가
		strncpy(maker, &json[t[index].start], t[index].end-t[index].start); //maker 값 읽어오기
		maker[t[index].end-t[index].start] = '\0'; //마지막 문자에 널문자 넣기
		strcpy(list[i]->maker, maker); //구조체의 maker member에 maker값 넣기

		index += 2; //index값 2증가
		strncpy(str_year, &json[t[index].start], t[index].end-t[index].start); //year 값 읽어오기
		str_year[t[index].end-t[index].start] = '\0'; //마지막 문자에 널문자 넣기
		year = atoi(str_year); //year을 int type으로 변환
		list[i]->year = year; //구조체의 year member에 year값 넣기

		index += 2; //index값 2증가
		strncpy(gas, &json[t[index].start], t[index].end-t[index].start); //gas 값 읽어오기
		gas[t[index].end-t[index].start] = '\0'; //마지막 문자에 널문자 넣기
		strcpy(list[i]->gastype, gas); //구조체의 gastype member에 gas값 넣기

		i++; // i 1증가

	} while(*(object+i) != 0); // object가 끝날때까지

	list[i] = NULL; //list의 마지막이 null을 가르키도록 NULL값 넣기

	return i; //구조체의 개수 i리턴
}


void printcarlist(car_t * list[], int carcount) { //구조체의 list를 출력하기위한 함수
	int i = 0; //반복문을 위한 변수 i생성 및 0으로 초기화

	printf("*************************************************************\n"); //출력
	printf(" 번호    모델명         제조사     제조년도     연료타입\n"); //출력
	printf("*************************************************************\n"); //출력

	while(list[i] != NULL) { //list가 NULL을 가르킬때까지 반복
		printf(" %2d\t ", i+1); //번호 출력
		printf("%s \t", list[i]->model); //모델명 출력
		printf("%s\t", list[i]->maker); //제조사 출력
		printf("   %d\t\t", list[i]->year); //제조년도 출력
		printf("%s\n", list[i]->gastype); //연료타입 출력

		i++; //i 1증가
	}

}

int main() {
	const char * string = (char *)malloc(sizeof(readjsonfile())+1); //읽어들인 json string
	car_t * carlist[5]; //구조체 포인터
	int count; //구조체 개수
	int i, r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	string = readjsonfile(); //파일의 내용을 string에 읽어온다
	jsmn_init(&p);
	r = jsmn_parse(&p, string, strlen(string), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || (t[0].type != JSMN_OBJECT && t[0].type != JSMN_ARRAY)) {
		printf("Object or Array expected\n");
		return 1;
	}

	count = makecarlist(string, t, r, carlist); //구조체에 각 member값을 담고 구조체의 개수를 count에 받는다
	printcarlist(carlist, count); //구조체의 목록을 출력

	return EXIT_SUCCESS;
}
