#ifndef _APP_OBJ_
#define _APP_OBJ_

typedef struct {
	int    ID;
	int    credits;
	char * phone_number;
	char * name;
} Customer;

typedef struct {
	int    stock_id;
	int    slot;
	int    price;
	int    quantity;
	char * name;
} Inventory;

#endif
