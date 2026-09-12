# 메인 게임 루프 연결

1. 에디터를 재시작하고 `SurvivalLoopActor` 기반 BP를 생성하여 플레이할 레벨에 하나 배치한다.
2. 해당 액터를 실제 선실 중심에 놓고 `InteriorBounds`의 Box Extent를 실내 공간에 맞춘다. 플레이어 액터 중심이 박스 안이면 내부다. 운석 충돌용 SafeArea 구체와는 별개다. 시작할 때 KeepWorldTransform으로 우주선에 붙으므로 처음 배치한 위치를 유지한다.
3. `SpaceShip`과 `Player`를 지정한다. 비워두면 등록된 우주선과 Player 0의 APlayerCharacter를 찾는다. Pawn을 늦게 생성한다면 AutoStart를 끄고 생성 및 초기화 완료 후 StartSurvival을 호출한다.
4. `Maps`에 StartDay/MapData를 등록한다. 1일 항목은 필수이고 날짜 중복은 금지다. 예: 1일 DA_A, 3일 DA_B, 7일 DA_C. 마지막 맵은 이후에도 유지된다. 순서는 상관없다.
5. DayDuration은 산소와 무관한 날짜 진행 시간이다. SurvivalLoop BP에서 원하는 초 단위 값(기본 300초)을 설정한다. 최대 산소, OxygenDrainRate, 산소통, 우주복 변경은 외부 활동 가능 시간만 바꾸며 현재 날이나 다음 날의 길이를 바꾸지 않는다. FirstSolarWindDamage/SolarWindDamageIncrease는 기존 기본값 10/5이며 3일 시작에 10, 6일에 15, 9일에 20의 피해를 준다. 내구도와 피해가 같아도 내구도가 0이 되어 사망한다.
6. 우주선의 초기 내구도가 양수여야 한다. 기존 UpgradeComponent 초기 데이터 설정을 먼저 완료한다.

## 하루 시작 전 초기 스폰 준비

`StartSurvival`은 이제 즉시 플레이를 시작하지 않고 **첫날 준비를 요청**한다. AutoStart를 켜도 준비 단계가 자동으로 실행된다. BP에서 별도로 초기 스폰 완료를 세거나 Delay를 추가할 필요가 없다.

처리 순서는 `StartSurvival / 이전 날 운석 해결 → PreparingDay → 오늘 MapData의 ItemSpawnInitCount개 생성·초기화 완료 → 하루 정산·태양풍 → Playing → OnDayStarted`다. 준비 완료는 비동기 스폰 콜백을 기준으로 확인하며, 이미 로딩되어 즉시 생성되는 경우도 같은 규칙을 따른다. 초기 수량이 0이면 다음 루프 틱에 시작한다. CurrentDay는 준비가 성공한 뒤 증가한다. OnDayPreparationStarted의 Day 인자는 준비 중인 다음 날짜다.

준비 중에는 날짜 타이머가 0으로 유지되고 산소/질식 피해, 플레이어 Pawn 입력과 이동 컴포넌트 틱이 멈춘다. 먼저 생성된 초기 아이템은 이동과 충돌을 끈 채 기다린다. 주기적 아이템·운석 스폰 타이머는 초기 스폰이 모두 성공한 뒤에 시작한다. 전체 월드를 Pause하지 않으므로 비동기 로딩과 준비 UI는 계속 동작한다. BP로 별도 생성한 액터의 자체 타이머·애니메이션까지 정지시키지는 않는다.

### MapData와 풀 설정

- `ItemSpawnInitCount`: 하루 시작 전에 반드시 생성할 아이템 액터 수. 기존처럼 아이템 1개당 액터 1개다. 하루 중에 추가되는 주기적 아이템이나 운석을 전부 미리 생성하는 설정은 아니다.
- SurvivalLoop BP의 `MinimumInitialSpawnRatio`는 준비 성공에 필요한 비율이며 기본 0.8이다. 필요 수량은 올림 처리한다. 예: 초기 수량 10은 8개, 3은 3개, 1은 1개가 성공해야 한다. 기준에 도달하면 남은 비동기 요청은 취소 세대로 넘기고 즉시 하루를 시작한다.
- `ItemSpawnRate`: 초기 아이템을 고르는 데이터/가중치. 사용하는 ItemData의 ItemActorClass와 Mesh가 유효해야 한다. Icon은 지정한 경우 정상 로딩되어야 한다.
- `ItemSpawnDist`: 우주선 SafeArea 반지름보다 커야 한다. 초기 위치는 우주선 위치를 기준으로 안전 구 밖에 배치한다. 안전한 이동 궤도 탐색 후 필요하면 접선 방향을 사용하므로 궤도 탐색 실패로 초기 수량을 조용히 누락하지 않는다.
- Project Settings의 기존 Object Pool 설정에서 해당 ItemActorClass의 풀을 등록한다. 한 클래스의 풀 MaxSize는 최소 성공 수량을 담을 만큼 잡거나 Grow 정책을 사용한다. ReuseOldest로 같은 초기 액터가 다시 선택된 결과는 성공 수에 포함하지 않는다. 성공 수가 설정 비율에 못 미치면 준비 실패다.
- `DayPreparationTimeout`: SurvivalLoop BP에서 설정, 기본 30초. 준비 완료를 기다리는 최대 시간이며 최소 연출 시간이 아니다.

### BP 이벤트 연결

1. SurvivalLoop BP의 **On Day Preparation Started**: 준비 안내 위젯 표시. 메인 패널/인벤토리가 열려 있다면 닫거나 조작 불가로 전환한다. C++가 Pawn 입력과 이동을 막지만 기존 UMG 버튼의 동작은 이 이벤트에서 막아야 한다.
2. 준비 위젯 ProgressBar에는 **Get Day Preparation Progress** 반환값(0~1)을 연결한다. 이는 완료된 초기 액터 수 비율이지 바이트 로딩 비율이 아니다.
3. **On Day Started**: 준비 위젯 제거, 게임 UI 갱신. C++가 기존 Pawn 입력/이동 상태를 복원한다. 이 이벤트에서 StartSurvival 또는 SetSpaceMapData를 다시 호출하지 않는다.
4. **On Day Preparation Failed**: Reason 문자열 표시. 이때 State는 PreparationFailed이고 시간/산소/입력은 계속 멈춘다. 재시도 버튼에서 **Retry Day Preparation**을 호출한다. 영구적인 에셋 경로/풀 설정 오류는 설정을 고친 뒤 재시작해야 한다. 실패한 준비에서는 날짜 증가나 운영비 차감이 발생하지 않는다.
5. **On Game Over**에서도 준비 위젯을 제거한다. 초기 스폰은 성공했지만 해당 날짜 태양풍으로 사망하면 OnDayStarted 대신 OnGameOver가 발생한다.

기존 BP의 SetSpaceMapData/초기 아이템 스폰/운석 타이머 호출은 제거한다. 생존 루프가 등록된 상태의 SetSpaceMapData 직접 호출은 중복 스폰 방지를 위해 거부한다. 실패·재시도·종료 후 도착한 이전 준비의 비동기 로딩은 풀 액터를 새로 생성하지 않도록 취소 세대를 확인한다.

## 산소·HP·하루 에너지 정산

1. 플레이어 BP의 StatComponent를 선택한다. OxygenDrainRate를 **1**로 명시적으로 설정한다. 기존 BP에 저장된 2는 C++ 기본값 변경만으로 바뀌지 않을 수 있다. BaseMaxOxygen은 기본 150, BaseMaxHealth는 기본 100이다. NoOxygenDamageRate는 기본 10/초이며 조절 가능하다.
2. 실내 산소 자동 회복은 제거되었다. OxygenRecoverRate는 이전 BP 참조 호환용으로만 남고 값은 사용하지 않는다. BP Tick/타이머에서 별도 산소 회복을 호출했다면 제거한다.
3. SurvivalLoop BP의 LowEnergyOxygenRatio는 0.7, DailyHealthRecoveryRatio는 0.5로 설정한다. HP 일일 회복을 끄려면 0을 입력한다. HP를 50%로 설정하는 것이 아니라 최대 HP의 50%를 더하고 최대치에서 제한한다.
4. 선체 초기 데이터 또는 선체 강화 테이블의 SpaceShipStat에 MaxEnergy와 OperationalEnergy를 입력한다. OperationalEnergy는 매일 한 번 차감하는 선체 운영비다. 레이저/머신암 운영비는 이번 계산에 합산하지 않는다. 예: MaxEnergy=30, OperationalEnergy=10.
5. 1일 시작은 운영비를 차감하지 않는다. 2일 시작부터 현재 에너지가 운영비 이상이면 전액 차감하고 산소를 최대치까지 충전한다. 부족하면 남은 에너지를 0까지 소비하고 최대 산소의 70%까지 충전한다. 이미 그보다 많은 산소가 있으면 유지한다. 에너지=운영비이면 정상 충전이다.
6. 충전과 HP 회복은 위치에 관계없이 날짜 전환의 초기 스폰 준비가 성공한 뒤 적용한다. 외출을 강제로 종료하거나 회복을 위해 귀환을 강제하지 않는다. 다만 진행 중 운석이 있으면 기존처럼 해결 후 준비를 시작하므로 회복도 늦어진다. WaitingForMeteor에서도 외부 산소/질식 피해는 계속되며 PreparingDay/PreparationFailed에서는 멈춘다.
7. OnDayStarted에는 UI 갱신만 연결한다. 산소 충전, HP 회복, 운영비 차감, 태양풍 피해를 BP에서 중복 실행하지 않는다. 이벤트는 정산과 태양풍 피해가 모두 끝난 뒤, 생존한 경우 방송된다. 태양풍으로 사망한 날에는 OnGameOver를 사용한다.

안전 상태는 IsPlayerSafe 노드로 읽는다. 좁은 실제 선실 안에 있고 문이 완전히 닫혔을 때만 안전하다. 문 열림/닫히는 중 또는 외부에서는 산소가 줄고, 고갈 이후 HP가 감소한다. 안전한 선실에서는 산소 0이어도 질식 피해가 멈춘다. 생존 루프가 등록된 플레이어는 StatComponent가 실제 안전 상태를 직접 확인하므로 중력 테스트 입력으로 산소 소모를 우회하지 못한다.

## 우주복 산소 증가 연결

기존 UEquipItemAction은 이미 EquipmentData.StatModifiers를 읽어 RecalculateMaxStats를 호출한다. 별도 자동 스탯 적용 이벤트를 중복 연결하지 않는다.

- UEquipmentDataAsset 기반 단계별 우주복 데이터에 EquipmentSlot=SpaceSuit를 설정한다.
- StatModifiers.OxygenBonus를 0/30/60/90처럼 입력하면 기본 150 기준 최대 산소가 150/180/210/240이 된다. 누적 증가분이 아니라 기본값에 더할 최종 보너스다.
- 아이템 사용 경로는 기존 EquipItemAction의 EquipmentData에 해당 단계 데이터를 지정한다. 기존 액터 클래스/메시 장착 설정은 유지한다. OxygenDrainMultiplier는 현행 RecalculateMaxStats에서 사용하지 않으므로 산소량 증가에는 OxygenBonus를 사용한다.
- 메인 패널의 별도 강화 버튼을 사용한다면 재료 소비 성공 후 `Player → Get Component By Class(StatComponent) → Recalculate Max Stats`에 해당 우주복 데이터의 **전체 StatModifiers**를 전달한다. 이동 보너스도 변경했다면 Player의 RefreshMovementSpeed도 호출한다. 기존 아이템 액션 경로를 호출했다면 이 과정을 다시 실행하지 않는다.
- 최대치만 증가하며 현재 산소는 자동 충전되지 않는다. 다음 날 정산부터 새 용량을 사용하지만 DayDuration은 변하지 않는다. 새 제작법/재료 비용/강화 버튼 에셋은 이 변경에서 만들지 않았다.
- 시작 우주복을 BP로 적용한다면 플레이어 BeginPlay 초기화 후, StartSurvival 전에 적용한다. 이 경우 AutoStart를 끄고 초기화 완료 시 한 번 StartSurvival을 호출하면 순서가 명확하다.

## 운석과 아이템

- 기존 MapData의 MeteorSpawnTime마다 발생 확률을 판정한다. MeteorSpawnDelayTime은 하루 시작 후 첫 판정 지연이다. MeteorSpawnTime <= 0이면 발생하지 않는다.
- MeteorSpawnRate는 기존 코드처럼 **0~1**이다. 0.25=25%, 1=100%. 기존 헤더의 [%] 주석과 달리 25를 입력하면 100%가 된다.
- 하루에도 여러 번 발생한다. 기존 회피 컴포넌트는 운석 하나만 관리하므로 경고/로딩/비행 중에는 다음 판정을 건너뛴다.
- 회피 실패 후 기존 비동기 아이템 팩토리로 생성한다. MeteorData에는 MeteorItemActor 계열과 올바른 풀 설정을 사용한다. MeteorActorClass 필드는 기존 팩토리 경로에서 사용하지 않는다.
- 생성 시 현재 레이저 Damage를 한 번 차감한다. 0 이하이면 풀로 반환하며 충돌을 활성화하지 않는다. OnMeteorSpawn은 이미 레이저 방어가 적용된 생존 운석의 연출용 통지다. 이 통지에서 피해를 다시 차감하지 않는다.
- 날짜가 끝나면 새 아이템/운석 판정을 멈추고 진행 중 운석이 해결된 뒤 전환한다. 늦게 완료된 이전 날짜 아이템 로딩은 풀로 반환한다. 맵 적용 시 이전 아이템을 정리하고 새 스폰 설정을 적용한다.
- 기존 테스트 BP에서 SetSpaceMapData/MeteorDetect를 별도 타이머로 호출했다면 메인 루프를 사용하는 레벨에서는 그 호출을 제거한다.

## 중력과 게임 오버

- 내부이면서 문이 완전히 닫혀야 중력이다. 문이 움직이는 동안과 외부는 무중력이다.
- 실제 운석 충돌 순간 외부 또는 문 미폐쇄 상태의 플레이어는 HP가 0이 되고 게임 오버가 된다. 실내+문 완전 닫힘일 때만 운석 충돌 즉사를 피한다.
- 내구도 0 또는 플레이어 사망이면 상태를 GameOver로 바꾸고 스폰을 중지하며 월드를 일시정지한다. 중복 종료는 무시한다.
- `OnDayStarted`를 날짜/맵 UI에, `OnGameOver`를 결과 위젯 생성과 입력 모드 설정에 연결한다. 이 변경은 결과 위젯 에셋을 자동 생성하지 않는다. 재시작은 현재 레벨 재로드로 처리한다.
- `RemainingDayTime`, `CurrentDay`, `CurrentMap`, `State`는 BP에서 읽을 수 있다. WaitingForMeteor에서는 남은 시간이 0으로 유지된다.
- `DayDuration`은 오늘의 전체 시간, `bLastDailyEnergySufficient`는 마지막 정산의 정상 충전 여부다. BP에는 Last Daily Energy Sufficient로 표시된다. 날짜 UI 갱신 시 읽어 에너지 부족 알림을 표시한다. 에너지·산소·HP 바는 기존 OnEnergyChange/OnOxygenChanged/OnHealthChanged 이벤트를 그대로 사용한다.

## 검증

Unreal Automation의 `ProjectSR.Survival` 그룹: 날짜 전환과 맵 선택, 3/6/9일 태양풍 피해, 내부 생존/외부 사망, 중력 전환, 게임 오버 후 날짜 정지, 운석 접근/통과 및 풀 반환을 검사한다. DailyResources는 산소 미회복, 질식 경계, 운영비 동률/부족, 70% 충전, HP 회복, 우주복 변경의 다음 날 반영, 문 미폐쇄 충돌 사망을 검사한다.
실제 선실 크기, 문 메시 회전, 아이템/운석 데이터 에셋, HUD 연결은 대상 레벨의 PIE에서 확인한다.

DayPreparation 테스트는 초기 액터 수와 준비 중 비활성 상태, 플레이어 입력/질식 정지, 준비 완료 후 활성화, 월드 원점에서 벗어난 선체 기준 배치, 실패 후 같은 날짜 재시도와 운영비 중복 차감 방지를 검사한다.
