/*
 * PlayRecordView.cpp
 *
 *  Created on: Jul 31, 2014
 *      Author: bastl
 */

#include "PlayRecordView.h"
#include <MIDICommand.h>
#include <DrumStep.h>

PlayRecordView::PlayRecordView() : hw_(0), recorder_(0), buttonMap_(0), synchronizer_(0), lastStepIndex_(0), turnOffStep_(true) {
}

PlayRecordView::~PlayRecordView() {
}

void PlayRecordView::init(ILEDsAndButtonsHW * hw, StepRecorder * recorder, IButtonMap * buttonMap, StepSynchronizer * synchronizer) {
	hw_ = hw;
	recorder_ = recorder;
	buttonMap_ = buttonMap;
	synchronizer_ = synchronizer;
}

void PlayRecordView::updateStatusView() {
	for (unsigned char panIndex = 0; panIndex < 4; panIndex++) {
		bool isCurrentPan = synchronizer_->getCurrentStepNumber() / 64 == panIndex;
		hw_->setLED(buttonMap_->getSubStepButtonIndex(panIndex), isCurrentPan ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char stepIndex = 0; stepIndex < 16; stepIndex++) {
		bool isCurrentStep = (synchronizer_->getCurrentStepNumber() % 64) / 4 == stepIndex;
		if (isCurrentStep && stepIndex != lastStepIndex_) {
			if (stepIndex == 0) {
				for (unsigned char index = 0; index < 16; index++) {
					hw_->setLED(buttonMap_->getStepButtonIndex(index), ILEDHW::OFF);
				}
			}
			if (turnOffStep_) {
				hw_->setLED(buttonMap_->getStepButtonIndex(lastStepIndex_), ILEDHW::OFF);
			}
			turnOffStep_ = true;
			lastStepIndex_ = stepIndex;
			hw_->setLED(buttonMap_->getStepButtonIndex(lastStepIndex_), ILEDHW::ON );
		}
	}
}

void PlayRecordView::update() {
	unsigned int newStatuses = 0;
	for (unsigned char i = 0; i < 6; i++) {
		bool currentStatus = (currentStatus_ & (1 << i)) != 0;
		bool newStatus = hw_->isButtonDown(buttonMap_->getInstrumentButtonIndex(i));
		if (!currentStatus && newStatus) {
			recorder_->startRecordNote(i);
			turnOffStep_ =  false;
		}
		if (currentStatus && !newStatus) {
			recorder_->stopRecordNote(i);
		}
		if (newStatus) {
			newStatuses = newStatuses | 1 << i;
		} else {
			newStatuses = newStatuses & ~(1 << i);
		}
	}
	currentStatus_ = newStatuses;
	updateStatusView();
}
