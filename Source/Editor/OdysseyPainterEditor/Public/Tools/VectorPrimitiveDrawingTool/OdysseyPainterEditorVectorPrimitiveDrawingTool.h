// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyPainterEditorVectorPrimitiveDrawingTool.generated.h"

UENUM()
enum class EOdysseyVectorPrimitiveType : uint8
{
    Ellipse,
    Rectangle,
    Line
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPrimitiveDrawingTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE_OneParam(FSelectionChanged,FOdysseyVectorScene*)
    FSelectionChanged mSelectionChanged;

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPrimitiveDrawingTool();

    //Constructor
    UOdysseyPainterEditorVectorPrimitiveDrawingTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;

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

    public:
        UPROPERTY(EditAnywhere, Category="Primitive Drawing Tool")
        EOdysseyVectorPrimitiveType PrimitiveType;

        UPROPERTY(EditAnywhere, Category="Primitive Drawing Tool", meta = (ClampMin = "0.0", UIMin = "0.0"))
        double StrokeWidth;

        UPROPERTY(EditAnywhere, Category="Primitive Drawing Tool")
        bool Uniform;
        bool UniformAtKeyDown;

       ::ULIS::FVec2D mMouseDown;
};
