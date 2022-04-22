// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDElement.h"

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

void UOdysseyHUDElement::Draw()
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it.Value()->Draw();
    }

    mIsInvalid = false;
}

void UOdysseyHUDElement::AddElement(UOdysseyHUDElement* iElementToAdd)
{
    if( iElementToAdd != nullptr )
        mElements.Emplace( iElementToAdd->mName.ToString(), iElementToAdd );

    Invalidate();
}

bool UOdysseyHUDElement::IsInvalid()
{
    InternalIsInvalid( mIsInvalid );
    return mIsInvalid;
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

void UOdysseyHUDElement::Invalidate()
{
    mIsInvalid = true;
}
