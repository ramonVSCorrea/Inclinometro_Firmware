#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include "Sensor_MPU6050.h"
#include "sdkconfig.h"

void app_main(void) {
    xTaskCreate(task_mpu6050, "Task_SensorMPU6050", 2048, NULL,
              configMAX_PRIORITIES - 1, NULL);
}
