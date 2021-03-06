/*
 * drumStepsView.cpp
 *
 *  Created on: Jul 29, 2014
 *      Author: bastl
 */

#include "DrumStepsView.h"

DrumStepsView::DrumStepsView() :
	buttonMap_(0),
	currentStatus_(0),
	lastStatus_(0),
	currentDownButton_(-1),
	ignoreOffs_(false),
	ignoreAll_(false),
	highlightedButton_(-1) {
}

DrumStepsView::~DrumStepsView() {
}

	void DrumStepsView::init (IButtonMap * buttonMap) {
		buttonMap_ = buttonMap;
		stepSwitches_.init(buttonMap_->getStepButtonArray(), 16, false, IButtonHW::UP);
	}

	void DrumStepsView::update() {
		lastStatus_ = currentStatus_;
		stepSwitches_.update();
		currentDownButton_ = -1;
		for (unsigned char i = 0; i < 16; i++) {
			if (LEDsAndButtonsHWWrapper::isButtonDown(buttonMap_->getStepButtonIndex(i))) {
				currentDownButton_ = i;
			}

			bool stepValue = (currentStatus_ & (1 << i)) != 0;
			bool newStepValue = stepSwitches_.getStatus(i);
			if (stepValue != newStepValue) {
				if (newStepValue) {
					if (ignoreAll_) {
						stepSwitches_.setStatus(i, false);
					} else {
						LEDsAndButtonsHWWrapper::setLED(buttonMap_->getStepButtonIndex(i), (i == highlightedButton_) ? ILEDHW::ON : ILEDHW::DULLON);
						currentStatus_ = currentStatus_ | (1 << i);
					}
				} else {
					if (ignoreOffs_ || ignoreAll_) {
						stepSwitches_.setStatus(i, true);
					} else {
						LEDsAndButtonsHWWrapper::setLED(buttonMap_->getStepButtonIndex(i), ILEDHW::OFF);
						currentStatus_ = currentStatus_ & ~(1 << i);
					}
				}
			}
		}
	}

	void DrumStepsView::setStatus(unsigned int status) {
		currentStatus_ = status;
		lastStatus_ = status;
		for (unsigned char i = 0; i < 16; i++) {
			bool stepValue = (currentStatus_ & (1 << i)) != 0;
			stepSwitches_.setStatus(i, stepValue);
			LEDsAndButtonsHWWrapper::setLED(buttonMap_->getStepButtonIndex(i), stepValue ? ILEDHW::DULLON: ILEDHW::OFF);
		}
	}

	unsigned int DrumStepsView::getNewOffs() {
		return (lastStatus_ ^  currentStatus_) & lastStatus_;
	}

	unsigned int DrumStepsView::getNewOns() {
		return (lastStatus_ ^  currentStatus_) & currentStatus_;
	}

	void DrumStepsView::setHighlightedButton(unsigned char index) {
		if (highlightedButton_ != index) {
			if (highlightedButton_ != -1) {
				bool stepValue = ((currentStatus_ & (1 << highlightedButton_)) != 0);
				LEDsAndButtonsHWWrapper::setLED(buttonMap_->getStepButtonIndex((unsigned char)highlightedButton_), stepValue ? ILEDHW::DULLON : ILEDHW::OFF);
			}
			highlightedButton_ = index;
			if (highlightedButton_ != -1) {
				LEDsAndButtonsHWWrapper::setLED(buttonMap_->getStepButtonIndex((unsigned char)highlightedButton_), ILEDHW::ON);
			}
		}
	}
