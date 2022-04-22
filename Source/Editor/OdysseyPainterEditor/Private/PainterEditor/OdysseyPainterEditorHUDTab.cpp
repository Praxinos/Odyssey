// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorHUDTab.h"

#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorHUDTab::~FOdysseyPainterEditorHUDTab()
{
    if( mHUD )
        mHUD->ConditionalBeginDestroy();
}

FOdysseyPainterEditorHUDTab::FOdysseyPainterEditorHUDTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_HUD"),
                            LOCTEXT( "OdysseyPainterEditorHUDTab", "HUD" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.HUD16" ))
    , mEditor(iEditor)
    , mHUD(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorHUDTab::CreateWidget()
{    
	if (!mHUD)
    {
        UOdysseyHUDElement* decoratedLine = NewObject<UOdysseyHUDElement>();

        UOdysseyHUDLine* line = NewObject<UOdysseyHUDLine>();
        line->Init( FName("Line1"), FVector2D(0, 0), FVector2D(500, 500), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line);
       
        FOdysseyPaintEngine* paintEngine = mEditor->PaintEngine();
        line->OnApplyHUDAction().BindLambda( [line, paintEngine]() 
        {
            UE_LOG(LogTemp, Display, TEXT("Applied"));

            //::ULIS::TArray<::ULIS::FVec2I> points;

            /*GenerateLinePoints(::ULIS::FVec2I(line->mStartPoint.X, line->mStartPoint.Y), ::ULIS::FVec2I(line->mFinishPoint.X, line->mFinishPoint.Y), points);
            for( int i = 0; i < points.Num(); i++ )
            {
                UE_LOG(LogTemp, Display, TEXT("%d, %d"), points[i].X, points[i].Y)
            }*/

            return FReply::Handled();
        } );

        mHUD = MakeShareable(decoratedLine);
    }

    return mHUD->CreateWidget();
}

void
FOdysseyPainterEditorHUDTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<UOdysseyHUDElement>
FOdysseyPainterEditorHUDTab::GetHUD()
{
    return mHUD;
}


#undef LOCTEXT_NAMESPACE
