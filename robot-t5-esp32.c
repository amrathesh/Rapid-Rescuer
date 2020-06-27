 /*
▪ * Team Id: 34
▪ * Author List: Abdul Khaliq Almel,Shashank K Holla,Amrathesh
▪ * Filename: robot-t5-esp32.c
▪ * Theme: Rapid Rescuer(RR)
▪ * Functions: onRecieve,wifi_init_softap,event_handler
▪ * Global Variables: rx,s_event_group,somethingToSend
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// UART Stuff
#include "driver/uart.h"
#include "driver/gpio.h"
#define ECHO_TEST_TXD (GPIO_NUM_32) // Connected to AVR Rx-0
#define ECHO_TEST_RXD (GPIO_NUM_33) // Connected to AVR Tx-0
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)
#define PORT 3333 // Port address for socket communication
#define RX_BUFFER_SIZE 128

static const char *TAG = "ESP32";
static EventGroupHandle_t s_wifi_event_group;

bool somethingToSend = false;

int onReceive(char *rx, int sock);
char rx[100];

/* Wi-Fi event handler */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/* Function to initialize Wi-Fi at station */
void wifi_init_softap() //my_wifi_config my_wifi
{
    s_wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "Team34",
            .password = "teamleader34",
            .ssid_len = 0,
            .channel = 6,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK, //WIFI_AUTH_WPA_WPA2_PSK, //WIFI_AUTH_OPEN
            .ssid_hidden = 0,
            .max_connection = 4,
            .beacon_interval = 100},
    };
    // printf(">>>>>>>> SSID: %s <<<<<<<<<\n", wifi_config.ap.ssid);
    // printf(">>>>>>>> PASS: %s <<<<<<<<<\n", wifi_config.ap.password);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_softap();
    char rx_buffer[RX_BUFFER_SIZE]; // buffer to store data from client
    char ipv4_addr_str[128];        // buffer to store IPv4 addresses as string
    char ipv4_addr_str_client[128]; // buffer to store IPv4 addresses as string
    int addr_family;
    int ip_protocol;
    //char *some_addr;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    char *sendBuf = (char *)malloc(RX_BUFFER_SIZE);
    uint8_t *data_uart = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntop(AF_INET, &dest_addr.sin_addr, ipv4_addr_str, INET_ADDRSTRLEN);
        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0)
        {
            printf("[ERROR] Unable to create socket. ERROR %d\n", listen_sock);
            break;
        }
        int flag = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            printf("[ERROR] Socket unable to bind. ERROR %d\n", err);
            break;
        }
        err = listen(listen_sock, 1);
        if (err != 0)
        {
            printf("[ERROR] Error occurred during listen. ERROR %d\n", err);
            break;
        }
        printf("[DEBUG] Socket listening\n");
        struct sockaddr_in6 source_addr; // Can store both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            printf("[ERROR] Error occurred during listen. ERROR %d\n", sock);
            break;
        }
        printf("[DEBUG] Socket accepted\n");

        int complete = 0;
        while (1)
        {
            int len_uart = uart_read_bytes(UART_NUM_1, data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
            if ((len_uart > 0 && (strstr((char *)data_uart, "@started@") != NULL)))
            {
                printf("[DEBUG] Got Start \n");
                strcpy(sendBuf, " @started@");
                int err = send(sock, sendBuf, strlen(sendBuf), 0);
                int i = 0;
                if (err != -1)
                {
                    printf("[DEBUG] Sent @started@ successfully\n");

                    while (1)
                    {
                        printf("[DEBUG] Going to recv\n");
                        int len = recv(sock, rx, sizeof(rx) - 1, 0);

                        if (len < 0)
                        {
                            printf("[error] recieving from client failed \n");
                        }
                        else
                        {

                            printf("[DEBUG] recvd inside    now %d %s\n", len, rx);
                            //printf("Path recieved : %s", rx);

                            uart_write_bytes(UART_NUM_1, (const char *)rx, strlen(rx));

                            while (1)
                            {
                                int len_uart = uart_read_bytes(UART_NUM_1, data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
                                data_uart[len_uart] = NULL;

                                if (len_uart > 0)
                                {
                                    printf("[debug] recived from atmega %s \n", (char *)data_uart);
                                    strcpy(sendBuf, (char *)data_uart);
                                    int err = send(sock, sendBuf, strlen(sendBuf), 0);

                                    if (err < 0)
                                    {
                                        printf("[error] sending to client failed \n");
                                    }
                                    else
                                    {
                                        if (strstr(sendBuf, " Task accomplished"))
                                        {
                                            complete = 1;
                                        }
                                        break;
                                    }
                                }
                                else
                                {
                                    printf("[error]could read from atmega \n");
                                }
                            }
                            if (complete == 1)
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    printf("[error] could not send @Started@ to client \n");
                }
            }
            else
            {
                printf("[error] the message was not start \n");
            }
        }
        if (sock != -1)
        {
            printf("[DEBUG] Shutting down socket and restarting...\n");
            shutdown(sock, 0);
            close(sock);
            shutdown(listen_sock, 0);
            close(listen_sock);
            vTaskDelay(5); // Required for FreeRTOS on ESP32
        }
    }
    return;
}
/*
if(i == 0){
                                    //printf("[DEBUG] i == 0 \n");
                                    sprintf(sendBuf, "@$|2|(6,9)@");
                                    //sprintf(sendBuf, "@start@");
                                    int err = send(sock,sendBuf, strlen(sendBuf), 0);
                                    i++;
                                    break;
                                }
                                if(i==1){
                                    sprintf(sendBuf, "@$|4|(8,6)@");
                                    int err = send(sock,sendBuf, strlen(sendBuf), 0);
                                    i++;
                                    break;
                                    

                                }
                                if(i==2){
                                    sprintf(sendBuf, "@HA reached, Task accomplished!@");
                                    int err = send(sock,sendBuf, strlen(sendBuf), 0);
                                    i++;
                                    break;
                                }

                                */