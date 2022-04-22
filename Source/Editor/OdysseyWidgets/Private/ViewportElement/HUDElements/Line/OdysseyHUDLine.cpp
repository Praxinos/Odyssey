// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Line/OdysseyHUDLine.h"


void UOdysseyHUDLine::Init( FName iName, FVector2D iStartPoint, FVector2D iFinishPoint, FOdysseyPaintEngineHUD* iPaintEngineHUD, FTransform2D const * iTransform )
{
    UOdysseyHUDElement::Init( iName, iPaintEngineHUD, iTransform );
    mStartPoint = iStartPoint;
    mFinishPoint = iFinishPoint;
}

void UOdysseyHUDLine::PostEditChangeProperty( FPropertyChangedEvent& iPropertyChangedEvent )
{
    UOdysseyHUDElement::PostEditChangeProperty( iPropertyChangedEvent );

    mIsInvalid = true;
}

void UOdysseyHUDLine::PreEditChange( FProperty* iPropertyAboutToChange )
{
    UOdysseyHUDElement::PreEditChange( iPropertyAboutToChange );

    Erase();
}

TSharedPtr<SWidget> UOdysseyHUDLine::CreateWidget()
{
    UOdysseyHUDElement::CreateWidget();

    //TODO: custom widget instead of property view. Will be cleaner.
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs args;

    mDetailsView = propertyModule.CreateDetailView(args);
    mDetailsView->SetObject(this);

    mElementsWidget->AddSlot()
    [
        mDetailsView->AsShared()
    ];

    return mElementsWidget;
}

void UOdysseyHUDLine::Draw()
{
    //Draw the children of this HUDElement
    UOdysseyHUDElement::Draw();

    if( !mPaintEngineHUD )
        return;

    if( mTransform )
    {
        FVector2D transformedStartPoint = ToViewport(mStartPoint);
        FVector2D transformedFinishPoint = ToViewport(mFinishPoint);

        /*UE_LOG(LogTemp, Display, TEXT("Start: %lf, %lf"), transformedStartPoint.X, transformedStartPoint.Y)
        UE_LOG(LogTemp, Display, TEXT("Finish: %lf, %lf"), transformedFinishPoint.X, transformedFinishPoint.Y)*/


        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawLine(*(mPaintEngineHUD->GetHUDBlock()), ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y), ::ULIS::FVec2I(transformedFinishPoint.X, transformedFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 255));
        ctx.Finish();
    }
    else
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawLine(*(mPaintEngineHUD->GetHUDBlock()), ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y), ::ULIS::FVec2I(mFinishPoint.X, mFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 255));
        ctx.Finish();
    }

    mPaintEngineHUD->GetHUDBlock()->Dirty();
}

void UOdysseyHUDLine::Erase()
{
    if (mTransform)
    {
        FVector2D transformedStartPoint = ToViewport(mStartPoint);
        FVector2D transformedFinishPoint = ToViewport(mFinishPoint);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawLine(*(mPaintEngineHUD->GetHUDBlock()), ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y), ::ULIS::FVec2I(transformedFinishPoint.X, transformedFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
        ctx.Finish();
    }
    else
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
        ctx.DrawLine(*(mPaintEngineHUD->GetHUDBlock()), ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y), ::ULIS::FVec2I(mFinishPoint.X, mFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
        ctx.Finish();
    }
}
