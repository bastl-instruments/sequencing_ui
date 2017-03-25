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
	void setInstrumentsMutes(unsigned char instrumentsMutes);
	void resetSelected();
private:
	ILEDHW * hw_;
	IButtonMap * buttonMap_;
	unsigned char instrumentCount_;
	unsigned char instrumentsMutes_;
	unsigned char currentSelectedStatuses_;
	unsigned char currentPlayingStatuses_;
	bool isActive_;

	void updateView();
};

inline void InstrumentBar::setInstrumentsMutes(unsigned char instrumentsMutes) {
	instrumentsMutes_ = instrumentsMutes;
	if (isActive_) updateView();
}


#endif /* INSTRUMENTBAR_H_ */
