// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Handle/OdysseyHUDHandle.h"


void UOdysseyHUDHandle::Init( FName iName, UOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FOdysseyPaintEngineHUD* iPaintEngineHUD, FTransform2D const * iTransform )
{
    UOdysseyHUDElement::Init( iName, iPaintEngineHUD, iTransform );
    mParent = iParent;
    mHandleSize = 2;
    mReferencePoint = iReferencePoint;
}

void UOdysseyHUDHandle::PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent)
{
    UOdysseyHUDElement::PostEditChangeProperty( iPropertyChangedEvent );
    
    mIsInvalid = true;
}

void UOdysseyHUDHandle::PreEditChange(FProperty* iPropertyAboutToChange)
{
    UOdysseyHUDElement::PreEditChange( iPropertyAboutToChange );

    Erase();
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

    if (mTransform)
    {
        FVector2D transformedReferencePoint = ToViewport(*mReferencePoint);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(transformedReferencePoint.X - mHandleSize, transformedReferencePoint.Y - mHandleSize), ::ULIS::FVec2I(transformedReferencePoint.X + mHandleSize, transformedReferencePoint.Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 255));
        ctx.Finish();
    }
    else
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mReferencePoint->X - mHandleSize, mReferencePoint->Y - mHandleSize), ::ULIS::FVec2I(mReferencePoint->X + mHandleSize, mReferencePoint->Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 255));
        ctx.Finish();
    }

    mPaintEngineHUD->GetHUDBlock()->GetBlock()->Dirty();
}

void UOdysseyHUDHandle::Erase()
{
    if (mTransform)
    {
        FVector2D transformedReferencePoint = ToViewport(*mReferencePoint);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(transformedReferencePoint.X - mHandleSize, transformedReferencePoint.Y - mHandleSize), ::ULIS::FVec2I(transformedReferencePoint.X + mHandleSize, transformedReferencePoint.Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 0));
        ctx.Finish();
    }
    else
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawRectangle(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mReferencePoint->X - mHandleSize, mReferencePoint->Y - mHandleSize), ::ULIS::FVec2I(mReferencePoint->X + mHandleSize, mReferencePoint->Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 0));
        ctx.Finish();
    }
}

void UOdysseyHUDHandle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    UOdysseyHUDElement::MouseMove( iViewport, iX, iY );

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

FReply UOdysseyHUDHandle::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply )
{
    UOdysseyHUDElement::InputKey( iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

    if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
    {
        float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()));
        if( distSquared < 25 )
        { 
            mIsCaptured = true;
            ioReply = FReply::Handled();
        }
    }
    else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released && mIsCaptured)
    {
        UE_LOG(LogTemp, Display, TEXT("ReleaseCapture"));
        mIsCaptured = false;
        ioReply = FReply::Handled();
    }

    return ioReply;
}

void UOdysseyHUDHandle::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    UOdysseyHUDElement::CapturedMouseMove( iViewport, iX, iY );

    if (mIsCaptured)
    {
        mParent->PreEditChange(nullptr);
        mReferencePoint->Set(iX, iY);
        mParent->PostEditChange();
        mIsInvalid = true;
    }
}
