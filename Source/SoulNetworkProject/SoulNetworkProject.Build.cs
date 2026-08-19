// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulNetworkProject : ModuleRules
{
	public SoulNetworkProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"GameplayTasks", 
			"AIModule", 
			"NavigationSystem", 
			"UMG", 
			"OnlineSubsystem", 
			"OnlineSubsystemSteam", 
			"Niagara", 
			"PhysicsCore",
			"MoviePlayer",
			"MovieScene",
			"LevelSequence"});

		//PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Niagara"));

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "ReplicationGraph" });
		
		// GameplayDebugger 는 Shipping / Test 를 제외한 구성에서만 존재한다.
		if (Target.bBuildDeveloperTools ||
		    (Target.Configuration != UnrealTargetConfiguration.Shipping &&
		     Target.Configuration != UnrealTargetConfiguration.Test))
		{
			PrivateDependencyModuleNames.Add("GameplayDebugger");
		}
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
