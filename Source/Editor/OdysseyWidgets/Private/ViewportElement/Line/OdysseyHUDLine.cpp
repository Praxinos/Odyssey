// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Line/OdysseyHUDLine.h"

void UOdysseyHUDLine::Init(FVector2D iStartPoint, FVector2D iFinishPoint, FOdysseyPaintEngineHUD* iPaintEngineHUD)
{
    mStartPoint = iStartPoint;
    mFinishPoint = iFinishPoint;
    mPaintEngineHUD = iPaintEngineHUD;
}

void UOdysseyHUDLine::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    //Check: maybe we don't even need to say to the viewport to draw. We can just draw the HUD when its property changed here
}

void UOdysseyHUDLine::PreEditChange( FProperty* PropertyAboutToChange )
{
    //Check: create erase function ? Or is it enough to just keep this function to erase ?
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawLine(*(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y), ::ULIS::FVec2I(mFinishPoint.X, mFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}

TSharedPtr<SWidget> UOdysseyHUDLine::CreateWidget()
{
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs args;

    mDetailsView = propertyModule.CreateDetailView(args);
    mDetailsView->SetObject(this);

    return mDetailsView;
}

void UOdysseyHUDLine::Draw()
{
    if( !mPaintEngineHUD )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawLine( *(mPaintEngineHUD->GetHUDBlock()->GetBlock()), ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y), ::ULIS::FVec2I(mFinishPoint.X, mFinishPoint.Y), ::ULIS::FColor::RGBA8( 0, 255, 0, 255 ) );
    ctx.Finish();

    mPaintEngineHUD->GetHUDBlock()->GetBlock()->Dirty();
}
