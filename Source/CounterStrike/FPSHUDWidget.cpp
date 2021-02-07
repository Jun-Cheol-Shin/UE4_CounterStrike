// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUDWidget.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanelSlot.h"

#include "Math/Color.h"
#include "Styling/SlateColor.h"	
#include "Styling/SlateBrush.h"

#include "FPSCharacter.h"
#include "FPSCharacterStatComponent.h"
#include "WGun.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/CString.h"
#include "Kismet/GameplayStatics.h"
#include "CounterStrikeGameModeBase.h"
#include "Components/Button.h"

#include "Blueprint/WidgetTree.h"



void UFPSHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACounterStrikeGameModeBase* Mode = Cast<ACounterStrikeGameModeBase>(GetWorld()->GetAuthGameMode());

	if (Mode)
	{
		Mode->Fuc_DeleSingle_OneParam.BindUFunction(this, FName("SetTimerText"));
	}

	CanvasPanel = this->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Canvas"));

	CanvasSlot = dynamic_cast< UCanvasPanelSlot* >(Crosshair->Slot);
	ImageSlot = dynamic_cast <UCanvasPanelSlot*> (MainRenderImage->Slot);
	UISlot =  dynamic_cast<UCanvasPanelSlot*> (UIOverlay->Slot);

	if (UISlot)
	{
		UISlot->SetSize(FVector2D(SettingViewPortSize().X, 50.0f));
	}

	AK47Button = Cast<UButton>(GetWidgetFromName(TEXT("AK47Button")));
	if (nullptr != AK47Button)
	{
		AK47Button->OnClicked.AddDynamic(this, &UFPSHUDWidget::OnClick);
	}
}

void UFPSHUDWidget::OnClick()
{
	UE_LOG(LogTemp, Warning, TEXT("AK-47"));
}

FVector2D UFPSHUDWidget::SettingViewPortSize()
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameUserSettings->GetScreenResolution());

	return ViewportSize;
}

void UFPSHUDWidget::FlashBang(AFPSCharacter* Player, float DeltaTime)
{
	if (Player)
	{
		if (Flash->GetRenderOpacity() < 1.f && !isFlashing)
		{
			Flash->SetRenderOpacity(1.f);

			if (!GetWorld()->GetTimerManager().IsTimerActive(FlashBack))
			{
				GetWorld()->GetTimerManager().SetTimer(FlashBack, [this]() {

					GetWorld()->GetTimerManager().ClearTimer(FlashBack);
					isFlashing = true;

				}, 1.1f, false);
			}
		}

		else if (isFlashing)
		{
			Flash->SetRenderOpacity(FMath::FInterpTo(Flash->GetRenderOpacity(), 0.f, DeltaTime, 1.f));

			if (FMath::IsNearlyZero(Flash->GetRenderOpacity()))
			{
				Player->bIsFlashBang = false;
				isFlashing = false;
				return;
			}
		}
	}
}

void UFPSHUDWidget::DynamicCrosshair(AFPSCharacter* Player, float DeltaTime)
{
	if (!Player) return;

	FVector2D TopVec = FVector2D::ZeroVector;
	FVector2D BottomVec = FVector2D::ZeroVector;
	FVector2D RightVec = FVector2D::ZeroVector;
	FVector2D LeftVec = FVector2D::ZeroVector;

	if (bUseMoveDynamic)
	{
		TopVec = FVector2D(0, -Player->GetFPSCharacterMovement()->Velocity.Size() * 0.1f);
		BottomVec = FVector2D(0, Player->GetFPSCharacterMovement()->Velocity.Size() * 0.1f);
		RightVec = FVector2D(Player->GetFPSCharacterMovement()->Velocity.Size() * 0.1f, 0);
		LeftVec = FVector2D(-Player->GetFPSCharacterMovement()->Velocity.Size() * 0.1f, 0);
	}

	if (bUseShootDynamic)
	{
		if (Player->GetFPSCharacterStatComponent())
		{
			if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
			{
				TopVec += FVector2D(0, -Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetShotCount() * 2);
				BottomVec += FVector2D(0, Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetShotCount() * 2);
				RightVec += FVector2D(Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetShotCount() * 2, 0);
				LeftVec += FVector2D(-Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetShotCount() * 2, 0);
			}
		}
	}

	TopVec.Y = FMath::Clamp(TopVec.Y, -ClampDynamicValue, 0.f);
	BottomVec.Y = FMath::Clamp(BottomVec.Y, 0.f, ClampDynamicValue);
	RightVec.X = FMath::Clamp(RightVec.X, 0.f, ClampDynamicValue);
	LeftVec.X = FMath::Clamp(LeftVec.X, -ClampDynamicValue, 0.f);

	Top->SetRenderTranslation(FMath::Vector2DInterpTo(Top->RenderTransform.Translation,
		TopVec, DeltaTime, 200.f));
	Bottom->SetRenderTranslation(FMath::Vector2DInterpTo(Bottom->RenderTransform.Translation,
		BottomVec, DeltaTime, 200.f));
	Left->SetRenderTranslation(FMath::Vector2DInterpTo(Left->RenderTransform.Translation,
		LeftVec, DeltaTime, 200.f));
	Right->SetRenderTranslation(FMath::Vector2DInterpTo(Right->RenderTransform.Translation,
		RightVec, DeltaTime, 200.f));
}

void UFPSHUDWidget::InitCharacterHealth(AFPSCharacter* Player)
{
	Player->GetFPSCharacterStatComponent()->InitCharacterHP();
	HealthText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentCharacterHP())));
}

void UFPSHUDWidget::SetArmor(AFPSCharacter* Player)
{
	Player->GetFPSCharacterStatComponent()->PurchaseKavlarAndHelmet();
	ArmorText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentCharacterKevlar())));
}

void UFPSHUDWidget::SetAmmoCount(AFPSCharacter* Player)
{
	if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
	{
		CurrentAmmoText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetCurrentAmmoCount())));
		AmmoText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetElseAmmoCount())));
	}

	else
	{
		CurrentAmmoText->SetText(FText::FromString(TEXT("")));
		AmmoText->SetText(FText::FromString(TEXT("")));
	}
}

void UFPSHUDWidget::SetArmorAndHealth(AFPSCharacter* Player)
{
	HealthText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentCharacterHP())));
	ArmorText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentCharacterKevlar())));
}

void UFPSHUDWidget::InitDollar(AFPSCharacter* Player)
{
	Player->GetFPSCharacterStatComponent()->InitDollar();
	SetDollar(Player);
}

void UFPSHUDWidget::SetDollar(AFPSCharacter* Player)
{
	DollarText->SetText(FText::FromString(FString::FromInt(Player->GetFPSCharacterStatComponent()->GetCurrentCharacterDollar())));
}

void UFPSHUDWidget::SetTimerText(uint8 TimeSec)
{
	uint8 minutes;
	uint8 sec;

	if (TimeSec / 60 < 1)
	{
		minutes = 0;
		sec = TimeSec % 60;
	}

	else
	{
		minutes = TimeSec / 60;
		sec = TimeSec % 60;
	}

	if (sec > 9)
	{
		TimeText->SetText(FText::FromString(FString::FromInt(minutes) + " : " + FString::FromInt(sec)));
	}

	else
	{
		TimeText->SetText(FText::FromString(FString::FromInt(minutes) + " : " + "0" + FString::FromInt(sec)));
	}
}

//void UFPSUIWidget::SetTimerText(uint8 min, uint8 seconds)
//{
//	TimeText->SetText(FText::FromString(FString::FromInt(min) + " : " + FString::FromInt(seconds)));
//}


void UFPSHUDWidget::UseDotSetting()
{
	FLinearColor Color;
	if (!bUseDot)
	{
		Color.A = 0.f;
	}

	else
	{
		Color.R = Color_r;
		Color.G = Color_g;
		Color.B = Color_b;
		Color.A = 1.f;
	}

	Dot->SetColorAndOpacity(Color);
}

void UFPSHUDWidget::SizeCrosshair()
{
	if (!CanvasSlot)
	{
		CanvasSlot = Cast< UCanvasPanelSlot >(Crosshair->Slot);
	}

	FVector2D vector = CanvasSlot->GetSize();

	vector.X = 6.f + Size;
	vector.Y = 6.f + Size;
	CanvasSlot->SetSize(vector);

	Top->Brush.ImageSize.Y = 3.f + Size * 0.5f;
	Bottom->Brush.ImageSize.Y = 3.f + Size * 0.5f;
	Left->Brush.ImageSize.X = 3.f + Size * 0.5f;
	Right->Brush.ImageSize.X = 3.f + Size * 0.5f;
}

void UFPSHUDWidget::ThicknessCrosshair()
{
	FVector2D Vector;
	
	if (Thickness > 0.f)
	{
		Top->Brush.ImageSize.X = Thickness + 1.f;
		Bottom->Brush.ImageSize.X = Thickness + 1.f;

		Left->Brush.ImageSize.Y = Thickness + 1.f;
		Right->Brush.ImageSize.Y = Thickness + 1.f;
	}

	else
	{
		Top->Brush.ImageSize.X = 0;
		Bottom->Brush.ImageSize.X = 0;

		Left->Brush.ImageSize.Y = 0;
		Right->Brush.ImageSize.Y = 0;
	}
}

void UFPSHUDWidget::ColorCrosshair()
{
	FLinearColor color;

	color.A = 1.f;
	color.R = Color_r;
	color.G = Color_g;
	color.B = Color_b;

	Top->SetColorAndOpacity(color);
	Bottom->SetColorAndOpacity(color);
	Left->SetColorAndOpacity(color);
	Right->SetColorAndOpacity(color);

}

void UFPSHUDWidget::GapCrosshair()
{
	if (!CanvasSlot)
	{
		CanvasSlot = Cast< UCanvasPanelSlot >(Crosshair->Slot);
	}

	FVector2D vector = CanvasSlot->GetSize();

	vector.X += Gap * 2.f;
	vector.Y += Gap * 2.f;

	CanvasSlot->SetSize(vector);
}


bool UFPSHUDWidget::OpenShop()
{
	bShopisOpen = !bShopisOpen;

	if (bShopisOpen)
	{
		ShopOverlay->SetVisibility(ESlateVisibility::Visible);

	}

	else
	{
		ShopOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	return bShopisOpen;
}