/*
 * InstrumnetBar.cpp
 *
 *  Created on: Sep 3, 2014
 *      Author: bastl
 */

#include "InstrumentBar.h"
#include <BitArrayOperations.h>


InstrumentBar::InstrumentBar() : hw_(0), buttonMap_(0), currentSelectedStatuses_(0), currentPlayingStatuses_(0), isActive_(true), instrumentCount_(10) {
}

void InstrumentBar::init(IHWLayer* hw, IButtonMap * buttonMap, unsigned char instrumentCount) {
	hw_ = hw;
	buttonMap_ = buttonMap;
	instrumentCount_ = instrumentCount;
}

void InstrumentBar::setActive(bool isActive) {
	isActive_ = isActive;
	for (unsigned char i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
		if (isActive && GETBIT(currentPlayingStatuses_, i)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), IHWLayer::ON);
		} else {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), IHWLayer::OFF);
		}
	}
}

void InstrumentBar::setInstrumentSelected(unsigned char instrumentIndex, bool isSelected) {
	SETBIT(currentSelectedStatuses_, instrumentIndex, isSelected);
	if (isActive_) {
		if (isSelected && !GETBIT(currentPlayingStatuses_, instrumentIndex)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), IHWLayer::DULLON);
		}
		if (!isSelected && !GETBIT(currentPlayingStatuses_, instrumentIndex)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), IHWLayer::OFF);
		}
	}
}

void InstrumentBar::setInstrumentPlaying(unsigned char instrumentIndex, bool isPlaying) {
	SETBIT(currentPlayingStatuses_, instrumentIndex, isPlaying);
	if (isActive_) {
		if (isPlaying) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex), IHWLayer::ON);
		} else {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(instrumentIndex),
						GETBIT(currentSelectedStatuses_, instrumentIndex) ? IHWLayer::DULLON : IHWLayer::OFF );
		}
	}
}

void InstrumentBar::resetSelected() {
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		SETBIT(currentSelectedStatuses_, i, false);
		if (!GETBIT(currentPlayingStatuses_, i)) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), IHWLayer::OFF);
		}
	}
}
