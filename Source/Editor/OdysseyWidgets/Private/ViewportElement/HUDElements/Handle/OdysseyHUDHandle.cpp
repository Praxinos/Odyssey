// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Handle/OdysseyHUDHandle.h"


void UOdysseyHUDHandle::Init( FName iName, FVector2D* iReferencePoint, FOdysseyPaintEngineHUD* iPaintEngineHUD )
{
    mHandleSize = 2;
    mName = iName;
    mReferencePoint = iReferencePoint;
    mPaintEngineHUD = iPaintEngineHUD;
    Invalidate();
}

void UOdysseyHUDHandle::PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent)
{
    UOdysseyHUDElement::PostEditChangeProperty( iPropertyChangedEvent );
    
    Invalidate();
    //Check: maybe we don't even need to say to the viewport to draw. We can just draw the HUD when its property changed here
}

void UOdysseyHUDHandle::PreEditChange(FProperty* iPropertyAboutToChange)
{
    UOdysseyHUDElement::PreEditChange( iPropertyAboutToChange );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mReferencePoint->X - mHandleSize, mReferencePoint->Y - mHandleSize), ::ULIS::FVec2I(mReferencePoint->X + mHandleSize, mReferencePoint->Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 0));
    ctx.Finish();
}

TSharedPtr<SWidget> UOdysseyHUDHandle::CreateWidget()
{
    UOdysseyHUDElement::CreateWidget();

    return mElementsWidget;
}

void UOdysseyHUDHandle::Draw()
{
    //Draw the children of this HUDElement
    UOdysseyHUDElement::Draw();

    if( !mPaintEngineHUD || !mReferencePoint )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mReferencePoint->X - mHandleSize, mReferencePoint->Y - mHandleSize), ::ULIS::FVec2I(mReferencePoint->X + mHandleSize, mReferencePoint->Y + mHandleSize), ::ULIS::FColor::RGBA8( 255, 0, 0, 255 ) );
    ctx.Finish();

    mPaintEngineHUD->GetHUDBlock()->GetBlock()->Dirty();
}

void UOdysseyHUDHandle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iX, iY));
    if ( mHandleSize != 5 && distSquared < 25)
    {
        FProperty* handleSizeProperty = FindFProperty<FProperty>(this->GetClass(), "mHandleSize");
        PreEditChange( handleSizeProperty );
        mHandleSize = 5;
        FPropertyChangedEvent propertyChangedEvent( handleSizeProperty, EPropertyChangeType::ValueSet);
        PostEditChangeProperty(propertyChangedEvent);
    }
    else if( mHandleSize != 2 && distSquared > 25 )
    {
        FProperty* handleSizeProperty = FindFProperty<FProperty>(this->GetClass(), "mHandleSize");
        PreEditChange(handleSizeProperty);
        mHandleSize = 2;
        FPropertyChangedEvent propertyChangedEvent(handleSizeProperty, EPropertyChangeType::ValueSet);
        PostEditChangeProperty(propertyChangedEvent);
    }
}

FReply UOdysseyHUDHandle::InputKey()
{
    return FReply::Unhandled();
}

void UOdysseyHUDHandle::CapturedMouseMove()
{

}
