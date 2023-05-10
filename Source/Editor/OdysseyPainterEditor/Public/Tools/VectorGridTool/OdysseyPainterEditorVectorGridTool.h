// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGridTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorGridTool();

    //Constructor
    UOdysseyPainterEditorVectorGridTool();
 
    void ActivateVector( FOdysseyVectorEngine* iEngine
                       , FOdysseyVectorScene* iScene );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , FOdysseyVectorUndo** iUndo
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    void PropertyChangedVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FName& iPropertyName );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

private:
    std::vector<FOdysseyVectorPoint*> mPointArray;
    FOdysseyVectorHUDGrid mGridHUD;

private:
    std::vector<FGridNode *> mGridNodeArray;
    bool mMultipleSelectionMode;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    uint32 DivisionsX;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    uint32 DivisionsY;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    double PickingRadius;
};
