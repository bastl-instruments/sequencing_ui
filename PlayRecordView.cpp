/*
 * PlayRecordView.cpp
 *
 *  Created on: Jul 31, 2014
 *      Author: bastl
 */

#include "PlayRecordView.h"
#include <MIDICommand.h>
#include <DrumStep.h>

PlayRecordView::PlayRecordView() : hw_(0), recorder_(0), buttonMap_(0), synchronizer_(0), lastStepIndex_(0), turnOffStep_(true), turnedSteps_(0),
								   stepReseted_(true), substepReseted_(true){
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
	unsigned int currentSubStep = synchronizer_->getCurrentStepNumber();
	if (currentSubStep % 4 == 0) {
		if (!substepReseted_) {
			turnedSubSteps_ = 0;
		}
		substepReseted_ = true;

	} else {
		substepReseted_ = false;
	}
	if ((currentSubStep / 4) % 16 == 0) {
		if (!stepReseted_) {
			turnedSteps_ = 0;
		}
		stepReseted_ = true;

	} else {
		stepReseted_ = false;
	}
	for (unsigned char panIndex = 0; panIndex < 4; panIndex++) {
		bool hasTurned = GETBIT(turnedSubSteps_, panIndex);
		hw_->setLED(buttonMap_->getSubStepButtonIndex(panIndex), hasTurned ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char stepIndex = 0; stepIndex < 16; stepIndex++) {
		bool isCurrentStep = (currentSubStep / 4) % 16 == stepIndex;
		bool hasTurned = GETBIT(turnedSteps_, stepIndex);
		hw_->setLED(buttonMap_->getStepButtonIndex(stepIndex),
				   (hasTurned || isCurrentStep) ? ILEDHW::ON :
						   	   	   	   	   	      ILEDHW::OFF);
	}
}

void PlayRecordView::update() {
	unsigned int newStatuses = 0;
	for (unsigned char i = 0; i < 6; i++) {
		bool currentStatus = (currentStatus_ & (1 << i)) != 0;
		bool newStatus = hw_->isButtonDown(buttonMap_->getInstrumentButtonIndex(i));
		if (!currentStatus && newStatus) {
			unsigned int substep = recorder_->startRecordNote(i);
			SETBITTRUE(turnedSteps_, (substep / 4) % 16);
			SETBITTRUE(turnedSubSteps_, (substep % 4));
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
