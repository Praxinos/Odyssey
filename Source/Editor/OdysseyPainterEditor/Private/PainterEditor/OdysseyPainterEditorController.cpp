// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "PainterEditor/OdysseyPainterEditorGUI.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "Dialogs/CustomDialog.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "PainterEditor/OdysseyPainterEditorState.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorController::~FOdysseyPainterEditorController()
{
}

FOdysseyPainterEditorController::FOdysseyPainterEditorController()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorController::InitOdysseyPainterEditorController(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    mMenuExtenders.Add(CreateMenuExtender(iToolkitCommands));

    // Add Menu Extender
    //GetMenuExtenders().Add(CreateMenuExtenders(iToolkitCommands));

    // Register our commands. This will only register them if not previously registered
    // FOdysseyPainterEditorCommands::Register();

    // Build commands
    FOdysseyPainterEditorController::BindCommands(iToolkitCommands);

	//Add PaintEngine Callbacks
	GetEditor()->PaintEngine()->OnPreviewBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged);
	GetEditor()->PaintEngine()->OnEditedBlockTilesWillChange().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange);
	GetEditor()->PaintEngine()->OnEditedBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged);
	GetEditor()->PaintEngine()->OnStrokeAbort().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeAbort);
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyPainterEditorController::GetMenuExtenders() const
{
	return mMenuExtenders;
}

TArray<TSharedPtr<FExtender>>&
FOdysseyPainterEditorController::GetMenuExtenders()
{
	return mMenuExtenders;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyPainterEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().AboutIliad,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnAboutIliad ),
        FCanExecuteAction() );
    
	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().Undo,
        FExecuteAction::CreateLambda( [this]() { this->OnUndoIliad(); } ),
        FCanExecuteAction() );
    
	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().Redo,
        FExecuteAction::CreateLambda( [this]() { this->OnRedoIliad(); } ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosWebsite ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosForums,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosForums ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().FillCurrentLayer,
        FExecuteAction::CreateLambda( [this]() { this->OnFill(); } ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ClearCurrentLayer,
        FExecuteAction::CreateLambda( [this]() { this->OnClear(); } ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().CreateNewLayer,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnCreateNewLayer ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().DuplicateCurrentLayer,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnDuplicateCurrentLayer ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().DeleteCurrentLayer,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnDeleteCurrentLayer ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().RefreshBrush,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnRefreshBrush ),
        FCanExecuteAction() );
        
    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().IncreaseBrushSize,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnAddBrushSize, 1 ),
        FCanExecuteAction() );
        
    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().DecreaseBrushSize,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnAddBrushSize, -1 ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeNormal,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_NORMAL ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeErase,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_ERASE ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeTop,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_TOP ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeBack,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_BACK ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeSub,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_SUB ),
        FCanExecuteAction() );
        
    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeAdd,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_ADD ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeMul,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_MUL ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeMin,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_MIN ),
        FCanExecuteAction() );

    iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetAlphaModeMax,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetAlphaMode, ::ul3::eAlphaMode::AM_MAX ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SwitchTabletAPI,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSwitchTabletAPI ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnImportTexturesAsLayers ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnExportLayersAsTextures ),
        FCanExecuteAction() );
}

TSharedPtr<FExtender>
FOdysseyPainterEditorController::CreateMenuExtender(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    FExtender* extender = new FExtender();

	extender->AddMenuExtension(
        "HelpApplication",
        EExtensionHook::After,
		iToolkitCommands,
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorController& >( &FOdysseyPainterEditorController::FillAboutMenu, *this ) );

	extender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
		iToolkitCommands,
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorController& >( &FOdysseyPainterEditorController::FillImportExportMenu, *this) );

    return MakeShareable(extender);
}

//static
void
FOdysseyPainterEditorController::FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyTextureEditor )
{
    ioMenuBuilder.BeginSection( "FileOdysseyTexture", LOCTEXT( "OdysseyTexture", "OdysseyTexture" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures );
    }
}

//static
void
FOdysseyPainterEditorController::FillAboutMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyPainterEditor )
{
    ioMenuBuilder.BeginSection( "About", LOCTEXT( "OdysseyPainter", "OdysseyPainter" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().AboutIliad );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosForums );
    }
}

TSharedRef<SWidget>
FOdysseyPainterEditorController::GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem )
{
	return  SNew(STextBlock)
            .Text( UOdysseyStylusInputSettings::GetFormatText( iItem ) );
}

void 
FOdysseyPainterEditorController::ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo )
{
    mComboBoxTabletAPISelected = iNewSelection;
}

FText 
FOdysseyPainterEditorController::GetComboBoxTabletAPISelectedAsText() const
{
    return UOdysseyStylusInputSettings::GetFormatText( mComboBoxTabletAPISelected );
}



void
FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineStrokeAbort()
{
}

void
FOdysseyPainterEditorController::OnAboutIliad()
{
    const FText aboutWindowTitle = LOCTEXT( "AboutIliad", "About Iliad" );

    TSharedPtr<SWindow> aboutWindow =
        SNew( SWindow )
        .Title( aboutWindowTitle )
        .ClientSize( FVector2D( 600.f, 460.f ) )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .SizingRule( ESizingRule::FixedSize )
        [
            SNew( SOdysseyAboutScreen )
        ];

	FSlateApplication::Get().AddModalWindow(aboutWindow.ToSharedRef(), GetGUI()->GetToolsTab()); //TODO: CORRECTLY /* mToolkit->GetToolkitHost()->GetParentWidget()*/ );
}

void
FOdysseyPainterEditorController::OnVisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorController::OnVisitPraxinosForums()
{
    FString URL = "https://praxinos.coop/forum";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Mesh Selector

void
FOdysseyPainterEditorController::OnMeshSelected( UStaticMesh* iMesh )
{
}

void
FOdysseyPainterEditorController::OnMeshChanged( UBlueprint* iMesh )
{
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Viewport Handlers
/* void
FOdysseyPainterEditorController::HandleViewportParameterChanged()
{
    if( GetEditor()->BrushInstance() )
    {
        FOdysseyPainterEditorState* state = new FOdysseyPainterEditorState( GetGUI()->GetViewportTab()->GetViewport()->GetZoom(), GetGUI()->GetViewportTab()->GetViewport()->GetRotationInDegrees(), GetGUI()->GetViewportTab()->GetViewport()->GetPan() );
        GetEditor()->BrushInstance()->AddOrReplaceState( FOdysseyPainterEditorState::GetId(), state );
    }
} */

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyPainterEditorController::HandleBrushParameterChanged()
{
    GetEditor()->PaintEngine()->TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers

void
FOdysseyPainterEditorController::HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture)
{
	if (!GetEditor()->DisplaySurface())
		return;

    ::ul3::FBlock* block = GetEditor()->DisplaySurface()->Block()->GetBlock();
    if (iPositionInTexture.X >= 0 && iPositionInTexture.X < block->Width() &&
        iPositionInTexture.Y >= 0 && iPositionInTexture.Y < block->Height())
    {
        const ::ul3::FPixelValue& color = block->PixelValue(iPositionInTexture.X, iPositionInTexture.Y);
        GetEditor()->PaintColor(color);
    }
        
    if (iEventState == eOdysseyEventState::kSet)
    {
        GetEditor()->PaintEngine()->SetColor(GetEditor()->PaintColor());
    }
}

void
FOdysseyPainterEditorController::HandlePaintColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor )
{
    GetEditor()->PaintColor(iColor);

    if (iEventState == eOdysseyEventState::kSet)
    {
	    GetEditor()->PaintEngine()->SetColor( iColor );
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
FOdysseyPainterEditorController::HandleSizeModifierChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetSizeModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleOpacityModifierChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetOpacityModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleFlowModifierChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetFlowModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleBlendingModeModifierChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetBlendingModeModifier( static_cast<::ul3::eBlendingMode>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetAlphaModeModifier( static_cast<::ul3::eAlphaMode>(iValue) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
/* void
FOdysseyPainterEditorController::HandleStrokeStepChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetStrokeStep( iValue );
}

void
FOdysseyPainterEditorController::HandleStrokeAdaptativeChanged( bool iValue )
{
	GetEditor()->PaintEngine()->SetStrokeAdaptative( iValue );
}

void
FOdysseyPainterEditorController::HandleStrokePaintOnTickChanged( bool iValue )
{
	GetEditor()->PaintEngine()->SetStrokePaintOnTick( iValue );
}

void
FOdysseyPainterEditorController::HandleInterpolationTypeChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetInterpolationType( static_cast<EOdysseyInterpolationType>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleSmoothingMethodChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetSmoothingMethod( static_cast<EOdysseySmoothingMethod>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleSmoothingStrengthChanged( int32 iValue )
{
	GetEditor()->PaintEngine()->SetSmoothingStrength( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingEnabledChanged( bool iValue )
{
	GetEditor()->PaintEngine()->SetSmoothingEnabled( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingRealTimeChanged( bool iValue )
{
	GetEditor()->PaintEngine()->SetSmoothingRealTime( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingCatchUpChanged( bool iValue )
{
	GetEditor()->PaintEngine()->SetSmoothingCatchUp( iValue );
} */

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

void
FOdysseyPainterEditorController::HandlePerformanceDrawBrushPreviewChanged( bool iValue )
{
    GetEditor()->DrawBrushPreview( iValue );
    if (GetEditor()->DisplaySurface())
    {
        GetEditor()->DisplaySurface()->Invalidate();
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Shortcuts actions

FReply
FOdysseyPainterEditorController::OnClear()
{
	GetEditor()->PaintEngine()->AbortStroke();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnFill()
{
	GetEditor()->PaintEngine()->AbortStroke();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnUndoIliad()
{
	GetEditor()->PaintEngine()->InterruptStrokeAndStampInPlace();
    return FReply::Handled();
}


FReply
FOdysseyPainterEditorController::OnRedoIliad()
{
	GetEditor()->PaintEngine()->InterruptStrokeAndStampInPlace();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnClearUndo()
{
    return FReply::Handled();
}

void
FOdysseyPainterEditorController::OnCreateNewLayer()
{
}

void
FOdysseyPainterEditorController::OnDuplicateCurrentLayer()
{
}

void
FOdysseyPainterEditorController::OnDeleteCurrentLayer()
{
}

void
FOdysseyPainterEditorController::OnRefreshBrush()
{
    UOdysseyBrush* brush = GetEditor()->PaintEngine()->Brush();
    if (brush)
    {
        GetEditor()->PaintEngine()->Brush(nullptr);
        GetEditor()->PaintEngine()->Brush(brush);
    }
}

void
FOdysseyPainterEditorController::OnSetAlphaMode(::ul3::eAlphaMode iAlphaMode)
{
    GetEditor()->PaintEngine()->SetAlphaModeModifier(iAlphaMode);
}

void
FOdysseyPainterEditorController::OnAddBrushSize(int32 iValue)
{
    GetEditor()->PaintEngine()->SetSizeModifier(GetEditor()->PaintEngine()->GetSizeModifier() + iValue);
}


void
FOdysseyPainterEditorController::OnSwitchTabletAPI()
{
    UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    mComboBoxTabletAPISelected = MakeShared<EOdysseyStylusInputDriver>(settings->StylusInputDriver);
    TArray<TSharedPtr<EOdysseyStylusInputDriver>> options;

    #if PLATFORM_WINDOWS
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Ink));
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Wintab));
    #elif PLATFORM_MAC
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_NSEvent));
    #endif

    options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_None));

	TSharedRef<SVerticalBox> dialogContents = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew( STextBlock )
            .Text( FText::FromString("Select tablet API") )
        ]
		+ SVerticalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<EOdysseyStylusInputDriver>>)
            .OptionsSource(&options)
            .OnGenerateWidget(this, &FOdysseyPainterEditorController::GenerateTabletAPIComboBoxItem)
            .OnSelectionChanged( this, &FOdysseyPainterEditorController::ChangeSelectionTabletAPIComboBoxItem )
            [
                SNew( STextBlock )
                .Text( this, &FOdysseyPainterEditorController::GetComboBoxTabletAPISelectedAsText )
            ]
		];

	TSharedPtr<SCustomDialog> customDialog;

	FText dialogTitle = LOCTEXT("SelectTabletAPI", "Select Tablet API" );

	FText oKText = LOCTEXT("OkSwitchAPI", "OK" );
	FText cancelText = LOCTEXT("CancelSwitchAPI", "Cancel");

	customDialog = SNew(SCustomDialog)
		.Title(dialogTitle)
		.DialogContent(dialogContents)
		.Buttons( { SCustomDialog::FButton(oKText), SCustomDialog::FButton(cancelText) } );

    if( customDialog->ShowModal() == 0/*OK*/ )
    {
        settings->StylusInputDriver = *(mComboBoxTabletAPISelected.Get());
        settings->RefreshStylusInputDriver();
    }
}


#undef LOCTEXT_NAMESPACE
