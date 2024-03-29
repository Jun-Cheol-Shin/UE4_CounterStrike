// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMatineeCameraShake.h"


UMyMatineeCameraShake::UMyMatineeCameraShake()
{
	bSingleInstance = false;

	
	OscillationDuration = 0.2f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.1f;


	RotOscillation.Pitch.Amplitude = 3.f;
	RotOscillation.Pitch.Frequency = 0.3f;

	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	
}