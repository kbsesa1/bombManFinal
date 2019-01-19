/*
 * SD.h
 *
 * Created: 9-1-2019 18:19:21
 *  Author: joost
 */ 


#ifndef SD_H_
#define SD_H_
#define F_CPU 16000000
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "SPI.h"

class SD
{
	
	public:
	SD();
	uint8_t Init();
	void fileSystemBegin();
	void read(unsigned long sector, unsigned short offset, unsigned char * buffer, unsigned short len);
	void getBlock(uint32_t sectorLBA,uint8_t* outBuffer);
	void getLongFilenameEntry(uint32_t directoryLBA, uint16_t directoryEntry, char *out);
	void getLongFileName(uint32_t directoryLBA, uint16_t directoryEntry, char *out);
	void getShortFilename(uint32_t directoryLBA, uint16_t directoryEntry, char *out);
	void getShortExtension(uint32_t directoryLBA, uint16_t directoryEntry, char *out);
	bool filenameIsShortened(uint32_t directoryLBA, uint16_t directoryEntry);
	uint8_t getDirEntryType(uint32_t directoryLBA, uint16_t directoryEntry);
	uint8_t getDirEntryAttribute(uint32_t directoryLBA, uint16_t directoryEntry);
	uint32_t getDirEntryCluster(uint32_t directoryLBA, uint16_t directoryEntry);
	long getFilenameCluster(uint32_t directoryLBA, char* filenameIn);
	long getNextCluster(uint32_t fatLBA, uint32_t searchCluster);
	uint32_t clusterToLBA(uint16_t cluster_number);
	
	enum partitionTable {
		ONE = 0x00,
		MBR = 0x01,
		GUID = 0x02
	};
	enum dirEntryType {
		shortEntry,
		longEntry,
		unused,
		endOfDirectory
	};
	enum dirEntryAttribute {
		systemFile,
		file,
		folder,
		volume,
	};
	
	unsigned char sectorBuffer[512];
	uint8_t volumeFormat;
	uint32_t partitionStartCluster;
	uint8_t clusterSize;
	uint32_t fatSize;
	uint32_t fat1StartCluster;
	uint32_t fat2StartCluster;
	uint32_t rootCluster;
	private:	
	void command(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read);
	uint8_t commandCheck(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read, unsigned char check);
	
	uint8_t cardCommand(uint8_t cmd, uint32_t arg);
	void Acommand(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read);
	uint8_t AcommandCheck(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read, unsigned char check);
	uint8_t getNumber8(uint8_t* in, uint32_t sector, uint32_t offset);
	uint16_t getNumber16(uint8_t* in, uint32_t sector, uint32_t offset);
	uint32_t getNumber32(uint8_t* in, uint32_t sector, uint32_t offset);
	uint16_t getStringLength(char* str);
	void strtrim(char* str);
	void strlow(char* str);
	uint8_t getPartitionType(uint8_t partitionNumber);
	uint32_t getSectorsBtwnMbr(uint8_t partitionNumber);
	
	
};




#endif /* SD_H_ */