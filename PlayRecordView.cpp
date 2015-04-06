/*
 * PlayRecordView.cpp
 *
 *  Created on: Jul 31, 2014
 *      Author: bastl
 */

#include "PlayRecordView.h"
#include <MIDICommand.h>
#include <DrumStep.h>
#include "SekvojModulePool.h"

PlayRecordView::PlayRecordView() : lastStepIndex_(0), turnOffStep_(true), turnedSteps_(0),
								   stepReseted_(true), substepReseted_(true){
}

PlayRecordView::~PlayRecordView() {
}

void PlayRecordView::updateStatusItem(unsigned char step,
		  	  	  	  	  	  	      unsigned char stepLength,
		  	  	  	  	  	  	      unsigned int & statuses,
		  	  	  	  	  	  	      bool & resetStatus,
		  	  	  	  	  	  	      unsigned char * buttonsReference,
		  	  	  	  	  	  	      bool markCurrentStep) {
	if (step == 0) {
		if (!resetStatus) {
			statuses = 0;
		}
		resetStatus = true;
	} else {
		resetStatus = false;
	}

	for (unsigned char stepIndex = 0; stepIndex < stepLength; stepIndex++) {
		bool isOn = GETBIT(statuses, stepIndex) || (markCurrentStep && step == stepIndex);
		SekvojModulePool::setLED(buttonsReference[stepIndex],isOn ? ILEDHW::ON :
							   	   	   	   	   	       ILEDHW::OFF);
	}
}

void PlayRecordView::updateStatusView() {
	unsigned int currentSubStep = SekvojModulePool::synchronizer_->getCurrentStepNumber();
	updateStatusItem((currentSubStep % 4), 4, turnedSubSteps_, substepReseted_,
					 SekvojModulePool::buttonMap_->getSubStepButtonArray());
	updateStatusItem((currentSubStep / 4) % 16, 16, turnedSteps_, stepReseted_,
					 SekvojModulePool::buttonMap_->getStepButtonArray(), true);
}

void PlayRecordView::update() {
	unsigned int newStatuses = 0;
	for (unsigned char i = 0; i < 6; i++) {
		bool currentStatus = (currentStatus_ & (1 << i)) != 0;
		bool newStatus = SekvojModulePool::hw_->isButtonDown(SekvojModulePool::buttonMap_->getInstrumentButtonIndex(i));
		if (!currentStatus && newStatus) {
			unsigned int substep = SekvojModulePool::recorder_->startRecordNote(i);
			SETBITTRUE(turnedSteps_, (substep / 4) % 16);
			SETBITTRUE(turnedSubSteps_, (substep % 4));
		}
		if (currentStatus && !newStatus) {
			SekvojModulePool::recorder_->stopRecordNote(i);
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
