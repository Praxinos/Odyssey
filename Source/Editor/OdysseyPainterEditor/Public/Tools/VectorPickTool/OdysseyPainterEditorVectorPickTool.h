// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSelect.h"
#include "OdysseyPainterEditorVectorPickTool.generated.h"

UENUM()
enum class EOdysseyVectorEditionMode : uint8
{
    Object,
    Vertex
};

UENUM()
enum class EOdysseyVectorPickingMode : uint8
{
    Freehand,
    Circle,
    Rectangle
};

class FOdysseyPainterEditorVectorPickToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPickTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    static bool DoubleClicked();
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPickTool();

    //Constructor
    UOdysseyPainterEditorVectorPickTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
 
    virtual void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    virtual void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    virtual bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    virtual ::ULIS::FRectI OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture );
    virtual bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FOdysseyPoint& iPointInTexture
                                , const FKey& iKey );
    virtual bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FKey& iKey ) override;
    virtual bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FKey& iKey ) override;

    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    void PropertyChangedVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FName& iPropertyName );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

    void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

    std::vector<::ULIS::FVec2D>& GetPointArray();

    EOdysseyVectorPickingMode GetPickingMode();

protected:
    void OnMouseUpVectorObjectMode( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    void OnMouseUpVectorVertexMode( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    ::ULIS::FRectD GenerateMask( FOdysseyVectorEngine* iEngine );

    void SelectVertexFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup );
    void SelectVertexFromPath( FOdysseyVectorPath* iPath );

protected:
    FOdysseyPainterEditorVectorPickToolHUD* mPickHUD;
    std::vector<::ULIS::FVec2D> mPointArray;
    ::ULIS::FVec2D mPressedMouseCoords;

public:
    UPROPERTY(EditAnywhere, Category="Picking Tool")
    EOdysseyVectorEditionMode EditionMode;

    UPROPERTY(EditAnywhere, Category="Picking Tool")
    EOdysseyVectorPickingMode PickingMode;
};
