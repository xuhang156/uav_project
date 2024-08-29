
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
#include "esp_chip_info.h"
#include "esp_log.h" 
#include "mpu6050_used.h"

void hardware_info(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);
}

void restart()
{
    fflush(stdout);
    esp_restart();
}

extern "C" void app_main()
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        mpu6050_used sensor;
        if(sensor.is_ok())
        {
            auto pose = sensor.get_pose_2d();
            printf("x:%.2f, y:%.2f\n", pose.roll, pose.pitch);
        }
    }
}
