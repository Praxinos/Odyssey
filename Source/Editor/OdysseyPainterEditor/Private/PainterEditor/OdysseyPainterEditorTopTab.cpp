// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorTopTab.h"

#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"

#include "FileHelpers.h"
#include "ObjectEditorUtils.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Spark16" ))
    , mEditor( iEditor )
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

void FOdysseyPainterEditorTopTab::SetMeshMaxSize(float iValue /*= -1*/)
{
    mWidget->SetMeshMaxSize( iValue );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Public Getter

bool
FOdysseyPainterEditorTopTab::IsEraserButtonActive() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return false;

    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    return blendParameters.bEraserMode;
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
//---------------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
    mWidget = SNew(SOdysseyPaintModifiers)
        .OnGetSize(this, &FOdysseyPainterEditorTopTab::OnGetSize)
        .OnGetOpacity(this, &FOdysseyPainterEditorTopTab::OnGetOpacity)
        .OnGetFlow(this, &FOdysseyPainterEditorTopTab::OnGetFlow)
        .BlendingMode(this, &FOdysseyPainterEditorTopTab::BlendingMode)
        .AlphaMode(this, &FOdysseyPainterEditorTopTab::AlphaMode)
        .OnSizeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnSizeChanged)
        .OnOpacityChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnOpacityChanged)
        .OnFlowChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnFlowChanged)
        .OnBlendingModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnBlendingModeChanged)
        .OnAlphaModeChanged_Raw(this, &FOdysseyPainterEditorTopTab::OnAlphaModeChanged)
        .OnSaveButtonClicked_Raw(this, &FOdysseyPainterEditorTopTab::OnSaveButtonClicked)
        .OnUndoButtonClicked_Raw(this, &FOdysseyPainterEditorTopTab::OnUndoButtonClicked)
        .OnRedoButtonClicked_Raw(this, &FOdysseyPainterEditorTopTab::OnRedoButtonClicked)
        .OnEraserButtonClicked_Raw(this, &FOdysseyPainterEditorTopTab::OnEraserButtonClicked)
        .IsPackageEdited_Raw(this, &FOdysseyPainterEditorTopTab::IsPackageEdited)
        .IsEraserButtonActive_Raw(this, &FOdysseyPainterEditorTopTab::IsEraserButtonActive);

    return mWidget;
}

void
FOdysseyPainterEditorTopTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorTopTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION( painterEditorCommands.IncreaseBrushSize,              AddSize,                  1 )
    MAP_ACTION( painterEditorCommands.DecreaseBrushSize,              AddSize,                 -1 )
    MAP_ACTION( painterEditorCommands.SetAlphaModeNormal,             SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Normal )
    MAP_ACTION( painterEditorCommands.SetAlphaModeErase,              SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Erase )
    MAP_ACTION( painterEditorCommands.SetAlphaModeTop,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Top )
    MAP_ACTION( painterEditorCommands.SetAlphaModeBack,               SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Back )
    MAP_ACTION( painterEditorCommands.SetAlphaModeSub,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Sub )
    MAP_ACTION( painterEditorCommands.SetAlphaModeAdd,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Add )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMul,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Mul )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMin,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Min )
    MAP_ACTION( painterEditorCommands.SetAlphaModeMax,                SetAlphaMode,            ::ULIS::eAlphaMode::Alpha_Max )
    MAP_ACTION( painterEditorCommands.SetBlendModeNormal,             SetBlendingMode,         ::ULIS::eBlendMode::Blend_Normal )
    MAP_ACTION( painterEditorCommands.SetBlendModeTop,                SetBlendingMode,         ::ULIS::eBlendMode::Blend_Top )
    MAP_ACTION( painterEditorCommands.SetBlendModeBack,               SetBlendingMode,         ::ULIS::eBlendMode::Blend_Back )
    MAP_ACTION( painterEditorCommands.SetBlendModeBehind,             SetBlendingMode,         ::ULIS::eBlendMode::Blend_Behind )
    MAP_ACTION( painterEditorCommands.SetBlendModeDissolve,           SetBlendingMode,         ::ULIS::eBlendMode::Blend_Dissolve )
    MAP_ACTION( painterEditorCommands.SetBlendModeBayerDither8x8,     SetBlendingMode,         ::ULIS::eBlendMode::Blend_BayerDither8x8 )
    MAP_ACTION( painterEditorCommands.SetBlendModeDarken,             SetBlendingMode,         ::ULIS::eBlendMode::Blend_Darken )
    MAP_ACTION( painterEditorCommands.SetBlendModeMultiply,           SetBlendingMode,         ::ULIS::eBlendMode::Blend_Multiply )
    MAP_ACTION( painterEditorCommands.SetBlendModeColorBurn,          SetBlendingMode,         ::ULIS::eBlendMode::Blend_ColorBurn )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearBurn,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_LinearBurn )
    MAP_ACTION( painterEditorCommands.SetBlendModeDarkerColor,        SetBlendingMode,         ::ULIS::eBlendMode::Blend_DarkerColor )
    MAP_ACTION( painterEditorCommands.SetBlendModeLighten,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_Lighten )
    MAP_ACTION( painterEditorCommands.SetBlendModeScreen,             SetBlendingMode,         ::ULIS::eBlendMode::Blend_Screen )
    MAP_ACTION( painterEditorCommands.SetBlendModeColorDodge,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_ColorDodge )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearDodge,        SetBlendingMode,         ::ULIS::eBlendMode::Blend_LinearDodge )
    MAP_ACTION( painterEditorCommands.SetBlendModeLighterColor,       SetBlendingMode,         ::ULIS::eBlendMode::Blend_LighterColor )
    MAP_ACTION( painterEditorCommands.SetBlendModeOverlay,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_Overlay )
    MAP_ACTION( painterEditorCommands.SetBlendModeSoftLight,          SetBlendingMode,         ::ULIS::eBlendMode::Blend_SoftLight )
    MAP_ACTION( painterEditorCommands.SetBlendModeHardLight,          SetBlendingMode,         ::ULIS::eBlendMode::Blend_HardLight )
    MAP_ACTION( painterEditorCommands.SetBlendModeVividLight,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_VividLight )
    MAP_ACTION( painterEditorCommands.SetBlendModeLinearLight,        SetBlendingMode,         ::ULIS::eBlendMode::Blend_LinearLight )
    MAP_ACTION( painterEditorCommands.SetBlendModePinLight,           SetBlendingMode,         ::ULIS::eBlendMode::Blend_PinLight )
    MAP_ACTION( painterEditorCommands.SetBlendModeHardMix,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_HardMix )
    MAP_ACTION( painterEditorCommands.SetBlendModePhoenix,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_Phoenix )
    MAP_ACTION( painterEditorCommands.SetBlendModeReflect,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_Reflect )
    MAP_ACTION( painterEditorCommands.SetBlendModeGlow,               SetBlendingMode,         ::ULIS::eBlendMode::Blend_Glow )
    MAP_ACTION( painterEditorCommands.SetBlendModeDifference,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_Difference )
    MAP_ACTION( painterEditorCommands.SetBlendModeExclusion,          SetBlendingMode,         ::ULIS::eBlendMode::Blend_Exclusion )
    MAP_ACTION( painterEditorCommands.SetBlendModeAdd,                SetBlendingMode,         ::ULIS::eBlendMode::Blend_Add )
    MAP_ACTION( painterEditorCommands.SetBlendModeSubstract,          SetBlendingMode,         ::ULIS::eBlendMode::Blend_Substract )
    MAP_ACTION( painterEditorCommands.SetBlendModeDivide,             SetBlendingMode,         ::ULIS::eBlendMode::Blend_Divide )
    MAP_ACTION( painterEditorCommands.SetBlendModeAverage,            SetBlendingMode,         ::ULIS::eBlendMode::Blend_Average )
    MAP_ACTION( painterEditorCommands.SetBlendModeNegation,           SetBlendingMode,         ::ULIS::eBlendMode::Blend_Negation )
    MAP_ACTION( painterEditorCommands.SetBlendModeHue,                SetBlendingMode,         ::ULIS::eBlendMode::Blend_Hue )
    MAP_ACTION( painterEditorCommands.SetBlendModeSaturation,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_Saturation )
    MAP_ACTION( painterEditorCommands.SetBlendModeColor,              SetBlendingMode,         ::ULIS::eBlendMode::Blend_Color )
    MAP_ACTION( painterEditorCommands.SetBlendModeLuminosity,         SetBlendingMode,         ::ULIS::eBlendMode::Blend_Luminosity )
    MAP_ACTION( painterEditorCommands.SetBlendModePartialDerivative,  SetBlendingMode,         ::ULIS::eBlendMode::Blend_PartialDerivative )
    MAP_ACTION( painterEditorCommands.SetBlendModeWhiteOut,           SetBlendingMode,         ::ULIS::eBlendMode::Blend_Whiteout )
    MAP_ACTION( painterEditorCommands.SetBlendModeAngleCorrected,     SetBlendingMode,         ::ULIS::eBlendMode::Blend_AngleCorrected )
    MAP_ACTION( painterEditorCommands.ToggleEraserButton,             ToggleEraserButton )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ULIS::eBlendMode
FOdysseyPainterEditorTopTab::BlendingMode() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return ::ULIS::Blend_Normal;

    return static_cast<::ULIS::eBlendMode>(drawingTool->GetBlendParameters().BlendingMode);
}

::ULIS::eAlphaMode
FOdysseyPainterEditorTopTab::AlphaMode() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return ::ULIS::Alpha_Normal;

    return static_cast<::ULIS::eAlphaMode>(drawingTool->GetBlendParameters().AlphaMode);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::OnSizeChanged( float iValue, EPropertyChangeType::Type iChangeType )
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    //TODO: Remove when the top will use singlePropertyview
    UOdysseyBrushOptions* brushOptions = drawingTool->GetBrushInstance()->GetBrushOptions();
    FOdysseyObjectEditorUtils::SetPropertyValue(brushOptions, "Size", iValue, iChangeType);
}

void
FOdysseyPainterEditorTopTab::OnOpacityChanged( int32 iValue, EPropertyChangeType::Type iChangeType )
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    //TODO: Remove when the top will use singlePropertyview
    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.Opacity = float(iValue);
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters, iChangeType);
}

void
FOdysseyPainterEditorTopTab::OnFlowChanged( int32 iValue, EPropertyChangeType::Type iChangeType )
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    //TODO: Remove when the top will use singlePropertyview
    UOdysseyBrushOptions* brushOptions = drawingTool->GetBrushInstance()->GetBrushOptions();
    FOdysseyObjectEditorUtils::SetPropertyValue(brushOptions, "Flow", float(iValue), iChangeType);
}

void
FOdysseyPainterEditorTopTab::OnBlendingModeChanged( int32 iValue )
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    //TODO: Remove when the top will use singlePropertyview
    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.BlendingMode = static_cast<EOdysseyBlendingMode>(iValue);
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters);
}

void
FOdysseyPainterEditorTopTab::OnAlphaModeChanged( int32 iValue )
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    //TODO: Remove when the top will use singlePropertyview
    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.AlphaMode = static_cast<EOdysseyAlphaMode>(iValue);
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters);
}

float
FOdysseyPainterEditorTopTab::OnGetSize() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return 0;

    return drawingTool->GetBrushInstance()->GetBrushOptions()->Size;
}

float
FOdysseyPainterEditorTopTab::OnGetOpacity() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return 0.f;

    return drawingTool->GetBlendParameters().Opacity;
}

float
FOdysseyPainterEditorTopTab::OnGetFlow() const
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return 0.f;

    return drawingTool->GetBrushInstance()->GetBrushOptions()->Flow;
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
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.bEraserMode = !blendParameters.bEraserMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters);
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
FOdysseyPainterEditorTopTab::SetAlphaMode(::ULIS::eAlphaMode iAlphaMode)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.AlphaMode = static_cast<EOdysseyAlphaMode>(iAlphaMode);
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters);
}

void
FOdysseyPainterEditorTopTab::SetBlendingMode(::ULIS::eBlendMode iBlendingMode)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    FOdysseyBlendParameters blendParameters = drawingTool->GetBlendParameters();
    blendParameters.BlendingMode = static_cast<EOdysseyBlendingMode>(iBlendingMode);
    FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool, "BlendParameters", blendParameters);
}

void
FOdysseyPainterEditorTopTab::AddSize(int32 iValue)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;

    UOdysseyBrushOptions* brushOptions = drawingTool->GetBrushInstance()->GetBrushOptions();
    FOdysseyObjectEditorUtils::SetPropertyValue(brushOptions, "Size", brushOptions->Size + iValue);
}

#undef LOCTEXT_NAMESPACE
