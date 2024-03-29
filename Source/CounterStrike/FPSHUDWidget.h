// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/Overlay.h"
#include "Global.h"
#include "Components/VerticalBox.h"
#include "FPSHUDWidget.generated.h"

/**
 * 
 */

class UOverlay;
class UImage;
class UTextBlock;
class AFPSCharacter;
class UWidgetTree;

UCLASS(meta = (DisableNativeTick))
class COUNTERSTRIKE_API UFPSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	FTimerHandle FlashBack;

	bool isFlashing = false;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* CanvasPanel;

	//UPROPERTY(meta = (BindWidget))
	//	class UImage* MainRenderImage;

	UCanvasPanelSlot* CanvasSlot = nullptr;
	//UCanvasPanelSlot* ImageSlot;
	UCanvasPanelSlot* UISlot;
	UCanvasPanelSlot* DamageSlot = nullptr;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		class UOverlay* Crosshair;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* UIOverlay;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* DamageOverlay;

	// crosshair...

	UPROPERTY(meta = (BindWidget))
		UImage* Dot;
	UPROPERTY(meta = (BindWidget))
		UImage* Top;
	UPROPERTY(meta = (BindWidget))
		UImage* Bottom;
	UPROPERTY(meta = (BindWidget))
		UImage* Left;
	UPROPERTY(meta = (BindWidget))
		UImage* Right;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ArmorText;
	//UPROPERTY(meta = (BindWidget))
	//	UTextBlock* TimeText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AmmoText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CurrentAmmoText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DollarText;

	UPROPERTY(meta = (BindWidget))
		UImage* Scope;

	UPROPERTY(meta = (BindWidget))
		class UImage* Flash;


	UPROPERTY(meta = (BindWidget))
		class UImage* FrontDamage;
	UPROPERTY(meta = (BindWidget))
		class UImage* BackDamage;
	UPROPERTY(meta = (BindWidget))
		class UImage* RightDamage;
	UPROPERTY(meta = (BindWidget))
		class UImage* LeftDamage;


public:
	void FlashBang(AFPSCharacter* Player, float DeltaTime);

	virtual void NativeConstruct() override;

	void InitCharacterHealth(AFPSCharacter* Player);
	void SetArmor(AFPSCharacter* Player);
	void SetAmmoCount(AFPSCharacter* Player);
	void SetArmorAndHealth(AFPSCharacter* Player);
	void InitDollar(AFPSCharacter* Player);

	void SetDollar(AFPSCharacter* Player);

	/*UFUNCTION()
		void SetTimerText(uint8 TimeSec);*/

	//void SetTimerText(uint8 min, uint8 seconds);

	// Crosshair Size Setting..
	UFUNCTION(BlueprintCallable)
		void SizeCrosshair();
	// Crosshair Thickness Setting...
	UFUNCTION(BlueprintCallable)
		void ThicknessCrosshair();
	UFUNCTION(BlueprintCallable)
		void ColorCrosshair();
	UFUNCTION(BlueprintCallable)
		void GapCrosshair();


	void DynamicCrosshair(AFPSCharacter* Player, float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Thickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Gap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Color_R;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Color_G;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Color_B;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Color_A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseMoveDynamic = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseShootDynamic = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseDot = true;

	UPROPERTY(EditAnywhere)
		float ClampDynamicValue = 60.f;

	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* UIText = nullptr;


public:
	UFUNCTION(BlueprintCallable)
		void Init(AFPSCharacter* ThisCharacter);

	UOverlay* GetCrosshair() { return Crosshair; }
	UImage* GetScope() { return Scope; }
	//UImage* GetFPSRender() { return MainRenderImage; }

	//UCanvasPanelSlot* GetImageSlot() { return ImageSlot; }
	FVector2D SettingViewPortSize();


	void SetDamageUI(EDamagedDirectionType Type);
};
