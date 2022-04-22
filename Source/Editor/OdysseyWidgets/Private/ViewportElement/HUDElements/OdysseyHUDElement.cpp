// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDElement.h"

void UOdysseyHUDElement::Init(FName iName, FOdysseyPaintEngineHUD* iPaintEngineHUD, FTransform2D const * iTransform )
{
    mName = iName;
    mPaintEngineHUD = iPaintEngineHUD;
    mIsInvalid = true;
    mIsCaptured = false;
    mTransform = iTransform;
}

TSharedPtr<SWidget> UOdysseyHUDElement::CreateWidget()
{
    mElementsWidget = SNew(SScrollBox);
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        mElementsWidget->AddSlot()
        [
            it.Value()->CreateWidget().ToSharedRef()
        ];
    }

    if (mOnApplyHUDAction.IsBound())
    { 
        mElementsWidget->AddSlot()
        [
            SNew(SButton)
            .OnClicked_Lambda([this]()->FReply{ return mOnApplyHUDAction.Execute(); })
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Apply")))
            ]
        ];
    }

    return mElementsWidget;
}

void UOdysseyHUDElement::Draw()
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Draw();
    }

    mIsInvalid = false;
}

void UOdysseyHUDElement::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->MouseMove(iViewport, iX, iY);
    }
}

FReply UOdysseyHUDElement::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        if (ioReply.IsEventHandled())
            return ioReply;

        it->Value->InputKey( iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );
    }

    return ioReply;
}

void UOdysseyHUDElement::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        if( it->Value->IsCaptured() )
            it->Value->CapturedMouseMove(iViewport, iX, iY);
    }
}

void UOdysseyHUDElement::Erase()
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Erase();
    }
}

void UOdysseyHUDElement::PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->PostEditChangeProperty( iPropertyChangedEvent );
    }
}

void UOdysseyHUDElement::PreEditChange(FProperty* iPropertyAboutToChange)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->PreEditChange( iPropertyAboutToChange );
    }
}

void UOdysseyHUDElement::AddElement(UOdysseyHUDElement* iElementToAdd)
{
    if( iElementToAdd != nullptr )
        mElements.Emplace( iElementToAdd->mName.ToString(), iElementToAdd );

    mIsInvalid = false;
}

bool UOdysseyHUDElement::IsInvalid()
{
    InternalIsInvalid( mIsInvalid );
    return mIsInvalid;
}

bool UOdysseyHUDElement::IsCaptured()
{
    bool isCaptured = mIsCaptured;

    InternalIsCaptured(isCaptured);
    return isCaptured;
}


UOdysseyHUDElement::FOnApplyHUDAction& UOdysseyHUDElement::OnApplyHUDAction()
{
    return mOnApplyHUDAction;
}

void UOdysseyHUDElement::InternalIsInvalid( bool &ioIsInvalid )
{
    if( ioIsInvalid )
        return;

    ioIsInvalid = mIsInvalid;
    if (ioIsInvalid)
    {
        return;
    }
    else
    {
        for (auto it = mElements.CreateConstIterator(); it; ++it)
        {
            it.Value()->InternalIsInvalid( ioIsInvalid );
        }
    }
}

void UOdysseyHUDElement::InternalIsCaptured(bool& ioIsCaptured)
{
    if (ioIsCaptured)
        return;

    ioIsCaptured = mIsCaptured;
    if (ioIsCaptured)
    {
        return;
    }
    else
    {
        for (auto it = mElements.CreateConstIterator(); it; ++it)
        {
            it.Value()->InternalIsCaptured(ioIsCaptured);
        }
    }
}
