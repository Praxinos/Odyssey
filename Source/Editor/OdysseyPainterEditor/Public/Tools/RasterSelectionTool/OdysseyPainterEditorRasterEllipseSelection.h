// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorRasterSelectionTool.h"
#include "OdysseyPainterEditorRasterEllipseSelection.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterEllipseSelection : public UOdysseyPainterEditorRasterSelectionTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterEllipseSelection();

    //Constructor
    UOdysseyPainterEditorRasterEllipseSelection();

public:
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey);

    virtual EMouseCursor::Type GetMouseCursor() const override;

private:
    TArray<::ULIS::FRectI> GetSelectionAreaAsScanlines();

    FVector2D mDownReference;

};
