// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathEditTool.generated.h"

typedef struct FStitchingPair
{
    FOdysseyVectorVertex* vertex[2];
    ::ULIS::FVec2D handle[2];

    FStitchingPair( FOdysseyVectorVertex* iVertex0, ::ULIS::FVec2D& iHandle0
                  , FOdysseyVectorVertex* iVertex1, ::ULIS::FVec2D& iHandle1 )
    {
        // Note: ordering will ease comparisons between stitching pairs.
        vertex[0] = iVertex0 < iVertex1 ? iVertex0 : iVertex1;
        handle[0] = iVertex0 < iVertex1 ? iHandle0 : iHandle1;

        vertex[1] = iVertex0 < iVertex1 ? iVertex1 : iVertex0;
        handle[1] = iVertex0 < iVertex1 ? iHandle1 : iHandle0;
    }

    bool operator==(const FStitchingPair& rhs)
    {
        return ( ( vertex[0] == rhs.vertex[0] ) && ( vertex[1] == rhs.vertex[1] ) );
    }
} FStitchingPair;

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
    bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FKey& iKey );
    bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                      , FOdysseyVectorScene* iScene
                      , const FKey& iKey );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;


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

    void DetectPickingMode();

private:
    FOdysseyVectorHUDPathCubic mCubicPathHUD;
    FOdysseyVectorHUDPicking mPickingHUD;
    std::vector<FOdysseyVectorPoint*> mPickedPointArray;
    uint64 mSelectionFlags;
    ::ULIS::FVec2D mOldPointInTexture;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathEdit Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double Radius;
};
