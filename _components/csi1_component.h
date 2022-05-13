#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include "math.h"
#include "sockets_component.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"

#define GPIO_MOSI 15
#define GPIO_MISO 2
#define GPIO_SCLK 14
#define GPIO_CS 13
#define GPIO_HANDSHAKE 25
char *project_type;
static xQueueHandle rdySem;
#define CSI_RAW 0
#define CSI_AMPLITUDE 1
#define CSI_PHASE 0
#define CSI_TYPE CSI_RAW
spi_device_handle_t handle;
//char tranbuf[64]="";

spi_bus_config_t buscfg = {
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
//        .max_transfer_sz=4096*2,      
	};;
spi_device_interface_config_t devcfg = {
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=10000000,
        .duty_cycle_pos=128,        //50% duty cycle
        .mode=0,
        .spics_io_num=GPIO_CS,
        .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size=3,
	};;
//GPIO config for the handshake line.
gpio_config_t io_conf={
    .intr_type=GPIO_PIN_INTR_POSEDGE,
    .mode=GPIO_MODE_INPUT,
    .pull_up_en=1,
    .pin_bit_mask=(1<<GPIO_HANDSHAKE)
};

esp_err_t spi_write(int8_t *csi, int8_t len){
    esp_err_t ret;
    spi_transaction_t t;
//    if (len==0) return;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer = csi;
//	printf("[");
//    for (int i = 0; i < 64; i++){
//        printf(" %d" , data[i]);
//    }
//    printf("]");
//    printf("\n");
//    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_transmit(handle, &t);  //Transmit!
//    ret=spi_device_polling_transmit(handle, &t);
    assert(ret==ESP_OK);            //Should have had no issues.
    return ret;
}
static void IRAM_ATTR gpio_handshake_isr_handler(void* arg)
{
    //Sometimes due to interference or ringing or something, we get two irqs after eachother. This is solved by
    //looking at the time between interrupts and refusing any interrupt too close to another one.
    static uint32_t lasthandshaketime;
    uint32_t currtime=xthal_get_ccount();
    uint32_t diff=currtime-lasthandshaketime;
    if (diff<240000) return; //ignore everything <1ms after an earlier irq
    lasthandshaketime=currtime;

    //Give the semaphore.
    BaseType_t mustYield=false;
    xSemaphoreGiveFromISR(rdySem, &mustYield);
    if (mustYield) portYIELD_FROM_ISR();
}
void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);
    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314
    int8_t *my_ptr;
    //128 if CSI_RAW
    int8_t test1[64];

#if CSI_RAW
    my_ptr = data->buf;

    for (int i = 0; i < 128; i++) {
		test1[i] = (int8_t)my_ptr[i];
    }
    xSemaphoreTake(rdySem, portMAX_DELAY);
    spi_write(test1, sizeof(test1));
#endif
#if CSI_AMPLITUDE
    my_ptr = data->buf;
    
    for (int i = 0; i < 64; i++) {
        test1[i] = (int8_t)sqrt(pow(my_ptr[i * 2], 2) + pow(my_ptr[(i * 2) + 1], 2));
    }
	xSemaphoreTake(rdySem, portMAX_DELAY);
	spi_write(test1, sizeof(test1));
#endif
#if CSI_PHASE
    my_ptr = data->buf;

    for (int i = 0; i < 64; i++) {
        test1[i] = atan2(my_ptr[i*2], my_ptr[(i*2)+1]);
    }
	xSemaphoreTake(rdySem, portMAX_DELAY);
	spi_write(test1, sizeof(test1));
#endif
    vTaskDelay(0);
}

void csi_init(char *type) {
    project_type = type;
    //spi initialize
	esp_err_t ret;	
	//Create the semaphore.
    rdySem=xSemaphoreCreateBinary();
    //Set up handshake line interrupt.
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_set_intr_type(GPIO_HANDSHAKE, GPIO_PIN_INTR_POSEDGE);
    gpio_isr_handler_add(GPIO_HANDSHAKE, gpio_handshake_isr_handler, NULL);

    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &handle);
    ESP_ERROR_CHECK(ret);
    xSemaphoreGive(rdySem);
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
