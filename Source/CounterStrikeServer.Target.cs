// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class CounterStrikeServerTarget : TargetRules
{
	public CounterStrikeServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "CounterStrikeServer" } );
		bUsesSteam = true;
		GlobalDefinitions.Add("UE4_PROJECT_STEAMGAMEDESC=\"Dedicated Server\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMPRODUCTNAME=\"ue_cs\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMGAMEDIR=\"ue_cs\"");
	}
}
