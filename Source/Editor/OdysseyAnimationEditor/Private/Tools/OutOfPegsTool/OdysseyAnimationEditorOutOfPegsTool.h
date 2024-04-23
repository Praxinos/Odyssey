// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyAnimationEditorOutOfPegsTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyAnimationCell;

UCLASS()
class UOdysseyAnimationEditorOutOfPegsTool :
    public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorOutOfPegsTool();

    //Constructor
    UOdysseyAnimationEditorOutOfPegsTool();

public:
    //OdysseyPainterEditorTool overrides
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;

    virtual EMouseCursor::Type GetMouseCursor() const override;
    virtual TSharedRef<SWidget> CreateTopTabWidget() override;

public:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& iEvent) override;

public:
    TSharedPtr<FOdysseyAnimationCell> GetCell() const;
    void SetCell(TSharedPtr<FOdysseyAnimationCell> iCell);

private:
    void OnCellOutOfPegsChanged(bool iIsInteractive);

public:
    UPROPERTY(EditAnywhere, Category="Out Of Pegs")
    FVector2D Pan = FVector2D(0, 0);

    UPROPERTY(EditAnywhere, Category="Out Of Pegs")
    float Rotation = 0.f;

    UPROPERTY(EditAnywhere, Category="Out Of Pegs")
    float Zoom = 1.f;

public:
    TSharedPtr<FOdysseyAnimationCell> mCell;
};
