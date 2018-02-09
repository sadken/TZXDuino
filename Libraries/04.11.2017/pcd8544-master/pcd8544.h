#ifndef PCD8544_H
#define PCD8544_H

#include <stdint.h>
#include <Print.h>

#if ARDUINO >= 100
  #include <Arduino.h> // Arduino 1.0
  #define WRITE_RESULT size_t
  #define WRITE_RETURN return 1;
#else
  #include <WProgram.h> // Arduino 0022
  #define WRITE_RESULT void
  #define WRITE_RETURN
#endif


#ifndef __AVR-GCC__
#define MAPLE 1

#ifndef SPI_NUM
#define SPI_NUM 1
#endif
#endif

class pcd8544 : public Print {
public:
	// Constructor for harware SPI
	pcd8544(uint8_t dc_pin, uint8_t reset_pin, uint8_t cs_pin, uint8_t hardware_spi = 1);

	// Constructor for software SPI.
	pcd8544(uint8_t dc_pin, uint8_t reset_pin, uint8_t cs_pin, uint8_t sdin_pin, uint8_t sclk_pin);

	// Call this first
	void begin(void);

	// Clear lcd without changing location
	void clear(void);

	// Change current position to (character) column and row
	void setCursor(uint8_t column, uint8_t row);

	// Change current location to 0 <= row <= 5,
	// 0 <= pixel_column <= 83
	void gotoRc(uint8_t row, uint8_t pixel_column);

	// Send data to lcd. Will draw data as one pixel wide, 8 pixel high.
	// LSB up.
	void data(uint8_t data);

	// Small numbers. 0<= num <=9 for number and num = 10 for decimal
	// point. Optional parameter shift will move the numbers up/down.
	// shift shold be 0,1,2,3 for the digit to be visible.
	void smallNum(uint8_t num, uint8_t shift = 1);
	
	void clearRestOfLine(void);
	void bitmap(uint8_t *data, uint8_t rows, uint8_t columns);

private:
	void send(uint8_t dc, uint8_t data);
	void command(uint8_t data);
	virtual WRITE_RESULT write(uint8_t ch);
	void inc_row_column(void);
	uint8_t dc;
	uint8_t cs;
	uint8_t reset;
	uint8_t hardware_spi_num;
	uint8_t sdin;
	uint8_t sclk;
	uint8_t current_row, current_column;
};

#define PCD8544_LINES 6
#define PCD8544_COLS  14
#define PCD8544_WIDTH  84
#define PCD8544_HEIGHT 48


#endif
