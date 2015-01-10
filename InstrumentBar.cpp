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
		ILEDHW::LedState state = (isActive && GETBIT(currentPlayingStatuses_, i)) ? ILEDHW::ON :
																					ILEDHW::OFF;
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), state);
	}
}

void InstrumentBar::setInstrumentSelected(unsigned char instrumentIndex, bool isSelected) {
	SETBIT(currentSelectedStatuses_, instrumentIndex, isSelected);
	if (isActive_ && !GETBIT(currentPlayingStatuses_, instrumentIndex)) {
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), isSelected ? ILEDHW::DULLON:
																						ILEDHW::OFF);
	}
}

void InstrumentBar::setInstrumentPlaying(unsigned char instrumentIndex, bool isPlaying) {
	SETBIT(currentPlayingStatuses_, instrumentIndex, isPlaying);
	if (isActive_) {
		ILEDHW::LedState status = GETBIT(currentSelectedStatuses_, instrumentIndex) ? ILEDHW::DULLON :
																					  ILEDHW::OFF;
		if (isPlaying) {
			status = ILEDHW::ON;
		}
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), status);
	}
}

void InstrumentBar::resetSelected() {
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		setInstrumentSelected(i, false);
	}
}
