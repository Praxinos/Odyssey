// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "OdysseyPainterEditorToolConfiguration.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyToolCollection.generated.h"

USTRUCT()
struct FToolPropertySnapshot
{
    GENERATED_BODY()

    // One entry per property
    UPROPERTY()
    TMap<FName, FInstancedStruct> Values;

    bool operator==(const FToolPropertySnapshot& iOther) const;
    bool operator!=(const FToolPropertySnapshot& iOther) const
    {
        return !(*this == iOther);
    }
};

/**
 * Odyssey Tool Collection
 */
UCLASS(BlueprintType, meta = (EditorIcon = "ClassIcon.OdysseyToolCollection"))
class ODYSSEYPAINTEREDITOR_API UOdysseyToolCollection : public UObject
{
    DECLARE_MULTICAST_DELEGATE(FOnCollectionChanged);

    GENERATED_UCLASS_BODY()

public:
    /**
     * Some Tool Collection can be transient (not saved in the asset)
     * Example : The Recent Tools category is a transient Tool Collection
     */
    bool IsCollectionTransient() const;

    UOdysseyPainterEditorToolConfiguration* AddToolConfiguration(UClass* iToolClass, TObjectPtr<UOdysseyPainterEditorTool> iTool, FIconToolConfiguration& iIconToolConfiguration, int32 iIndex = INDEX_NONE);
    void RemoveToolConfigurationAtIndex(int iIndex);
    void RemoveToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig);
    void MoveToolConfiguration(int32 iFromIndex, int32 iToIndex);

    /* Returns the index of iTool in mToolsConfig, if it's in it */
    int32 GetIndexOfToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig);

    /* Check by comparison of similarities (class and properties) */
    bool ContainsSimilarToolConfiguration(UClass* iToolClass, UOdysseyPainterEditorTool* iTool);

    const TArray<UOdysseyPainterEditorToolConfiguration*> GetToolConfigurations() const;

protected:
    static void ConvertObjectToPropertySnapshot(UObject* iObject, FToolPropertySnapshot& oSnapshot, TSet<const UObject*> Visited = TSet<const UObject*>());

public:
    FOnCollectionChanged OnCollectionChanged;

private:
    UPROPERTY()
    TArray<TObjectPtr<UOdysseyPainterEditorToolConfiguration>> mToolsConfig;
};
