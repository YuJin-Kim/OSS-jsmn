#ifndef __PRODUCTLIST_H_
#define __PRODUCTLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int tokindex;
  int objectindex;
} NameTokenInfo;

typedef enum {
	CLEANSING = 0,
	SKIN = 1,
	LOTION = 2,
	CREAM = 3,
	AMPOULE = 4
} function_t;

typedef struct {
	function_t function;
	char name[80];
	char event[20];
	int price;
	int count;
} product_t;

#ifdef __cplusplus
}
#endif

#endif /* __PRODUCTLIST_H_ */
