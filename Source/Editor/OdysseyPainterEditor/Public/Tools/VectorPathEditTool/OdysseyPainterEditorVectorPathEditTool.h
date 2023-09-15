// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathEditTool.generated.h"

class FOdysseyPainterEditorVectorPathEditToolHUD;

UENUM()
enum class ePathPickingMode : uint8
{
    Vertex = 0,
    VertexHandle = 1,
    SegmentHandle = 2
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathEditTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathEditTool();

    //Constructor
    UOdysseyPainterEditorVectorPathEditTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;
    virtual bool OnKeyDown( const FKey& iKey ) override;

    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
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

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

    uint64 GetPickingFlags();
    ePathPickingMode GetPickingMode();

    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    void PropertyChanged( const FName& iPropertyName );

private:
    FOdysseyVectorPathCubic* FetchPath( FOdysseyVectorEngine* iVectorEngine
                                      , FOdysseyVectorScene* iScene
                                      , double iWorldX
                                      , double iWorldY );
    void OnMouseDownDeletePoint( FOdysseyVectorEngine* iEngine
                               , FOdysseyVectorScene* iScene
                               , const FOdysseyPoint& iPointInTexture
                               , const FKey& iKey );
    void OnMouseDownPickPoint( FOdysseyVectorEngine* iEngine
                             , FOdysseyVectorScene* iScene
                             , const FOdysseyPoint& iPointInTexture
                             , const FKey& iKey );
    void GroupPaintDeletePoint( FOdysseyVectorGroupPaint* iGroupPaint
                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                              , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                              , double iSelectionRadius
                              , const FOdysseyPoint& iPointInTexture );
    void DetectPickingMode();

    ::ULIS::FRectD DragPoint( FOdysseyVectorPoint *iPoint
                            , double iWorldX
                            , double iWorldY
                            , double iDeltaX
                            , double iDeltaY
                            , bool iWidenAllAlong );
    void AlterVertexRadius( FOdysseyVectorVertex* vertex
                          , FOdysseyVectorSegment* iFromSegment
                          , double iDeltaRadius );
    void GetPathsFromSelection( FOdysseyVectorScene* iScene
                             , std::vector<FOdysseyVectorPath*>& oPathArray );

private:
    FOdysseyPainterEditorVectorPathEditToolHUD *mPathEditHUD;

    std::vector<FOdysseyVectorPoint*> mPickedPointArray;
    std::vector<FOdysseyVectorPath*> mSelectedPathArray;
    uint64 mPickingFlags;
    ePathPickingMode mPickingMode;
    ::ULIS::FVec2D mOldPointInTexture;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathEdit Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double PickingRadius;

    UPROPERTY(EditAnywhere, Category="Odyssey PathEdit Tool" )
    bool WidenAllAlong;
};
