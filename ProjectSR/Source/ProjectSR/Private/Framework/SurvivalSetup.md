# 메인 게임 루프 연결

1. 에디터를 재시작하고 `SurvivalLoopActor` 기반 BP를 생성하여 플레이할 레벨에 하나 배치한다.
2. 해당 액터를 실제 선실 중심에 놓고 `InteriorBounds`의 Box Extent를 실내 공간에 맞춘다. 플레이어 액터 중심이 박스 안이면 내부다. 운석 충돌용 SafeArea 구체와는 별개다. 시작할 때 KeepWorldTransform으로 우주선에 붙으므로 처음 배치한 위치를 유지한다.
3. `SpaceShip`과 `Player`를 지정한다. 비워두면 등록된 우주선과 Player 0의 APlayerCharacter를 찾는다. Pawn을 늦게 생성한다면 AutoStart를 끄고 생성 및 초기화 완료 후 StartSurvival을 호출한다.
4. `Maps`에 StartDay/MapData를 등록한다. 1일 항목은 필수이고 날짜 중복은 금지다. 예: 1일 DA_A, 3일 DA_B, 7일 DA_C. 마지막 맵은 이후에도 유지된다. 순서는 상관없다.
5. DayDuration(초), FirstSolarWindDamage, SolarWindDamageIncrease를 설정한다. 기본값 300초/10/5는 테스트용이며 실제 밸런스에 맞춰 바꾼다. 3일 시작에 10, 6일에 15, 9일에 20의 내구도 피해가 발생한다.
6. 우주선의 초기 내구도가 양수여야 한다. 기존 UpgradeComponent 초기 데이터 설정을 먼저 완료한다.

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
- 기존 플레이어/스탯/이동 코드는 수정하지 않고 공개 함수 및 OnPlayerDeath를 사용한다.
- 실제 운석 충돌 순간 외부 플레이어는 HP가 0이 되고 게임 오버가 된다. 내부 플레이어는 운석 충돌만으로 즉사하지 않는다.
- 내구도 0 또는 플레이어 사망이면 상태를 GameOver로 바꾸고 스폰을 중지하며 월드를 일시정지한다. 중복 종료는 무시한다.
- `OnDayStarted`를 날짜/맵 UI에, `OnGameOver`를 결과 위젯 생성과 입력 모드 설정에 연결한다. 이 변경은 결과 위젯 에셋을 자동 생성하지 않는다. 재시작은 현재 레벨 재로드로 처리한다.
- `RemainingDayTime`, `CurrentDay`, `CurrentMap`, `State`는 BP에서 읽을 수 있다. WaitingForMeteor에서는 남은 시간이 0으로 유지된다.

## 검증

Unreal Automation의 `ProjectSR.Survival` 그룹: 날짜 전환과 맵 선택, 3/6/9일 태양풍 피해, 내부 생존/외부 사망, 중력 전환, 게임 오버 후 날짜 정지, 운석 접근/통과 및 풀 반환을 검사한다.
실제 선실 크기, 문 메시 회전, 아이템/운석 데이터 에셋, HUD 연결은 대상 레벨의 PIE에서 확인한다.
