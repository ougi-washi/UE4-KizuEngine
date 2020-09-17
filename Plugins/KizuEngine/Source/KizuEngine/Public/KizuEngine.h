// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Modules/ModuleManager.h"

class FKizuEngineModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogKizu, Log, All);

//Logging during sessions and game Initialization phase
DECLARE_LOG_CATEGORY_EXTERN(LogKizuInit, Log, All);

//Logging for your AI system
DECLARE_LOG_CATEGORY_EXTERN(LogKizuAI, Log, All);
