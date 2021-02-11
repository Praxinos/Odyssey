// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorTopTab.h"

#include "OdysseyPainterEditor.h"

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
        /* .Size(this, &FOdysseyPainterEditorTopTab::Size)
        .Opacity(this, &FOdysseyPainterEditorTopTab::Opacity)
        .Flow(this, &FOdysseyPainterEditorTopTab::Flow)
        .AlphaMode(this, &FOdysseyPainterEditorTopTab::AlphaMode)*/
        .OnSizeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnSizeModifierChanged )
        .OnOpacityChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnOpacityModifierChanged )
        .OnFlowChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnFlowModifierChanged )
        .OnBlendingModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnBlendingModeModifierChanged )
        .OnAlphaModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnAlphaModeModifierChanged );

	mPaintModifiers->SetAlphaMode(mEditor->PaintEngine()->GetAlphaMode()); //TODO: Do an AlphaMode Getter (beware layer alphamode lock) 
	mPaintModifiers->SetSize(20);
	mPaintModifiers->SetOpacity(100);
	mPaintModifiers->SetFlow(100);
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
//----------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyPaintModifiers>
FOdysseyPainterEditorTopTab::PaintModifiers()
{
    return mPaintModifiers;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::OnSizeModifierChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetSizeModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnOpacityModifierChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetOpacityModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnFlowModifierChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetFlowModifier( iValue );
}

void
FOdysseyPainterEditorTopTab::OnBlendingModeModifierChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetBlendingModeModifier( static_cast<::ul3::eBlendingMode>( iValue ) );
}

void
FOdysseyPainterEditorTopTab::OnAlphaModeModifierChanged( int32 iValue )
{
	mEditor->PaintEngine()->SetAlphaModeModifier( static_cast<::ul3::eAlphaMode>(iValue) );
}

#undef LOCTEXT_NAMESPACE
