/*s
 * SekvojRackSDPreset.cpp
 *
 *  Created on: Dec 16, 2014
 *      Author: dasvaclav
 */

#include "SekvojRackSDPreset.h"
#include "BitArrayOperations.h"
#include <SdFat.h>
//#include <SdFatUtil.h>

SdFat card;
SdFile file;

const uint8_t CHIP_SELECT = 10;
SekvojRackSDPreset::SekvojRackSDPreset(){

}

char presetFileName[7]="iffccc";

void SekvojRackSDPreset::initCard(unsigned char * data, unsigned char * settingsData){
	dataReference_ = data;
	bool fileOpened = true;
	if (!card.begin(CHIP_SELECT, SPI_FULL_SPEED)){};
	if (!file.open(presetFileName, O_RDWR )) { //&root,
		file.close();
		if (!file.open( presetFileName, O_RDWR | O_CREAT | O_AT_END)) { //&root,
			fileOpened = false;
		} else {
			for (int j = 0; j < 128; j++) {
				file.write(&data[0],290);
				file.write(&settingsData[0], 11);
				file.write(&data[0],211);
			}
		}
		file.close();
		file.open(presetFileName, O_RDWR );
	}
	copyAllData(OFFSET, 0);
}

void SekvojRackSDPreset::copyAllData(unsigned long fromOffset, unsigned long toOffset, unsigned int * manipulatedPatternsBitArray) {
	for (unsigned char pattern = 0; pattern < 64; pattern++) {
		bool copy = !manipulatedPatternsBitArray || GETBIT(manipulatedPatternsBitArray[pattern / 16], pattern % 16);
		if (copy) {
			unsigned long indexOffset = pattern * 512;
			file.seekSet(fromOffset + indexOffset);
			file.read(&dataReference_[0], 290);
			file.seekSet(toOffset + indexOffset);
			file.write(&dataReference_[0], 290);
		}
	}
}

void SekvojRackSDPreset::getPatternData(unsigned char patternIndex) {
	loadData(patternIndex, 0, 0, 290);
}

void SekvojRackSDPreset::setPatternData(unsigned char patternIndex) {
	file.seekSet(patternIndex * 512);
	file.write(&dataReference_[0], 290);
}

void SekvojRackSDPreset::loadData(unsigned char patternIndex,
								  unsigned int sourceOffset,
								  unsigned int targetOffset,
								  unsigned int size) {
	file.seekSet(patternIndex * 512 + sourceOffset);
	file.read(&dataReference_[targetOffset], size);
}

void SekvojRackSDPreset::getSettingsData(unsigned char * data) {
	file.seekSet(290);
	file.read(&data[0], 11);
}

void SekvojRackSDPreset::setSettingsData(unsigned char * data) {
	file.seekSet(290);
	file.write(&data[0], 11);
	file.seekSet(OFFSET + 290);
	file.write(&data[0], 11);
	file.seekSet(802);
	file.read(&data[0], 11);

}

void SekvojRackSDPreset::debug(){


}


