# SoulNetworkProject

UE 4.27 · 리슨 서버 멀티플레이 소울라이크 · C++ 46,861줄

플레이 영상: [(링크)](https://youtu.be/GUYUvON7JdE)
포트폴리오: (PDF 링크)

## 담당 범위
(팀이면 본인이 한 부분을 명확히)

## 주요 구현 위치
| 항목 | 파일 |
|---|---|
| 서버 권위 무기 판정 | `Private/Classes/CombatComponent.cpp` |
| Replication Graph | `Private/SoulNetworkReplicationGraph.cpp` |
| 세션 생성 · 매치메이킹 | `SoulNetworkProjectGameInstance.cpp` |
| 락온 타게팅 | `Private/Classes/LockonSystemComponent.cpp` |
| 디버그 인프라 | `Public/SoulNetworkDebug.h` |

## 빌드
Content 에셋은 라이선스 문제로 제외되어 있어 빌드는 되지 않습니다.
실행 파일: (Drive 링크)
