#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdstring.h"
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

static bool nmcli_connection_info(void *handle, wifi_network_into_t *info)
{
    FILE *cmd_file = NULL;
    char line[512];

    if (!info)
        return false;

    cmd_file = popen("nmcli -f NAME,TYPE c show --active | tail -n2", "r");
    if (fgets(line, sizeof(line), cmd_file)) {
        if(string_ends_with(string_trim(line), "wifi")) {
            char *rest = line;
            char *ssid = strtok_r(rest, " ", &rest);
            strncpy(info->ssid, ssid, sizeof(info->ssid));
            info->connected = true;
            return true;
        }
    }

    return false;
}

void nmcli_scan(void *handle)
{

}

wifi_backend_t wifi_nmcli = {
    .init = nmcli_init,
    .free = nmcli_free,
    .enable = nmcli_enable,
    .connection_info = nmcli_connection_info,
    .ident = "nmcli"
};

