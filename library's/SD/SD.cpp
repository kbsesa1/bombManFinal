/*
 * SD.cpp
 *
 * Created: 9-1-2019 18:19:37
 *  Author: joost
 */ 

#include "SD.h"

SPI SD_spi = SPI(&DDRD,&PORTD,4,0);
// SD card commands

#define CMD0 0X00
#define CMD1 0X01
#define CMD8 0X08
#define CMD9 0X09
#define CMD10 0X0A
#define CMD13 0X0D
#define CMD17 0X11
#define CMD24 0X18
#define CMD25 0X19
#define CMD32 0X20
#define CMD33 0X21
#define CMD38 0X26
#define CMD55 0X37
#define CMD58 0X3A
#define ACMD23 0X17
#define ACMD41 0X29

SD::SD(){
	
}
uint8_t SD::Init(){
	bool sdInit;
	uint8_t tries = 0;
	SD_spi.init();
	SD_spi.endTransmission();
	for (int i = 0;i<10;i++)//write 80 clock cycles to sd to start
	{
		SD_spi.transfer(0xFF);
	}
	command(CMD0,0x00,0x95,8);
	command(CMD8,0x000001AA,0x87,16);
	while (sdInit == 0 && tries<= 10){
		sdInit = AcommandCheck(ACMD41,0x40000000,0xFF,8,0x00);
		tries ++;
		
	}
	if (sdInit) return 1;
		else return 0;
	
}
void SD::command(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read) {
	SD_spi.beginTransmission();
	SD_spi.transfer(cmd | 0x40);
	SD_spi.transfer(arg>>24);
	SD_spi.transfer(arg>>16);
	SD_spi.transfer(arg>>8);
	SD_spi.transfer(arg);
	SD_spi.transfer(crc);
	
		for(uint32_t i = 0; i<read; i++){
			SD_spi.transfer(0xFF);
		}
	
	SD_spi.endTransmission();
	
}
void SD::Acommand(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read){
	command(CMD55,0x00,0xFF,8);
	command(cmd,arg,crc,read);
}
uint8_t SD::commandCheck(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read, unsigned char check) {
	
	
	SD_spi.beginTransmission();
	SD_spi.transfer(cmd);
	SD_spi.transfer(arg>>24);
	SD_spi.transfer(arg>>16);
	SD_spi.transfer(arg>>8);
	SD_spi.transfer(arg);
	SD_spi.transfer(crc);
	
	for(uint32_t i = 0; i<read; i++){
		if (SD_spi.transfer(0xFF)==check)
		{
			SD_spi.endTransmission();
			return 1;
		}
	}
	SD_spi.endTransmission();
	return 0;
	
}
uint8_t SD::AcommandCheck(unsigned char cmd, unsigned long arg, unsigned char crc, uint32_t read, unsigned char check) {
	command(CMD55,0x00,0xFF,8);
	return commandCheck(cmd,arg,crc,read,check);
	
}
void SD::read(unsigned long sector, unsigned short offset, unsigned char * buffer, unsigned short len) {
	unsigned short i = 0;
	
	SD_spi.beginTransmission();
	SD_spi.transfer(CMD17 | 0x40);
	SD_spi.transfer(sector>>15); // sector*512 >> 24
	SD_spi.transfer(sector>>7);  // sector*512 >> 16
	SD_spi.transfer(sector<<1);  // sector*512 >> 8
	SD_spi.transfer(0);          // sector*512
	SD_spi.transfer(0xFF);
	
	for(i=0; i<10 && SD_spi.transfer(0xFF) != 0x00; i++) {} // wait for 0
	
	for(i=0; i<10 && SD_spi.transfer(0xFF) != 0xFE; i++) {} // wait for data start
	
	for(i=0; i<offset; i++) // "skip" bytes
	SD_spi.transfer(0xFF);
	
	for(i=0; i<len; i++) // read len bytes
	buffer[i] = SD_spi.transfer(0xFF);
	
	for(i+=offset; i<512; i++) // "skip" again
	SD_spi.transfer(0xFF);
	
	// skip checksum
	SD_spi.transfer(0xFF);
	SD_spi.transfer(0xFF);

	SD_spi.endTransmission();
}
uint8_t SD::cardCommand(uint8_t cmd, uint32_t arg) {
	uint8_t status_ = 0xFF;
	// select card
	SD_spi.beginTransmission();

	
	// send command
	SD_spi.transfer(cmd | 0x40);

	// send argument
	for (int8_t s = 24; s >= 0; s -= 8) SD_spi.transfer(arg >> s);

	// send CRC
	uint8_t crc = 0XFF;
	if (cmd == 0x00) crc = 0X95;  // correct crc for CMD0 with arg 0
	if (cmd == 0x08) crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
	SD_spi.transfer(crc);

	// wait for response
	for (uint8_t i = 0; ((status_ = SD_spi.transfer(0xFF)) & 0X80) && i != 0XFF; i++)
	;
	return status_;
}
void SD::getBlock(uint32_t sectorLBA,uint8_t* outBuffer){
	unsigned short i = 0;
	uint8_t command = CMD17;
	command |= 0x40;
	SD_spi.beginTransmission();
	SD_spi.transfer(command);
	SD_spi.transfer(sectorLBA>>24); // sector*512 >> 24
	SD_spi.transfer(sectorLBA>>16);  // sector*512 >> 16
	SD_spi.transfer(sectorLBA>>8);  // sector*512 >> 8
	SD_spi.transfer(sectorLBA);          // sector*512
	SD_spi.transfer(0xFF);
	
	for(i=0; i<10 && SD_spi.transfer(0xFF) != 0x00; i++) {} // wait for 0
	
	for(i=0; i<10 && SD_spi.transfer(0xFF) != 0xFE; i++) {} // wait for data start
	
	for(i=0; i<512; i++){ // read len bytes
		uint8_t d = SD_spi.transfer(0xFF);
		outBuffer[i] = d;
	}
	// skip checksum
	SD_spi.transfer(0xFF);
	SD_spi.transfer(0xFF);

	SD_spi.endTransmission();
}
uint8_t SD::getNumber8(uint8_t* in, uint32_t sector, uint32_t offset) {
	uint8_t output = 0;
	uint32_t sectorStart = sector;
	uint32_t sectorNumber = offset/512;
	unsigned long startbyte = (sector * 512) + offset;
	sectorStart += sectorNumber;
	startbyte -= (sectorNumber*512);
	getBlock(sectorStart,in);
	for (int i = 0; i < 1; ++i) {
		output |= (in[startbyte] << (8 * i));
	}
	return output;
}
uint16_t SD::getNumber16(uint8_t* in, uint32_t sector, uint32_t offset) {
	uint16_t output = 0;
	uint32_t sectorStart = sector;
	uint32_t sectorNumber = offset/512;
	unsigned long startbyte = (sector * 512) + offset;
	sectorStart += sectorNumber;
	startbyte -= (sectorNumber*512);
	getBlock(sectorStart,in);
	for (int i = 0; i < 2; ++i) {
		output |= (in[startbyte+i] << (8 * i));
	}
	return output;
}
uint32_t SD::getNumber32(uint8_t* in, uint32_t sector, uint32_t offset) {
	uint32_t output = 0;
	uint32_t sectorStart = sector;
	uint32_t sectorNumber = offset/512;
	unsigned long startbyte = (sector * 512) + offset;
	sectorStart += sectorNumber;
	startbyte -= (sectorNumber*512);
	getBlock(sectorStart,in);
	for (int i = 0; i < 4; ++i) {
		output |= (in[startbyte+i] << (8 * i));
	}
	return output;
}
uint16_t SD::getStringLength(char* str){
	char keys[] = {'\0'};
	return strcspn (str,keys);
}
void SD::strtrim(char* str) {
	int start = 0; // number of leading spaces
	char* buffer = str;
	while (*str && *str++ == ' ') ++start;
	while (*str++); // move to end of string
	int end = str - buffer - 1;
	while (end > 0 && buffer[end - 1] == ' ') --end; // backup over trailing spaces
	buffer[end] = 0; // remove trailing spaces
	if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
	str = buffer + start;
	while ((*buffer++ = *str++));  // remove leading spaces: K&R
}
void SD::strlow(char* str){
	for (uint16_t i = 0; i < getStringLength(str); ++i) {
		if (( str[i] >= 'A' ) && ( str[i] <= 'Z' ))//if highercase
		{
			str[i] += 0x20;//reset bit 5
		}
	}
}
uint8_t SD::getPartitionType(uint8_t partitionNumber){
	uint8_t output = 0;
	output = getNumber8(sectorBuffer,0,0x1C2+partitionNumber*32);
	return output;
}
uint32_t SD::getSectorsBtwnMbr(uint8_t partitionNumber){
	uint32_t output = 0;
	output = getNumber32(sectorBuffer,0,0x1C6+(partitionNumber*32));
	return output;
}
void SD::fileSystemBegin() {
	//read first byte to check if it has a partitiontable
	if (getNumber8(sectorBuffer,0,0) == 0) {
		if (getNumber8(sectorBuffer,0,0x450) == GUID) {
			volumeFormat = GUID;
			uint32_t headerStart = getNumber32(sectorBuffer, 0, 0x1C6);
			uint32_t entryStartLba = getNumber32(sectorBuffer, headerStart, 0x48);
			uint32_t numberOfEntries = getNumber32(sectorBuffer, headerStart, 0x50);
			for (uint16_t i = 0; i < numberOfEntries; ++i) {
				uint32_t entryLBA = entryStartLba + (i * 0x80);
				if (getNumber8(sectorBuffer,0,entryLBA) != 0) {
					partitionStartCluster = getNumber32(sectorBuffer, entryStartLba, (entryLBA + 0x20));
				}
			}
			} else {
			volumeFormat = MBR;
			
			for (int i = 0; i < 4; ++i) {
				if (getPartitionType(i) > 0) {
					partitionStartCluster = getSectorsBtwnMbr(i);
				}
			}
			
		}
		} else {
		volumeFormat = ONE;
	}
	//now that we have the partition start sector we can get the rest of the information
	clusterSize = getNumber8(sectorBuffer, partitionStartCluster, 0x0D);//found in fat32 boot record
	fatSize = getNumber32(sectorBuffer, partitionStartCluster, 0x24);
	fat1StartCluster = partitionStartCluster +
	getNumber16(sectorBuffer, partitionStartCluster, 0x0E);//partition start + reserved sectors
	fat2StartCluster = fat1StartCluster + fatSize;//fat1 start + size of fat table
	rootCluster = fat1StartCluster + (fatSize * 2);
	
}
void SD::getLongFilenameEntry(uint32_t directoryLBA, uint16_t directoryEntry, char *out) {
	uint32_t startByte = directoryEntry * 32;
	char output[13];
	uint16_t outputIndex = 0;
	for (uint16_t i = 0;i<getStringLength(output);i++)output[i] = 0;
	for (int i = 0; i < 5; ++i) {
		char c = 0;
		c |= getNumber16(sectorBuffer, directoryLBA, startByte + 0x01 + (i * 2));
		output[outputIndex] = c;
		outputIndex++;
	}
	for (int i = 0; i < 6; ++i) {
		char c = 0;
		c |= getNumber16(sectorBuffer, directoryLBA, startByte + 0x0E + (i * 2));
		output[outputIndex] = c;
		outputIndex++;
	}
	for (int i = 0; i < 2; ++i) {
		char c = 0;
		c |= getNumber16(sectorBuffer, directoryLBA, startByte + 0x1C + (i * 2));
		output[outputIndex] = c;
		outputIndex++;
	}
	output[outputIndex] = '\0';
	for (int j = 0; j < 13; ++j) {
		out[j] = output[j];
	}
}
void SD::getLongFileName(uint32_t directoryLBA, uint16_t directoryEntry, char *out) {
	uint8_t numberOfEntries = 1;
	uint8_t entryIndex = 0;
	uint32_t entryByte = 0;
	entryIndex |= (directoryEntry - 1);
	entryByte |= entryIndex * 32;
	uint8_t sequenceNumber = getNumber8(sectorBuffer, directoryLBA, entryByte);


	while ((sequenceNumber & 0x40) != 0) {
		numberOfEntries++;
		entryIndex--;
		entryByte |= entryIndex * 32;
		sequenceNumber = getNumber8(sectorBuffer, directoryLBA, entryByte);

	}
	numberOfEntries++;
	uint16_t longNameSize = (numberOfEntries * 12) + 1;
	char longName[longNameSize];
	for (uint16_t j = 0; j < longNameSize; ++j) {
		longName[j] = 0;
	}
	for (int i = 0; i < numberOfEntries; ++i) {
		char name[13];
		getLongFilenameEntry(directoryLBA, directoryEntry - (i + 1), name);
		strncat(longName, name, 13);
	}
	strcpy(out, longName);
}
void SD::getShortFilename(uint32_t directoryLBA, uint16_t directoryEntry, char *out) {
	uint32_t startByte = directoryEntry * 32;
	char output[13];
	uint16_t outputIndex = 0;
	for (uint16_t i = 0;i<getStringLength(output);i++)output[i] = 0;
	for (int i = 0; i < 8; ++i) {
		char c = 0;
		c = getNumber8(sectorBuffer, directoryLBA, startByte + i);
		output[outputIndex] = c;
		outputIndex++;
	}
	output[outputIndex] = '\0';
	strcpy(out, output);
}
void SD::getShortExtension(uint32_t directoryLBA, uint16_t directoryEntry, char *out) {
	uint32_t startByte = directoryEntry * 32;
	char output[4];
	uint16_t outputIndex = 0;
	for (uint16_t i = 0;i<getStringLength(output);i++)output[i] = 0;
	for (int i = 0; i < 3; ++i) {
		char c = 0;
		c = getNumber8(sectorBuffer, directoryLBA, startByte + i +0x08);
		output[outputIndex] = c;
		outputIndex++;
	}
	output[outputIndex] = '\0';
	strcpy(out, output);
}
bool SD::filenameIsShortened(uint32_t directoryLBA, uint16_t directoryEntry) {
	int entryByte = directoryEntry * 32;
	uint8_t filenameByte = getNumber8(sectorBuffer, directoryLBA, entryByte + 6);
	return filenameByte == '~';
}
uint8_t SD::getDirEntryType(uint32_t directoryLBA, uint16_t directoryEntry) {
	int entryByte = directoryEntry * 32;
	uint8_t output;
	uint8_t firstByte = getNumber8(sectorBuffer, directoryLBA, entryByte);
	uint8_t attribute = getNumber8(sectorBuffer, directoryLBA, entryByte + 0x0B);
	if (firstByte == 0x00)output = endOfDirectory;
	else if (firstByte == 0xE5)output = unused;
	else if (attribute == 0x0F)output = longEntry;
	else output = shortEntry;
	return output;
}
uint8_t SD::getDirEntryAttribute(uint32_t directoryLBA, uint16_t directoryEntry) {
	int entryByte = directoryEntry * 32;
	uint8_t output;
	uint8_t attribute = getNumber8(sectorBuffer, directoryLBA, entryByte + 0x0B);
	if ((attribute & 0x04) >> 2)output = 0;
	else if ((attribute & 0x08) >> 3)output = 3;
	else if ((attribute & 0x10) >> 4)output = 2;
	else output = 1;
	return output;
}
uint32_t SD::getDirEntryCluster(uint32_t directoryLBA, uint16_t directoryEntry){
	uint32_t output = 0;
	int entryByte = directoryEntry * 32;
	output |= getNumber16(sectorBuffer,directoryLBA,entryByte+0x1A);
	output |= getNumber16(sectorBuffer,directoryLBA,entryByte+0x14)<<16;
	return output;
}
long SD::getFilenameCluster(uint32_t directoryLBA, char* filenameIn){
	uint16_t entryIndex = 0;
	char filename[256];
	char extension[4];
	while (getDirEntryType(directoryLBA, entryIndex) != endOfDirectory) {
		if (getDirEntryType(directoryLBA, entryIndex) == shortEntry) {
			if (filenameIsShortened(directoryLBA, entryIndex))
			getLongFileName(directoryLBA, entryIndex, filename);
			else{
				if(getDirEntryAttribute(directoryLBA,entryIndex) == file){
					getShortFilename(directoryLBA, entryIndex, filename);
					strtrim(filename);
					uint8_t end = getStringLength(filename);

					filename[end]='.';
					filename[end+1] = '\0';
					getShortExtension(directoryLBA,entryIndex,extension);
					strcat (filename,extension);
				}
				else getShortFilename(directoryLBA, entryIndex, filename);

			}
			strlow(filenameIn);
			strlow(filename);
			strtrim(filename);
			if(strcmp(filenameIn,filename) == 0){//filenames are equal
				return getDirEntryCluster(directoryLBA,entryIndex);
			}
		}

		entryIndex++;
	}
	return -1;
}
long SD::getNextCluster(uint32_t fatLBA, uint32_t searchCluster){
	uint32_t output;
	uint32_t fatOffset = (searchCluster * 4);

	output = getNumber32(sectorBuffer, fatLBA,fatOffset);
	if (output >= 0x0FFFFFFF)return -1;

	else return output;
}
uint32_t SD::clusterToLBA(uint16_t cluster_number) {
	return (rootCluster + (cluster_number - 2) * clusterSize);
}
