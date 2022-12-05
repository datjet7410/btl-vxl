#include <Arduino.h>

#include <Wire.h>

#include "afe.h"

const int slave_address = 0x58;

void write_register(int register_address, unsigned long data) {
    Wire.beginTransmission(slave_address);
    byte configData[3];
    configData[0] = (byte)(data >> 16);
    configData[1] = (byte)(((data & 0x00FFFF) >> 8));
    configData[2] = (byte)(((data & 0x0000FF)));

    Wire.write(register_address);
    Wire.write(configData, 3);
    Wire.endTransmission();
}

void init_afe() {
    write_register(0x00, 0);

    write_register(0x01, 80);
    write_register(0x02, 399);
    write_register(0x03, 800);
    write_register(0x04, 1199);
    write_register(0x05, 480);
    write_register(0x06, 799);
    write_register(0x07, 880);
    write_register(0x08, 1199);
    write_register(0x09, 0);
    write_register(0x0A, 399);
    write_register(0x0B, 1279);
    write_register(0x0C, 1598);
    write_register(0x0D, 408);
    write_register(0x0E, 1467);
    write_register(0x0F, 1476);
    write_register(0x10, 2535);
    write_register(0x11, 2544);
    write_register(0x12, 3603);
    write_register(0x13, 3612);
    write_register(0x14, 4671);
    write_register(0x15, 401);
    write_register(0x16, 407);
    write_register(0x17, 1469);
    write_register(0x18, 1475);
    write_register(0x19, 2537);
    write_register(0x1A, 2543);
    write_register(0x1B, 3605);
    write_register(0x1C, 3611);
    write_register(0x1D, 39999);
    write_register(0x1E, 259);

    write_register(0x20, 0); // using 2 different TIA gain ?
    write_register(0x21,
                   (1 << 8) | (2 << 3) | 5); // PROG_TG_EN, TIA_CF, TIA_GAIN
    write_register(0x22, (0 << 6) | 4);                 // ILED2, ILED1
    write_register(0x23, 0);
    write_register(0x32, 0x00155F);
    write_register(0x33, 0x00991F);
    write_register(0x36, 0x000190);
    write_register(0x37, 0x00031F);
    write_register(0x34, 0x001563);
    write_register(0x35, 0x00991F);
        write_register(0x3A,
                       (1 << 9) | (15 << 5)); // POL_OFFDAC_LED1,
                       //I_OFFDAC_LED1

    write_register(0x23, 0x104218); // Set CLK Mode to internal clock (default
                                    // Wert: 124218 mit interner CLK)
    write_register(
        0x29,
        0x2); // Don´t set the internal clock to the CLK pin for outside usage
    write_register(0x31, 0x000021); // Division ratio for external clock mode
                                    // set to fit the Arduino Mega 16MHz
    write_register(0x39, 0); // Set the lowes sampling rate to 61Hz (~17 ms)

    write_register(0x00, 1);
}

long read_register(int register_address) {
    byte configData[3];
    signed long retVal;

    byte *read_Data = configData;
    int byteCount = 3;

    Wire.beginTransmission(slave_address);
    Wire.write(register_address);
    Wire.endTransmission();
    Wire.requestFrom(slave_address, 3);
    while (Wire.available() && (byteCount != 0)) {
        *read_Data++ = Wire.read();
        byteCount--;
    }
    retVal = configData[0];
    retVal = (retVal << 8) | configData[1];
    retVal = (retVal << 8) | configData[2];
    if (register_address >= 0x2A && register_address <= 0x2F) {
        if (retVal &
            0x00200000) // check if the ADC value is positive or negative
        {
            retVal &= 0x003FFFFF; // convert it to a 22 bit value
            return (retVal ^ 0xFFC00000);
        }
    }
    return retVal;
}
