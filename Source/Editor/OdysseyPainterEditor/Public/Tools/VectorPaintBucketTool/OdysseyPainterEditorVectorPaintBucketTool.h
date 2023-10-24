// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyPainterEditorVectorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorVectorPaintBucketToolHUD;
class FOdysseyPainterEditorVectorPaintBucketToolContextMenu;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    //Inactivates the tool
    virtual bool IsActivable() const override;

    virtual void Load();
    virtual void Unload();


    static bool DoubleClicked();

    // Destructor
    virtual ~UOdysseyPainterEditorVectorPaintBucketTool();

    //Constructor
    UOdysseyPainterEditorVectorPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyPainterEditorTool overrides
    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    // Raster Mouse Down
    bool OnMouseDownRaster( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    // Vector Mouse Down
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    void OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                           , FOdysseyVectorScene* iScene
                           , const FOdysseyPoint& iPointInTexture );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FKey& iKey );
    bool OnKeyUpVector(FOdysseyVectorEngine* iEngine
                      ,FOdysseyVectorScene* iScene
                      ,const FKey& iKey);
    void PropertyChangedVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FName& iPropertyName );

    virtual void Commit() override;

    std::vector<FOdysseyVectorCycle*>& GetPickedCycleArray();

    std::list<FOdysseyVectorObject*>& GetFocusedObjectList( FOdysseyVectorScene* iScene );

    bool GetShowControls();

private:
    void SetBucketColor( FOdysseyVectorBucket* iBucket );
    void PopUpMenu( FOdysseyVectorBucket* iBucket );

protected:
    void OnMouseUpVectorClearBucket( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorBucket* iBucket );
    void OnMouseUpVectorCreateBucket( FOdysseyVectorScene* iScene
                                    , const FOdysseyPoint& iPointInTexture
                                    , const FKey& iKey );
    void OnMouseUpVectorRemoveBucket( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorBucket* iBucket );
    void OnMouseUpVectorMovePoint( FOdysseyVectorScene* iScene
                                 , FOdysseyVectorPoint* iPoint
                                 , ::ULIS::FVec2D iPointOriginalPosition );
    void OnMouseUpVectorPropagateBucket( FOdysseyVectorScene* iScene
                                        , FOdysseyVectorBucket* iBucket
                                        , bool iPropagate );
    void OnMouseUpVectorColorBucket( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorBucket* iBucket );
    void OnMouseDownVectorRotateBucket( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorBucket* iBucket );
    double GetRotationAngle( FOdysseyVectorBucket* iBucket
                            , const FOdysseyPoint& iPointInTexture );

public:
    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    bool RestrictToSelection;

    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    bool Propagate;

    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    eBucketColorMode ColorMode;

    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    FColor Color1;

    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    FColor Color2;

    UPROPERTY( EditAnywhere, Category = VectorPaintBucketTool )
    double PickingRadius;

protected:
    //std::vector<FOdysseyVectorCycle*> mPickedCycleArray;
    FOdysseyPaintEngine mPaintEngine;
    FOdysseyVectorBucket* mPickedBucket;
    //FOdysseyVectorObject* mPickedObject;
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    double mDownMouseX;
    double mDownMouseY;
    ::ULIS::FVec2D mPointPosition;
    double mPointRotation;
    FOdysseyPainterEditorVectorPaintBucketToolHUD* mBucketHUD;
    FOdysseyPainterEditorVectorPaintBucketToolContextMenu* mContextMenu;
    uint32 mPickedArea;
    ::ULIS::FVec2D mOldPointInTexture;
    bool mShowControls;
};
