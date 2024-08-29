#pragma once

#include "mpu6050.h"
#ifdef __cplusplus
extern "C" {
#endif

class mpu6050_used
{
private:
    mpu6050_handle_t m_mpu6050_handle = NULL;
    complimentary_angle_t pose_2d;
    bool ok = false;

private:
    void i2c_bus_init(void);
    mpu6050_handle_t i2c_sensor_mpu6050_init();
    void load_mpu6050_sensor_data();
    void reset();

public:
    mpu6050_used();
    ~mpu6050_used();
    
    bool is_ok();
    complimentary_angle_t get_pose_2d() const;
};


#ifdef __cplusplus
}
#endif
