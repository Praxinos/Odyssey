// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDSlateElement.h"
#include "IOdysseyHUDViewportElement.h"
#include "Widgets/Layout/SScrollBox.h"

#include "OdysseyHUDElement.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDElement
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDElement : public UOdysseyHUDSlateElement,
                                              public IOdysseyHUDViewportElement
{
    GENERATED_BODY()

//UOdysseyHUDSlateElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyHUDViewportElement overrides
public:
    void Draw() override;

public:
    void AddElement( UOdysseyHUDElement* iElementToAdd );
    bool IsInvalid();

private:
    void InternalIsInvalid( bool &ioIsInvalid );

protected: 
    void Invalidate();

protected:
    TMap<FString, UOdysseyHUDElement*> mElements;
    TSharedPtr<SScrollBox> mElementsWidget;

private:
    bool mIsInvalid;
};
