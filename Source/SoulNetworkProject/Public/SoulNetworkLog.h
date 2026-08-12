// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*
 * SoulNetworkLog
 * ----------------------------------------------------------------------------
 * 프로젝트 전역 로그 카테고리.
 * 헤더 비용을 최소화하기 위해 로그만 필요한 파일은 이 헤더만 포함한다.
 * 디버그 도형 / 화면 로그가 필요하면 SoulNetworkDebug.h 를 포함할 것.
 *
 * 사용 예:  UE_LOG(LogSoulNetwork, Warning, TEXT("Session created: %s"), *Name);
 * 필터링:   콘솔에서  Log LogSoulNetwork Verbose
 */

DECLARE_LOG_CATEGORY_EXTERN(LogSoulNetwork, Log, All);	// 세션 / 리플리케이션 / 조인
DECLARE_LOG_CATEGORY_EXTERN(LogSoulCombat, Log, All);	// 전투 / 데미지 계산
DECLARE_LOG_CATEGORY_EXTERN(LogSoulAI, Log, All);		// Behavior Tree / AI
DECLARE_LOG_CATEGORY_EXTERN(LogSoulItem, Log, All);		// 인벤토리 / 아이템
DECLARE_LOG_CATEGORY_EXTERN(LogSoulUI, Log, All);		// 위젯 / 메뉴
DECLARE_LOG_CATEGORY_EXTERN(LogSoulGame, Log, All);		// 그 외 게임플레이
