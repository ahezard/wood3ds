#include <nds/card.h>

void ioR4CardWaitReady(u32 flags, u8 *command)
{
  bool ready = false;

  do {
    cardWriteCommand(command);
    REG_ROMCTRL = flags;
    do {
      if (REG_ROMCTRL & CARD_DATA_READY)
        if (!CARD_DATA_RD) ready = true;
    } while (REG_ROMCTRL & CARD_BUSY);
  } while (!ready);
}

void ioR4ByteCardPolledTransfer(uint32 flags, uint32 * destination, uint32 length, uint8 * command)
{
  u32 data;;
  cardWriteCommand(command);
  REG_ROMCTRL = flags;
  uint32 * target = destination + length;
  do {
    // Read data if available
    if (REG_ROMCTRL & CARD_DATA_READY) {
      data=CARD_DATA_RD;
      if (destination < target) {
        ((uint8*)destination)[0] = data & 0xff;
        ((uint8*)destination)[1] = (data >> 8) & 0xff;
        ((uint8*)destination)[2] = (data >> 16) & 0xff;
        ((uint8*)destination)[3] = (data >> 24) & 0xff;
      }
      destination++;
    }
  } while (REG_ROMCTRL & CARD_BUSY);
}

void ioR4LogicCardRead(u32 address, u32 *destination, u32 length)
{
  u8 command[8];

  command[7] = 0xb9;
  command[6] = (address >> 24) & 0xff;
  command[5] = (address >> 16) & 0xff;
  command[4] = (address >> 8)  & 0xff;
  command[3] =  address        & 0xff;
  command[2] = 0;
  command[1] = 0;
  command[0] = 0;
  ioR4CardWaitReady(0xa7586000, command);
  command[7] = 0xba;
  if ((u32)destination & 0x03)
    ioR4ByteCardPolledTransfer(0xa1586000, destination, length, command);
  else
    cardPolledTransfer(0xa1586000, destination, length, command);
}

void ioR4CryptedRead(u32 address,u32* destination,u32 length)
{
  u8 command[8];

  command[7] = 0xb6;
  command[6] = (address >> 24) & 0xff;
  command[5] = (address >> 16) & 0xff;
  command[4] = (address >> 8)  & 0xff;
  command[3] =  address        & 0xff;
  command[2] = 0;
  command[1] = 0;
  command[0] = 0;
  ioR4CardWaitReady(0xa7586000, command);
  command[7] = 0xbf;
  cardPolledTransfer(0xa1586000, destination, length, command);
}

u32 ioR4ReadCardInfo(void)
{
  u8 command[8];
  u32 ret;

  command[7] = 0xb0;
  command[6] = 0;
  command[5] = 0;
  command[4] = 0;
  command[3] = 0;
  command[2] = 0;
  command[1] = 0;
  command[0] = 0;
  cardPolledTransfer(0xa7586000, &ret, 1, command);
  return ret;
}

void ioR4LogicCardWrite(u32 address, u32 *source, u32 length)
{
  u8 command[8];
  u32 data = 0;

  command[7] = 0xbb;
  command[6] = (address >> 24) & 0xff;
  command[5] = (address >> 16) & 0xff;
  command[4] = (address >> 8)  & 0xff;
  command[3] =  address        & 0xff;
  command[2] = 0;
  command[1] = 0;
  command[0] = 0;
  cardWriteCommand(command);
  REG_ROMCTRL = 0xe1586000;
  u32 * target = source + length;
  do {
    // Write data if ready
    if (REG_ROMCTRL & CARD_DATA_READY) {
      if (source < target) {
        if ((u32)source & 0x03)
          data = ((uint8*)source)[0] | (((uint8*)source)[1] << 8) | (((uint8*)source)[2] << 16) | (((uint8*)source)[3] << 24);
        else
          data = *source;
      }
      source++;
      CARD_DATA_RD = data;
    }
  } while (REG_ROMCTRL & CARD_BUSY);
  command[7] = 0xbc;
  ioR4CardWaitReady(0xa7586000, command);
}

bool startup(void)
{
  u32 CardInfo;

  CardInfo = ioR4ReadCardInfo();
  return ((CardInfo & 0x07) == 0x04);
}

bool isInserted(void)
{
  u32 CardInfo;

  CardInfo = ioR4ReadCardInfo();
  return ((CardInfo & 0x07) == 0x04);
}

bool readSectors(u32 sector, u32 numSecs, void* buffer)
{
  u32 *u32_buffer = (u32*)buffer, i;

  for (i = 0; i < numSecs; i++) {
    ioR4LogicCardRead(sector << 9, u32_buffer, 128);
    sector++;
    u32_buffer += 128;
  }
  return true;
}

bool writeSectors(u32 sector, u32 numSecs, void* buffer)
{
  u32 *u32_buffer = (u32*)buffer, i;

  for (i = 0; i < numSecs; i++) {
    ioR4LogicCardWrite(sector << 9, u32_buffer, 128);
    sector++;
    u32_buffer += 128;
  }
  return true;
}

bool clearStatus(void)
{
  return true;
}

bool shutdown(void)
{
  return true;
}

void ioR4SendMap(u32 address)
{
  u8 command[8];

  command[7] = 0xb4;
  command[6] = (address >> 24) & 0xff;
  command[5] = (address >> 16) & 0xff;
  command[4] = (address >> 8)  & 0xff;
  command[3] =  address        & 0xff;
  command[2] = 0;
  command[1] = 0;
  command[0] = 0;

  ioR4CardWaitReady(0xa7586000,command);
}

u32 isSDHC(void)
{
  return 0;
}

u32 ioVersion(void)
{
  return 0;
}

u32 ioSize(void)
{
  return 1;
}

int ioSD(void)
{
  return 0;
}
