#include <nds/fifocommon.h>
#include <nds/ipc.h>
#include <nds/interrupts.h>
#include <nds/bios.h>

#include <string.h>

// Some aspects of this configuration can be changed...

// FIFO_BUFFER_ENTRIES - number of words that can be stored temporarily while waiting to deque them - default=256 arm7, 512 arm9
#define FIFO_BUFFER_ENTRIES				64

// Note about memory commitments:
// The memory overhead of this library (per CPU) is:
// 12 + (Num Channels)*12 + FIFO_BUFFER_ENTRIES*6
// for 16 channels and 64 entries, this is 16+192+384 = 592 bytes of ram.
// Some padding may be added by the compiler, though.

// Common interface: Both the arm7 and arm9 have the same set of functions.
// for sending/receiving, true=success - for checking things, true=exists

#define FIFO_BUFFER_TERMINATE	0xFFFF

// fifo buffer helpers

#define FIFO_BUFFER_DATA(index) (fifo_buffer_data[index])

#define FIFO_BUFFER_GETNEXT(index) (fifo_buffer_next[index])

#define FIFO_BUFFER_SETCONTROL(index,next) \
	fifo_buffer_next[index] = next

#define FIFO_BUFFER_SETNEXT(index, next) \
	fifo_buffer_next[index] = next


// FIFO_BUFFER_ENTRIES*8 of global buffer space
vu32	fifo_buffer_data[FIFO_BUFFER_ENTRIES];
vu16	fifo_buffer_next[FIFO_BUFFER_ENTRIES];

typedef struct fifo_queue {
	vu16 head;
	vu16 tail;
} fifo_queue;
	

fifo_queue fifo_buffer_free = {0,FIFO_BUFFER_ENTRIES-1};
fifo_queue fifo_receive_queue = { FIFO_BUFFER_TERMINATE, FIFO_BUFFER_TERMINATE};

vu32	fifo_freewords = FIFO_BUFFER_ENTRIES;

fifo_queue					fifo_value32_queue[FIFO_NUM_CHANNELS];
FifoValue32HandlerFunc		fifo_value32_func[FIFO_NUM_CHANNELS];
void *						fifo_value32_data[FIFO_NUM_CHANNELS];


// fifoSetValue32Handler - Set a callback to receive incoming value32 messages on a specific channel.
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;

	int oldIME = enterCriticalSection();

	fifo_value32_func[channel] = newhandler;
	fifo_value32_data[channel] = userdata;

	if(newhandler) {
		while(fifoCheckValue32(channel)) {
			newhandler( fifoGetValue32(channel) , userdata );
		}
	}


	leaveCriticalSection(oldIME);

	return true;
}

static u32 fifo_allocBlock() {
	if (fifo_freewords == 0) return FIFO_BUFFER_TERMINATE;
	u32 entry = fifo_buffer_free.head;
	fifo_buffer_free.head = FIFO_BUFFER_GETNEXT(fifo_buffer_free.head);
	FIFO_BUFFER_SETCONTROL( entry, FIFO_BUFFER_TERMINATE );
	fifo_freewords--;
	return entry;
}

static void fifo_freeBlock(u32 index) {
	FIFO_BUFFER_SETCONTROL( index, FIFO_BUFFER_TERMINATE );
	FIFO_BUFFER_SETCONTROL( fifo_buffer_free.tail, index );
	fifo_buffer_free.tail = index;
	fifo_freewords++;
}

static bool fifoInternalSend(u32 firstword) {
	bool result = false;
	int oldIME = enterCriticalSection();

	if ( !(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL) ) {

		REG_IPC_FIFO_TX = firstword;
		result = true;
	}

	leaveCriticalSection(oldIME);
	return result;
}

bool fifoSendValue32(int channel, u32 value32) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	while(true)
	{
		if(fifoInternalSend(FIFO_PACK_VALUE32(channel, value32))) break;
		swiDelay(1000);
	}
	return true;
}

u32 fifoGetValue32(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return 0;
	int block = fifo_value32_queue[channel].head;
	if ( block == FIFO_BUFFER_TERMINATE) return 0;

	int oldIME = enterCriticalSection();
	u32 value32 = FIFO_BUFFER_DATA(block);
	fifo_value32_queue[channel].head = FIFO_BUFFER_GETNEXT(block);
	fifo_freeBlock(block);
	leaveCriticalSection(oldIME);
	return value32;
}

bool fifoCheckValue32(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	return fifo_value32_queue[channel].head != FIFO_BUFFER_TERMINATE;
}

u32 fifoValue32Mask(void)
{
	return FIFO_VALUE32_MASK;
}
u32 fifoPackValue32(int channel, u32 value32)
{
	return FIFO_PACK_VALUE32(channel, value32);
}

static void fifo_queueBlock(fifo_queue *queue, int block) {
	FIFO_BUFFER_SETNEXT(block,FIFO_BUFFER_TERMINATE);
	if(queue->head == FIFO_BUFFER_TERMINATE ) {
		queue->head = block;
		queue->tail = block;
	} else {
		FIFO_BUFFER_SETNEXT(queue->tail,block);
		queue->tail = block;
	}

}

int processing=0;

static void fifoInternalRecvInterrupt() {
	REG_IE &= ~IRQ_FIFO_NOT_EMPTY;
	REG_IME=1;

	u32 data, block=FIFO_BUFFER_TERMINATE;

	while( !(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) ) {
		
		REG_IME=0;
		block=fifo_allocBlock();
		if (block != FIFO_BUFFER_TERMINATE ) {
			FIFO_BUFFER_DATA(block)=REG_IPC_FIFO_RX;
			fifo_queueBlock(&fifo_receive_queue,block);
		}

		REG_IME=1;

	}

	REG_IME=0;
	REG_IE |= IRQ_FIFO_NOT_EMPTY;


	if (!processing && fifo_receive_queue.head != FIFO_BUFFER_TERMINATE) {

		processing = 1;
		REG_IME=1;
		
		do {
		
			block = fifo_receive_queue.head;
			data = FIFO_BUFFER_DATA(block);

			u32 channel = FIFO_UNPACK_CHANNEL(data);

			u32 value32;

			value32 = FIFO_UNPACK_VALUE32_NOEXTRA(data);
			REG_IME=0;
			fifo_receive_queue.head = FIFO_BUFFER_GETNEXT(block);
			if (fifo_value32_func[channel]) {
				fifo_freeBlock(block);
				fifo_value32_func[channel]( value32, fifo_value32_data[channel] );
			} else {
				FIFO_BUFFER_DATA(block)=value32;
				fifo_queueBlock(&fifo_value32_queue[channel],block);
			}
			REG_IME=1;

		} while( fifo_receive_queue.head != FIFO_BUFFER_TERMINATE);
		
		REG_IME = 0;
		processing = 0;
	}
}

static volatile int __timeout = 0;

static void __timeoutvbl() {
	__timeout++;
}
bool fifoInit() {


	int i;

	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_EMPTY | IPC_FIFO_SEND_EMPTY;

	for(i=0;i<FIFO_NUM_CHANNELS;i++) {

		fifo_value32_queue[i].head = FIFO_BUFFER_TERMINATE;
		fifo_value32_queue[i].tail = FIFO_BUFFER_TERMINATE;

		fifo_value32_data[i] = 0;
		fifo_value32_func[i] = 0;
	}

	for(i=0;i<FIFO_BUFFER_ENTRIES-1;i++) {
		FIFO_BUFFER_DATA(i) = 0;
		FIFO_BUFFER_SETCONTROL(i, i+1);
	}
	FIFO_BUFFER_SETCONTROL(FIFO_BUFFER_ENTRIES-1, FIFO_BUFFER_TERMINATE);

	#define __SYNC_START	0
	#define __SYNC_END		14
	#define __TIMEOUT		120

	int their_value=0,my_value=__SYNC_START,count=0;

	irqSet(IRQ_VBLANK,__timeoutvbl);
	irqEnable(IRQ_IPC_SYNC|IRQ_VBLANK);

	do {
		their_value = IPC_GetSync();
		if(their_value == __SYNC_END && count > 2) break;
		if(count>3) break;
		if( ((my_value + 1)&7 ) == their_value ) count++;
		my_value=(their_value+1)&7;
		IPC_SendSync(my_value);
		swiIntrWait(1,IRQ_IPC_SYNC|IRQ_VBLANK);
	} while (__timeout < __TIMEOUT);

	irqDisable(IRQ_IPC_SYNC);
	irqSet(IRQ_VBLANK,0);
	
	if (__timeout>= __TIMEOUT) {
		IPC_SendSync(0);
		return false;
	}

	IPC_SendSync(__SYNC_END);
	irqSet(IRQ_FIFO_NOT_EMPTY,fifoInternalRecvInterrupt);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
	irqEnable(IRQ_FIFO_NOT_EMPTY);

	return true;
}
