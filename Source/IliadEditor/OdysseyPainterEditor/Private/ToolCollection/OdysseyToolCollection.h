// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "ToolConfiguration/OdysseyPainterEditorToolConfiguration.h"
#include "OdysseyToolCollection.generated.h"

/**
 * Odyssey Tool Collection
 */
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyToolCollection : public UObject
{
    DECLARE_MULTICAST_DELEGATE(FOnCollectionChanged);

    GENERATED_UCLASS_BODY()

public:
    bool IsCollectionTransient() const;

    UOdysseyPainterEditorToolConfiguration* AddToolConfiguration(UClass* iToolClass, FToolPropertySnapshot& iSnapshotConfig, FSlateBrush &iIcon, int32 iIndex = INDEX_NONE);
    void RemoveToolConfigurationAtIndex( int iIndex );
    void RemoveToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig);
    void MoveToolConfiguration(int32 iFromIndex, int32 iToIndex);

    /* Returns the index of iTool in mToolsConfig, if it's in it */
    int32 GetIndexOfToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig);

    /* Check by comparison of similarities (class and properties) */
    bool ContainsSimilarToolConfiguration(UClass* iToolClass, FToolPropertySnapshot& iSnapshotConfig);

    const TArray<UOdysseyPainterEditorToolConfiguration*> GetToolConfigurations() const;

public:
    FOnCollectionChanged OnCollectionChanged;

private:
    UPROPERTY()
    TArray<TObjectPtr<UOdysseyPainterEditorToolConfiguration>> mToolsConfig;
};
