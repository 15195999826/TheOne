// Fill out your copyright notice in the Description page of Project Settings.


#include "Development/TheOneDeveloperSettings.h"

FName UTheOneDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
