// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulNetworkDebug.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogSoulNetwork);
DEFINE_LOG_CATEGORY(LogSoulCombat);
DEFINE_LOG_CATEGORY(LogSoulAI);
DEFINE_LOG_CATEGORY(LogSoulItem);
DEFINE_LOG_CATEGORY(LogSoulUI);
DEFINE_LOG_CATEGORY(LogSoulGame);

#if !UE_BUILD_SHIPPING

namespace SoulDebug
{
	TAutoConsoleVariable<int32> CVarDrawDebug(
		TEXT("soul.DrawDebug"),
		0,
		TEXT("전투 / AI / 락온 디버그 도형 및 화면 로그를 표시한다.\n")
		TEXT("  0: 비활성 (기본)\n")
		TEXT("  1: 활성"),
		ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarShowNetRole(
		TEXT("soul.ShowNetRole"),
		0,
		TEXT("캐릭터 머리 위에 네트워크 롤(Authority / AutonomousProxy / SimulatedProxy)을 표시한다.\n")
		TEXT("  0: 비활성 (기본)\n")
		TEXT("  1: 활성"),
		ECVF_Cheat);

	void ScreenLog(const FString& Message, FColor Color, float Duration)
	{
		if (GEngine && IsDrawEnabled())
		{
			GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
		}
	}
}

#endif	// !UE_BUILD_SHIPPING
