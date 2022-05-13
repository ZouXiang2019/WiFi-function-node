#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include "math.h"
#include <time.h>
//#include "soc/efuse_reg.h"

char *project_type;

#define CSI_RAW 0
#define CSI_AMPLITUDE 1
#define CSI_PHASE 0

int8_t test[64];
#define CSI_TYPE CSI_RAW
    

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);//source mac of csi
    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314

    int8_t *my_ptr;

#if CSI_RAW
    my_ptr = data->buf;

    for (int i = 0; i < 128; i++) {
        test1[i] = (int8_t)my_ptr[i];
    }
#endif
#if CSI_AMPLITUDE
    my_ptr = data->buf;
	for (int i = 0; i < 64; i++) {
	    test[i] = (int8_t)sqrt(pow(my_ptr[i * 2], 2) + pow(my_ptr[(i * 2) + 1], 2));
//	    printf(" %d" , test[i]);
	}
#endif
#if CSI_PHASE
    my_ptr = data->buf;

    for (int i = 0; i < 64; i++) {
        test[i] = atan2(my_ptr[i*2], my_ptr[(i*2)+1]);
    }
#endif
    vTaskDelay(2);
}

void csi_init(char *type) {
    project_type = type;

#ifdef CONFIG_SHOULD_COLLECT_CSI
    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    // @See: https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L401
    wifi_csi_config_t configuration_csi;
    configuration_csi.lltf_en = 1;/**< enable to receive legacy long training field(lltf) data. Default enabled */
    configuration_csi.htltf_en = 1;/**< enable to receive HT long training field(htltf) data. Default enabled */
    configuration_csi.stbc_htltf2_en = 1;/**< enable to receive space time block code HT long training field(stbc-htltf2) data. Default enabled */
    configuration_csi.ltf_merge_en = 1;/**< enable to generate htlft data by averaging lltf and ht_ltf data when receiving HT packet. Otherwise, use ht_ltf data directly. Default enabled */
    configuration_csi.channel_filter_en = 1;/**< enable to turn on channel filter to smooth adjacent sub-carrier. Disable it to keep independence of adjacent sub-carrier. Default enabled */
    configuration_csi.manu_scale = 0;/**< manually scale the CSI data by left shifting or automatically scale the CSI data. If set true, please set the shift bits. false: automatically. true: manually. Default false */

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL));

#endif
}

#endif //ESP32_CSI_CSI_COMPONENT_H
