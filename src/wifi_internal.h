#ifndef __WIFI_INTERNAL_H__
#define __WIFI_INTERNAL_H__

#include <stdbool.h>

typedef struct wifi_backend
{
    void* (*init)(void);
    void (*free)(void *handle);
    const char *ident;
} wifi_backend_t;

extern wifi_backend_t wifi_nmcli;

#endif