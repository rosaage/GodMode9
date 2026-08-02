#include "common.h"
#include "i2c.h"
#include "ui.h"
#include "fs.h"
#include "timer.h"

#define MCU_FIRM_SIZE                       0x4003
#define MCUREG_LEGACY_VERSION_HIGH          0x0F
#define MCUREG_LEGACY_VERSION_LOW           0x10
#define MCUREG_LEGACY_FIRM_UPLOAD           0x3B
#define MCUREG_FIRMWARE_UPLOAD_0            0x05

void FlashMCU() {
    u8 mcu_version[2];
    int result = 0;
    I2C_readRegBuf(I2C_DEV_MCU, 0x00, mcu_version, sizeof(mcu_version));
    result = ShowPrompt(true, "Current MCU version: %02u.%02u\nContinue?", mcu_version[0]-0x10, mcu_version[1]);
    if (result == 0) {
        ShowPrompt(false, "%s", "Aborting MCU flash");
        return;
    }

    u8 mcu_firm[MCU_FIRM_SIZE] __attribute__((aligned(4)));
    if (FileGetData("0:/hoge.bin", mcu_firm, MCU_FIRM_SIZE, 0) != MCU_FIRM_SIZE) {
        ShowPrompt(false, "%s", "Failed to read full MCU firmware file");
        return;
    }
    
    if (mcu_firm[0] != 0x6A || mcu_firm[1] != 0x68 || mcu_firm[2] != 0x6C) {
        ShowPrompt(false, "%s", "Invalid MCU firmware file");
        return;
    }

    // ShowPrompt(false, "File:  0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", mcu_firm[0], mcu_firm[1], mcu_firm[2], mcu_firm[3], mcu_firm[4], mcu_firm[5], mcu_firm[6], mcu_firm[7]);
    // ShowPrompt(false, "File2: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", mcu_firm[0x3FFB], mcu_firm[0x3FFC], mcu_firm[0x3FFD], mcu_firm[0x3FFE], mcu_firm[0x3FFF], mcu_firm[0x4000], mcu_firm[0x4001], mcu_firm[0x4002]);

    ShowPrompt(false, "%s", "Start flashing MCU");

    u8 legacy_ver_high = 0, legacy_ver_low = 0;
	u8 target_reg = 0;

    I2C_readRegBuf(I2C_DEV_MCU, MCUREG_LEGACY_VERSION_HIGH, &legacy_ver_high, 1);
    I2C_readRegBuf(I2C_DEV_MCU, MCUREG_LEGACY_VERSION_LOW, &legacy_ver_low, 1);

    if (legacy_ver_high == 0 && legacy_ver_low == 1)
		target_reg = MCUREG_LEGACY_FIRM_UPLOAD;
	else target_reg = MCUREG_FIRMWARE_UPLOAD_0;

    result = ShowPrompt(true, "Upload register set to 0x%02X (%s)\nContinue?", target_reg, target_reg == MCUREG_LEGACY_FIRM_UPLOAD ? "legacy" : "new");
    if (result == 0) {
        ShowPrompt(false, "%s", "Aborting MCU flash");
        return;
    }

    bool write_result = I2C_writeRegBuf(I2C_DEV_MCU, target_reg, mcu_firm, MCU_FIRM_SIZE);

    wait_msec(1000);

    ShowPrompt(false, "Finished flashing MCU, status: %s", write_result ? "success" : "failure");

    return;
}
