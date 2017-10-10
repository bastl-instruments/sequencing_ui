/*
 * SekvojModulePool.h
 *
 *  Created on: Jan 11, 2015
 *      Author: martin
 */

#ifndef SEKVOJMODULEPOOL_H_
#define SEKVOJMODULEPOOL_H_

#include <Player.h>
#include <IStepMemory.h>
#include <PlayerSettings.h>
#include <BastlMetronome.h>
#include <StepRecorder.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"
#include <StepSynchronizer.h>
#include <ITapper.h>
#include "SekvojRackSDPreset.h"

/* This class is bit dangerous to the system but shall save a lot of memory needed
 * for program so we take that risk and do it!
 */
class SekvojModulePool
{
    public:

        static Player * player_;
        static StepRecorder  recorder_;
        static IStepMemory * memory_;
        static PlayerSettings * settings_;
        static InstrumentBar * instrumentBar_;
        static IButtonMap * buttonMap_;
        static SekvojRackSDPreset sd_;
        static StepSynchronizer synchronizer_;
        static ITapper * tapper_;

        static unsigned char buttonMap_getSubStepButtonIndex(unsigned char index) {
        		return buttonMap_->getSubStepButtonIndex(index);
        }

        static unsigned char * buttonMap_getInstrumentButtonArray() {
        		return buttonMap_->getInstrumentButtonArray();
        }

        static unsigned char * buttonMap_getSubStepButtonArray() {
        		return buttonMap_->getSubStepButtonArray();
        }


 };
#endif /* SEKVOJMODULEPOOL_H_ */
