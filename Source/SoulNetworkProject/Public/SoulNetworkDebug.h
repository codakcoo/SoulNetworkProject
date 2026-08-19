// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SoulNetworkLog.h"

/*
 * SoulNetworkDebug
 * ----------------------------------------------------------------------------
 * 프로젝트 전역 로그 카테고리와 디버그 시각화 게이트를 한곳에서 관리한다.
 *
 * 콘솔 명령:
 *   soul.DrawDebug 1     전투/AI/락온 디버그 도형 + 화면 로그 표시
 *   soul.ShowNetRole 1   캐릭터 머리 위에 네트워크 롤 표시
 *
 * Shipping 빌드에서는 모든 게이트가 컴파일 타임 false로 접혀
 * 디버그 코드가 실행 파일에 남지 않는다.
 */

// ─────────────────────────────────────────────────────────────
// Debug Gate
// ─────────────────────────────────────────────────────────────
namespace SoulDebug
{
#if !UE_BUILD_SHIPPING

	extern TAutoConsoleVariable<int32> CVarDrawDebug;
	extern TAutoConsoleVariable<int32> CVarShowNetRole;

	/** 디버그 도형 / 화면 로그를 그릴지 여부. */
	FORCEINLINE bool IsDrawEnabled()
	{
		return CVarDrawDebug.GetValueOnAnyThread() > 0;
	}

	/** 캐릭터 머리 위 네트워크 롤 표시 여부. */
	FORCEINLINE bool IsNetRoleShown()
	{
		return CVarShowNetRole.GetValueOnAnyThread() > 0;
	}

	/** 게이트가 켜져 있을 때만 화면 좌상단에 메시지를 출력한다. */
	void ScreenLog(const FString& Message, FColor Color = FColor::White, float Duration = 5.0f);

#else	// UE_BUILD_SHIPPING

	FORCEINLINE bool IsDrawEnabled() { return false; }
	FORCEINLINE bool IsNetRoleShown() { return false; }
	FORCEINLINE void ScreenLog(const FString&, FColor = FColor::White, float = 5.0f) {}

#endif

	/**
	 * 게이트가 켜져 있을 때만 트레이스를 시각화한다.
	 * 호출부에서 EDrawDebugTrace::ForDuration 을 그대로 넘기던 자리를 대체한다.
	 */
	FORCEINLINE EDrawDebugTrace::Type Trace(EDrawDebugTrace::Type WhenEnabled = EDrawDebugTrace::ForDuration)
	{
		return IsDrawEnabled() ? WhenEnabled : EDrawDebugTrace::None;
	}

	/**
	 * 전역 CVar(마스터 스위치)와 컴포넌트별 플래그(필터)를 AND로 결합한다.
	 * 블루프린트에서 컴포넌트 플래그가 켜져 있어도 soul.DrawDebug 0 이면 그리지 않는다.
	 */
	FORCEINLINE bool ShouldDraw(bool bComponentFlag)
	{
		return IsDrawEnabled() && bComponentFlag;
	}

	/** ShouldDraw() 결과에 따라 트레이스 시각화 타입을 반환한다. */
	FORCEINLINE EDrawDebugTrace::Type TraceIf(bool bComponentFlag, EDrawDebugTrace::Type WhenEnabled = EDrawDebugTrace::ForDuration)
	{
		return ShouldDraw(bComponentFlag) ? WhenEnabled : EDrawDebugTrace::None;
	}
}
