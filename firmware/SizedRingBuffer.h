/*! ************************************************************************** *
 * @file SizedRingBuffer.h
 * @brief Ring buffer library (byte-length)
 *
 * @author Benjamin BALGA
 * @version 1.2
 *
 *  ************************************************************************** */

#ifndef _SIZED_RING_BUFFER_H
#define _SIZED_RING_BUFFER_H

#include "stdint.h"
#include "lpc_types.h"



//------------------------------------------------------------------------------
//    Ring buffer structure
//------------------------------------------------------------------------------
/// @brief Sized ring buffer structure
typedef
struct _SizedRingBuffer
{
    uint8_t *u8BufferIn;      /// Buffer
    uint16_t u16StartIndex,    /// FIFO start = first readable element
             u16EndIndex;      /// FIFO end = last element + 1
    uint16_t u16MaxSize;       /// Max buffer size = allocated data size
    uint16_t u16Count;         /// Buffer size = number of elements stored
} xSizedRingBuffer;

//------------------------------------------------------------------------------
//    Ring buffer states
//------------------------------------------------------------------------------
/// @brief Ring buffer states
typedef
enum _BufferState { 
	Error = 0, 			// Returned in case of null pointer
	Ok = 1, 			// Buffer contains data but not empty or (almost) full
	Empty = 2, 			// Buffer is empty
	NowEmpty = 3, 		// The read emptied the buffer
	//AlmostFull = 4, 	// The buffer is almost full (limit defined with UART_0_BUFFER_WARNING_LIMIT)
	Full = -1, 			// Buffer full
	Overflow = -2 		// Buffer push overflowed (the byte is not written)
} xBufferState;


//------------------------------------------------------------------------------
//    Static initialisation macros
//------------------------------------------------------------------------------
/// @brief Macro to initalise xSizedRingBuffer structure at compile-time with buffer ptr and buffer size
#define RINGBUFFER_COMPILETIME_INIT(BUFFER_PTR,BUFFER_SIZE)	{BUFFER_PTR,0,0,BUFFER_SIZE,0}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//    Functions prototypes
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Initialisation (must be called before any other function)
xBufferState xSRB_initBuffer(xSizedRingBuffer * const xSRBuf, uint8_t *pu8Buffer, const uint16_t u16Size);

//---------------------------------------------------------------------------
// Buffer state
uint16_t u16SRB_count(xSizedRingBuffer const *xSRBuf);
bool bSRB_isEmpty(xSizedRingBuffer const *xSRBuf);
bool bSRB_isFull(xSizedRingBuffer const *xSRBuf);

//---------------------------------------------------------------------------
// Push / Pop / Clear
xBufferState xSRB_push(xSizedRingBuffer * const xSRBuf, const uint8_t u8Value);
xBufferState xSRB_pop(xSizedRingBuffer * const xSRBuf, uint8_t * const pu8Value);
xBufferState xSRB_peek(xSizedRingBuffer * const xSRBuf, uint8_t * const pu8Value);
xBufferState xSRB_get(xSizedRingBuffer * const xSRBuf, uint16_t u16Index, uint8_t * const pu8Value);

void vSRB_reset(xSizedRingBuffer * const xSRBuf);
void vSRB_clear(xSizedRingBuffer * const xSRBuf);
void vSRB_lost(xSizedRingBuffer * const xSRBuf, uint16_t u16no);

#endif  // _SIZED_RING_BUFFER_H
