// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TheOneUserWidget.h"

void UTheOneUserWidget::SetViewController(UTheOneViewController* VC)
{
	this->ViewController = VC;
	ViewControllerProvided(VC);
}

void UTheOneUserWidget::BuildGrid()
{
}
