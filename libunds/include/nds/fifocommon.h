#ifndef FIFOCOMMON_H
#define FIFOCOMMON_H

#include "ndstypes.h"

/*! \file fifocommon.h
	\brief low level FIFO API.
*/

//! Enum values for the different fifo channels.
typedef enum {
   FIFO_PM		= 0,	/*!< \brief fifo channel reserved for power management. */
   FIFO_INPUT 	= 1,	/*!< \brief fifo channel reserved for input functions. */
   FIFO_CLOCK 	= 2,	/*!< \brief fifo channel reserved for clock functions. */
   FIFO_USER_01	= 3,	/*!< \brief fifo channel available for users. */
   FIFO_USER_02	= 4,	/*!< \brief fifo channel available for users. */
   FIFO_USER_03	= 5,	/*!< \brief fifo channel available for users. */
   FIFO_USER_04	= 6,	/*!< \brief fifo channel available for users. */
   FIFO_USER_05	= 7,	/*!< \brief fifo channel available for users. */
   FIFO_USER_06	= 8,	/*!< \brief fifo channel available for users. */
   FIFO_USER_07	= 9,	/*!< \brief fifo channel available for users. */
   FIFO_USER_08	= 10,	/*!< \brief fifo channel available for users. */
   FIFO_USER_09	= 11,	/*!< \brief fifo channel available for users. */
   FIFO_USER_10	= 12,	/*!< \brief fifo channel available for users. */
   FIFO_USER_11	= 13,	/*!< \brief fifo channel available for users. */
   FIFO_USER_12	= 14,	/*!< \brief fifo channel available for users. */
   FIFO_USER_13	= 15 	/*!< \brief fifo channel available for users. */
} FifoChannels;

//! Enum values for the fifo power management commands.
typedef enum {
	PM_REQ_ON = (1<<16),
	PM_REQ_OFF  = (2<<16),
	PM_REQ_LED  = (3<<16),
	PM_REQ_SLEEP  = (4<<16)
}FifoPMCommands;

//! Power Management LED blink mode control bits.
typedef enum {
	PM_LED_ON		=(0<<4),	/*!< \brief  Steady on */
	PM_LED_SLEEP	=(1<<4),	/*!< \brief Blinking, mostly off */
	PM_LED_BLINK	=(3<<4),	/*!< \brief Blinking, mostly on */
}PM_LedBlinkMode;

#ifdef __cplusplus
extern "C" {
#endif

/*!
	\brief fifo callback function pointer with the sent value and the callback's user data.

	The handler is called when new data arrives.

	\note callback functions are called from interrupt level, but are well secured. not too much caution is necessary,
			but don't call alloc, free or printf from within them, just to be safe.
*/
typedef void (*FifoValue32HandlerFunc)(u32 value32, void * userdata);


/*!
	\brief Initializes the fifo system.

	Attempts to sync with the other CPU, if it fails, fifo services won't be provided.

	\note call irqInit() before calling this function.

	\return true if syncing worked, false if something went wrong.
*/
bool fifoInit();


/*!
	\brief Send a 32bit value.

	Transmits a 32bit value to the other CPU.

	\param channel channel number to send to
	\param value32 32bit value to send

	\return true if the value has been send, false if something went wrong.

	\note Transfer is more efficient if the top 8 bits are zero. So sending smaller values or bitmasks that don't include the top bits is preferred.

*/
bool fifoSendValue32(int channel, u32 value32);


/*!
	\brief Set user value32 message callback.

	Set a callback to receive incoming value32 messages on a specific channel.

	\param channel channel number to send to.
	\param newhandler a function pointer to the new handler function.
	\param userdata a pointer that will be passed on to the handler when it will be called.

	\return true if the handler has been set, false if something went wrong.

	\note Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
*/
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata);


/*!
	\brief checks if there is any values in the fifo queue.

	\param channel the channel to check.

	\return true if there is any values in the queue and if there isn't a value handler in place for the channel.
*/
bool fifoCheckValue32(int channel);


/*!
	\brief Get the first value32 in queue for a specific channel.

	\param channel the channel to check.

	\return the first value32 in queue, or 0 if there is no message.
*/
u32 fifoGetValue32(int channel);

// Some aspects of this configuration can be changed...

// FIFO_CHANNEL_BITS - number of bits used to specify the channel in a packet - default=4

#define FIFO_CHANNEL_BITS				4
#define FIFO_NUM_CHANNELS				(1<<FIFO_CHANNEL_BITS)
#define FIFO_CHANNEL_SHIFT				(32-FIFO_CHANNEL_BITS)
#define FIFO_CHANNEL_MASK				((1<<FIFO_CHANNEL_BITS)-1)
#define FIFO_VALUE32_MASK				((1<<FIFO_CHANNEL_SHIFT)-1)

// some helpers

#define FIFO_PACK_VALUE32(channel, value32) ( ((channel)<<FIFO_CHANNEL_SHIFT) | (((value32))&FIFO_VALUE32_MASK) )
#define FIFO_UNPACK_CHANNEL(dataword) ( ((dataword)>>FIFO_CHANNEL_SHIFT)&FIFO_CHANNEL_MASK )
#define FIFO_UNPACK_VALUE32_NOEXTRA(dataword) ((dataword)&FIFO_VALUE32_MASK)

#ifdef __cplusplus
};
#endif


#endif
