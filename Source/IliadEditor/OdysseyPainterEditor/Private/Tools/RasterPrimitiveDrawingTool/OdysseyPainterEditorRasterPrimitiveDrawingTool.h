// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "ScopedTransaction.h"

#include "OdysseyShape.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorRasterPrimitiveDrawingTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterPrimitiveDrawingTool : public UOdysseyPainterEditorRasterBaseTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterPrimitiveDrawingTool();

    //Constructor
    UOdysseyPainterEditorRasterPrimitiveDrawingTool();


public:
    //TOOL
    template<class T> T* CreateShape(FName iName);

public:
    virtual bool IsActivable() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;

    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Load() override;
    virtual void Unload() override;

    virtual void Flush() override;
    virtual void Commit() override;
    virtual void Tick(float iDeltaTime) override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual FText GetTooltip() const override;

private:
    // Internal - Callbacks
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlockFromEditor(bool iCreate) const;

    void OnShapeBegin();
    void OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset);
    void OnShapeAbort();

    void OnRasterSelectionChanged();

protected:
    FOdysseyPaintEngine mPaintEngine;

    TArray<FOdysseyPoint> mPath;

    TSharedPtr<FOdysseyHUDElement> mShapeHUD;
    TSharedPtr<FScopedTransaction> mTransaction;

public:
    /** The list of available shapes. */
    UPROPERTY(EditAnywhere, Category="Shape", meta=(IgnoreToolConfiguration) )
    FOdysseyShapes Shapes;

    /** The stroke width. */
    UPROPERTY(EditAnywhere, Category="Parameters", meta=(UIMin="0", ClampMin="0", LinearDeltaSensitivity="15", Delta="1") )
    float StrokeWidth = 1.0f;

    /** Apply the antialiasing. */
    UPROPERTY(EditAnywhere, Category="Parameters" )
    bool Antialiasing = true;

    /** Use subpixel mode. */
    UPROPERTY(EditAnywhere, Category="Parameters" )
    bool SubPixel = true;

    /** Fill the shape. */
    UPROPERTY(EditAnywhere, Category="Parameters" )
    bool Filled = true;

    UPROPERTY(EditInstanceOnly, Category="Blending", meta = (ShowOnlyInnerProperties ) )
    FOdysseyBlendParameters BlendParameters;
};
