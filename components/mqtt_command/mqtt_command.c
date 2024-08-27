#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "include/mqtt_command.h"

#define MQTT_TOPIC "say_hello"
#define MQTT_MSG "Hello MQTT"
#define MQTT_QOS 1
#define MQTT_CLIENT_ID "esp32_client"
#define TAG "MQTT"

static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_mqtt_client_subscribe(client, MQTT_TOPIC, MQTT_QOS);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA, Topic=%s", event->topic);
            ESP_LOGI(TAG, "MQTT_EVENT_DATA, msg_data=%s", event->data);
            char* datas = malloc(event->data_len + 1);
            memcpy(datas, event->data, event->data_len);
            datas[event->data_len] = '\0';
            printf("data: %s\n", datas);
            free(datas);
            break;
    }
}


static void mqtt_start(void)
{
    //初始化
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://pi.33521.wang",
        .port = 1883,
        .client_id = MQTT_CLIENT_ID,
        .username = "xuhang",
        .password = "184hang."
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    
}