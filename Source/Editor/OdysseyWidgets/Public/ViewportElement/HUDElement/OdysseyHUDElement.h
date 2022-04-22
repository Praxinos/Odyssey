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
    DECLARE_DELEGATE_RetVal(FReply, FOnApplyHUDAction);
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
    FOnApplyHUDAction& OnApplyHUDAction();

protected:
    void Invalidate();

private:
    void InternalIsInvalid( bool &ioIsInvalid );


protected:
    TMap<FString, UOdysseyHUDElement*> mElements;
    TSharedPtr<SScrollBox> mElementsWidget;

private:
    /** If this element is invalid, then, we'll need to redraw all mElements*/
    bool mIsInvalid;

    /** Action linked to the "Apply" button of the HUD in the interface. If not bound, the button won't show */
    FOnApplyHUDAction mOnApplyHUDAction;
};
