// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Tools/OdysseyPainterEditorTool.h"
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

    void AddTool(UOdysseyPainterEditorTool* iTool);
    bool ContainsSimilarTool( UOdysseyPainterEditorTool* iTool );
    void RemoveToolAtIndex( int iIndex );
    void RemoveTool(UOdysseyPainterEditorTool* iTool);
    void MoveTool(int32 iFromIndex, int32 iToIndex);
    bool ContainsTool(UOdysseyPainterEditorTool* iTool);
    const TArray<UOdysseyPainterEditorTool*> GetTools() const;

public:
    FOnCollectionChanged OnCollectionChanged;

private:
    UPROPERTY(Instanced)
    TArray<UOdysseyPainterEditorTool*> mToolsConfig;
};
