#ifndef _MY_DISPLAY_H_INCLUDED_
#define _MY_DISPLAY_H_INCLUDED_

#include "ILcdDisplay.h"
#include "HT1621.h"
#include "stdlib.h"

/**
 * ILcdDisplay Interface implementation for LCD based on the HT1621 chipset.
 * The LCD is supposed to communicate with the HT1621 through a 3-wire serial protocol.
 * The display is initialized with 256KHz internal RC clock frequency, 1/3 Bias and 3 Commons.
 * Refer to the manual of class HT1621 on how to change these settings.
*/
class MyDisplay : public ILcdDisplay {
public:
    /// 3-Wire protocol will be used for the communication.
    MyDisplay(uint8_t ss, uint8_t rw, uint8_t data) : _ht(ss, rw, data) {}

    /// Initialize the LCD with 256KHz internal RC oscillator, 1/3 Bias and 4 COM.
    virtual void begin();

    /// clear display
    virtual void clear();

    /// Turn off the display.
    virtual inline void noDisplay() { _ht.sendCommand(HT1621::LCD_OFF); };

    /// Turn on the display.
    virtual inline void display() { _ht.sendCommand(HT1621::LCD_ON); };

    virtual void write(uint8_t pos, uint8_t symbol);
    virtual void write(uint8_t pos, uint8_t symbol, uint8_t period);

    virtual uint8_t read(uint8_t pos);

    virtual void print(uint16_t number);
    virtual void print(float number);
    virtual void print(char * text);

    virtual void symbol(uint8_t symbol);

    virtual void beep(uint16_t ms, HT1621::Commands tone);

private:
    uint8_t _convert(uint8_t symbol);

    static const uint8_t Ascii[128];
    static const uint8_t MaxDigits = 8; ///< Available number of digits in my LCD
    uint8_t _buffer[MaxDigits]; ///< Buffer to store displayed symbols. Used in read method.
    HT1621 _ht;
};

// special M118 symbols
#define S_STOPWATCH     0
#define S_MIN           1
#define S_FLASHLIGHT    2
#define S_MAX           3

#define S_FAHRENHEIT    4
#define S_CELSIUS       5
#define S_hFE           6
#define S_NCV           7

#define S_AMPERE        8
#define S_VOLT          9
#define S_HZ            10
#define S_BATTERY       11

#define S_FARAD         12
#define S_OHM           13
#define S_TRIANGLE      14
#define S_HOLD          15

#define S_MILLI         16
#define S_PERCENT       17
#define S_MEGA          18
#define S_SPEAKER       19

#define S_MICRO         20
#define S_NANO          21
#define S_KILO          22
#define S_DIODE         23

#define S_ACTRMS        56
#define S_DC            57
#define S_AUTO          58

#endif // _MY_DISPLAY_H_INCLUDED_