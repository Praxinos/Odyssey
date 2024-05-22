// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDElement.h"

IMPLEMENT_HIT_PROXY(HOdysseyHUDElementHitProxy, HHitProxy)

FOdysseyHUDElement::~FOdysseyHUDElement()
{
    EmptyHUDElements();
}

FOdysseyHUDElement::FOdysseyHUDElement(FName iName) :
    mName( iName ),
    //mIsInvalid( true ),
    mIsCaptured( false )
{
}

/* void FOdysseyHUDElement::Invalidate()
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->Invalidate();
    }

    mIsInvalid = true;
}

void FOdysseyHUDElement::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->Draw(ioBlock, iTransform);
    }

    mIsInvalid = false;
}*/

void
FOdysseyHUDElement::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->Render(iParams);
    }
}

/* void FOdysseyHUDElement::MouseMove( const FOdysseyPoint& iPointInTexture )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->MouseMove( iPointInTexture );
    }
}

bool FOdysseyHUDElement::OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        if( it->Value->OnKeyDown(iPointInTexture, iKey) )
            return true;
    }

    return false;
}

bool FOdysseyHUDElement::OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        if (it->Value->OnKeyUp(iPointInTexture, iKey))
            return true;
    }

    return false;
}

void FOdysseyHUDElement::CapturedMouseMove( const FOdysseyPoint& iPointInTexture )
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        if( it->Value->IsCaptured() )
            it->Value->CapturedMouseMove( iPointInTexture );
    }
}

void FOdysseyHUDElement::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    for (auto it = mElements.CreateConstIterator(); it; ++it)
    {
        it->Value->Erase(ioBlock, iTransform);
        it->Value->mIsInvalid = true;
    }
}*/

void FOdysseyHUDElement::AddElement(TSharedPtr<FOdysseyHUDElement> iElementToAdd)
{
    if( iElementToAdd != nullptr )
        mElements.Emplace( iElementToAdd->mName.ToString(), iElementToAdd );

    //mIsInvalid = false;
}

void FOdysseyHUDElement::RemoveElement(TSharedPtr<FOdysseyHUDElement> iElementToRemove)
{
    if( iElementToRemove != nullptr )
    {
        mElements.Remove( iElementToRemove->mName.ToString() );
    }

    //mIsInvalid = false;
}

void
FOdysseyHUDElement::EmptyHUDElements()
{
    mElements.Empty();
}

/* bool FOdysseyHUDElement::IsInvalid()
{
    InternalIsInvalid( mIsInvalid );
    return mIsInvalid;
} */

FName
FOdysseyHUDElement::GetName() const
{
    return mName;
}

bool FOdysseyHUDElement::IsCaptured() const
{
    bool isCaptured = mIsCaptured;

    //InternalIsCaptured(isCaptured);
    return isCaptured;
}

void FOdysseyHUDElement::Capture(bool iCapture)
{
    mIsCaptured = iCapture;
}

/* void FOdysseyHUDElement::InternalIsInvalid( bool &ioIsInvalid )
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
            it->Value->InternalIsInvalid( ioIsInvalid );
        }
    }
}

void FOdysseyHUDElement::InternalIsCaptured(bool& ioIsCaptured)
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
            it->Value->InternalIsCaptured(ioIsCaptured);
        }
    }
} */

bool
FOdysseyHUDElement::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
FOdysseyHUDElement::OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
FOdysseyHUDElement::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void
FOdysseyHUDElement::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
FOdysseyHUDElement::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}
