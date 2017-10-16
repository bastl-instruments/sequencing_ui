/*
 * InstrumnetBar.cpp
 *
 *  Created on: Sep 3, 2014
 *      Author: bastl
 */

#include "InstrumentBar.h"
#include <BitArrayOperations.h>


InstrumentBar::InstrumentBar() : hw_(0), buttonMap_(0), instrumentCount_(10), instrumentsMutes_(255), currentSelectedStatuses_(0), currentPlayingStatuses_(0), isActive_(true) {
}

void InstrumentBar::init(ILEDHW* hw, IButtonMap * buttonMap, unsigned char instrumentCount) {
	hw_ = hw;
	buttonMap_ = buttonMap;
	instrumentCount_ = instrumentCount;
}

void InstrumentBar::setActive(bool isActive) {
	isActive_ = isActive;
	updateView();
}

void InstrumentBar::setInstrumentSelected(unsigned char instrumentIndex, bool isSelected) {
	setBit(currentSelectedStatuses_, instrumentIndex, isSelected);
	if (isActive_ ) {
		updateView();
	}
}

void InstrumentBar::updateView() {
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		bool isPlaying = GETBIT(currentPlayingStatuses_, i) &&
						 GETBIT(instrumentsMutes_, i);
		bool isSelected = GETBIT(currentSelectedStatuses_, i);
		ILEDHW::LedState state = ILEDHW::OFF;
		if (isActive_) {
			if (isPlaying) state = ILEDHW::ON;
			else if (isSelected) state = ILEDHW::DULLON;
		}
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), state);
	}
}

void InstrumentBar::setInstrumentPlaying(unsigned char instrumentIndex, bool isPlaying) {
	setBit(currentPlayingStatuses_, instrumentIndex, isPlaying);
	if (isActive_) {
		updateView();
	}
}

void InstrumentBar::resetSelected() {
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		setInstrumentSelected(i, false);
	}
}
