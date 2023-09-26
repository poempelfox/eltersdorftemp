
#include "network.h"
#include <driver/gpio.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <nvs_flash.h>
#include <time.h>
#include "sdkconfig.h"
#include "secrets.h"

EventGroupHandle_t network_event_group;
esp_netif_t * mainnetif = NULL;
uint8_t mainmac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


/** Event handler for WiFi events */
static time_t lastwifireconnect = 0;
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_sta_connected_t * ev_co = (wifi_event_sta_connected_t *)event_data;
    wifi_event_sta_disconnected_t * ev_dc = (wifi_event_sta_disconnected_t *)event_data;
    switch (event_id) {
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI("network.c", "WiFi Connected: channel %u bssid %02x%02x%02x%02x%02x%02x",
                           ev_co->channel, ev_co->bssid[0], ev_co->bssid[1], ev_co->bssid[2],
                           ev_co->bssid[3], ev_co->bssid[4], ev_co->bssid[5]);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI("network.c", "WiFi Disconnected: reason %u", ev_dc->reason);
            if (ev_dc->reason == WIFI_REASON_ASSOC_LEAVE) break; /* This was an explicit call to disconnect() */
            if ((lastwifireconnect == 0)
             || ((time(NULL) - lastwifireconnect) > 5)) {
              /* Last reconnect attempt more than 5 seconds ago - try it again */
              ESP_LOGI("network.c", "Attempting WiFi reconnect");
              lastwifireconnect = time(NULL);
              esp_wifi_connect();
            }
            break;
        default: break;
    }
}

/* Event handler for IP_EVENT_(ETH|STA)_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t * event4;
    const esp_netif_ip_info_t *ip_info;
    ip_event_got_ip6_t * event6;
    const esp_netif_ip6_info_t * ip6_info;
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
      ESP_LOGI("network.c", "We got an IPv4 address!");
      event4 = (ip_event_got_ip_t *)event_data;
      ip_info = &event4->ip_info;
      ESP_LOGI("network.c", "IP:     " IPSTR, IP2STR(&ip_info->ip));
      ESP_LOGI("network.c", "NETMASK:" IPSTR, IP2STR(&ip_info->netmask));
      ESP_LOGI("network.c", "GW:     " IPSTR, IP2STR(&ip_info->gw));
      break;
    case IP_EVENT_GOT_IP6:
      ESP_LOGI("network.c", "We got an IPv6 address!");
      event6 = (ip_event_got_ip6_t *)event_data;
      ip6_info = &event6->ip6_info;
      ESP_LOGI("network.c", "IPv6:" IPV6STR, IPV62STR(ip6_info->ip));
      break;
    case IP_EVENT_STA_LOST_IP:
      ESP_LOGI("network.c", "IP-address lost.");
    };
    xEventGroupSetBits(network_event_group, NETWORK_CONNECTED_BIT);
}

void network_prepare(void)
{
    /* WiFi does not work without this because... who knows, who cares. */
    nvs_flash_init();
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    network_event_group = xEventGroupCreate();
    mainnetif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(mainnetif, "eltersdorftemp");
    wifi_init_config_t wicfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wicfg));
    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &got_ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &got_ip_event_handler, NULL));

    wifi_config_t wccfg = {
      .sta = {
        .ssid = WIFISSID,
        .password = WIFIPASSWORD,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK
      }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wccfg));
    esp_base_mac_addr_get(mainmac);
}

void network_on(void)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void network_off(void)
{
    xEventGroupClearBits(network_event_group, NETWORK_CONNECTED_BIT);
    ESP_ERROR_CHECK(esp_wifi_stop());
}

