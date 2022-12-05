#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

#include "afe.h"

const uint8_t rst_pin = 4;
const uint8_t clk_pin = 5;
const uint8_t int_pin = 6;

const int WIDTH = 128;
const int HEIGHT = 64;
const int LENGTH = WIDTH;

int x;
int y[LENGTH];

void drawY() {
    u8g.drawPixel(0, y[0]);
    for (int i = 1; i < LENGTH; i++) {
        if (y[i] != -1) {
            // u8g.drawPixel(i, y[i]);
            u8g.drawLine(i - 1, y[i - 1], i, y[i]);
        } else {
            break;
        }
    }
}

void clearY() {
    for (int i = 0; i < LENGTH; i++) {
        y[i] = -1;
    }
}

void setup() {
    pinMode(rst_pin, OUTPUT);
    pinMode(clk_pin, OUTPUT);
    pinMode(int_pin, INPUT);

    Serial.begin(115200);

    /*
       Power on sensor and reset registers
    */
    digitalWrite(rst_pin, HIGH);
    delayMicroseconds(30);
    digitalWrite(rst_pin, LOW);
    delayMicroseconds(30);
    digitalWrite(rst_pin, HIGH);
    delay(10);

    /*
     * Setup sensor
     */
    init_afe();

    /*
     * Clear OLED
     */
    x = 0;
    clearY();
}

void loop() {
    u8g.firstPage();
    do {

        /*
           Read sensor data
        */
        if (digitalRead(int_pin)) {
            long val = read_register(0x2C); // LED1
            y[x] = map(val, 0, 300000, 0, HEIGHT - 1);

            x++;
            if (x >= WIDTH) {
                x = 0;
                clearY();
            }

            Serial.println(val);
        }

        /*
             Display data
        */
        drawY();

    } while (u8g.nextPage());
}
