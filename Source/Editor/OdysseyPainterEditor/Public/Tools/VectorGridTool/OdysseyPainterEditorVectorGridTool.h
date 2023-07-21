// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

class FGridNode;
class FOdysseyPainterEditorVectorGridToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGridTool : public UOdysseyPainterEditorVectorPickTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorGridTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture,const FKey& iKey ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    //Constructor
    UOdysseyPainterEditorVectorGridTool();
    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
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
    FOdysseyPainterEditorVectorGridToolHUD* mGridHUD;
    std::vector<FGridNode *> mGridNodeArray;
    bool mMultipleSelectionMode;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool", meta = (ClampMin = "1", ClampMax = "32", UIMin = "1", UIMax = "32") )
    uint32 DivisionsX;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool", meta = (ClampMin = "1", ClampMax = "32", UIMin = "1", UIMax = "32") )
    uint32 DivisionsY;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double PickingRadius;
};
