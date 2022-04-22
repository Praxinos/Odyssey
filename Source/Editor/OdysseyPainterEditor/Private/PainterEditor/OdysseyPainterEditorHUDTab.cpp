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
        line->Init( FName("Line1"), FVector2D(0, 0), FVector2D(0, 0), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line);

        UOdysseyHUDLine* subLine = NewObject<UOdysseyHUDLine>();
        subLine->Init(FName("SubLine"), FVector2D(100, 100), FVector2D(100, 200), mEditor->PaintEngineHUD());
        line->AddElement(subLine);

        UOdysseyHUDLine* line2 = NewObject<UOdysseyHUDLine>();
        line2->Init( FName("Line2"), FVector2D(500, 500), FVector2D(200, 200), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line2);

        UOdysseyHUDLine* line3 = NewObject<UOdysseyHUDLine>();
        line3->Init(FName("Line3"), FVector2D(500, 500), FVector2D(200, 200), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line3);

        UOdysseyHUDLine* line4 = NewObject<UOdysseyHUDLine>();
        line4->Init(FName("line4"), FVector2D(500, 500), FVector2D(200, 200), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line4);

        UOdysseyHUDLine* line5 = NewObject<UOdysseyHUDLine>();
        line5->Init(FName("line5"), FVector2D(500, 500), FVector2D(200, 200), mEditor->PaintEngineHUD());
        decoratedLine->AddElement(line5);

        mHUD = MakeShareable(decoratedLine);
    }

    return mHUD->CreateWidget();
    return SNullWidget::NullWidget;
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
