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
	bool bShopisOpen = false;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
		class UImage* MainRenderImage;

	UCanvasPanelSlot* CanvasSlot = nullptr;
	UCanvasPanelSlot* ImageSlot;
	UCanvasPanelSlot* UISlot;

protected:
	UPROPERTY(meta = (BindWidget))
		class UButton* AK47Button;
	UPROPERTY(meta = (BindWidget))
		class UButton* M4A1Button;
	UPROPERTY(meta = (BindWidget))
		class UButton* AWPButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* SSGButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* MAC10Button;
	UPROPERTY(meta = (BindWidget))
		class UButton* NovaButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* KevlarButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* KevlarHelmetButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* GlockButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* UspButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* DeagleButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* GrenadeButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* SmokeButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* FlashButton;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* Crosshair;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* UIOverlay;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* ShopOverlay;

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
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TimeText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AmmoText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CurrentAmmoText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DollarText;

	UPROPERTY(meta = (BindWidget))
		UImage* PlusImage;
	UPROPERTY(meta = (BindWidget))
		UImage* MinusImage;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AddDollar;

	UPROPERTY(meta = (BindWidget))
		UImage* Scope;

	UPROPERTY(meta = (BindWidget))
		class UImage* Flash;

public:

	void OnClick();

	void FlashBang(AFPSCharacter* Player, float DeltaTime);

	virtual void NativeConstruct() override;

	void InitCharacterHealth(AFPSCharacter* Player);
	void SetArmor(AFPSCharacter* Player);
	void SetAmmoCount(AFPSCharacter* Player);
	void SetArmorAndHealth(AFPSCharacter* Player);
	void InitDollar(AFPSCharacter* Player);

	void SetDollar(AFPSCharacter* Player);

	UFUNCTION()
		void SetTimerText(uint8 TimeSec);

	//void SetTimerText(uint8 min, uint8 seconds);

	// Setting Dot..
	UFUNCTION(BlueprintCallable)
		void UseDotSetting();
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

	UPROPERTY(EditAnywhere)
		float Thickness;
	UPROPERTY(EditAnywhere)
		float Size;
	UPROPERTY(EditAnywhere)
		float Gap;

	UPROPERTY(EditAnywhere)
		float Color_r;
	UPROPERTY(EditAnywhere)
		float Color_g;
	UPROPERTY(EditAnywhere)
		float Color_b;

	UPROPERTY(EditAnywhere)
		bool bUseDot = false;
	UPROPERTY(EditAnywhere)
		bool bUseMoveDynamic = true;
	UPROPERTY(EditAnywhere)
		bool bUseShootDynamic = true;

	UPROPERTY(EditAnywhere)
		float ClampDynamicValue = 60.f;

	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* UIText = nullptr;


public:
	bool OpenShop();


	bool GetbIsShopOpen() { return bShopisOpen; }
	UOverlay* GetCrosshair() { return Crosshair; }
	UImage* GetScope() { return Scope; }
	UImage* GetFPSRender() { return MainRenderImage; }

	UCanvasPanelSlot* GetImageSlot() { return ImageSlot; }
	FVector2D SettingViewPortSize();
};
