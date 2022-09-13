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

bool nmcli_enable(void __attribute__((unused)) *handle, bool enabled)
{
    int ret;

    if (enabled)
        pclose(popen("nmcli radio wifi on", "r"));
    else
        pclose(popen("nmcli radio wifi off", "r"));
    
    if (WIFEXITED(ret) != 1 || WEXITSTATUS(ret) != 0)
        return false;
    else
        return true;
}

static bool nmcli_connection_info(void __attribute__((unused)) *handle, wifi_network_info_t *network)
{
    FILE *cmd_file = NULL;
    char line[512];

    if (!network)
        return false;

    cmd_file = popen("nmcli -f NAME,TYPE c show --active | tail -n+2", "r");
    if (fgets(line, sizeof(line), cmd_file)) {
        if(string_ends_with(string_trim(line), "wifi")) {
            char *rest = line;
            char *ssid = strtok_r(rest, " ", &rest);
            strncpy(network->ssid, ssid, sizeof(network->ssid));
            network->connected = true;
            return true;
        }
    }

    return false;
}

void nmcli_scan(void *handle)
{
    nmcli_t *nmcli = (nmcli_t *)handle;
    char line[512];
    FILE *cmd_file = NULL;

    /* Clean up */
    free_wifi_network(nmcli);

    cmd_file = popen("nmcli -f IN-USE,SSID,SIGNAL dev wifi | tail -n+2", "r");
    while (fgets(line, 512, cmd_file)) {
        wifi_network_info_t *network = calloc(1, sizeof(wifi_network_info_t));
        string_trim(line);
        if (line[0] == '\0')
            continue;
        if (line[0] == '*') {
            network->connected = true;
            line[0] = ' ';
            string_trim(line);
        }
        char *tokens[2];
        size_t count, i;
        count = string_split(line, " ", tokens, 2);
        if (count == 2) {
            strncpy(network->ssid, tokens[0], sizeof(network->ssid));
            network->signal = strtoul(tokens[1], NULL, 10);
        }
        for (i=0; i<count; i++)
            free(tokens[i]);
        list_add_tail(&network->list, &nmcli->wifi_network);
    }
    pclose(cmd_file);
}

bool nmcli_connect_ssid(void *handle, wifi_network_info_t *network)
{
    nmcli_t *nmcli = (nmcli_t *)handle;
    char cmd[256];
    char line[512];
    FILE *cmd_file = NULL;

    if (!nmcli || !network)
        return false;
    
    snprintf(cmd, sizeof(cmd),
        "nmcli dev wifi connect \"%s\" password \"%s\" 2>&1", network->ssid, network->password);
    pclose(popen(cmd, "r"));

    cmd_file = popen("nmcli -f IN-USE,SSID dev wifi | tail -n+2", "r");
    while (fgets(line, 512, cmd_file)) {
        string_trim(line);
        if (string_starts_with(line, "*")) {
            line[0] = ' ';
            if (!strncmp(string_trim(line), network->ssid, sizeof(line))) {
                network->connected = true;
                return true;
            }
        }
    }
    pclose(cmd_file);

    return false;
}

bool nmcli_disconnect_ssid(void __attribute__((unused)) *handle, wifi_network_info_t *network)
{
    char cmd[256];
    int ret;

    snprintf(cmd, sizeof(cmd), "nmcli c down \"%s\"", network->ssid);
    ret = pclose(popen(cmd, "r"));
    if (WIFEXITED(ret) != 1 || WEXITSTATUS(ret) != 0) {
        return false;
    } else {
        network->connected = false;
        return true;
    }
}
wifi_backend_t wifi_nmcli = {
    .init = nmcli_init,
    .free = nmcli_free,
    .enable = nmcli_enable,
    .connection_info = nmcli_connection_info,
    .scan = nmcli_scan,
    .connect_ssid = nmcli_connect_ssid,
    .disconnect_ssid = nmcli_disconnect_ssid,
    .ident = "nmcli"
};

