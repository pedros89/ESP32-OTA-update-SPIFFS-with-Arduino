/* OTA example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"
#include "errno.h"
#include <CertMeren.h>
#include <WiFi.h>
#include <WiFiClient.h>

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

#define URL_SPIFFS "https://www.merenel.com/fileSystemImage.bin"

#define BUFFSIZE 1024
#define HASH_LEN 32 /* SHA-256 digest length */

static const char *TAG = "native_ota_example";
/*an ota data write buffer ready to write to the flash*/
static char ota_write_data[BUFFSIZE + 1] = { 0 };
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define OTA_URL_SIZE 256


void connectWiFI(){
  const char* ssid = "TEISWLAN";
  const char* password = "stry+tek-r75";
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print("connecting to WiFi");
  }
}

static void http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

static void print_sha256 (const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s: %s", label, hash_print);
}

static void infinite_loop(void)
{
    int i = 0;
    ESP_LOGI(TAG, "When a new firmware is available on the server, press the reset button to download it");
    while(1) {
        ESP_LOGI(TAG, "Waiting for a new firmware ... %d", ++i);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

static void __attribute__((noreturn)) task_fatal_error(void)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");
    (void)vTaskDelete(NULL);

    while (1) {
        ;
    }
}

void ota_spiffs_task()
{
esp_err_t err;
/* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
esp_ota_handle_t update_handle = 0 ;
const esp_partition_t *update_partition = NULL;
esp_partition_t *spiffs_partition=NULL;
ESP_LOGI(TAG, "Starting OTA SPIFFS...");
 
const esp_partition_t *configured = esp_ota_get_boot_partition();
const esp_partition_t *running = esp_ota_get_running_partition();
 
/*Update SPIFFS : 1/ First we need to find SPIFFS partition  */
 
esp_partition_iterator_t spiffs_partition_iterator=esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS,NULL);
while(spiffs_partition_iterator !=NULL){
    spiffs_partition = (esp_partition_t *)esp_partition_get(spiffs_partition_iterator);
    printf("main: partition type = %d.\n", spiffs_partition->type);
    printf("main: partition subtype = %d.\n", spiffs_partition->subtype);
    printf("main: partition starting address = %x.\n", spiffs_partition->address);
    printf("main: partition size = %x.\n", spiffs_partition->size);
    printf("main: partition label = %s.\n", spiffs_partition->label);
    printf("main: partition subtype = %d.\n", spiffs_partition->encrypted);
    printf("\n");
    printf("\n");
    spiffs_partition_iterator=esp_partition_next(spiffs_partition_iterator);
}
vTaskDelay(1000/portTICK_RATE_MS);
esp_partition_iterator_release(spiffs_partition_iterator);
 
/* Wait for the callback to set the CONNECTED_BIT in the
   event group.
*/

//xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,false, true, portMAX_DELAY);
ESP_LOGI(TAG, "Connect to Wifi ! Start to Connect to Server....");
 
esp_http_client_config_t config;
  memset(&config, 0, sizeof(esp_http_client_config_t));
  config.url = URL_SPIFFS;
  config.cert_pem = UPGRADE_SERVER_CERT;


  
esp_http_client_handle_t client = esp_http_client_init(&config);
if (client == NULL) {
    ESP_LOGE(TAG, "Failed to initialise HTTP connection");
    task_fatal_error();
}
err = esp_http_client_open(client, 0);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    task_fatal_error();
}
esp_http_client_fetch_headers(client);
 
/* 2: Delete SPIFFS Partition  */
err=esp_partition_erase_range(spiffs_partition,spiffs_partition->address,spiffs_partition->size);
 
 
int binary_file_length = 0;
/*deal with all receive packet*/
int offset=0;
    /*deal with all receive packet*/
    while (1) {
        int data_read = esp_http_client_read(client, ota_write_data, BUFFSIZE);
        if (data_read < 0) {
            ESP_LOGE(TAG, "Error: SSL data read error");
            http_cleanup(client);
            task_fatal_error();
        } else if (data_read > 0) {
            /* 3 : WRITE SPIFFS PARTITION */
            err= esp_partition_write(spiffs_partition,offset,(const void *)ota_write_data, data_read);
            
            if (err != ESP_OK) {
                http_cleanup(client);
                task_fatal_error();
            }
            offset=offset+1024;
       
        binary_file_length += data_read;
        ESP_LOGD(TAG, "Written image length %d", binary_file_length);
    } else if (data_read == 0) {
        ESP_LOGI(TAG, "Connection closed,all data received");
        break;
    }
}
ESP_LOGI(TAG, "Total Write binary data length : %d", binary_file_length);
 
//ESP_LOGI(TAG, "Prepare to launch ota APP task or restart!");
//xTaskCreate(&ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
vTaskDelete(NULL);
}



void setup(){
  connectWiFI();
  ota_spiffs_task();
  }

void loop(){}
