#include "mpu6050.h"
#include "mpu6050_used.h"


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_log.h" 

#define I2C_MASTER_SCL_IO 22      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

#define MPU6050_INIT_ERR_STR             "mpu6050 init error"

mpu6050_used::mpu6050_used()
{
    i2c_bus_init();
    m_mpu6050_handle = i2c_sensor_mpu6050_init();
    load_mpu6050_sensor_data();
    reset();
}

void mpu6050_used::reset()
{
    mpu6050_delete(m_mpu6050_handle);
    i2c_driver_delete(I2C_MASTER_NUM);
    m_mpu6050_handle = NULL;
}

mpu6050_used::~mpu6050_used()
{
    if(m_mpu6050_handle != NULL)
        reset();
}

bool mpu6050_used::is_ok()
{
    return ok;
}

void mpu6050_used::i2c_bus_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

/**
 * @brief i2c master initialization
 */
mpu6050_handle_t mpu6050_used::i2c_sensor_mpu6050_init()
{
    mpu6050_handle_t mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    if(mpu6050 == NULL)
        return NULL;

    if (ESP_OK != mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS))
    {
        ESP_LOGE("MPU6050", MPU6050_INIT_ERR_STR);
        mpu6050_delete(mpu6050);
        return NULL;
    }

    if (ESP_OK != mpu6050_wake_up(mpu6050))
    {
        printf("mpu6050_wake error\n");
        mpu6050_delete(mpu6050);
        return NULL;
    }
    return mpu6050;
}

void mpu6050_used::load_mpu6050_sensor_data()
{
    if(m_mpu6050_handle == NULL)
    {
        printf("mpu6050 init error\n");
        return;
    }
    uint8_t mpu6050_deviceid;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;

    // mpu6050_get_deviceid(m_mpu6050_handle, &mpu6050_deviceid);
    // printf("Get device id   : %d\n", mpu6050_deviceid);

    mpu6050_get_acce(m_mpu6050_handle, &acce);
    printf("acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z);

    mpu6050_get_gyro(m_mpu6050_handle, &gyro);
    printf("gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f\n", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);

    mpu6050_get_temp(m_mpu6050_handle, &temp);

    printf("t:%.2f \n", temp.temp);

    mpu6050_complimentory_filter(m_mpu6050_handle, &acce, &gyro, &pose_2d);
    printf("com value: pitch::%.2f, roll:%.2f\n",pose_2d.pitch,pose_2d.roll);
    ok = true;
}

complimentary_angle_t mpu6050_used::get_pose_2d() const
{
    return pose_2d;
}