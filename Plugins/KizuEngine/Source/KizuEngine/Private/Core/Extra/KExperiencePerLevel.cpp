// Copyright (c) 2019 Nexus Division. All rights reserved.


#include "Core/Extra/KExperiencePerLevel.h"

int32 UKExperiencePerLevel::GetExperience(const int32 Level)
{
	if (ExperiencePerLevel.IsValidIndex(Level - 1))
	{
		return ExperiencePerLevel[Level - 1];
	}
	return -1;
}
