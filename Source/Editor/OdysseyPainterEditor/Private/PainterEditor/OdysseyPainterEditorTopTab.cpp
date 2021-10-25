// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "FileHelpers.h"

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
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ))
    , mEditor( iEditor )
    , mToolDefaultBlendingMode( ::ul3::eBlendingMode::BM_NORMAL )
    , mToolDefaultAlphaMode( ::ul3::eAlphaMode::AM_NORMAL )
    , mIsEraserButtonActive( false )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

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
//------------------------------------------------------------------------ Public Getter

bool
FOdysseyPainterEditorTopTab::IsEraserButtonActive() const
{
    return mIsEraserButtonActive;
}

bool
FOdysseyPainterEditorTopTab::IsPackageEdited() const
{
    for( int i=0; i<mEditor->GetEditedObjects().Num(); i++ )
    {
        if( mEditor->GetEditedObjects()[i]->GetOutermost()->IsDirty() )
            return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Setters

void 
FOdysseyPainterEditorTopTab::SetToolDefaultBlendingMode( ::ul3::eBlendingMode iBlendingMode )
{
    mToolDefaultBlendingMode = iBlendingMode;
    return;
}

void 
FOdysseyPainterEditorTopTab::SetToolDefaultAlphaMode( ::ul3::eAlphaMode iAlphaMode )
{
    mToolDefaultAlphaMode = iAlphaMode;
    return;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
	return SNew( SOdysseyPaintModifiers )
        .OnGetSize( this, &FOdysseyPainterEditorTopTab::OnGetSize )
        .OnGetOpacity( this, &FOdysseyPainterEditorTopTab::OnGetOpacity )
        .OnGetFlow( this, &FOdysseyPainterEditorTopTab::OnGetFlow )
        .BlendingMode( this, &FOdysseyPainterEditorTopTab::BlendingMode )
        .AlphaMode( this, &FOdysseyPainterEditorTopTab::AlphaMode )
        .OnSizeChanged_Raw( this, &FOdysseyPainterEditorTopTab::OnSizeChanged )
        .OnOpacityChanged_Raw( this, &FOdysseyPainterEditorTopTab::OnOpacityChanged )
        .OnFlowChanged_Raw( this, &FOdysseyPainterEditorTopTab::OnFlowChanged )
        .OnBlendingModeChanged_Raw( this, &FOdysseyPainterEditorTopTab::OnBlendingModeChanged )
        .OnAlphaModeChanged_Raw( this, &FOdysseyPainterEditorTopTab::OnAlphaModeChanged )
        .OnSaveButtonClicked_Raw( this, &FOdysseyPainterEditorTopTab::OnSaveButtonClicked )
        .OnUndoButtonClicked_Raw( this, &FOdysseyPainterEditorTopTab::OnUndoButtonClicked )
        .OnRedoButtonClicked_Raw( this, &FOdysseyPainterEditorTopTab::OnRedoButtonClicked )
        .OnEraserButtonClicked_Raw( this, &FOdysseyPainterEditorTopTab::OnEraserButtonClicked )
        .IsPackageEdited_Raw( this, &FOdysseyPainterEditorTopTab::IsPackageEdited )
        .IsEraserButtonActive_Lambda( [this](){return mIsEraserButtonActive;} );
}

void
FOdysseyPainterEditorTopTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorTopTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION( painterEditorCommands.IncreaseBrushSize,              AddSize,                  1 )
    MAP_ACTION( painterEditorCommands.DecreaseBrushSize,              AddSize,                 -1 )
    MAP_ACTION( painterEditorCommands.SetAlphaModeNormal,             SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_NORMAL )
    MAP_ACTION( painterEditorCommands.SetAlphaModeErase,              SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_ERASE )
    MAP_ACTION( painterEditorCommands.SetAlphaModeTop,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_TOP )
    MAP_ACTION( painterEditorCommands.SetAlphaModeBack,               SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_BACK )
    MAP_ACTION( painterEditorCommands.SetAlphaModeSub,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_SUB )
    MAP_ACTION( painterEditorCommands.SetAlphaModeAdd,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_ADD )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMul,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_MUL )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMin,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_MIN )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMax,                SetAlphaModeShortcut,    ::ul3::eAlphaMode::AM_MAX )
    MAP_ACTION( painterEditorCommands.SetBlendModeNormal,             SetBlendingMode,         ::ul3::eBlendingMode::BM_NORMAL )
    MAP_ACTION( painterEditorCommands.SetBlendModeTop,                SetBlendingMode,         ::ul3::eBlendingMode::BM_TOP )
    MAP_ACTION( painterEditorCommands.SetBlendModeBack,               SetBlendingMode,         ::ul3::eBlendingMode::BM_BACK )
    MAP_ACTION( painterEditorCommands.SetBlendModeBehind,             SetBlendingMode,         ::ul3::eBlendingMode::BM_BEHIND )
    MAP_ACTION( painterEditorCommands.SetBlendModeDissolve,           SetBlendingMode,         ::ul3::eBlendingMode::BM_DISSOLVE )
    MAP_ACTION( painterEditorCommands.SetBlendModeBayerDither8x8,     SetBlendingMode,         ::ul3::eBlendingMode::BM_BAYERDITHER8x8 )
    MAP_ACTION( painterEditorCommands.SetBlendModeDarken,             SetBlendingMode,         ::ul3::eBlendingMode::BM_DARKEN )
    MAP_ACTION( painterEditorCommands.SetBlendModeMultiply,           SetBlendingMode,         ::ul3::eBlendingMode::BM_MULTIPLY )
    MAP_ACTION( painterEditorCommands.SetBlendModeColorBurn,          SetBlendingMode,         ::ul3::eBlendingMode::BM_COLORBURN )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearBurn,         SetBlendingMode,         ::ul3::eBlendingMode::BM_LINEARBURN )
    MAP_ACTION( painterEditorCommands.SetBlendModeDarkerColor,        SetBlendingMode,         ::ul3::eBlendingMode::BM_DARKERCOLOR )
    MAP_ACTION( painterEditorCommands.SetBlendModeLighten,            SetBlendingMode,         ::ul3::eBlendingMode::BM_LIGHTEN )
    MAP_ACTION( painterEditorCommands.SetBlendModeScreen,             SetBlendingMode,         ::ul3::eBlendingMode::BM_SCREEN )
    MAP_ACTION( painterEditorCommands.SetBlendModeColorDodge,         SetBlendingMode,         ::ul3::eBlendingMode::BM_COLORDODGE )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearDodge,        SetBlendingMode,         ::ul3::eBlendingMode::BM_LINEARDODGE )
    MAP_ACTION( painterEditorCommands.SetBlendModeLighterColor,       SetBlendingMode,         ::ul3::eBlendingMode::BM_LIGHTERCOLOR )
    MAP_ACTION( painterEditorCommands.SetBlendModeOverlay,            SetBlendingMode,         ::ul3::eBlendingMode::BM_OVERLAY )
    MAP_ACTION( painterEditorCommands.SetBlendModeSoftLight,          SetBlendingMode,         ::ul3::eBlendingMode::BM_SOFTLIGHT )
    MAP_ACTION( painterEditorCommands.SetBlendModeHardLight,          SetBlendingMode,         ::ul3::eBlendingMode::BM_HARDLIGHT )
    MAP_ACTION( painterEditorCommands.SetBlendModeVividLight,         SetBlendingMode,         ::ul3::eBlendingMode::BM_VIVIDLIGHT )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearLight,        SetBlendingMode,         ::ul3::eBlendingMode::BM_LINEARLIGHT )
    MAP_ACTION( painterEditorCommands.SetBlendModePinLight,           SetBlendingMode,         ::ul3::eBlendingMode::BM_PINLIGHT )
    MAP_ACTION( painterEditorCommands.SetBlendModeHardMix,            SetBlendingMode,         ::ul3::eBlendingMode::BM_HARDMIX )
    MAP_ACTION( painterEditorCommands.SetBlendModePhoenix,            SetBlendingMode,         ::ul3::eBlendingMode::BM_PHOENIX )
    MAP_ACTION( painterEditorCommands.SetBlendModeReflect,            SetBlendingMode,         ::ul3::eBlendingMode::BM_REFLECT )
    MAP_ACTION( painterEditorCommands.SetBlendModeGlow,               SetBlendingMode,         ::ul3::eBlendingMode::BM_GLOW )
    MAP_ACTION( painterEditorCommands.SetBlendModeDifference,         SetBlendingMode,         ::ul3::eBlendingMode::BM_DIFFERENCE )
    MAP_ACTION( painterEditorCommands.SetBlendModeExclusion,          SetBlendingMode,         ::ul3::eBlendingMode::BM_EXCLUSION )
    MAP_ACTION( painterEditorCommands.SetBlendModeAdd,                SetBlendingMode,         ::ul3::eBlendingMode::BM_ADD )
    MAP_ACTION( painterEditorCommands.SetBlendModeSubstract,          SetBlendingMode,         ::ul3::eBlendingMode::BM_SUBSTRACT )
    MAP_ACTION( painterEditorCommands.SetBlendModeDivide,             SetBlendingMode,         ::ul3::eBlendingMode::BM_DIVIDE )
    MAP_ACTION( painterEditorCommands.SetBlendModeAverage,            SetBlendingMode,         ::ul3::eBlendingMode::BM_AVERAGE )
    MAP_ACTION( painterEditorCommands.SetBlendModeNegation,           SetBlendingMode,         ::ul3::eBlendingMode::BM_NEGATION )
    MAP_ACTION( painterEditorCommands.SetBlendModeHue,                SetBlendingMode,         ::ul3::eBlendingMode::BM_HUE )
    MAP_ACTION( painterEditorCommands.SetBlendModeSaturation,         SetBlendingMode,         ::ul3::eBlendingMode::BM_SATURATION )
    MAP_ACTION( painterEditorCommands.SetBlendModeColor,              SetBlendingMode,         ::ul3::eBlendingMode::BM_COLOR )
    MAP_ACTION( painterEditorCommands.SetBlendModeLuminosity,         SetBlendingMode,         ::ul3::eBlendingMode::BM_LUMINOSITY )
    MAP_ACTION( painterEditorCommands.SetBlendModePartialDerivative,  SetBlendingMode,         ::ul3::eBlendingMode::BM_PARTIALDERIVATIVE )
    MAP_ACTION( painterEditorCommands.SetBlendModeWhiteOut,           SetBlendingMode,         ::ul3::eBlendingMode::BM_WHITEOUT )
    MAP_ACTION( painterEditorCommands.SetBlendModeAngleCorrected,     SetBlendingMode,         ::ul3::eBlendingMode::BM_ANGLECORRECTED )
    MAP_ACTION( painterEditorCommands.ToggleEraserButton,             ToggleEraserButton )

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
        SetAlphaMode( static_cast<::ul3::eAlphaMode>(iValue) );
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

FReply
FOdysseyPainterEditorTopTab::OnSaveButtonClicked()
{
    TArray<UPackage*> packages;
    for( int i=0; i<mEditor->GetEditedObjects().Num(); i++ )
    {
        packages.Add( mEditor->GetEditedObjects()[i]->GetOutermost() );
    }
    FEditorFileUtils::PromptForCheckoutAndSave(packages, /*bCheckDirtyOnAssetSave*/ true, /*bPromptToSave=*/ false);
    
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnUndoButtonClicked()
{
    mEditor->Undo();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnRedoButtonClicked()
{
    mEditor->Redo();
    return FReply::Handled();
}

void
FOdysseyPainterEditorTopTab::ToggleEraserButton()
{
    if( !mIsEraserButtonActive )
    {
        mToolDefaultBlendingMode = mEditor->PaintEngine()->GetBlendingModeModifier();
        mToolDefaultAlphaMode = mEditor->PaintEngine()->GetAlphaModeModifier();
        SetAlphaMode( ::ul3::eAlphaMode::AM_ERASE );
        mEditor->PaintEngine()->SetBlendingModeModifier( ::ul3::eBlendingMode::BM_BACK );
        mIsEraserButtonActive = true;
    }
    else
    {
        SetAlphaMode( mToolDefaultAlphaMode );
        mEditor->PaintEngine()->SetBlendingModeModifier( mToolDefaultBlendingMode );
        mIsEraserButtonActive = false;
    }
}

FReply
FOdysseyPainterEditorTopTab::OnEraserButtonClicked()
{
    FOdysseyPainterEditorTopTab::ToggleEraserButton();
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::SetAlphaModeShortcut(::ul3::eAlphaMode iAlphaMode)
{
    if( !mIsEraserButtonActive )
    {
        SetAlphaMode( iAlphaMode );
    }
}

void
FOdysseyPainterEditorTopTab::SetAlphaMode(::ul3::eAlphaMode iAlphaMode)
{
        mEditor->PaintEngine()->SetAlphaModeModifier( iAlphaMode );
}

void
FOdysseyPainterEditorTopTab::SetBlendingMode(::ul3::eBlendingMode iBlendingMode)
{
    if( !mIsEraserButtonActive )
    {
        mEditor->PaintEngine()->SetBlendingModeModifier( iBlendingMode );
    }
}

void
FOdysseyPainterEditorTopTab::AddSize(int32 iValue)
{
    mEditor->PaintEngine()->SetSizeModifier( mEditor->PaintEngine()->GetSizeModifier() + iValue );
}

#undef LOCTEXT_NAMESPACE
