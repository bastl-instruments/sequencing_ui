/*
 * SekvojRackSDPreset.h
 *
 *  Created on: Dec 16, 2014
 *      Author: dasvaclav
 */

#ifndef SEKVOJRACKSDPRESET_H_
#define SEKVOJRACKSDPRESET_H_

#define OFFSET 32768


class SekvojRackSDPreset
{
public:
	SekvojRackSDPreset();
	void debug();
	void initCard(unsigned char * data, unsigned char * settingsData);
	void getPatternData(unsigned char patternIndex);
	void setPatternData(unsigned char patternIndex);
	void getSettingsData(unsigned char * data);
	void setSettingsData(unsigned char * data);
	void loadData(unsigned char patternIndex,
				  unsigned int sourceOffset,
				  unsigned int targetOffset,
				  unsigned int size);
	unsigned char getCurrentPattern(){return currentPattern;};
	void save(unsigned int * manipulatedPatternsBitArray);
	void discard(unsigned int * manipulatedPatternsBitArray);
	void copyAllData(unsigned long fromOffset, unsigned long toOffset , unsigned int * manipulatedPatternsBitArray = 0);
private:
	unsigned char currentPattern;
	unsigned char * dataReference_;

};

inline void SekvojRackSDPreset::save(unsigned int * manipulatedPatternsBitArray) {
	copyAllData(0, OFFSET, manipulatedPatternsBitArray);
}

inline void SekvojRackSDPreset::discard(unsigned int * manipulatedPatternsBitArray) {
	copyAllData(OFFSET, 0, manipulatedPatternsBitArray);
}

#endif /* SEKVOJRACKSDPRESET_H_ */
