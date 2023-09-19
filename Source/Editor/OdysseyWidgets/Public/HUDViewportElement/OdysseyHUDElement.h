// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

public:
    // Destructor
    virtual ~UOdysseyHUDElement();

    //Constructor
    UOdysseyHUDElement();


public:     
    void Init(FName iName, FTransform2D iTransform = FTransform2D());

//UOdysseyHUDSlateElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyHUDViewportElement overrides
public:
    virtual void Invalidate() override;
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    virtual void MouseMove( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture ) override;
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());

public:
    void AddElement( UOdysseyHUDElement* iElementToAdd );
    void EmptyHUDElements();
    bool IsInvalid();
    bool IsCaptured();

private:
    void InternalIsInvalid( bool &ioIsInvalid );
    void InternalIsCaptured( bool& ioIsCaptured );


protected:
    TMap<FString, UOdysseyHUDElement*> mElements;
    TSharedPtr<SScrollBox> mElementsWidget;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;
};
