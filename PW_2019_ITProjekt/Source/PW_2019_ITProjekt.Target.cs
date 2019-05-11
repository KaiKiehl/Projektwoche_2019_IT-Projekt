// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PW_2019_ITProjektTarget : TargetRules
{
	public PW_2019_ITProjektTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "PW_2019_ITProjekt" } );
	}
}
