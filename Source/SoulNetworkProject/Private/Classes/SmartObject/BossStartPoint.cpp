// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/BossStartPoint.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "SoulNetworkDebug.h"

ABossStartPoint::ABossStartPoint()
{
	LevelSequence = CreateDefaultSubobject<ULevelSequence>(TEXT("LevelSequence"));
	LevelSequenceActor = CreateDefaultSubobject<ALevelSequenceActor>(TEXT("LevelSequenceActor"));
}

void ABossStartPoint::BeginPlay()
{
	Super::BeginPlay();

	//FMovieSceneSequencePlaybackSettings Settings;
	//Settings.bAutoPlay = false;
	//Settings.bPauseAtEnd = true;
	//LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, Settings, LevelSequenceActor);
}

void ABossStartPoint::StartSequence()
{
	if(LevelSequencePlayer)
	{
		LevelSequencePlayer->Play();
		SoulDebug::ScreenLog(__FUNCTION__, FColor::Blue, 10.f);
	}
}
