#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "wifi_internal.h"

typedef struct nmcli_handle {
    int i;
} nmcli_t;

static void* nmcli_init(void)
{
    nmcli_t *nmcli = calloc(1, sizeof(nmcli_t));
    return nmcli;
}

void nmcli_free(void *handle)
{
    if (handle)
        free(handle);
}

bool nmcli_enable(void *handle, bool enabled)
{
    if (enabled)
        pclose(popen("nmcli radio wifi on", "r"));
    else
        pclose(popen("nmcli radio wifi off", "r"));
}

bool nmcli_connection_info(void *handle, wifi_network_into_t *info)
{
}

void nmcli_scan(void *handle)
{

}

wifi_backend_t wifi_nmcli = {
    nmcli_init,
    nmcli_free,
    nmcli_enable,
    "nmcli"
};

