#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "../../_components/nvs_component.h"
#include "../../_components/csi_component.h"
#include "../../_components/time_component.h"
#include "../../_components/input_component.h"
#include "../../_components/sockets_component2.h"
#include "../../_components/sockets_component3.h"
#include "../../_components/sockets_component4.h"
#include "../../_components/sockets_component5.h"
#include "../../_components/sockets_component6.h"
#include "../../_components/sockets_component7.h"
#include "../../_components/sockets_component8.h"
#include "../../_components/sockets_component9.h"
#include "../../_components/sockets_component10.h"
#include "../../_components/sockets_component11.h"

/*
 * The examples use WiFi configuration that you can set via 'make menuconfig'.
 *
 * If you'd rather not, just change the below entries to strings with
 * the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
 */
#define LEN_MAC_ADDR 20
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_MAX_STA_CONN       32


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

//const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "Active CSI collection (AP)";

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "station:"
            MACSTR
            " join, AID=%d",
                    MAC2STR(event->event_info.sta_connected.mac),
                    event->event_info.sta_connected.aid);
            break;

            //xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "station:"
            MACSTR
            "leave, AID=%d",
                    MAC2STR(event->event_info.sta_disconnected.mac),
                    event->event_info.sta_disconnected.aid);
            break;

            //xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        default:
            break;
    }
    return ESP_OK;
}

/*bool is_wifi_connected() {
    return (xEventGroupGetBits(s_wifi_event_group) & WIFI_CONNECTED_BIT);
}*/

void softap_init() {
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
            .ap = {
                    .ssid = EXAMPLE_ESP_WIFI_SSID,
                    .password = EXAMPLE_ESP_WIFI_PASS,
                    .max_connection = EXAMPLE_MAX_STA_CONN,
                    .authmode = WIFI_AUTH_WPA_WPA2_PSK,
                    .channel = 8,
            },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_AP, 3));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BW_HT20));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_LOGI(TAG, "softap_init finished. SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}
TaskHandle_t xHandle = NULL;

void vTask_socket_transmitter_sta_loop2(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop2();
    }
}
void vTask_socket_transmitter_sta_loop3(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop3();
    }
}
void vTask_socket_transmitter_sta_loop4(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop4();
    }
}
void vTask_socket_transmitter_sta_loop5(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop5();
    }
}
void vTask_socket_transmitter_sta_loop6(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop6();
    }
}
void vTask_socket_transmitter_sta_loop7(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop7();
    }
}
void vTask_socket_transmitter_sta_loop8(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop8();
    }
}
void vTask_socket_transmitter_sta_loop9(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop9();
    }
}
void vTask_socket_transmitter_sta_loop10(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop10();
    }
}
void vTask_socket_transmitter_sta_loop11(void *pvParameters) {
    for (;;) {
        socket_transmitter_sta_loop11();
    }
}
void app_main() {
    nvs_init();
    softap_init();
    // csi_init("AP");

    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop2, "socket_transmitter_sta_loop2",
                            10000, NULL, 100, xHandle, 1);
    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop3, "socket_transmitter_sta_loop3",
                            10000, NULL, 150, xHandle, 1);
    //Add multiple socket function if multiple STA connected
//    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop4, "socket_transmitter_sta_loop4",
//                            4096, NULL, 200, xHandle, 1);
//	xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop5, "socket_transmitter_sta_loop5",
//                            4096, NULL, 250, xHandle, 1);
//	xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop6, "socket_transmitter_sta_loop6",
//                            4096, NULL, 300, xHandle, 1);
//    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop7, "socket_transmitter_sta_loop7",
//                            4096, NULL, 350, xHandle, 1);
//    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop8, "socket_transmitter_sta_loop8",
//                            4096, NULL, 400, xHandle, 1);
//	xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop9, "socket_transmitter_sta_loop9",
//                            4096, NULL, 450, xHandle, 1);
//    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop10, "socket_transmitter_sta_loop10",
//                            4096, NULL, 500, xHandle, 1);
//    xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop11, "socket_transmitter_sta_loop11",
//                            4096, NULL, 550, xHandle, 1);    
    /*xTaskCreatePinnedToCore(vTask_socket_transmitter_sta_loop, "socket_transmitter_sta_loop",
                            4096, (void *) &is_wifi_connected, 100, xHandle, 1);*/
}
