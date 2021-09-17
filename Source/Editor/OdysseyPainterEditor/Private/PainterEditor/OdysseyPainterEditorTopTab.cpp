// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorTopTab"

class FOdysseyPainterEditor;

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTopTab::~FOdysseyPainterEditorTopTab()
{
}

FOdysseyPainterEditorTopTab::FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_TopBar"),
                            LOCTEXT( "OdysseyPainterEditorTopTab", "Top Bar" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.TopBar16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
	return SNew( SOdysseyPaintModifiers )
        .OnGetSize(this, &FOdysseyPainterEditorTopTab::OnGetSize)
        .OnGetOpacity(this, &FOdysseyPainterEditorTopTab::OnGetOpacity)
        .OnGetFlow(this, &FOdysseyPainterEditorTopTab::OnGetFlow)
        .BlendingMode(this, &FOdysseyPainterEditorTopTab::BlendingMode)
        .AlphaMode(this, &FOdysseyPainterEditorTopTab::AlphaMode)
        .OnSizeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnSizeChanged )
        .OnOpacityChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnOpacityChanged )
        .OnFlowChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnFlowChanged )
        .OnBlendingModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnBlendingModeChanged )
        .OnAlphaModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnAlphaModeChanged );
}

void
FOdysseyPainterEditorTopTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorTopTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.IncreaseBrushSize, AddSize, 1 )
    MAP_ACTION(painterEditorCommands.DecreaseBrushSize, AddSize, -1 )
    MAP_ACTION(painterEditorCommands.SetAlphaModeNormal, SetAlphaMode, ::ul3::eAlphaMode::AM_NORMAL )
    MAP_ACTION(painterEditorCommands.SetAlphaModeErase, SetAlphaMode, ::ul3::eAlphaMode::AM_ERASE )
    MAP_ACTION(painterEditorCommands.SetAlphaModeTop, SetAlphaMode, ::ul3::eAlphaMode::AM_TOP )
    MAP_ACTION(painterEditorCommands.SetAlphaModeBack, SetAlphaMode, ::ul3::eAlphaMode::AM_BACK )
    MAP_ACTION(painterEditorCommands.SetAlphaModeSub, SetAlphaMode, ::ul3::eAlphaMode::AM_SUB )
    MAP_ACTION(painterEditorCommands.SetAlphaModeAdd, SetAlphaMode, ::ul3::eAlphaMode::AM_ADD )
    MAP_ACTION(painterEditorCommands.SetAlphaModeMul, SetAlphaMode, ::ul3::eAlphaMode::AM_MUL )
    MAP_ACTION(painterEditorCommands.SetAlphaModeMin, SetAlphaMode, ::ul3::eAlphaMode::AM_MIN )
    MAP_ACTION(painterEditorCommands.SetAlphaModeMax, SetAlphaMode, ::ul3::eAlphaMode::AM_MAX )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

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

int
FOdysseyPainterEditorTopTab::OnGetSize() const
{
    return static_cast<int>( mEditor->PaintEngine()->GetSizeModifier() );
}

float
FOdysseyPainterEditorTopTab::OnGetOpacity() const
{
    return mEditor->PaintEngine()->GetOpacityModifier();
}

float
FOdysseyPainterEditorTopTab::OnGetFlow() const
{
    return mEditor->PaintEngine()->GetFlowModifier();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::SetAlphaMode(::ul3::eAlphaMode iAlphaMode)
{
    mEditor->PaintEngine()->SetAlphaModeModifier( iAlphaMode );
}

void
FOdysseyPainterEditorTopTab::AddSize(int32 iValue)
{
    mEditor->PaintEngine()->SetSizeModifier( mEditor->PaintEngine()->GetSizeModifier() + iValue );
}

#undef LOCTEXT_NAMESPACE
