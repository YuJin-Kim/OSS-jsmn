#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include "../myproduct.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

// static const char *JSON_STRING =
// 	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
// 	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
//

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char * readJSONFile() {
	char * JSON_STRING;
	char filename[30];
	char str[256];
	int count = 0;
	FILE *f;

	// printf("원하는 파일명 입력: ");
	// scanf("%s", filename);
	// strcat(filename,".json");

	if ((f = fopen("myproduct.json","r")) == NULL) {
		printf("%s 파일이 존재하지 않습니다.\n", filename);
		return NULL;
	}

	JSON_STRING = (char *)malloc(sizeof(str));

	while (1) {
		fgets(str, 256, f);
		if (feof(f)) break;
		count+= strlen(str)+1;
		JSON_STRING = (char*)realloc(JSON_STRING, sizeof(char)*count);
		strcat(JSON_STRING, str);
	}

	fclose(f);

	return JSON_STRING;

}

int * jsonObjectList(char * jsonstr, jsmntok_t * t, int tokcount) {
	int i, count = 1; // 반복문을 위한 변수 i, Name count를 위한 변수 count 생성 및 1로 초기

	int * objectTokIndex = (int *)malloc(sizeof(int)*count);

	if (t[0].type == JSMN_OBJECT && t[2].type == JSMN_ARRAY) {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == 2) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count));
			}
	}
	else if (t[0].type == JSMN_OBJECT) {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == -1) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count));
			}
	}
	else {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == 0) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count));
			}
	}

	*(objectTokIndex+count-1) = 0;

	return objectTokIndex;
}

int makeProduct(const char * json, jsmntok_t * t, int tokcount, product_t * p[]) {
	//parsing된 토큰의 정보를 이용하여 제품 정보에 맞춰 만든 제품 구조체의 각 정보를 설정하고, 구조체 포인터 배열에 할당
	int i = 0;
	int index, price, n;
	char s_function[20], s_name[80], s_event[20], s_price[10], s_count[5];
	int * objectTokIndex = jsonObjectList(json, t, tokcount);

	do {
		p[i] = (product_t *)malloc(sizeof(product_t));
		index = *(objectTokIndex+i)+2;
		strncpy(s_function, &json[t[index].start], t[index].end-t[index].start);
		s_function[t[index].end-t[index].start] = '\0';

		if(strcmp(s_function, "CLEANSING") == 0)
			p[i]->function = CLEANSING;
		else if(strcmp(s_function, "SKIN") == 0)
			p[i]->function = SKIN;
		else if(strcmp(s_function, "LOTION") == 0)
			p[i]->function = LOTION;
		else if(strcmp(s_function, "CREAM") == 0)
			p[i]->function = CREAM;
		else if(strcmp(s_function, "AMPOULE") == 0)
			p[i]->function = AMPOULE;

		index += 2;
		strncpy(s_name, &json[t[index].start], t[index].end-t[index].start);
		s_name[t[index].end-t[index].start] = '\0';
		strcpy(p[i]->name, s_name);
		index += 2;
		strncpy(s_event, &json[t[index].start], t[index].end-t[index].start);
		s_event[t[index].end-t[index].start] = '\0';
		strcpy(p[i]->event, s_event);
		index += 2;
		strncpy(s_price, &json[t[index].start], t[index].end-t[index].start);
		price = atoi(s_price);
		p[i]->price = price;
		index += 2;
		strncpy(s_count, &json[t[index].start], t[index].end-t[index].start);
		n = atoi(s_count);
		p[i]->count = n;
		i++;
	} while(*(objectTokIndex+i) != 0);
	p[i] = (product_t *)malloc(sizeof(product_t));
	p[i] = NULL;

	return i;
}

void printProduct(product_t * p[], int count) {
	//구조체를 이용하여 화면에 정보를 출력하는 함수 printProduct 함수 만들기
	int i = 0;

	printf("******************************************************************************\n");
	printf(" 번호      기능                제품명           가격  개수 총가격    이벤트\n");
	printf("******************************************************************************\n");

	while(p[i] != NULL) {
		printf(" %2d ", i+1);

		if (p[i]->function == CLEANSING)
			printf("   CLEANSING   ");
		else if (p[i]->function == SKIN)
			printf("     SKIN   ");
		else if (p[i]->function == LOTION)
			printf("    LOTION ");
		else if (p[i]->function == CREAM)
			printf("     CREAM    ");
		else if(p[i]->function == AMPOULE)
			printf("    AMPOULE         ");

		printf("   %s", p[i]->name);
		printf("  %4d", p[i]->price);
		printf("%4d   ", p[i]->count);
		printf("%-10d", p[i]->price * p[i]->count);
		printf("%s\n", p[i]->event);
		i++;
	}


}

int main() {

	product_t * cosmetic[20];
	char * str_example = (char *)malloc(sizeof(readJSONFile())+1);

	str_example = readJSONFile();

	if (str_example == NULL)
		return 1;

	int i, r;
	int product_count;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, str_example, strlen(str_example), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || (t[0].type != JSMN_OBJECT && t[0].type != JSMN_ARRAY)) {
		printf("Object or Array expected\n");
		return 1;
	}

	product_count = makeProduct(str_example, t, r, cosmetic);
	printProduct(cosmetic, product_count);

	return EXIT_SUCCESS;
}
