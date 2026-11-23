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
    /**
     * Read a string value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigString( const FString& Section, const FString& Key, FString& Value , const FString& Filename) const;

    /**
     * Read a text value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigText( const FString& Section, const FString& Key, FText& Value , const FString& Filename) const;

    /**
     * Read an integer value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigInt(const FString& Section, const FString& Key, int32& Value, const FString& Filename) const;

    /**
     * Read a float value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigFloat(const FString& Section, const FString& Key, float& Value, const FString& Filename) const;

    /**
     * Read a double value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigDouble(const FString& Section, const FString& Key, double& Value, const FString& Filename) const;

    /**
     * Read a boolean value from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return true if the key was found.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    bool GetConfigBool( const FString& Section, const FString& Key, bool& Value , const FString& Filename) const;

    /**
     * Read an array of strings from a config file.
     *
     * Look up the key in the section of the given ini file.
     * Return the number of values read.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    int32 GetConfigArray(const FString& Section, const FString& Key, TArray<FString>& Value, const FString& Filename) const;

    /**
     * Write a string value to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigString(const FString& Section, const FString& Key, const FString& Value, const FString& Filename);

    /**
     * Write a text value to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigText(const FString& Section, const FString& Key, const FText& Value, const FString& Filename);

    /**
     * Write a float value to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigFloat(const FString& Section, const FString& Key, float Value, const FString& Filename);

    /**
     * Write a double value to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigDouble(const FString& Section, const FString& Key, double Value, const FString& Filename);

    /**
     * Write a boolean value to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigBool(const FString& Section, const FString& Key, bool Value, const FString& Filename);

    /**
     * Write an array of strings to a config file.
     *
     * Set the key in the section of the given ini file.
     */
    UFUNCTION(BlueprintCallable, Category = "Config")
    void SetConfigArray(const FString& Section, const FString& Key, const TArray<FString>& Value, const FString& Filename);

    /** Editor ini file locations - stored per engine version (shared across all projects). Migrated between versions on first run. */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorIni() const { return GEditorIni; }
    /** Get the path of the editor key bindings ini file (EditorKeyBindings). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorKeyBindingsIni() const { return GEditorKeyBindingsIni; }
    /** Get the path of the editor layout ini file (EditorLayout). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorLayoutIni() const { return GEditorLayoutIni; }
    /** Get the path of the editor settings ini file (EditorSettings). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorSettingsIni() const { return GEditorSettingsIni; }

    /** Editor per-project ini files - stored per project. */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetEditorPerProjectIni() const { return GEditorPerProjectIni; }

    /** Get the path of the compatibility ini file (Compat). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetCompatIni() const { return GCompatIni; }
    /** Get the path of the Lightmass ini file (Lightmass). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetLightmassIni() const { return GLightmassIni; }
    /** Get the path of the scalability ini file (Scalability). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetScalabilityIni() const { return GScalabilityIni; }
    /** Get the path of the hardware ini file (Hardware). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetHardwareIni() const { return GHardwareIni; }
    /** Get the path of the input ini file (Input). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetInputIni() const { return GInputIni; }
    /** Get the path of the game ini file (Game). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameIni() const { return GGameIni; }
    /** Get the path of the game user settings ini file (GameUserSettings). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameUserSettingsIni() const { return GGameUserSettingsIni; }
    /** Get the path of the runtime options ini file (RuntimeOptions). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetRuntimeOptionsIni() const { return GRuntimeOptionsIni; }
    /** Get the path of the install bundle ini file (InstallBundle). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetInstallBundleIni() const { return GInstallBundleIni; }
    /** Get the path of the device profiles ini file (DeviceProfiles). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetDeviceProfilesIni() const { return GDeviceProfilesIni; }
    /** Get the path of the gameplay tags ini file (GameplayTags). */
    UFUNCTION(BlueprintCallable, Category = "Config")
    FString GetGameplayTagsIni() const { return GGameplayTagsIni; }
};
