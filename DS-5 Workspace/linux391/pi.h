#ifndef _WIFI_H_
#define _WIFI_H_

#include "serial.h"
#include "appObjects.h"

int scan_id(SerialConf *, Customer *);
int get_inventory(SerialConf *, int, Inventory **, int *);
SerialConf *Init_Pi(void*);

#endif
