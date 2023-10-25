// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#include "OdysseyPainterEditorVectorPathSmoothTool.generated.h"

class FOdysseyPainterEditorVectorPathSmoothToolHUD;

UENUM()
enum class ePathSmoothingMode : uint8
{
    Round = 0,
    Sharp = 1
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathSmoothTool : public UOdysseyPainterEditorVectorBaseTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathSmoothTool();

    //Constructor
    UOdysseyPainterEditorVectorPathSmoothTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;
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

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

protected:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    void PropertyChanged( const FName& iPropertyName );

private:
    FOdysseyPainterEditorVectorPathSmoothToolHUD* mPathSmoothHUD;
    FOdysseyVectorUndoSegmentReshape* mUndoSegmentReshape;

public:
    UPROPERTY( EditAnywhere, Category = PathSmoothTool )
    ePathSmoothingMode SmoothingMode;
    ePathSmoothingMode SmoothingModeAtKeyDown; // when pressing shift

    UPROPERTY( EditAnywhere, Category = PathSmoothTool, meta = (ClampMin = "0.0", UIMin = "0.0"))
    double PickingRadius;

    UPROPERTY( EditAnywhere, Category = PathSmoothTool )
    bool RestrictToSelection;

    UPROPERTY( EditAnywhere, Category = PathSmoothTool )
    bool PreserveHandleLength;
};
