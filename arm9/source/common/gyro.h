#pragma once

#include "common.h"

typedef enum {
    GYRO_0_WHO_AM_I = 0x00,
    GYRO_0_PWR_MGM = 0x3E
} GyroModel0Register;

typedef enum {
    GYRO_1_PWR_MGM_1 = 0x6B,
    GYRO_1_WHO_AM_I = 0x75
} GyroModel1Register;

typedef enum {
    GYRO_2_PWR_MGM = 0x39
} GyroModel2Register;

u32 GetGyroModel();
