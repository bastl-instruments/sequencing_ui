/*
 * SekvojButtonMap.h
 *
 *  Created on: Sep 3, 2014
 *      Author: bastl
 */

#ifndef IBUTTONMAP_H_
#define IBUTTONMAP_H_

class IButtonMap {
public:
	virtual unsigned char getButtonIndex(unsigned char index) = 0;
	virtual unsigned char getMainMenuButtonIndex(unsigned char index) = 0;
	virtual unsigned char getInstrumentButtonIndex(unsigned char index) = 0;
	virtual unsigned char getStepButtonIndex(unsigned char index) = 0;
	virtual unsigned char getSubStepButtonIndex(unsigned char index) = 0;
	virtual unsigned char * getMainMenuButtonArray() = 0;
	virtual unsigned char * getInstrumentButtonArray() = 0;
	virtual unsigned char * getStepButtonArray() = 0;
	virtual unsigned char * getSubStepButtonArray() = 0;

	virtual unsigned char getPlayButtonIndex() = 0;
	virtual unsigned char getRecordButtonIndex() = 0;
	virtual unsigned char getJumpButtonIndex() = 0;
	virtual unsigned char getFunctionButtonIndex() = 0;
	virtual unsigned char getPatternButtonIndex() = 0;
	virtual unsigned char getActiveButtonIndex() = 0;

};

#endif /* IBUTTONMAP_H_ */

