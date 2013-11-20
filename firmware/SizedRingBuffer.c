/*! ************************************************************************** *
 * @file SizedRingBuffer.c
 * @brief Ring buffer library (byte-length)
 *
 * This library implement a byte-length ring buffer with user-defined max size.
 * The buffer memory must be previously allocated.
 *
 *
 * @author Benjamin BALGA
 * @version 1.2
 *
 * @changelog 17-11-2013 Total library re-write.
 *
 *  ************************************************************************** */

//#include <ctype.h>

#include "SizedRingBuffer.h"


/**
 * @brief Buffer initialization. Clear all fields
 * @sa xSRB_push()
 * @sa xSRB_pop()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer structure to initialize
 * @param pu8Buffer Pointer to the buffer (must be allocated)
 * @param u16Size Buffer size
 * @return Error if invalid pointer, Ok otherwise.
 */
xBufferState xSRB_initBuffer(xSizedRingBuffer * const xSRBuf, uint8_t *pu8Buffer, const uint16_t u16Size)
{
    if(!xSRBuf) return Error;
    if(!pu8Buffer) return Error;

    xSRBuf->u8BufferIn = pu8Buffer;

    xSRBuf->u16StartIndex = 0;
    xSRBuf->u16EndIndex = 0;
    xSRBuf->u16Count = 0;
    xSRBuf->u16MaxSize = u16Size;

    return Ok;
}


/*! ************************************************************************** *
 * @brief Number of bytes stored in the buffer.
 * @sa bSRB_isEmpty()
 * @sa bSRB_isFull()
 *
 * @param xSRBuf Pointer to the buffer
 *  ************************************************************************** */
uint16_t u16SRB_count(xSizedRingBuffer const *xSRBuf)
{
    return xSRBuf->u16Count;
}


/*! ************************************************************************** *
 * @brief Check if buffer is empty
 * @sa bSRB_isFull()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer
 * @return true if empty, false otherwise
 *  ************************************************************************** */
bool bSRB_isEmpty(xSizedRingBuffer const *xSRBuf)
{
    return (bool)(xSRBuf->u16Count == 0);
}

/*! ************************************************************************** *
 * @brief Check if buffer is full
 * @sa bSRB_isEmpty()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer
 * @return true if full, false otherwise
 *  ************************************************************************** */
bool bSRB_isFull(xSizedRingBuffer const *xSRBuf)
{
    return (bool)(xSRBuf->u16Count == xSRBuf->u16MaxSize);
}



/*! ************************************************************************** *
 * @brief Push byte to buffer.
 * @sa xSRB_pop()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer to use
 * @param u8Value byte to push in buffer
 * @return Buffer state
 *  ************************************************************************** */
xBufferState xSRB_push(xSizedRingBuffer * const xSRBuf, const uint8_t u8Value)
{
    // Buffer is full, this byte would overflow the buffer: ignore and return
    if (bSRB_isFull(xSRBuf))
        return Overflow;

    xSRBuf->u8BufferIn[xSRBuf->u16EndIndex++] = u8Value; // Add value
    
    ++xSRBuf->u16Count;  // Increment count value
    
    if (xSRBuf->u16EndIndex >= xSRBuf->u16MaxSize) // Ring buffer, back to zero if max size reached
        xSRBuf->u16EndIndex = 0;

    // Buffer is now full
    if (bSRB_isFull(xSRBuf))
        return Full;

    return Ok;
}


/*! ************************************************************************** *
 * @brief Get byte from buffer at @a u16Index. Data is not poped.
 * @sa xSRB_push()
 * @sa xSRB_pop()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer to use
 * @param u16Index Index of data to read (0 =  FIFO tail (pop), count = FIFO head (last push))
 * @param pu8Value pointer where the byte read will be stored. Must not be null.
 * @return Buffer state
 *  ************************************************************************** */
xBufferState xSRB_get(xSizedRingBuffer * const xSRBuf, uint16_t u16Index, uint8_t * const pu8Value)
{
    if (!pu8Value) return Error;

    if (bSRB_isEmpty(xSRBuf))
        return Empty;

    if (u16Index+1 > xSRBuf->u16Count)
        return Error;

    *pu8Value = xSRBuf->u8BufferIn[(xSRBuf->u16StartIndex + u16Index) % xSRBuf->u16MaxSize];

    return Ok;
}

/*! ************************************************************************** *
 * @brief Get first byte from buffer. Data is not poped.
 * @sa xSRB_push()
 * @sa xSRB_pop()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer to use
 * @param pu8Value pointer where the byte read will be stored. Must not be null.
 * @return Buffer state
 *  ************************************************************************** */
xBufferState xSRB_peek(xSizedRingBuffer * const xSRBuf, uint8_t * const pu8Value)
{
    if (!pu8Value) return Error;

    if (bSRB_isEmpty(xSRBuf))
        return Empty;

    *pu8Value = xSRBuf->u8BufferIn[xSRBuf->u16StartIndex];

    return Ok;
}

/*! ************************************************************************** *
 * @brief Pop byte from buffer.
 * @sa xSRB_push()
 * @sa u16SRB_count()
 *
 * @param xSRBuf Pointer to the buffer to use
 * @param pu8Value pointer where the byte read will be stored. Must not be null.
 * @return Buffer state
 *  ************************************************************************** */
xBufferState xSRB_pop(xSizedRingBuffer * const xSRBuf, uint8_t * const pu8Value)
{
    if (!pu8Value) return Error;

    if (bSRB_isEmpty(xSRBuf))
        return Empty;

    *pu8Value = xSRBuf->u8BufferIn[xSRBuf->u16StartIndex];    // Read buffer

    //xSRBuf->u8BufferIn[xSRBuf->u16StartIndex] = 0;               // Reset buffer elt
    ++xSRBuf->u16StartIndex; // Increment start index
    --xSRBuf->u16Count;  // Decrement count value

    if (xSRBuf->u16StartIndex >= xSRBuf->u16MaxSize) // Ring buffer, back to zero if max size reached
        xSRBuf->u16StartIndex = 0;

    // If buffer is now empty
    if (bSRB_isEmpty(xSRBuf))
        return NowEmpty;

    return Ok;
}

/*! ************************************************************************** *
 * @brief Reset buffer. Data is not cleared.
 * @sa vSRB_clear()
 *
 * @param xSRBuf Pointer to the buffer to reset
 *  ************************************************************************** */
void vSRB_reset(xSizedRingBuffer * const xSRBuf)
{
    xSRBuf->u16StartIndex = 0;
    xSRBuf->u16EndIndex = 0;
    xSRBuf->u16Count = 0;
}

/*! ************************************************************************** *
 * @brief Clear buffer. Data is cleared with zeros and buffer is reset.
 * @sa vSRB_reset()
 *
 * @param xSRBuf Pointer to the buffer to clear
 *  ************************************************************************** */
void vSRB_clear(xSizedRingBuffer * const xSRBuf)
{
    uint_fast16_t i;
    
    vSRB_reset(xSRBuf);

    for (i = xSRBuf->u16MaxSize; i; --i)
        xSRBuf->u8BufferIn[i-1] = 0;
}


/*! ************************************************************************** *
 * @brief Lost n bytes
 * @sa vSRB_clear()
 * @sa vSRB_reset()
 *
 * @param xSRBuf Pointer to the buffer to use
 * @param u16no Number of bytes to lost
 *  ************************************************************************** */
void vSRB_lost(xSizedRingBuffer * const xSRBuf, uint16_t u16no)
{
    // Buffer empty ?
    if (bSRB_isEmpty(xSRBuf)) return;

    if (u16no >= xSRBuf->u16Count) {
        xSRBuf->u16StartIndex = xSRBuf->u16EndIndex;
        xSRBuf->u16Count = 0;
    } else {
        xSRBuf->u16StartIndex = (xSRBuf->u16StartIndex + u16no) % xSRBuf->u16MaxSize;
        xSRBuf->u16Count -= u16no;
    }
}



