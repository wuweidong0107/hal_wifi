#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "wifi_internal.h"
#include "wifi.h"

struct wifi_handle {
    const wifi_backend_t *backend;
    void *backend_handle;

    struct {
        int c_errno;
        char errmsg[128];
    } error;
};

const wifi_backend_t *wifi_backends[] = {
    &wifi_nmcli,
    NULL,
};

static int _wifi_error(wifi_t *wifi, int code, int c_errno, const char *fmt, ...)
{
    va_list ap;
    
    wifi->error.c_errno = c_errno;
    va_start(ap, fmt);
    vsnprintf(wifi->error.errmsg, sizeof(wifi->error.errmsg), fmt, ap);
    va_end(ap);

    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(wifi->error.errmsg + strlen(wifi->error.errmsg),
                 sizeof(wifi->error.errmsg) - strlen(wifi->error.errmsg),
                 ": %s [errno %d]", buf, c_errno);
    }

    return code;
}

wifi_t *wifi_new(void)
{
    wifi_t *wifi = calloc(1, sizeof(wifi_t));
    if (wifi == NULL)
        return NULL;
    
    return wifi;
}

void wifi_free(wifi_t *wifi)
{
    free(wifi);
}

const char *wifi_errmsg(wifi_t *wifi)
{
    return wifi->error.errmsg;
}