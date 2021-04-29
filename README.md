# 목차
* [플레이 영상](https://youtu.be/MGhXNbpuY7g)
* [소개](#소개)
* [구현 내용](#구현-내용)
* [어려웠던 점](#어려웠던-점)

## 소개
* 카운터 스트라이크 온라인 리소스를 이용해 UE4로 제작
* Listen 서버를 이용한 멀티플레이 게임 (최대 4인)
* 언리얼 UI 및 멀티플레이 튜토리얼 참고하여 제작
* 개발 기간 : 20. 1/1 ~ 21. 3/31
* 개발 인원 : 1인

## 구현 내용
* [총기 클래스 구조](#총기-클래스-구조)
* [총기 연사 및 단발 구현](#총기-연사-및-단발-구현)
* [스프레이 패턴 구현](#스프레이-패턴-구현)
* [월 샷 구현](#월-샷-구현)
* [데칼 표현](#데칼-표현)
* [크로스헤어 구현](#크로스헤어-구현)
* [그 외 구현 사항](#그-외-구현-사항)
___

### 총기 클래스 구조
* **WBase**라는 클래스에서 칼, 총, 폭탄으로 파생되고 무기에 따른 **단발,연발 유무, 무기 사정거리 등** 존재
* 총기류는 라이플, 샷건, 권총, 스나이퍼, SMG로 파생되고, 소음기가 달린 무기는 추가로 상속 (M4A1, USP)
* 폭탄류는 데미지를 주는 Grenade와 섬광효과를 주는 Flash로 파생 (**현재 WSmoke 클래스는 삭제**)
<img src="https://user-images.githubusercontent.com/77636255/116535832-2bde4780-a91f-11eb-81ab-501891c2e218.png" width = "800">

* 블루프린트 에디터에서 주로 사용되는 변수를 모두 빼내어 수정하기 용이하게 함. (매직넘버 최소화)
<img src="https://user-images.githubusercontent.com/77636255/116535884-41ec0800-a91f-11eb-8678-779d1cd531a1.png" width = "800">

___

### 총기 연사 및 단발 구현
* **bCanAutoFire** bool 변수를 이용하여 연사 무기와 단발 무기를 분류
* 타이머를 이용해 클릭을 유지 했을 경우 계속 공격이 나가도록 구현
* 타이머가 애니메이션 실행 후 특정 DelayTime에 실행되므로 **애니메이션 길이 - 딜레이 값**을 다음 애니메이션 실행 타이머 시간에 넣어준다.
* 클릭을 하지 않거나 단발 무기인 경우 StopFire 후 Idle로 이동 혹은 Idle로 이동
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

___

### 스프레이 패턴 구현
<img src="https://user-images.githubusercontent.com/77636255/113293428-b471c400-9330-11eb-8ec3-926d43a51118.gif" width = "450"> AK-47| <img src="https://user-images.githubusercontent.com/77636255/113296331-4cbd7800-9334-11eb-87a6-05cbf7f44aef.gif" width = "450"> M4A1-S
:-------------------------:|:-------------------------:

* 총을 쏠 때마다 ShotCount가 하나씩 추가되며 ShotCount의 숫자에 따라 switch문을 따라
* Pitch값이 정해진 RealHitImpactLimit값을 초과 할 수 없으며 초과한 경우 Pitch값은 더 이상 올라가지 않음
* Pitch가 일정 수치에 도달한 경우 Yaw값이 왼쪽 오른쪽으로 이동하도록 만듬

```c++
	switch (ShotCount)
	{
	case 0:
		Player->AddControllerPitchInput(-RandomRecoil * 0.1f);
		break;
	case 1:
	case 2:
	case 3:
		RandomRecoil = FMath::RandRange(0.8f, 1.15f);
		Rotation.Pitch += RandomRecoil;
		RandomRecoil += RecoilWeight;
		Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
		break;

	default:
		if (RandomRecoil < RealHitImpactLimit)
		{
			// 반동 값에 가중치를 더한다
			RandomRecoil += RecoilWeight;
			// 가중치가 증가
			RecoilWeight += .1f;

			// 일정 수치 초과 시 수직 반동이 없어짐.
			if (RandomRecoil > RealHitImpactLimit)
			{
				RandomRecoil = RealHitImpactLimit;
			}

			Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
		}


		// 카메라의 위치 값 조정
		if (Player->IsCrouchHeld)
		{
			Rotation.Pitch += RandomRecoil * 0.6f;
		}

		else
		{
			Rotation.Pitch += RandomRecoil;
		}

		break;
	}

	if (ShotCount > 1)
	{
		// RandomRecoil이 일정 수치를 초과 시 수평 반동의 방향을 결정 해 준다.
		float a = RandomHorizontalDirection();
		Rotation.Yaw += a;
		// 카메라의 위치 값 조정
		Player->AddControllerYawInput(a * 0.15f);
	}
```

___

### 월 샷 구현
<img src="https://user-images.githubusercontent.com/77636255/113298891-4381da80-9337-11eb-877f-89cf31e546ba.gif" width = "450"> | <img src="https://user-images.githubusercontent.com/77636255/113298950-51cff680-9337-11eb-9041-a109eecea6c2.gif" width = "450">
:-------------------------:|:-------------------------:

* 처음 총을 발사 했을 때 캐릭터, 물체에 맞았을 경우 실행 (**빨간 선**)
* 수치를 지정해서(**thickness**) 수치 만큼 이동한 벡터를 생성 (**Start**)
* 총알 쐈던 **반대 방향**으로 Trace를 실행 만약 맞았다면 관통에 성공했으므로 데칼을 생성
* 데칼을 생성한 지점에서 다시 Trace를 실행 (**파란 선**)

```c++
bool AWGun::CheckPenetrationShot(FHitResult Point, FVector Direction)
{
	// 총알이 물체 맞았다면 실행...

	// 물체의 크기가 정해진 두께보다 얇다면...
	float thickness = Point.GetActor()->GetActorScale3D().Size2D() * 150.f;
	FHitResult FinalHit;
	FVector Start = Point.ImpactPoint + Direction * thickness;

	bool bSuccess = false;

	bSuccess = GetWorld()->LineTraceSingleByChannel(FinalHit,
		Start, Start - Direction * thickness,
		ECollisionChannel::ECC_Visibility, FCollisionQueryParams());

	// 관통 성공.. 동시에 물체의 반대편에 데칼 생성
	if (bSuccess)
	{
		if (FinalHit.GetActor())
		{
			if (!FinalHit.GetActor()->IsA(AFPSCharacter::StaticClass()))
			{
				SpawnDecal(FinalHit, EDecalPoolList::EDP_BULLETHOLE);
			}

			else
			{
				SpawnDecal(FinalHit, EDecalPoolList::EDP_BLOOD);
			}

			return true;
		}
	}

	return false;
}
```
___

### 데칼 표현
<img src="https://user-images.githubusercontent.com/77636255/113320590-a0888b00-934d-11eb-96dd-96e01d14b567.gif" width = "500"> 벽이 있을 때| <img src="https://user-images.githubusercontent.com/77636255/113320629-aaaa8980-934d-11eb-9cbe-a1d53570b388.gif" width = "500"> 벽이 없을 때
:-------------------------:|:-------------------------:

* 총에 맞은 액터를 기점으로 총알이 날라온 방향으로 CheckWall 함수를 발동
* CheckWall 함수는 FHitResult, FVector, bool을 받아 Trace를 실행시켜 FHitResult를 반환한다.
```
FHitResult PenetrationResult = CheckWall(Hit, (End - Location).GetSafeNormal() * 200.f, true);
```

* Result값이 없는 경우 다시 한 번 **바닥 방향으로 CheckWall을 실행** 바닥이 있는 경우 바닥에 랜덤한 위치로 데칼 호출
```
PenetrationResult = CheckWall(Hit, -Hit.GetActor()->GetActorUpVector() * 1000.f, true);
	if (PenetrationResult.GetActor())
	{
		FVector Vec = FVector(FMath::RandRange(PenetrationResult.ImpactPoint.X - 50.f, PenetrationResult.ImpactPoint.X + 50.f),
			FMath::RandRange(PenetrationResult.ImpactPoint.Y - 50.f, PenetrationResult.ImpactPoint.Y + 50.f), PenetrationResult.ImpactPoint.Z);
	}
```
___

### 크로스헤어 구현

* 순서대로 민감도, 색상 RGB값, 크기, 가운데 공간, 두께를 지정할 수 있다
<img src="https://user-images.githubusercontent.com/77636255/113444898-bb333080-942f-11eb-829f-21c25dffc24a.PNG">

* 사용자의 취향에 따라 다양한 크로스헤어 설정이 가능

<img src="https://user-images.githubusercontent.com/77636255/113445182-5af0be80-9430-11eb-8722-98c6e181f3f0.PNG" width = "500"> | <img src="https://user-images.githubusercontent.com/77636255/113445209-68a64400-9430-11eb-9237-2524614ae281.PNG" width = "500">
:-------------------------:|:-------------------------:

#### [CharacterMovement 컴포넌트의 가속도 변수를 참조해서 제작](https://github.com/Jun-Cheol-Shin/UE4_CounterStrike/blob/main/Source/CounterStrike/Private/FPSHUDWidget.cpp#L96)

<img src="https://user-images.githubusercontent.com/77636255/113447185-53331900-9434-11eb-8d09-45701fbbf8e6.gif" width = "500">  움직임 보정| <img src="https://user-images.githubusercontent.com/77636255/113447207-5c23ea80-9434-11eb-988b-d793165640ea.gif" width = "500">  사격 보정
:-------------------------:|:-------------------------:
___

### 그 외 구현 사항

* MoveForward, MoveRight의 Value에 따라 CurrentLowerRotation 값을 수정시켜 Hips 본을 회전시킨다.

<img src="https://user-images.githubusercontent.com/77636255/114875610-6d0f2b80-9e38-11eb-9ff5-6f0de5d53f75.gif" width = "500"> 이동방향에 따라 다리의 회전값이 달라진다. | <img src="https://user-images.githubusercontent.com/77636255/114875709-887a3680-9e38-11eb-8b2a-5c1b660ad13d.PNG" width = "500"> 애님 인스턴스 블루프린트
:-------------------------:|:-------------------------:

* 에임 오프셋

<img src="https://user-images.githubusercontent.com/77636255/114878723-7fd72f80-9e3b-11eb-8485-4490deb31a02.gif" width = "500"> 좌우 | <img src="https://user-images.githubusercontent.com/77636255/114878777-8c5b8800-9e3b-11eb-804b-5c8664efdf9b.gif" width = "500"> 상하
:-------------------------:|:-------------------------:
<img src="https://user-images.githubusercontent.com/77636255/114878970-bd3bbd00-9e3b-11eb-9672-bc5e90d6640b.gif" width = "500"> 90도 회전 시 오프셋 초기화 | <img src="https://user-images.githubusercontent.com/77636255/114879367-16a3ec00-9e3c-11eb-92cc-d020dbd535c9.gif" width = "500"> 가상 본으로 무기 위치 조정

* 히트 박스

<img src="https://user-images.githubusercontent.com/77636255/114883888-47862000-9e40-11eb-9b00-a38b765b8927.PNG" width = "500"> 본 셰이프 구성화면 | <img src="https://user-images.githubusercontent.com/77636255/114884231-90d66f80-9e40-11eb-9c87-4c4f0c708139.PNG" width = "500"> 스켈레톤 트리
:-------------------------:|:-------------------------:

* 부위마다 데스 모션을 다르게 실행

<img src="https://user-images.githubusercontent.com/77636255/114885258-705ae500-9e41-11eb-8000-64ccc0eef6cf.gif" width = "500"> | <img src="https://user-images.githubusercontent.com/77636255/114885310-7b157a00-9e41-11eb-867b-b3c8f1636f26.gif" width = "500">
:-------------------------:|:-------------------------:

___

## 어려웠던 점

* 양 클라이언트가 같은 이펙트와 같은 데칼, 같은 애니메이션, 같은 무기를 보도록 동기화하는 작업에서 시간을 많이 소모함.
* 무기를 착용했을 때 그 무기를 착용한 모습을 내 컴퓨터의 내 캐릭터와 상대방 컴퓨터의 내 캐릭터가 같은 무기를 드는 작업을 오래 작업 한 것 같다.
* 무기를 착용할 때 모든 클라이언트한테 이 캐릭터가 이 무기를 착용한다는 것을 알려줘야 하는 것을 깨달음
* 나중에 다른 클라이언트가 들어올 때마다 이 작업을 해줘야 했으므로 해결이 되지 않았다.
* 결론적으로 GameMode에서 대기실에서 정해진 플레이어 수 만큼 플레이어가 있다면 일괄적으로 동기화 작업을 실행해주었다.
___
