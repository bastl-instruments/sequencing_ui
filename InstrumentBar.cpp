/*
 * InstrumnetBar.cpp
 *
 *  Created on: Sep 3, 2014
 *      Author: bastl
 */

#include "InstrumentBar.h"
#include <BitArrayOperations.h>


InstrumentBar::InstrumentBar() : hw_(0), buttonMap_(0), instrumentCount_(10), currentSelectedStatuses_(0), currentPlayingStatuses_(0), isActive_(true) {
}

void InstrumentBar::init(ILEDHW* hw, IButtonMap * buttonMap, unsigned char instrumentCount) {
	hw_ = hw;
	buttonMap_ = buttonMap;
	instrumentCount_ = instrumentCount;
}

void InstrumentBar::setActive(bool isActive) {
	isActive_ = isActive;
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		if (isActive && GETBIT(currentPlayingStatuses_, i)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), ILEDHW::ON);
		} else {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), ILEDHW::OFF);
		}
	}
}

void InstrumentBar::setInstrumentSelected(unsigned char instrumentIndex, bool isSelected) {
	SETBIT(currentSelectedStatuses_, instrumentIndex, isSelected);
	if (isActive_) {
		if (isSelected && !GETBIT(currentPlayingStatuses_, instrumentIndex)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), ILEDHW::DULLON);
		}
		if (!isSelected && !GETBIT(currentPlayingStatuses_, instrumentIndex)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), ILEDHW::OFF);
		}
	}
}

void InstrumentBar::setInstrumentPlaying(unsigned char instrumentIndex, bool isPlaying) {
	SETBIT(currentPlayingStatuses_, instrumentIndex, isPlaying);
	if (isActive_) {
		if (isPlaying) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), ILEDHW::ON);
		} else {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex),
						GETBIT(currentSelectedStatuses_, instrumentIndex) ? ILEDHW::DULLON : ILEDHW::OFF );
		}
	}
}

void InstrumentBar::resetSelected() {
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		SETBIT(currentSelectedStatuses_, i, false);
		if (!GETBIT(currentPlayingStatuses_, i)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), ILEDHW::OFF);
		}
	}
}
