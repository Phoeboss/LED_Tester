#include "MyDisplay.h"

// naming convention
// (clockwise from top)
//   A          16
// F   B     256   1
//   G          32
// E   C     512   2
//   D   H      64   4
//   I         1024
// segment bits in my specific display, where each position has 9 segments
// (so a digit/letter can be encoded in 12-bits in 2 bytes)
#define S_A 128
#define S_B 8
#define S_C 2
#define S_D 1
#define S_E 32
#define S_F 64
#define S_G 4
#define S_H 16
#define S_I 1024

// seven segment glyphs
// (which segment is active in a digit/letter)

// digits
//      G_x  S_A   S_B   S_C   S_D   S_E   S_F   S_G
#define G_0 (S_A + S_B + S_C + S_D + S_E + S_F + 0)
#define G_1 (0 + S_B + S_C + 0 + 0 + 0 + 0)
#define G_2 (S_A + S_B + 0 + S_D + S_E + 0 + S_G)
#define G_3 (S_A + S_B + S_C + S_D + 0 + 0 + S_G)
#define G_4 (0 + S_B + S_C + 0 + 0 + S_F + S_G)
#define G_5 (S_A + 0 + S_C + S_D + 0 + S_F + S_G)
#define G_6 (S_A + 0 + S_C + S_D + S_E + S_F + S_G)
#define G_7 (S_A + S_B + S_C + 0 + 0 + 0 + 0)
#define G_8 (S_A + S_B + S_C + S_D + S_E + S_F + S_G)
#define G_9 (S_A + S_B + S_C + S_D + 0 + S_F + S_G)

// special
#define G_space 0
#define G_minus S_G
#define G_no S_G
#define G_period S_H
#define G_underscore S_D
#define G_vertical (S_E + S_F)
#define G_equal (S_D + S_G)
#define G_square_bracket_open (S_A + S_D + S_E + S_F)
#define G_square_bracket_close (S_A + S_B + S_C + S_D)

// letters
//      G_x  S_A   S_B   S_C   S_D   S_E   S_F   S_G
#define G_A (S_A + S_B + S_C + 0 + S_E + S_F + S_G) // A
#define G_b (0 + 0 + S_C + S_D + S_E + S_F + S_G)   // b
#define G_c (0 + 0 + 0 + S_D + S_E + 0 + S_G)       // c
#define G_C (S_A + 0 + 0 + S_D + S_E + S_F + 0)     // C
#define G_d (0 + S_B + S_C + S_D + S_E + 0 + S_G)   // d
#define G_E (S_A + 0 + 0 + S_D + S_E + S_F + S_G)   // E
#define G_F (S_A + 0 + 0 + 0 + S_E + S_F + S_G)     // F
#define G_g (S_A + S_B + S_C + S_D + 0 + S_F + S_G) // g
#define G_G (S_A + 0 + S_C + S_D + S_E + S_F + 0)   // G
#define G_h (0 + 0 + S_C + 0 + S_E + S_F + S_G)     // h
#define G_H (0 + S_B + S_C + 0 + S_E + S_F + S_G)   // H
#define G_I (0 + S_B + S_C + 0 + 0 + 0 + 0)         // I
#define G_J (0 + S_B + S_C + S_D + 0 + 0 + 0)       // J
#define G_K G_no                                    // -
#define G_L (0 + 0 + 0 + S_D + S_E + S_F + 0)       // L
#define G_M G_no                                    // -
#define G_n (0 + 0 + S_C + 0 + S_E + 0 + S_G)       // n
#define G_o (0 + 0 + S_C + S_D + S_E + 0 + S_G)     // o
#define G_O (S_A + S_B + S_C + S_D + S_E + S_F + 0) // O
#define G_P (S_A + S_B + 0 + 0 + S_E + S_F + S_G)   // P
#define G_q (S_A + S_B + S_C + 0 + 0 + S_F + S_G)   // q
#define G_r (0 + 0 + 0 + 0 + S_E + 0 + S_G)         // r
#define G_S (S_A + 0 + S_C + S_D + 0 + S_F + S_G)   // S
#define G_t (0 + 0 + 0 + S_D + S_E + S_F + S_G)     // t
//#define G_u ( 0  +  0  + S_C + S_D + S_E +  0  +  0 ) // u, same as v
#define G_U (0 + S_B + S_C + S_D + S_E + S_F + 0) // U
#define G_v (0 + 0 + S_C + S_D + S_E + 0 + 0)     // v
#define G_W G_no                                  // -
#define G_X G_no                                  // -
#define G_y (0 + S_B + S_C + S_D + 0 + S_F + S_G) // y
#define G_Z (S_A + S_B + 0 + S_D + S_E + 0 + S_G) // Z

const uint8_t MyDisplay::Ascii[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, // special
    0, 0, 0, 0, 0, 0, 0, 0, // special
    0, 0, 0, 0, 0, 0, 0, 0, // special
    0, 0, 0, 0, 0, 0, 0, 0, // special
    G_space, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, G_minus, G_period, 0,
    G_0, G_1, G_2, G_3, G_4, G_5, G_6, G_7,
    G_8, G_9, 0, 0, 0, G_equal, 0, 0,
    0, G_A, G_b, G_C, G_d, G_E, G_F, G_G, // upper case letters
    G_H, G_I, G_J, G_K, G_L, G_M, G_n, G_O,
    G_P, G_q, G_r, G_S, G_t, G_U, G_v, G_W,
    G_X, G_y, G_Z, G_square_bracket_open, 0, G_square_bracket_close, 0, G_underscore,
    0, G_A, G_b, G_c, G_d, G_E, G_F, G_g, // lower case letters
    G_h, G_I, G_J, G_K, G_L, G_M, G_n, G_o,
    G_P, G_q, G_r, G_S, G_t, G_U, G_v, G_W,
    G_X, G_y, G_Z, 0, G_vertical, 0, 0, 0};



void MyDisplay::begin()
{
    _ht.begin();
    _ht.sendCommand(HT1621::RC256K);
    _ht.sendCommand(HT1621::BIAS_THIRD_4_COM);
    _ht.sendCommand(HT1621::SYS_EN);
    _ht.sendCommand(HT1621::LCD_ON);
}

void MyDisplay::clear()
{
    for (uint8_t i = 0; i < _ht.MAX_ADDR; i++)
        _ht.write(i, 0);

    memset(_buffer, 0, sizeof(_buffer));
}

// pos is calculated from left to right
void MyDisplay::write(uint8_t pos, uint8_t symbol)
{
    _buffer[pos] = symbol;
    uint16_t data = _convert(symbol);
    // each address points 4 bits
    // so each char position takes 3*4 bits
    uint8_t address = 6 + 2 * (pos);
    _ht.write(address, data, 10);
}
// pos is calculated from left to right
void MyDisplay::write(uint8_t pos, uint8_t symbol, uint8_t period)
{
    _buffer[pos] = symbol;
    uint16_t data = _convert(symbol);
    data += period;
    // each address points 4 bits
    // so each char position takes 3*4 bits
    uint8_t address = 6 + 2 * (pos);
    _ht.write(address, data, 10);
}

uint8_t MyDisplay::read(uint8_t pos)
{
    return _buffer[pos];
}

uint8_t MyDisplay::_convert(uint8_t symbol)
{
    return Ascii[symbol];
}

void MyDisplay::print(uint16_t number)
{
    char buffer[4];
    sprintf(buffer, "%4d", number);
    for (uint8_t i = 0; i < 4; i++)
    {
        write(3-i, buffer[i]);
    }
}

void MyDisplay::print(float number)
{
    uint16_t tmp;
    uint8_t decimal=0;
    char buffer[4];
    if(number < 10){
        tmp = (uint16_t)(number*1000);
        decimal=1;
    }
    else {
        tmp = (uint16_t)(number*100);
        decimal=2;
    }
    sprintf(buffer, "%4d", tmp);
    for (uint8_t i = 0; i < 4; i++)
    {
        if(i != decimal)
            write(3-i, buffer[i]);
        else
            write(3-i, buffer[i], G_period);
    }
}

void MyDisplay::print(char *text)
{
    char buffer[4];
    sprintf(buffer, "%s", text);
    for (uint8_t i = 0; i < 4; i++)
    {
        write(3 - i, buffer[i]);
    }
}

void MyDisplay::symbol(uint8_t symbol){
    uint8_t address = symbol/4;
    uint8_t data = 1<<(symbol%4);
    _ht.write(address, data, 4);
}


void MyDisplay::beep(uint16_t ms, HT1621::Commands tone){
  _ht.sendCommand(tone);
  _ht.sendCommand(HT1621::TONE_ON);
  delay(ms);
  _ht.sendCommand(HT1621::TONE_OFF);
}

