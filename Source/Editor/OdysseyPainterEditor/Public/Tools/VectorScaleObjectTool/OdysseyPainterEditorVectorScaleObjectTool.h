// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVectorPathBuilder.h"

#include "OdysseyPainterEditorVectorScaleObjectTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorScaleObjectTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorScaleObjectTool();

    //Constructor
    UOdysseyPainterEditorVectorScaleObjectTool();
 
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

private:
    void Redraw(const TArray<::ULIS::FRectI>& iRects);

private:
    double mOldLocalMouseX;
    double mOldLocalMouseY;

public:
    // Setters
    virtual bool CanDraw();

    UPROPERTY(EditAnywhere, Category="Odyssey ScaleObject Tool")
    double Size;

protected:

};
