#include <nds/disc_io.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/system.h>
#include <nds/arm9/cache.h>
#include <nds/debug.h>
#include <nds/dma.h>
#include <nds.h>

//---------------------------------------------------------------------------------
bool sdio_Startup() {
//---------------------------------------------------------------------------------
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c sdio_Startup\n");
	#endif//DEBUG
	
	//if (!REG_DSIMODE) return false;
	
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMMC,SDMMC_HAVE_SD);\n");
	#endif//DEBUG
	
	fifoSendValue32(FIFO_SDMMC,SDMMC_HAVE_SD);
	
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c fifoCheckValue32(FIFO_SDMMC);\n");
	#endif//DEBUG
	
	while(!fifoCheckValue32(FIFO_SDMMC)) {
		printf("libnds/arm9/dldi/dsi_sd.c fifoCheckValue32(FIFO_SDMMC);\n");
	}
	
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c int result = fifoGetValue32(FIFO_SDMMC);\n");
	#endif//DEBUG
	
	int result = fifoGetValue32(FIFO_SDMMC);

	if(result==0) return false;

	fifoSendValue32(FIFO_SDMMC,SDMMC_SD_START);

	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32(FIFO_SDMMC);\n");
	#endif//DEBUG
	
	fifoWaitValue32(FIFO_SDMMC);

	result = fifoGetValue32(FIFO_SDMMC);
	
	return result == 0;
}

//---------------------------------------------------------------------------------
bool sdio_IsInserted() {
//---------------------------------------------------------------------------------
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c sdio_IsInserted\n");
	#endif//DEBUG
	
	//if (!REG_DSIMODE) return false;

	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMMC,SDMMC_SD_IS_INSERTED);\n");
	fifoSendValue32(FIFO_SDMMC,SDMMC_SD_IS_INSERTED);

	fifoWaitValue32(FIFO_SDMMC);

	int result = fifoGetValue32(FIFO_SDMMC);

	return true;
}

#define	BYTES_PER_READ		512
static	uint8_t sectorBuffer[BYTES_PER_READ] __attribute__((aligned(32)));

//---------------------------------------------------------------------------------
bool sdio_ReadSectors(sec_t sector, sec_t numSectors,void* buffer) {
//---------------------------------------------------------------------------------
	printf("libnds/arm9/dldi/dsi_sd.c sdio_ReadSectors\n");
	char buf[64];
	siprintf(buf, "%X-%X-%X", sector, numSectors, buffer);
	printf(buf);
	
	//if (!REG_DSIMODE) return false;

	//while(true){
	DC_FlushRange(buffer,numSectors * BYTES_PER_READ);
	
	// switch to MSG mode
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);\n");
	#endif//DEBUG
		
	fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);	
	
	#ifdef DEBUG
    printf("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32\n");
	#endif//DEBUG
	
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, SDMMC_SD_READ_SECTORS);\n");
	#endif//DEBUG
	
	fifoSendValue32(FIFO_SDMSG, SYS_SD_READ_SECTORS);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, sector);\n");
	fifoSendValue32(FIFO_SDMSG, sector);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);	
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, numSectors);\n");
	fifoSendValue32(FIFO_SDMSG, numSectors);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	printf("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, buffer);\n");
	fifoSendValue32(FIFO_SDMSG, buffer);

    printf("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32\n");
	int i;
	int j;

	
	fifoWaitValue32(FIFO_SDMMC);

    printf("libnds/arm9/dldi/dsi_sd.c fifoGetValue32\n");
	int* result = fifoGetValue32(FIFO_SDMMC);
	
	for(i = 0; i < 0x200/4; i+=1) {
		//fifoWaitValue32(FIFO_SDMMC);
		//if(fifoCheckValue32(FIFO_SDMMC)){
			int value = result[i];
 			// for(j = 0; j < 1000; j++){
			//printf("%x", value);
			//} 
			//i+=4;
		//}
	}
	
	//printf("%x", result);
	
	//DC_FlushRange(buffer,numSectors * BYTES_PER_READ);
	//dmaCopy(sectorBuffer, buffer, numSectors*BYTES_PER_READ);
	
	siprintf(buf, "libnds/arm9/dldi/dsi_sd.c sdio_ReadSectors Result : %X", result);
	printf(buf);
	
/* 	while(1){
		printf("%x\n", *(u32*)(0x0400490C));
	} */
	
	return true;
}

//---------------------------------------------------------------------------------
bool sdio_WriteSectors(sec_t sector, sec_t numSectors,const void* buffer) {
//---------------------------------------------------------------------------------
	#ifdef DEBUG
	printf("libnds/arm9/dldi/dsi_sd.c sdio_WriteSectors\n");
	char buf[64];
	siprintf(buf, "%X-%X-%X", sector, numSectors, buffer);
	printf(buf);	
	#endif//DEBUG
	
	//if (!REG_DSIMODE) return false;
	FifoMessage msg;

	DC_FlushRange(buffer,numSectors * 512);
	
	// switch to MSG mode
	fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);	
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);

	msg.type = SYS_SD_READ_SECTORS;
	msg.sdParams.startsector = sector;
	msg.sdParams.numsectors = numSectors;
	msg.sdParams.buffer = (void*)buffer;
	
	fifoSendValue32(FIFO_SDMSG, msg.type);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, msg.sdParams.startsector);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, msg.sdParams.numsectors);
	fifoWaitValue32(FIFO_SDMMC);fifoGetValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, (int)msg.sdParams.buffer);

	fifoWaitValue32(FIFO_SDMMC);

	int result = fifoGetValue32(FIFO_SDMMC);
	
	return result == 0;
}


//---------------------------------------------------------------------------------
bool sdio_ClearStatus() {
//---------------------------------------------------------------------------------
	//if (!REG_DSIMODE) return false;
	return true;
}

//---------------------------------------------------------------------------------
bool sdio_Shutdown() {
//---------------------------------------------------------------------------------
	//if (!REG_DSIMODE) return false;
	return true;
}

const DISC_INTERFACE __io_dsisd = {
	DEVICE_TYPE_DSI_SD,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE,
	(FN_MEDIUM_STARTUP)&sdio_Startup,
	(FN_MEDIUM_ISINSERTED)&sdio_IsInserted,
	(FN_MEDIUM_READSECTORS)&sdio_ReadSectors,
	(FN_MEDIUM_WRITESECTORS)&sdio_WriteSectors,
	(FN_MEDIUM_CLEARSTATUS)&sdio_ClearStatus,
	(FN_MEDIUM_SHUTDOWN)&sdio_Shutdown
};
