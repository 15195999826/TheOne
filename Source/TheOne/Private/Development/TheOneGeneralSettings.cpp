// Fill out your copyright notice in the Description page of Project Settings.


#include "Development/TheOneGeneralSettings.h"

FName UTheOneGeneralSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
