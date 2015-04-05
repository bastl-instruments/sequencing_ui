/*
 * SekvojModulePool.h
 *
 *  Created on: Jan 11, 2015
 *      Author: martin
 */

#ifndef SEKVOJMODULEPOOL_H_
#define SEKVOJMODULEPOOL_H_

#include <ILEDsAndButtonsHW.h>
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

        static ILEDsAndButtonsHW * hw_;
        static Player * player_;
        static StepRecorder * recorder_;
        static IStepMemory * memory_;
        static PlayerSettings * settings_;
        static InstrumentBar * instrumentBar_;
        static IButtonMap * buttonMap_;
        static SekvojRackSDPreset * sd_;
        static StepSynchronizer * synchronizer_;
        static ITapper * tapper_;

        static void setLED(unsigned char ledIndex, ILEDHW::LedState state) {
        	hw_->setLED(ledIndex, state);
        }

 };
#endif /* SEKVOJMODULEPOOL_H_ */
