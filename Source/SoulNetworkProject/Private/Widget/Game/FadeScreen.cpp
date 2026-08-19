// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Game/FadeScreen.h"

void UFadeScreen::PlayFadein()
{
	//AddToViewport();

	PlayAnimation(Fadein);
}

void UFadeScreen::PlayFadeout()
{
	//AddToViewport();

	PlayAnimation(Fadeout);
}
