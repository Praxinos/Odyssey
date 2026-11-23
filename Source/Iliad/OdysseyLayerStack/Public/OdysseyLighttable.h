// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyRenderingAbility.h"

#include "OdysseyLighttable.generated.h"

USTRUCT(BlueprintType)
struct FOdysseyLighttableKey
{
    GENERATED_BODY()

    /** Whether this Light Table key is displayed. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    bool bIsActivated = false;

    /** Opacity of this Light Table key, in percent. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float Opacity = 0.f;
};

UENUM(BlueprintType)
enum class EOdysseyLighttableDisplayPosition : uint8
{
    AboveLayer,
    UnderLayer
};

USTRUCT(BlueprintType)
struct FOdysseyLighttable
{
    GENERATED_BODY()

    /**
     * Whether the Light Table is enabled on the layer.
     *
     * The Light Table displays by transparency the content of previous
     * and next Cells (also called onion skin).
     */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    bool bIsActivated = false;

    /**
     * Display position of the Light Table.
     *
     * Force the Light Table to be displayed above or under the current
     * Cell. Useful with colorized Vector Layers.
     */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    EOdysseyLighttableDisplayPosition DisplayPosition = EOdysseyLighttableDisplayPosition::UnderLayer;

    /** Color used to display the previous Cells in the Light Table. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    FLinearColor PreviousKeysColor = FColor::Orange;

    /** Color used to display the next Cells in the Light Table. */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    FLinearColor NextKeysColor = FColor(0, 128, 255);

    /**
     * Contrast applied to the previous Cells, in percent.
     *
     * Help to tell lines and colored areas apart, especially with
     * colorized Vector Layers.
     */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float PreviousKeysContrast = 50.f;

    /**
     * Contrast applied to the next Cells, in percent.
     *
     * Help to tell lines and colored areas apart, especially with
     * colorized Vector Layers.
     */
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float NextKeysContrast = 50.f;

    UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyLighttableFunctionLibrary::GetPreviousKey())
    FOdysseyLighttableKey PreviousKeys[10];

    UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyLighttableFunctionLibrary::GetPreviousKey())
    FOdysseyLighttableKey NextKeys[10];
};

UCLASS()
class UOdysseyLighttableFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Return the settings of a previous Cell key of the Light Table.
     *
     * Index 0 is the closest previous Cell (up to 10 keys).
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttableKey GetPreviousKey(FOdysseyLighttable Lighttable, int Index = 0);

    /**
     * Return the settings of a next Cell key of the Light Table.
     *
     * Index 0 is the closest next Cell (up to 10 keys).
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttableKey GetNextKey(FOdysseyLighttable Lighttable, int Index = 0);

    /**
     * Return a copy of the Light Table with a previous key replaced.
     *
     * Index 0 is the closest previous Cell (up to 10 keys).
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttable SetPreviousKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index = 0);

    /**
     * Return a copy of the Light Table with a next key replaced.
     *
     * Index 0 is the closest next Cell (up to 10 keys).
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttable SetNextKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index = 0);
};
