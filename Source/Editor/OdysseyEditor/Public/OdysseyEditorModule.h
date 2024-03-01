// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOdysseyClipboard;

class ODYSSEYEDITOR_API FOdysseyEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	TSharedPtr<FOdysseyClipboard> GetClipboard() const;

public:
	//Tabs State Loading / Saving
	void SetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iTabIds);
	const TArray<FName>& GetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);

private:
	FString GetOpenedTabIdsProjectPath() const;
	FString GetOpenedTabIdsSavedPath() const;
	void LoadOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);
	void SaveOpenedTabIds(const FName& iEditorName);
	void OnEnginePreExit();

private:
	TMap<FName, TArray<FName>> mOpenedTabIds; //Ids of tabs that should be opened when activating a Mode Editor
	TSharedPtr<FOdysseyClipboard> mClipboard;
};

