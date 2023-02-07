// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVectorPathBuilder.h"
#include "HUD/OdysseyVectorHUDSelection.h"

#include "OdysseyPainterEditorVectorObjectPickTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectPickTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE(FSelectionChanged)
    FSelectionChanged mSelectionChanged;

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectPickTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectPickTool();
 
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

private:
    void Redraw(const TArray<::ULIS::FRectI>& iRects);

private:
    FOdysseyVectorHUDSelection *mSelectionHUD;
    std::vector<::ULIS::FVec2D> mPointArray;

public:
    // Setters
    virtual bool CanDraw();

protected:

};
