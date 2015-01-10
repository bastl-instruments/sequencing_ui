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
	void getPatternData(unsigned char patternIndex, unsigned char * data);
	void setPatternData(unsigned char patternIndex, unsigned char * data);
	void getSettingsData(unsigned char * data);
	void setSettingsData(unsigned char * data);
	void loadData(unsigned char patternIndex,
				  unsigned int sourceOffset,
				  unsigned int targetOffset,
				  unsigned char * data,
				  unsigned int size);
	unsigned char getCurrentPattern(){return currentPattern;};
	void save();
	void discard();
	void copyAllData(unsigned long fromOffset, unsigned long toOffset );
private:
	unsigned char currentPattern;

};

inline void SekvojRackSDPreset::save() {
	copyAllData(0, OFFSET);
}

inline void SekvojRackSDPreset::discard() {
	copyAllData(OFFSET, 0);
}

#endif /* SEKVOJRACKSDPRESET_H_ */
