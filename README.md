# 목차
* [목차](#목차)
* [A. 소개](#소개)
* [B. 구현 내용](#구현-내용)

## A. 소개

* 개발 기간 : 20. 12 ~ 21. 3/31
* 개발 인원 : 1인 


## B. 구현 내용
* [총기 클래스 구조](#총기-클래스-구조)
* [총기 연사 및 단발 구현](#총기-연사-및-단발-구현)
* [스프레이 패턴 구현](#스프레이-패턴-구현)
* [월 샷 구현](#월-샷-구현)
* [히트박스 및 데미지 구현](#히트박스-및-데미지-구현)
* [근접 공격 구현](#근접-공격-구현)
* [데칼 표현](#데칼-표현)
* [월 샷 구현](#월-샷-구현)
* [RPC을 이용한 이펙트 표현](#RPC을-이용한-이펙트-표현)
* [크로스헤어 구현](#크로스헤어-구현)
* [기타 UI 및 애니메이션](#기타-UI-및-애니메이션)

### 총기 클래스 구조
<img src="https://user-images.githubusercontent.com/77636255/113281699-e1b67600-9320-11eb-960b-094108508016.PNG" width = "800">

* WBase라는 모든 무기 클래스에서 칼, 총, 폭탄으로 파생되어 생성 Base에는 무기에 따른 캐릭터 스피드, 무기 사정거리 등 존재
* 총기류는 라이플, 샷건, 권총, 스나이퍼, SMG로 파생되고, 소음기가 달린 무기는 추가로 상속 (M4A1, USP)
* 폭탄류는 데미지를 주는 Grenade와 섬광효과를 주는 Flash로 파생 (현재 WSmoke 클래스는 없는 것으로 수정됨)

### 총기 연사 및 단발 구현
```C++
GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
	// 타이머 초기화
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	// 애니메이션 공격을 멈추도록 false
	Player->AttackAnimCall = false;

	// RPC 함수를 호출 다른 클라이언트들에게 보이는 공격 애니메이션 설정
	if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
	{
		Player->SyncClientAttack(Player->AttackAnimCall, Player->DelayTime);
	}

	// 만약 마우스를 클릭하고 있다면...
	if (Player->IsAttackHeld)
	{
		// 연사가 가능한 무기라면..
		if (bCanAutoFire)
		{
			// 총을 들고 있고, 총의 현재탄약이 0이라면...
			if ((eWeaponNum == EWeaponNum::E_Rifle ||
				eWeaponNum == EWeaponNum::E_Sub) &&
				Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetCurrentAmmoCount() == 0)
			{
				// 현재 재생하고있는 애니메이션 길이와 무기 공격 후 딜레이를 빼서 Idle 애니메이션이 자연스럽게 이어지도록 WaitTime을 설정
				WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
				Idle();
			}

			else
			{
				Fire();
			}
		}

		// 아닌 경우..
		else
		{
			StopFire();
			WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
			Idle();
		}
	}

	// 클릭을 하지 않고 있다면...
	else
	{
		WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
		Idle();
	}

}, GetAttackDelay(), false);
```

* bCanAutoFire 라는 bool 변수를 이용해 마우스를 꾹 눌러도 공격이 나가도록 설정
* 타이머가 애니메이션 실행 후 특정 DelayTime에 실행되기 때문에 애니메이션 길이 - 딜레이 값을 다음 애니메이션 실행 타이머 시간에 넣어준다.
* 클릭을 하지 않거나 bCanAutoFire가 false인 무기인 경우 StopFire 후 Idle로 이동 혹은 Idle로 이동
