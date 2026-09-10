# 업그레이드 연결

## 데이터 테이블

BP_SpaceShipActor의 기본 UpgradeComponent에서 다음 에셋을 지정합니다.

| 속성 | DataTable 행 구조체 |
| --- | --- |
| SpaceShipUpgradeTable | FSpaceShipUpgrade |
| LazerUpgradeTable | FLazerUpgrade |
| MachineArmUpgradeTable | FMachineArmUpgrade |

기존 FShipUpgradeRow 테이블은 행 구조체가 다르므로 그대로 지정할 수 없습니다.
새 구조체로 테이블을 만들거나 데이터를 옮겨야 합니다. 수치는 이 코드에서 임의로 정하지 않습니다.

- 행의 바깥 Level은 도달할 레벨입니다. 행 이름은 자유롭게 정할 수 있습니다.
- 내부 Stat.Level은 호환성을 위해 남겨두며, 적용 시 바깥 Level로 맞춥니다.
- Stat에는 증가량이 아니라 해당 레벨의 최종 수치를 입력합니다.
- Ingredients는 해당 레벨로 올라갈 때 소모하는 재료입니다.
- 같은 재료가 여러 항목에 있으면 합산합니다. Quantity는 양수여야 합니다.
- 마지막 행이 최대 레벨입니다. 현재 레벨 이후 행 사이에 빈 레벨이 있으면 데이터 오류로 처리합니다.
- UnlockRecipeIds에는 기존 RecipeTable의 행 이름을 입력합니다. 레시피는 bLockedByDefault=true로 설정합니다.
- 레이저 업그레이드 이후 레벨은 우주선 레벨보다 작아야 합니다.
- 초기 능력치도 이 조건에 맞추세요. 예: 우주선 1, 레이저 0.
- 머신암은 별도 우주선 레벨 제한이 없습니다.

## 인벤토리와 제작 컴포넌트

MainPanel의 BindToSpaceShip 경로가 업그레이드 위젯에 컴포넌트를 전달합니다.
기본 재료 출처는 해당 위젯 소유 플레이어의 인벤토리입니다.

창고도 사용할 경우 BP에서 GetUpgradeComponent → SetInventories로 인벤토리 배열을 지정하세요.
명시적으로 지정된 배열은 MainPanel 바인딩에서 덮어쓰지 않습니다.
플레이어 Pawn이 늦게 생성되거나 교체되는 경우 SetInventories를 다시 호출하세요.

제작 화면이 사용하는 동일한 UCraftingComponent를 GetUpgradeComponent → SetCraftingComponent에 전달하세요.
우주선 자체에 제작 컴포넌트가 있으면 BeginPlay에서 찾아 사용합니다.
해금할 ID가 있는데 제작 연결이나 레시피가 없으면 재료 소비 전에 업그레이드를 거부합니다.

RestoreRecipeUnlocks는 현재 우주선 레벨 이하의 레시피를 다시 해금합니다.
저장 복원으로 레벨을 적용한 뒤 호출하세요. 재료 소비나 내구도/에너지 회복은 하지 않습니다.
기존 제작 화면은 다시 열 때 해금 목록을 읽습니다. 열린 화면 즉시 갱신은
OnUpgradeStateChanged에서 기존 RefreshRecipeListWidget을 연결하면 됩니다.
기존 제작 함수와 제작 UI는 수정하지 않았습니다.

## 상세 위젯

기존 세 WBP의 C++ 부모 클래스 이름은 유지됩니다.
새 공통 부모 UShipUpgradeDetailUserWidget이 요청, 재료 목록, 버튼 상태를 처리합니다.

선택적으로 WBP에 다음 이름의 컨트롤을 추가하면 원하는 위치에 표시할 수 있습니다.

| 이름 | 타입 |
| --- | --- |
| UpgradeButton | Button |
| IngredientList | VerticalBox 등 PanelWidget |
| UpgradeMessage | TextBlock |

누락된 컨트롤은 실행 시 기존 루트 아래에 기본 세로 영역으로 생성합니다.
기존 능력치 텍스트 이름은 변경하지 않았습니다.
레이저의 CurrentDurability/NextDurability에는 Damage가 표시됩니다.
IngredientWidgetClass에 기존 ManufactureIngredientItemWidget 계열 WBP를 지정하면 아이콘 재료 행을 사용합니다.
지정하지 않으면 아이템 이름과 보유량/필요량을 텍스트로 표시합니다.

컴포넌트 변경 알림, 인벤토리 변경, 페이지 열기 시 표시를 다시 계산합니다.
버튼을 눌러도 재검사하며 위젯이 레벨을 직접 올리지 않습니다.
Blueprint에서 OnClicked에 별도의 TryUpgrade 호출을 중복 연결하지 마세요.
업그레이드 성공 시 우주선 내구도와 에너지는 새 최대치로 채워집니다.

## 변경 범위와 검증

주요 구현은 SpaceShip과 Widget/MainPanel에 있습니다.
CommonHeader/SpaceShipStruct.h에는 테이블 행 지원, 입력 필드와 머신암 행을 추가했습니다.
범위 밖의 InventoryComponent에는 기존 명령 경로를 바꾸지 않는 ProcessIngredients/GetSpendableItemCount를,
CraftingComponent에는 읽기 전용 HasRecipe만 추가했습니다.

자동 테스트: ProjectSR.Upgrade.AtomicIngredients, ProjectSR.Upgrade.Progression.
에디터의 Session Frontend → Automation에서 ProjectSR.Upgrade로 검색할 수 있습니다.
상속과 UPROPERTY가 추가되었으므로 저장 후 에디터를 재시작하고 WBP를 컴파일하세요.

