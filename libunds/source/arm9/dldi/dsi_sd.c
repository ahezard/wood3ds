#include <nds/disc_io.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/system.h>
#include <nds/arm9/cache.h>
#include <nds/debug.h>

//---------------------------------------------------------------------------------
bool sdio_Startup() {
//---------------------------------------------------------------------------------
	nocashMessage("libnds/arm9/dldi/dsi_sd.c sdio_Startup\n");
	if (!REG_DSIMODE) return false;

	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMMC,SDMMC_HAVE_SD);\n");
	fifoSendValue32(FIFO_SDMMC,SDMMC_HAVE_SD);
	
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoCheckValue32(FIFO_SDMMC);\n");
	while(!fifoCheckValue32(FIFO_SDMMC)) {
		nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoCheckValue32(FIFO_SDMMC);\n");
	}
	nocashMessage("libnds/arm9/dldi/dsi_sd.c int result = fifoGetValue32(FIFO_SDMMC);\n");
	int result = fifoGetValue32(FIFO_SDMMC);

	if(result==0) return false;

	fifoSendValue32(FIFO_SDMMC,SDMMC_SD_START);

	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32(FIFO_SDMMC);\n");
	fifoWaitValue32(FIFO_SDMMC);

	result = fifoGetValue32(FIFO_SDMMC);
	
	return result == 0;
}

//---------------------------------------------------------------------------------
bool sdio_IsInserted() {
//---------------------------------------------------------------------------------
	nocashMessage("libnds/arm9/dldi/dsi_sd.c sdio_IsInserted\n");
	if (!REG_DSIMODE) return false;

	fifoSendValue32(FIFO_SDMMC,SDMMC_SD_IS_INSERTED);

	fifoWaitValue32(FIFO_SDMMC);

	int result = fifoGetValue32(FIFO_SDMMC);

	return result == 1;
}

//---------------------------------------------------------------------------------
bool sdio_ReadSectors(sec_t sector, sec_t numSectors,void* buffer) {
//---------------------------------------------------------------------------------
	nocashMessage("libnds/arm9/dldi/dsi_sd.c sdio_ReadSectors\n");
	char buf[64];
	siprintf(buf, "%X-%X-%X", sector, numSectors, buffer);
	nocashMessage(buf);
	
	if (!REG_DSIMODE) return false;

	DC_FlushRange(buffer,numSectors * 512);
	
	// switch to MSG mode
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);\n");
	fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);	
    nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32\n");
	fifoWaitValue32(FIFO_SDMMC);
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32\n");
	fifoGetValue32(FIFO_SDMMC);
	
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, SDMMC_SD_READ_SECTORS);\n");
	fifoSendValue32(FIFO_SDMSG, SDMMC_SD_READ_SECTORS);
	fifoWaitValue32(FIFO_SDMMC);
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, sector);\n");
	fifoSendValue32(FIFO_SDMSG, sector);
	fifoWaitValue32(FIFO_SDMMC);
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, numSectors);\n");
	fifoSendValue32(FIFO_SDMSG, numSectors);
	fifoWaitValue32(FIFO_SDMMC);
	nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoSendValue32(FIFO_SDMSG, buffer);\n");
	fifoSendValue32(FIFO_SDMSG, buffer);

    nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoWaitValue32\n");
	fifoWaitValue32(FIFO_SDMMC);

    nocashMessage("libnds/arm9/dldi/dsi_sd.c fifoGetValue32\n");
	int result = fifoGetValue32(FIFO_SDMMC);
	
	return result == 0;
}

//---------------------------------------------------------------------------------
bool sdio_WriteSectors(sec_t sector, sec_t numSectors,const void* buffer) {
//---------------------------------------------------------------------------------
	nocashMessage("libnds/arm9/dldi/dsi_sd.c sdio_WriteSectors\n");
	char buf[64];
	siprintf(buf, "%X-%X-%X", sector, numSectors, buffer);
	nocashMessage(buf);	
	
	if (!REG_DSIMODE) return false;
	FifoMessage msg;

	DC_FlushRange(buffer,numSectors * 512);
	
	// switch to MSG mode
	fifoSendValue32(FIFO_SDMMC, SDMMC_MSG);	
	fifoWaitValue32(FIFO_SDMMC);
	fifoGetValue32(FIFO_SDMMC);

	msg.type = SDMMC_SD_WRITE_SECTORS;
	msg.sdParams.startsector = sector;
	msg.sdParams.numsectors = numSectors;
	msg.sdParams.buffer = (void*)buffer;
	
	fifoSendValue32(FIFO_SDMSG, msg.type);
	fifoWaitValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, msg.sdParams.startsector);
	fifoWaitValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, msg.sdParams.numsectors);
	fifoWaitValue32(FIFO_SDMMC);
	fifoSendValue32(FIFO_SDMSG, msg.sdParams.buffer);

	fifoWaitValue32(FIFO_SDMMC);

	int result = fifoGetValue32(FIFO_SDMMC);
	
	return result == 0;
}


//---------------------------------------------------------------------------------
bool sdio_ClearStatus() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	return true;
}

//---------------------------------------------------------------------------------
bool sdio_Shutdown() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
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
