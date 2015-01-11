/*
 * SekvojModulePool.cpp
 *
 *  Created on: Jan 11, 2015
 *      Author: martin
 */

#include "SekvojModulePool.h"

ILEDsAndButtonsHW * SekvojModulePool::hw_ = 0;
Player * SekvojModulePool::player_ = 0;
StepRecorder * SekvojModulePool::recorder_ = 0;
IStepMemory * SekvojModulePool::memory_ = 0;
PlayerSettings * SekvojModulePool::settings_ = 0;
InstrumentBar * SekvojModulePool::instrumentBar_ = 0;
IButtonMap * SekvojModulePool::buttonMap_ = 0;
SekvojRackSDPreset * SekvojModulePool::sd_ = 0;
StepSynchronizer * SekvojModulePool::synchronizer_ = 0;
ITapper * SekvojModulePool::tapper_ = 0;

