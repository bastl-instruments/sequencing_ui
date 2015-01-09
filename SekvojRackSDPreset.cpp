/*
 * SekvojRackSDPreset.cpp
 *
 *  Created on: Dec 16, 2014
 *      Author: dasvaclav
 */

#include "SekvojRackSDPreset.h"

#include <SdFat.h>
//#include <SdFatUtil.h>

SdFat card;
SdFile file;

const uint8_t CHIP_SELECT = 10;
SekvojRackSDPreset::SekvojRackSDPreset(){

}
char presetFileName[7]="iffccc";
void SekvojRackSDPreset::initCard(unsigned char * data, unsigned char * settingsData){
	if (!card.begin(CHIP_SELECT, SPI_FULL_SPEED)){};
	if (!file.open(presetFileName, O_RDWR )) { //&root,
		file.close();
		if (!file.open( presetFileName, O_RDWR | O_CREAT | O_AT_END)) { //&root,
			//add error loop
		} else {
			for (int j = 0; j < 128; j++) {
				file.write(&data[0],290);
				file.write(&settingsData[0], 8);
				file.write(&data[0],214);
			}
		}
		file.close();

	}

}

void SekvojRackSDPreset::getPatternData(unsigned char patternIndex, unsigned char * data) {
	loadData(patternIndex, 0, 0, data, 290);
}

void SekvojRackSDPreset::setPatternData(unsigned char patternIndex, unsigned char * data) {
	file.seekSet(patternIndex * 512);
	file.write(&data[0], 290);
}

void SekvojRackSDPreset::loadData(unsigned char patternIndex,
								  unsigned int sourceOffset,
								  unsigned int targetOffset,
								  unsigned char * data,
								  unsigned int size) {
	file.seekSet(patternIndex * 512 + sourceOffset);
	file.read(&data[targetOffset], size);
}

void SekvojRackSDPreset::getSettingsData(unsigned char * data) {
	file.seekSet(290);
	file.read(&data[0], 6);
}

void SekvojRackSDPreset::setSettingsData(unsigned char * data) {
	file.seekSet(290);
	file.write(&data[0], 6);
	file.seekSet(802);
	file.read(&data[0], 8);

}


void SekvojRackSDPreset::debug(){


}


