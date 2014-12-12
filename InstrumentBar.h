/*
 * InstrumentBar.h
 *
 *  Created on: Sep 3, 2014
 *      Author: bastl
 */

#ifndef INSTRUMENTBAR_H_
#define INSTRUMENTBAR_H_

#include <ILEDHW.h>
#include "IButtonMap.h"

class InstrumentBar {
public:
	InstrumentBar();
	void init(ILEDHW * hw, IButtonMap * buttonMap_, unsigned char instrumentCount);
	void setActive(bool isActive);
	void setInstrumentSelected(unsigned char instrumentIndex, bool isSelected);
	void setInstrumentPlaying(unsigned char instrumentIndex, bool isPlaying);
	void resetSelected();
private:
	ILEDHW * hw_;
	IButtonMap * buttonMap_;
	int currentSelectedStatuses_;
	int currentPlayingStatuses_;
	unsigned char instrumentCount_;
	bool isActive_;
};

#endif /* INSTRUMENTBAR_H_ */
