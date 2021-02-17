// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorTopTab.h"

#include "OdysseyPainterEditor.h"
#include "SOdysseyPaintModifiers.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorTopTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTopTab::~FOdysseyPainterEditorTopTab()
{
}

FOdysseyPainterEditorTopTab::FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_TopBar"),
                            LOCTEXT( "OdysseyPainterEditorTopTab", "Top Bar" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.TopBar16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
	return mPaintModifiers = SNew( SOdysseyPaintModifiers )
        .Size(this, &FOdysseyPainterEditorTopTab::Size)
        .Opacity(this, &FOdysseyPainterEditorTopTab::Opacity)
        .Flow(this, &FOdysseyPainterEditorTopTab::Flow)
        .BlendingMode(this, &FOdysseyPainterEditorTopTab::BlendingMode)
        .AlphaMode(this, &FOdysseyPainterEditorTopTab::AlphaMode)
        .OnSizeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnSizeChanged )
        .OnOpacityChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnOpacityChanged )
        .OnFlowChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnFlowChanged )
        .OnBlendingModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnBlendingModeChanged )
        .OnAlphaModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnAlphaModeChanged );
}

TSharedRef< SDockTab >
FOdysseyPainterEditorTopTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == ID() );

    return SNew( SDockTab )
        .Label( DisplayName() )
        .ShouldAutosize( true )
        [
            Widget().ToSharedRef()
        ];
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

float
FOdysseyPainterEditorTopTab::Size() const
{
    return mEditor->PaintEngine()->GetSizeModifier();
}

float
FOdysseyPainterEditorTopTab::Opacity() const
{
    return mEditor->PaintEngine()->GetOpacityModifier();
}

float
FOdysseyPainterEditorTopTab::Flow() const
{
    return mEditor->PaintEngine()->GetFlowModifier();
}

::ul3::eBlendingMode
FOdysseyPainterEditorTopTab::BlendingMode() const
{
    return mEditor->PaintEngine()->GetBlendingModeModifier();
}

::ul3::eAlphaMode
FOdysseyPainterEditorTopTab::AlphaMode() const
{
    return mEditor->PaintEngine()->GetAlphaModeModifier();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyPaintModifiers>
FOdysseyPainterEditorTopTab::PaintModifiers()
{
    return mPaintModifiers;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::OnSizeChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetSizeModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnOpacityChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetOpacityModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnFlowChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetFlowModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnBlendingModeChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetBlendingModeModifier( static_cast<::ul3::eBlendingMode>( iValue ) );
}

void
FOdysseyPainterEditorTopTab::OnAlphaModeChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetAlphaModeModifier( static_cast<::ul3::eAlphaMode>(iValue) );
}

#undef LOCTEXT_NAMESPACE
