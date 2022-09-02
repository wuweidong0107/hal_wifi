#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

enum wifi_error_code {
    WIFI_ERROR_OPEN  = -1,
};

typedef struct wifi_handle wifi_t;

/* Primary Functions */
wifi_t *wifi_new(void);
void wifi_free(wifi_t *wifi);
const char *wifi_errmsg(wifi_t *wifi);

#ifdef __cplusplus
}
#endif

#endif