// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUD.h"
#include "IOdysseyHUDViewportElement.h"

#include "OdysseyHUDLine.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDLine
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDLine : public UOdysseyHUD, 
                                           public IOdysseyHUDViewportElement
{
    GENERATED_BODY()

public:
    void Init( FVector2D iStartPoint, FVector2D iFinishPoint, FOdysseyPaintEngineHUD* iPaintEngineHUD );

//UObject overrides
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PreEditChange(FProperty* PropertyAboutToChange) override;

//UOdysseyHUD overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyHUDViewportElement overrides
public:
    void Draw() override;

public:
    UPROPERTY( EditAnywhere )
    FVector2D mStartPoint;

    UPROPERTY( EditAnywhere )
    FVector2D mFinishPoint;

private:
    /** The widget representation of the line in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
