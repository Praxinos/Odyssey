// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "EditorSubsystem.h"
#include "CoreGlobals.h"

#include "OdysseyConfigSubsystem.generated.h"

UCLASS()
class UOdysseyConfigSubsystem :
    public UEditorSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigString( const FString& Section, const FString& Key, FString& Value , const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigText( const FString& Section, const FString& Key, FText& Value , const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigInt(const FString& Section, const FString& Key, int32& Value, const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigFloat(const FString& Section, const FString& Key, float& Value, const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigDouble(const FString& Section, const FString& Key, double& Value, const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigBool( const FString& Section, const FString& Key, bool& Value , const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    int32 GetConfigArray(const FString& Section, const FString& Key, TArray<FString>& Value, const FString& Filename) const;

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigString(const FString& Section, const FString& Key, const FString& Value, const FString& Filename);

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigText(const FString& Section, const FString& Key, const FText& Value, const FString& Filename);

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigFloat(const FString& Section, const FString& Key, float Value, const FString& Filename);

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigDouble(const FString& Section, const FString& Key, double Value, const FString& Filename);

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigBool(const FString& Section, const FString& Key, bool Value, const FString& Filename);

    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigArray(const FString& Section, const FString& Key, const TArray<FString>& Value, const FString& Filename);

    /** Editor ini file locations - stored per engine version (shared across all projects). Migrated between versions on first run. */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorIni() const { return GEditorIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorKeyBindingsIni() const { return GEditorKeyBindingsIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorLayoutIni() const { return GEditorLayoutIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorSettingsIni() const { return GEditorSettingsIni; }

    /** Editor per-project ini files - stored per project. */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorPerProjectIni() const { return GEditorPerProjectIni; }

    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetCompatIni() const { return GCompatIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetLightmassIni() const { return GLightmassIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetScalabilityIni() const { return GScalabilityIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetHardwareIni() const { return GHardwareIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetInputIni() const { return GInputIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameIni() const { return GGameIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameUserSettingsIni() const { return GGameUserSettingsIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetRuntimeOptionsIni() const { return GRuntimeOptionsIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetInstallBundleIni() const { return GInstallBundleIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetDeviceProfilesIni() const { return GDeviceProfilesIni; }
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameplayTagsIni() const { return GGameplayTagsIni; }
};
