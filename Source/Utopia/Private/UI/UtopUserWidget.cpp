// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UtopUserWidget.h"

void UUtopUserWidget::SetWidgetOwner(AActor* InWidgetOwner)
{
	WidgetOwner = InWidgetOwner;
	WidgetOwnerSet();
}
