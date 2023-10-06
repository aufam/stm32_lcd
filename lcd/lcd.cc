#include "lcd/lcd.h"
#include "cmsis_os2.h"
#include "etl/array.h"
#include "etl/keywords.h"

using namespace Project;

val static special1 = etl::array<uint8_t> (
        0b00000,
        0b11001,
        0b11011,
        0b00110,
        0b01100,
        0b11011,
        0b10011,
        0b00000
);

val static special2 = etl::array<uint8_t> (
        0b11000,
        0b11000,
        0b00110,
        0b01001,
        0b01000,
        0b01001,
        0b00110,
        0b00000
);

fun static delayUsInit() {
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
	CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;

	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; 
	DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; 

	DWT->CYCCNT = 0;

	/* 3 NO OPERATION instructions */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
}

fun static delayUs(uint32_t us) {
	val cycles = (SystemCoreClock / 1'000'000u) * us;
	val start = DWT->CYCCNT;

	uint32_t volatile cnt;
	do cnt = DWT->CYCCNT - start; while(cnt < cycles);
}

fun LCD::init() -> void {
	_Backlight = BACKLIGHT;
	_Function = F_4BITMODE | F_1LINE | F_5x8DOTS | (_Row > 1 ? F_2LINE : F_5x10DOTS);
	delayUsInit();

	// initialization
	HAL_Delay(50);
	expanderWrite(_Backlight);
	HAL_Delay(1000);

	// 4 bit mode
	write4Bits(0x03 << 4);
	delayUs(4500);

	write4Bits(0x03 << 4);
	delayUs(4500);

	write4Bits(0x03 << 4);
	delayUs(4500);

	write4Bits(0x02 << 4);
	delayUs(100);

	// display control
	sendCommand(FUNCTIONSET | _Function);

	_Control = DISPLAYON | CURSOROFF | BLINKOFF;
	displayOn();
	clear();

	// display mode
	_Mode = ENTRYLEFT | ENTRYSHIFTDECREMENT;
	sendCommand(ENTRYMODESET | _Mode);
	delayUs(4500);

	createSpecialChar(0, special1.buf);
	createSpecialChar(1, special2.buf);

	home();
}

fun LCD::clear() -> void {
	sendCommand(CLEARDISPLAY);
	delayUs(2000);
}

fun LCD::home() -> void {
	sendCommand(RETURNHOME);
	delayUs(2000);
}

fun LCD::setCursor(uint8_t col, uint8_t row) -> void {
	val row_offsets = etl::array<uint8_t>(0x00, 0x40, 0x14, 0x54);
	col = etl::clamp(col, 0, _Column - 1);
	row = etl::clamp(row, 0, _Row - 1);
	sendCommand(SETDDRAMADDR | (col + row_offsets[row]));
}

fun LCD::displayOff() -> void {
	_Control &= ~DISPLAYON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::displayOn() -> void {
	_Control |= DISPLAYON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::blinkOff() -> void {
	_Control &= ~BLINKON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::blinkOn() -> void {
	_Control |= BLINKON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::cursorOff() -> void {
	_Control &= ~CURSORON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::cursorOn()  -> void {
	_Control |= CURSORON;
	sendCommand(DISPLAYCONTROL | _Control);
}

fun LCD::backlightOff() -> void {
	_Backlight = NOBACKLIGHT;
	expanderWrite(0);
}

fun LCD::backlightOn() -> void {
	_Backlight = BACKLIGHT;
	expanderWrite(0);
}

fun LCD::autoScrollOff() -> void {
	_Mode &= ~ENTRYSHIFTINCREMENT;
	sendCommand(ENTRYMODESET | _Mode);
}

fun LCD::autoScrollOn() -> void {
	_Mode |= ENTRYSHIFTINCREMENT;
	sendCommand(ENTRYMODESET | _Mode);
}

fun LCD::scrollDisplayLeft() -> void {
	sendCommand(CURSORSHIFT | DISPLAYMOVE | MOVELEFT);
}

fun LCD::scrollDisplayRight() -> void {
	sendCommand(CURSORSHIFT | DISPLAYMOVE | MOVERIGHT);
}

fun LCD::leftToRight() -> void {
	_Mode |= ENTRYLEFT;
	sendCommand(ENTRYMODESET | _Mode);
}

fun LCD::rightToLeft() -> void {
	_Mode &= ~ENTRYLEFT;
	sendCommand(ENTRYMODESET | _Mode);
}

fun LCD::print(const char* text) -> void {
	printStr(text);
}

fun LCD::print(char ch) -> void {
	sendChar(ch);
}

fun LCD::createSpecialChar(uint8_t location, const uint8_t (&charmap)[8]) -> void {
	location &= 0x7;
	sendCommand(SETCGRAMADDR | (location << 3));
	for (val ch in charmap)
		sendChar(ch);
}

fun LCD::printStr(const char c[]) -> void {
	while(*c) sendChar(*c++);
}

fun LCD::sendCommand(uint8_t cmd) -> void {
	send(cmd, 0);
}

fun LCD::sendChar(uint8_t ch) -> void {
	send(ch, RS);
}

fun LCD::send(uint8_t value, uint8_t mode) -> void {
	val highnib = value & 0xF0;
	val lownib = (value << 4) & 0xF0;
	write4Bits(highnib | mode);
	write4Bits(lownib | mode);
}

fun LCD::write4Bits(uint8_t value) -> void {
	expanderWrite(value);
	pulseEnable(value);
}

fun LCD::expanderWrite(uint8_t data) -> void {
	data |= _Backlight;
	HAL_I2C_Master_Transmit(&i2c.hi2c, address, &data, 1, timeout);
}

fun LCD::pulseEnable(uint8_t data) -> void {
	expanderWrite(data | ENABLE);
	delayUs(20);

	expanderWrite(data & ~ENABLE);
	delayUs(20);
}
