// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDElement.h"

void UOdysseyHUDElement::Init(FName iName, FTransform2D iTransform /*= FTransform2D()*/ )
{
    mName = iName;
    mIsInvalid = true;
    mIsCaptured = false;
    mPreviousTransform = iTransform;
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

    return mElementsWidget;
}

void UOdysseyHUDElement::Invalidate()
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Invalidate();
    }

    mIsInvalid = true;
}

void UOdysseyHUDElement::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Draw(ioBlock, iTransform);
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

void UOdysseyHUDElement::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Erase(ioBlock, iTransform);
        it.Value()->mIsInvalid = true;
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
