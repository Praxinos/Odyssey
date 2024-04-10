// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorTopTab.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"

#include "FileHelpers.h"
#include "ObjectEditorUtils.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorTopTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_TopBar"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

class FOdysseyPainterEditor;

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTopTab::~FOdysseyPainterEditorTopTab()
{
    mEditor->OnSelectedToolChanged().RemoveAll(this);
}

FOdysseyPainterEditorTopTab::FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(LOCTEXT( "top-tab.name", "Top Bar" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Spark16" ))
    , mEditor( iEditor )
    , mToolWidgetSlot(nullptr)
    , mCurrentTool(nullptr)
{   
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

void
FOdysseyPainterEditorTopTab::Init()
{
    mEditor->OnSelectedToolChanged().AddSP(SharedThis<FOdysseyPainterEditorTopTab>(this), &FOdysseyPainterEditorTopTab::OnSelectedToolChanged);
    FOdysseyEditorTab::Init();
}

TSharedRef< SDockTab >
FOdysseyPainterEditorTopTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
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
FOdysseyPainterEditorTopTab::IsPackageEdited() const
{
    UObject* editedObject = mEditor->GetEditedObject();
    if (editedObject && editedObject->GetOutermost()->IsDirty())
        return true;

    TArray<UObject*> additionalEditedObjects = mEditor->GetAdditionalEditedObjects();
    for( UObject* additionalEditedObject : additionalEditedObjects )
    {
        if( additionalEditedObject->GetOutermost()->IsDirty() )
            return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorTopTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
    TSharedPtr<SWidget> widget = SAssignNew( mWrapBox, SWrapBox )
        .UseAllottedSize(true)
        //.InnerSlotPadding(FVector2D(3.f, 3.f))
        +SWrapBox::Slot()
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.save", "Save the edited asset.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( this, &FOdysseyPainterEditorTopTab::OnSaveButtonClicked )
            .IsEnabled( this, &FOdysseyPainterEditorTopTab::IsPackageEdited )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Save32" ) )
            ]
        ]
        + SWrapBox::Slot()
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.undo", "Undo the previous action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnUndoButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Undo32" ) )
            ]
        ]
        + SWrapBox::Slot()
        .Padding( 0.f, 0.f, 30.f, 0.f )
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.redo", "Redo the next action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnRedoButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Redo32" ) )
            ]
        ]

        + SWrapBox::Slot()
        .Padding( 0.f, 0.f, 30.f, 0.f )
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.clear", "Clear the whole canvas.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnClearButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Clear32" ) )
            ]
        ]
        
        + SWrapBox::Slot()
        .FillEmptySpace(true)
        .VAlign(VAlign_Center)
        .Expose(mToolWidgetSlot)
        [
            SNullWidget::NullWidget
        ];

    UpdateToolWidget();
    return widget;
}

void
FOdysseyPainterEditorTopTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorTopTab::__VA_ARGS__ ), FCanExecuteAction() );

    /* MAP_ACTION( painterEditorCommands.IncreaseBrushSize,              AddSize,                  1 )
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
    MAP_ACTION( painterEditorCommands.ToggleEraserButton,             ToggleEraserButton ) */

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::UpdateToolWidget()
{
    //Clear the tool widget content
    //mToolWidgetSlot->DetachWidget();
    if (mCurrentTool == mEditor->GetSelectedTool())
        return; //Fix jumping UI when moving in animation timeline

    mCurrentTool = mEditor->GetSelectedTool();
    if(!mCurrentTool)
    {
        mToolWidgetSlot->AttachWidget(SNullWidget::NullWidget);
        return;
    }

    mToolWidgetSlot->AttachWidget(mCurrentTool->GetTopTabWidget());
}

void
FOdysseyPainterEditorTopTab::OnSelectedToolChanged()
{
    UpdateToolWidget();
}

FReply
FOdysseyPainterEditorTopTab::OnSaveButtonClicked()
{
    TArray<UPackage*> packages;
    UObject* editedObject = mEditor->GetEditedObject();
    if (editedObject)
        packages.Add(editedObject->GetOutermost());

    TArray<UObject*> additionalEditedObjects = mEditor->GetAdditionalEditedObjects();
    for( UObject* additionalEditedObject : additionalEditedObjects )
    {
        packages.Add( additionalEditedObject->GetOutermost() );
    }

    FEditorFileUtils::PromptForCheckoutAndSave(packages, true, false);
    
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnUndoButtonClicked()
{
    GEditor->UndoTransaction(true);
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnRedoButtonClicked()
{
    GEditor->RedoTransaction();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnClearButtonClicked()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source)
        return FReply::Unhandled();
    
    source->Clear();
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners


#undef LOCTEXT_NAMESPACE
