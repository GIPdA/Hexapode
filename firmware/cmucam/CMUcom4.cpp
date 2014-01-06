/***************************************************************************//**
* @file
* Portable serial and timer wrapper library.
*
* @version @n 1.0
* @date @n 8/3/2012
*
* @authors @n Kwabena W. Agyeman & Christopher J. Leaf
* @copyright @n (c) 2012 Kwabena W. Agyeman & Christopher J. Leaf
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v0.1 - Beta code - 3/20/2012
* @n v0.9 - Original release - 4/18/2012
* @n v1.0 - Documented and updated release - 8/3/2012
*******************************************************************************/

#include "CMUcom4.h"
#include "uart_17xx_40xx.h"

/*******************************************************************************
* Constructor Functions
*******************************************************************************/

CMUcom4::CMUcom4()
{
}

CMUcom4::CMUcom4(int port)
{
    _port = port;
}

/*******************************************************************************
* Public Functions
*******************************************************************************/

void CMUcom4::begin(unsigned long baud)
{
    // Setup UART for 115.2K8N1
    Chip_UART_Init(CMUCOM4_SERIAL);
	Chip_UART_SetBaud(CMUCOM4_SERIAL, baud);
	Chip_UART_ConfigData(CMUCOM4_SERIAL, UART_DATABIT_8, UART_PARITY_NONE, UART_STOPBIT_1);

    // Enable UART Transmit
	Chip_UART_TxCmd(CMUCOM4_SERIAL, ENABLE);
    
    //delayMilliseconds(CMUCOM4_BEGIN_DELAY);
}

void CMUcom4::end()
{
    Chip_UART_TxCmd(CMUCOM4_SERIAL, DISABLE);
    Chip_UART_DeInit(CMUCOM4_SERIAL);
}

int CMUcom4::read()
{
    uint8_t data;

	if (Chip_UART_ReceiveByte(CMUCOM4_SERIAL, &data) == SUCCESS) {
		return (int) data;
	}
    
    return EOF;
}

size_t CMUcom4::write(uint8_t c)
{
    while (Chip_UART_SendByte(CMUCOM4_SERIAL, (uint8_t) c) == ERROR) {}
}

size_t CMUcom4::write(const char * str)
{
    while (*str != '\0') {
		write(*str++);
	}
}

size_t CMUcom4::write(const uint8_t * buffer, size_t size)
{
    return Chip_UART_Send(CMUCOM4_SERIAL, buffer, size, BLOCKING);
}

int CMUcom4::available()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    switch(_port)
    {
        case CMUCOM4_SERIAL1: return Serial1.available(); break;
        case CMUCOM4_SERIAL2: return Serial2.available(); break;
        case CMUCOM4_SERIAL3: return Serial3.available(); break;
        default: return Serial.available(); break;
    }
#else
    return Serial.available();
#endif
}

void CMUcom4::flush()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    switch(_port)
    {
        case CMUCOM4_SERIAL1: Serial1.flush(); break;
        case CMUCOM4_SERIAL2: Serial2.flush(); break;
        case CMUCOM4_SERIAL3: Serial3.flush(); break;
        default: Serial.flush(); break;
    }
#else
    Serial.flush();
#endif
}

int CMUcom4::peek()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    switch(_port)
    {
        case CMUCOM4_SERIAL1: return Serial1.peek(); break;
        case CMUCOM4_SERIAL2: return Serial2.peek(); break;
        case CMUCOM4_SERIAL3: return Serial3.peek(); break;
        default: return Serial.peek(); break;
    }
#else
    return Serial.peek();
#endif
}

void CMUcom4::delayMilliseconds(unsigned long ms)
{
    return delay(ms);
}

unsigned long CMUcom4::milliseconds()
{
    return millis();
}

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
