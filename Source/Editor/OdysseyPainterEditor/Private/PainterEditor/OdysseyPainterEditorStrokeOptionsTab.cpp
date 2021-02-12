// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorStrokeOptionsTab.h"

#include "SOdysseyStrokeOptions.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorStrokeOptionsTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorStrokeOptionsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorStrokeOptionsTab::~FOdysseyPainterEditorStrokeOptionsTab()
{
}

FOdysseyPainterEditorStrokeOptionsTab::FOdysseyPainterEditorStrokeOptionsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_StrokeOptions"),
                            LOCTEXT( "OdysseyPainterEditorStrokeOptionsTab", "Stroke Options" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ))
    , mEditor(iEditor)
	, mStrokeOptions(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorStrokeOptionsTab::CreateWidget()
{
	
	mStrokeOptions = SNew(SOdysseyStrokeOptions)
        .OnStrokeStepChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnStrokeStepChanged )
        .OnStrokeAdaptativeChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnStrokeAdaptativeChanged )
        .OnStrokePaintOnTickChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnStrokePaintOnTickChanged )
        .OnInterpolationTypeChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnInterpolationTypeChanged )
        .OnSmoothingMethodChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingMethodChanged )
        .OnSmoothingStrengthChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingStrengthChanged )
        .OnSmoothingEnabledChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingEnabledChanged )
        .OnSmoothingRealTimeChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingRealTimeChanged )
        .OnSmoothingCatchUpChanged_Raw(this, &FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingCatchUpChanged );

	//TODO: Best way to make this better, is by turning the PaintEngine into a UObject

	mStrokeOptions->SetStrokeStep(20);
	mStrokeOptions->SetStrokeAdaptative(true);
	mStrokeOptions->SetStrokePaintOnTick(false);
	mStrokeOptions->SetInterpolationType((int32)EOdysseyInterpolationType::kCatmullRom);
	mStrokeOptions->SetSmoothingMethod((int32)EOdysseySmoothingMethod::kAverage);
	mStrokeOptions->SetSmoothingStrength(10);
	mStrokeOptions->SetSmoothingEnabled(true);
	mStrokeOptions->SetSmoothingRealTime(true);
	mStrokeOptions->SetSmoothingCatchUp(true);

	return mStrokeOptions;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyStrokeOptions>
FOdysseyPainterEditorStrokeOptionsTab::StrokeOptions()
{
	return mStrokeOptions;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorStrokeOptionsTab::OnStrokeStepChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetStrokeStep( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnStrokeAdaptativeChanged( bool iValue )
{
	mEditor->PaintEngine()->SetStrokeAdaptative( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnStrokePaintOnTickChanged( bool iValue )
{
	mEditor->PaintEngine()->SetStrokePaintOnTick( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnInterpolationTypeChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetInterpolationType( static_cast<EOdysseyInterpolationType>( iValue ) );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingMethodChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetSmoothingMethod( static_cast<EOdysseySmoothingMethod>( iValue ) );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingStrengthChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetSmoothingStrength( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingEnabledChanged( bool iValue )
{
	mEditor->PaintEngine()->SetSmoothingEnabled( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingRealTimeChanged( bool iValue )
{
	mEditor->PaintEngine()->SetSmoothingRealTime( iValue );
}

void
FOdysseyPainterEditorStrokeOptionsTab::OnSmoothingCatchUpChanged( bool iValue )
{
	mEditor->PaintEngine()->SetSmoothingCatchUp( iValue );
}

#undef LOCTEXT_NAMESPACE
