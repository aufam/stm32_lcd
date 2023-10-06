#ifndef PROJECT_LCD_H
#define PROJECT_LCD_H

#include "periph/i2c.h"

namespace Project {

    class LCD {
        inline static constexpr uint32_t timeout = 10;

        uint8_t _Function = 0;
        uint8_t _Control = 0;
        uint8_t _Mode = 0;
        uint8_t _Backlight = 0;
        uint8_t _Column = 0;
        uint8_t _Row = 0;

    public:
        periph::I2C& i2c;
        uint8_t address;

        /// default constructor
        /// @param i2c reference to periph::i2c 
        /// @param column number of column, default = 16
        /// @param row number of row, default = 2
        /// @param address device address, default = 0x4E
        constexpr explicit LCD(periph::I2C& i2c, uint8_t column = 16, uint8_t row = 2, uint8_t address = 0x4E) 
        : _Column(column)
        , _Row(row)
        , i2c(i2c)
        , address(address) {}

        /// set the LCD display in the correct begin state, must be called before anything else is done
        void init();

        /// clear display, next write operation will start from the first position on LCD display
        void clear();

        /// next write operation will start from the first position on LCD display
        void home();

        /// set cursor position
        /// @param column column position, max value is specified column number - 1
        /// @param row row position, max value is specified row number - 1
        void setCursor(uint8_t column, uint8_t row);

        /// do not show any characters on the LCD display, backlight state will remain unchanged,
	    /// also all characters written on the display will return when the display is enabled again
        void displayOff();

        /// show the characters on the LCD display 
        /// @note This method should only be used after displayOff() has been used
        void displayOn();

        /// do not blink the cursor indicator
        void blinkOff();

        /// do blink the cursor indicator
        void blinkOn();

        /// do not show the cursor indicator
        void cursorOff();

        /// do show the cursor indicator
        void cursorOn();

        /// turn off backlight
        void backlightOff();

        /// turn on backlight
        void backlightOn();

        /// turn off auto scroll
        void autoScrollOff();
        
        /// turn on auto scroll 
        void autoScrollOn();

        /// scroll display left to right
        void scrollDisplayLeft();

        /// scroll display right to left
        void scrollDisplayRight();

        void leftToRight();
        void rightToLeft();

        void print(const char* text);
        void print(char ch);

        LCD& operator<<(const char* str) { print(str); return *this; }
        LCD& operator<<(char ch) { print(ch); return *this; }

    private:
        void createSpecialChar(uint8_t location, const uint8_t (&charmap)[8]);
        void printStr(const char[]);

        void sendCommand(uint8_t cmd);
        void sendChar(uint8_t ch);
        void send(uint8_t value, uint8_t mode);
        void write4Bits(uint8_t value);
        void expanderWrite(uint8_t data);
        void pulseEnable(uint8_t data);

        enum {
            CLEARDISPLAY = 0x01,
            RETURNHOME = 0x02,
            ENTRYMODESET = 0x04,
            DISPLAYCONTROL = 0x08,
            CURSORSHIFT = 0x10,
            FUNCTIONSET = 0x20,
            SETCGRAMADDR = 0x40,
            SETDDRAMADDR = 0x80,

            /* Entry Mode */
            ENTRYRIGHT = 0x00,
            ENTRYLEFT = 0x02,
            ENTRYSHIFTINCREMENT = 0x01,
            ENTRYSHIFTDECREMENT = 0x00,

            /* Display On/Off */
            DISPLAYON = 0x04,
            DISPLAYOFF = 0x00,
            CURSORON = 0x02,
            CURSOROFF = 0x00,
            BLINKON = 0x01,
            BLINKOFF = 0x00,

            /* Cursor Shift */
            DISPLAYMOVE = 0x08,
            CURSORMOVE = 0x00,
            MOVERIGHT = 0x04,
            MOVELEFT = 0x00,

            /* Function Set */
            F_8BITMODE = 0x10,
            F_4BITMODE = 0x00,
            F_2LINE = 0x08,
            F_1LINE = 0x00,
            F_5x10DOTS = 0x04,
            F_5x8DOTS = 0x00,

            /* Backlight */
            BACKLIGHT = 0x08,
            NOBACKLIGHT = 0x00,

            /* Enable Bit */
            ENABLE = 0x04,

            /* Read Write Bit */
            RW = 0x0,

            /* Register Select Bit */
            RS = 0x01,
        };

    };
}

#endif