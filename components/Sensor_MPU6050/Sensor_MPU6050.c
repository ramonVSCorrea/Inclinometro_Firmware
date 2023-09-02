#include "Sensor_MPU6050.h"
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include "Variaveis_Globais.h"
#include "sdkconfig.h"

static char tag[] = "MPU6050";
i2c_cmd_handle_t cmd;

// Ângulo lateral:
float p1 = -4.44e-08;
float p2 = 7.454e-06;
float p3 = -0.0002898;
float p4 = -0.01061;
float p5 = 0.01591;
float p1_2 = -1.014e-07;
float p2_2 = 3.026e-05;
float p3_2 = -0.003384;
float p4_2 = 0.1549;
float p5_2 = -2.207;

// Ângulo frontal:
float p6 = 2.313e-05;
float p7 = -0.004459;
float p8 = 0.2771;
float p9 = -5.054;

#define PIN_SDA 21
#define PIN_CLK 22
#define I2C_ADDRESS 0x68  // I2C address of MPU6050

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1 0x6B

/*
 * The following registers contain the primary data we are interested in
 * 0x3B MPU6050_ACCEL_XOUT_H
 * 0x3C MPU6050_ACCEL_XOUT_L
 * 0x3D MPU6050_ACCEL_YOUT_H
 * 0x3E MPU6050_ACCEL_YOUT_L
 * 0x3F MPU6050_ACCEL_ZOUT_H
 * 0x50 MPU6050_ACCEL_ZOUT_L
 * 0x41 MPU6050_TEMP_OUT_H
 * 0x42 MPU6050_TEMP_OUT_L
 * 0x43 MPU6050_GYRO_XOUT_H
 * 0x44 MPU6050_GYRO_XOUT_L
 * 0x45 MPU6050_GYRO_YOUT_H
 * 0x46 MPU6050_GYRO_YOUT_L
 * 0x47 MPU6050_GYRO_ZOUT_H
 * 0x48 MPU6050_GYRO_ZOUT_L
 */

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)                   \
  do {                                       \
    esp_err_t rc = (x);                      \
    if (rc != ESP_OK) {                      \
      ESP_LOGE("err", "esp_err_t = %d", rc); \
      assert(0 && #x);                       \
    }                                        \
  } while (0);

void task_mpu6050() {
  // ESP_LOGD(tag, ">> mpu6050");
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = PIN_SDA;
  conf.scl_io_num = PIN_CLK;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = 100000;
  ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
  ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

  vTaskDelay(200 / portTICK_PERIOD_MS);

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(
      i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
  i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1);
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(
      i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
  i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1);
  i2c_master_write_byte(cmd, 0, 1);
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  uint8_t data[14];

  short accel_x;
  short accel_y;
  short accel_z;
  short t;

  while (1) {
    // Tell the MPU6050 to position the internal register pointer to register
    // MPU6050_ACCEL_XOUT_H.
    cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(
        i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(
        i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));

    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 1, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 2, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 3, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 4, 0));
    // ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 5, 1));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 5, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 6, 0));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + 7, 1));

    // i2c_master_read(cmd, data, sizeof(data), 1);
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);

    accel_x = (data[0] << 8) | data[1];
    accel_y = (data[2] << 8) | data[3];
    accel_z = (data[4] << 8) | data[5];
    t = (data[6] << 8) | data[7];
    // ESP_LOGE(tag, "T: %d", t);

    t = t / 340 + 36.53;

    float offsetLat = p1 * (t ^ 4) + p2 * (t ^ 3) + p3 * (t ^ 2) + p4 * t +
                      p5;  // offset lateral
    float offsetFront =
        p6 * (t ^ 3) + p7 * (t ^ 2) + p8 * t + p9;  // offset frontal

    // ESP_LOGI(tag, "accel_x: %d, accel_y: %d, accel_z: %d", accel_x,
    // accel_y,
    //          accel_z);

    // Suponha que os valores brutos estejam em accel_x, accel_y e
    // accel_z float roll = atan2(accel_y, accel_z) * (180.0 / M_PI);
    // float pitch = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z *
    // accel_z)) *
    //               (180.0 / M_PI);

    // ESP_LOGI(tag, "ROLL: %.2f, PITCH: %.2f", roll, pitch);

    AnguloLateral = (atan((float)accel_x / (float)accel_z)) * 180 / M_PI;
    AnguloLateral -= offsetLat;
    AnguloFrontal = (atan((float)accel_y / (float)accel_z)) * 180 / M_PI;
    AnguloFrontal -= offsetFront;

    ESP_LOGI(tag, "Lateral: %.2f, Frontal: %.2f", AnguloLateral, AnguloFrontal);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}
