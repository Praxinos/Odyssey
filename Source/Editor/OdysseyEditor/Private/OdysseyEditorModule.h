// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOdysseyEditorModule : public IModuleInterface
{

public:
	virtual void ShutdownModule() override;

public:
	//Tabs State Loading / Saving
	void SetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iTabIds);
	const TArray<FName>& GetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);
	FString GetOpenedTabIdsProjectPath() const;
	FString GetOpenedTabIdsSavedPath() const;
	void LoadOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);
	void SaveOpenedTabIds(const FName& iEditorName);

private:
	TMap<FName, TArray<FName>> mOpenedTabIds; //Ids of tabs that should be opened when activating a Mode Editor
};

