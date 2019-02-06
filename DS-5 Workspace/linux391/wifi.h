#ifndef _WIFI_H_
#define _WIFI_H_

#include "serial.h"

int exec_lua(SerialConf*, char*, char*);
SerialConf *Init_WiFi(void*);


#endif
