// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "OdysseyPainterEditorRasterPrimitiveDrawingTool.generated.h"

UENUM()
enum class EOdysseyPrimitiveShape : uint8
{
    kLine           UMETA(DisplayName = "Line"),
    kRectangle      UMETA(DisplayName = "Rectangle"),
    kPolygon        UMETA(DisplayName = "Polygon"),
    kEllipse        UMETA(DisplayName = "Ellipse"),
    kBezier         UMETA(DisplayName = "Bezier"),
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterPrimitiveDrawingTool : public UOdysseyPainterEditorTool
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


private:
    // Internal - Callbacks
    void SelectedShapeChanged();

    void OnShapePathEnd(const FOdysseyPoint& iPoint);

    void OnShapePathAbort();

    void OnShapePathReset();

public:
    virtual void PropertyChanged(const FName& iPropertyName) override;
    FSimpleMulticastDelegate& OnShapeChanged();

protected:
    FOdysseyPaintEngine mPaintEngine;

    FSimpleMulticastDelegate            mOnShapeChanged;

public:
    UPROPERTY(EditAnywhere, Category = "Shape")
    EOdysseyPrimitiveShape SelectedShape;

    UPROPERTY(EditAnywhere, Category = "Shape")
    EOdysseyDrawingPrecision Precision = EOdysseyDrawingPrecision::kRaw;

    UPROPERTY(EditAnywhere, Category = "Shape")
    bool Filled = false;

    UPROPERTY(VisibleInstanceOnly, Category = "Shape", Instanced, meta = (ShowInnerProperties))
    class UOdysseyShape* SelectedShapeInstance;

    // Hidden properties
    UPROPERTY()
    TMap<EOdysseyPrimitiveShape, class UOdysseyShape*> AvailableShapes;

    UPROPERTY(EditInstanceOnly, Category = "Blending", meta = (ShowOnlyInnerProperties))
    FOdysseyBlendParameters BlendParameters;
};
