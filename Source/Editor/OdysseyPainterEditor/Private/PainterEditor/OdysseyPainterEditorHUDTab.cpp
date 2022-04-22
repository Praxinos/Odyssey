// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorHUDTab.h"

#include "OdysseyPainterEditor.h"
#include "Line/OdysseyHUDLine.h"
#include "Handle/OdysseyHUDHandle.h"

#include "ULIS/include/Math/ShapeGeneration/Line.h"

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
        FTransform2D const * transform2D = &(mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetTransform());
        UOdysseyHUDElement* decoratedLine = NewObject<UOdysseyHUDElement>();
        decoratedLine->Init( FName("DecoratedLine"), mEditor->PaintEngineHUD() );

        UOdysseyHUDLine* line = NewObject<UOdysseyHUDLine>();
        line->Init( FName("Line1"), FVector2D(0, 0), FVector2D(500, 500), mEditor->PaintEngineHUD() );
        decoratedLine->AddElement(line);
       
        FOdysseyPaintEngine* paintEngine = mEditor->PaintEngine();
        line->OnApplyHUDAction().BindLambda( [line, paintEngine]() 
        {
            ::ULIS::TArray<::ULIS::FVec2I> points;

            GenerateLinePoints(::ULIS::FVec2I(line->mStartPoint.X, line->mStartPoint.Y), ::ULIS::FVec2I(line->mFinishPoint.X, line->mFinishPoint.Y), points);
            for( int i = 0; i < points.Size(); i++ )
            {
                paintEngine->PushStroke( FOdysseyStrokePoint( points[i].x, points[i].y ) );
            }
            paintEngine->EndStroke();

            return FReply::Handled();
        } );

        /*UOdysseyHUDHandle* handleStart = NewObject<UOdysseyHUDHandle>();
        handleStart->Init( FName("HandleStart"), line, &(line->mStartPoint), mEditor->PaintEngineHUD(), transform2D);
        UOdysseyHUDHandle* handleFinish = NewObject<UOdysseyHUDHandle>();
        handleFinish->Init(FName("HandleFinish"), line, &(line->mFinishPoint), mEditor->PaintEngineHUD(), transform2D);
        line->AddElement( handleStart );
        line->AddElement( handleFinish );*/


        /*UOdysseyHUDLine* line2 = NewObject<UOdysseyHUDLine>();
        line2->Init(FName("Line2"), FVector2D(58, 2), FVector2D(315, 251), mEditor->PaintEngineHUD(), transform2D);
        decoratedLine->AddElement(line2);/*

        UOdysseyHUDLine* line3 = NewObject<UOdysseyHUDLine>();
        line3->Init(FName("Line3"), FVector2D(425, 352), FVector2D(220, 102), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line3);

        UOdysseyHUDLine* line4 = NewObject<UOdysseyHUDLine>();
        line4->Init(FName("Line4"), FVector2D(10, 36), FVector2D(40, 90), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line4);

        UOdysseyHUDLine* line5 = NewObject<UOdysseyHUDLine>();
        line5->Init(FName("Line5"), FVector2D(234, 122), FVector2D(622, 874), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line5);

        UOdysseyHUDLine* line6 = NewObject<UOdysseyHUDLine>();
        line6->Init(FName("Line6"), FVector2D(150, 150), FVector2D(400, 400), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line6);

        UOdysseyHUDLine* line7 = NewObject<UOdysseyHUDLine>();
        line7->Init(FName("Line7"), FVector2D(200, 200), FVector2D(300, 300), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line7);*/

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
