#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdstring.h"
#include "wifi_internal.h"

typedef struct nmcli_handle {
    struct list_head wifi_network;
} nmcli_t;

static void* nmcli_init(void)
{
    nmcli_t *nmcli = calloc(1, sizeof(nmcli_t));
    INIT_LIST_HEAD(&nmcli->wifi_network);
    return nmcli;
}

static void free_wifi_network(nmcli_t *nmcli)
{
    wifi_network_info_t *network;

    while(!list_empty(&nmcli->wifi_network)) {
        network = list_first_entry(&nmcli->wifi_network, wifi_network_info_t, list);
        list_del(&network->list);
        free(network);
    }
}

static void __attribute__((unused)) dump_network_info(nmcli_t *nmcli)
{
   wifi_network_info_t *network;

	list_for_each_entry(network, &nmcli->wifi_network, list) {
        printf("ssid:%s, signal:%d, connected:%d\n", network->ssid, network->signal, network->connected);
	}
}

void nmcli_free(void *handle)
{
    nmcli_t *nmcli = (nmcli_t *)handle;

    free_wifi_network(nmcli);
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

static bool nmcli_connection_info(void *handle, wifi_network_info_t *info)
{
    FILE *cmd_file = NULL;
    char line[512];

    if (!info)
        return false;

    cmd_file = popen("nmcli -f NAME,TYPE c show --active | tail -n+2", "r");
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
    char line[512];
    nmcli_t *nmcli = (nmcli_t *)handle;
    FILE *cmd_file = NULL;

    /* Clean up */
    free_wifi_network(nmcli);

    cmd_file = popen("nmcli -f IN-USE,SSID,SIGNAL dev wifi | tail -n+2", "r");
    while (fgets(line, 512, cmd_file)) {
        wifi_network_info_t *info = calloc(1, sizeof(wifi_network_info_t));
        string_trim(line);
        if(!line || line[0] == '\0')
            continue;
        if(line[0] == '*') {
            info->connected = true;
            line[0] = ' ';
            string_trim(line);
        }
        char *tokens[2];
        size_t count, i;
        count = string_split(line, " ", tokens, 2);
        if (count == 2) {
            strncpy(info->ssid, tokens[0], sizeof(info->ssid));
            info->signal = strtoul(tokens[1], NULL, 10);
        }
        for (i=0; i<count; i++)
            free(tokens[i]);
        list_add_tail(&info->list, &nmcli->wifi_network);
    }
    dump_network_info(nmcli);
}

wifi_backend_t wifi_nmcli = {
    .init = nmcli_init,
    .free = nmcli_free,
    .enable = nmcli_enable,
    .connection_info = nmcli_connection_info,
    .scan = nmcli_scan,
    .ident = "nmcli"
};

