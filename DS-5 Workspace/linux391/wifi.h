#ifndef _WIFI_H_
#define _WIFI_H_

#include "serial.h"

int exec_lua(SerialConf*, char*, char*);
void wait_for(SerialConf *, char *);
SerialConf *Init_WiFi(void*);
int send_sms(SerialConf *sc, char * to, char * body);

#endif
