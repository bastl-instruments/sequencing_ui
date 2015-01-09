/*
 * SekvojRackSDPreset.h
 *
 *  Created on: Dec 16, 2014
 *      Author: dasvaclav
 */

#ifndef SEKVOJRACKSDPRESET_H_
#define SEKVOJRACKSDPRESET_H_

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
private:
	unsigned char currentPattern;

};




#endif /* SEKVOJRACKSDPRESET_H_ */
