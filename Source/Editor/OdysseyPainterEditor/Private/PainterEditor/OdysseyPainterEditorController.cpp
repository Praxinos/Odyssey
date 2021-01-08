// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "PainterEditor/OdysseyPainterEditorData.h"
#include "PainterEditor/OdysseyPainterEditorGUI.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPaintEngine.h"

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
    FOdysseyPainterEditorCommands::Register();

    // Build commands
    FOdysseyPainterEditorController::BindCommands(iToolkitCommands);

	//Add PaintEngine Callbacks
	GetData()->PaintEngine()->OnPreviewBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged);
	GetData()->PaintEngine()->OnEditedBlockTilesWillChange().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange);
	GetData()->PaintEngine()->OnEditedBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged);
	GetData()->PaintEngine()->OnStrokeAbort().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeAbort);

	const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    GetGUI()->GetBrushSelectorTab()->SelectBrush(settings.BrushDefaults.DefaultBrush);
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
        FOdysseyPainterEditorCommands::Get().ResetViewportPosition,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnResetViewportPosition ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ResetViewportRotation,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnResetViewportRotation ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().RotateViewportLeft,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnRotateViewportLeft ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().RotateViewportRight,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnRotateViewportRight ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom10Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.1 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom20Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.2 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom30Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.3 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom40Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.4 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom50Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.5 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom60Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.6 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom70Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.7 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom80Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.8 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom90Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 0.9 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoom100Percent,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoom, 1.0 ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SetZoomFitScreen,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSetZoomFitScreen ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ZoomIn,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnZoomIn ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ZoomOut,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnZoomOut ),
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
//--------------------------------------------------------- Paint engine driving methods

void
FOdysseyPainterEditorController::OnBrushSelected( UOdysseyBrush* iBrush )
{
	GetData()->Brush(iBrush);

    if(GetData()->BrushInstance())
    {
		GetData()->BrushInstance()->RemoveFromRoot();
		GetData()->BrushInstance(NULL);
        GetData()->PaintEngine()->SetBrushInstance(NULL);
    }

    if(GetData()->Brush())
    {
        //@todo: check
        //mBrush->OnChanged().AddSP( this, &FOdysseyPainterEditorController::OnBrushChanged );

		GetData()->Brush()->OnCompiled().AddSP( this, &FOdysseyPainterEditorController::OnBrushCompiled );

		UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), GetData()->Brush()->GeneratedClass);
		brushInstance->AddToRoot();

        FOdysseyBrushPreferencesOverrides& overrides = brushInstance->Preferences;
        if( overrides.bOverride_Step )          GetGUI()->GetStrokeOptionsTab()->SetStrokeStep( overrides.Step );
        if( overrides.bOverride_Adaptative )    GetGUI()->GetStrokeOptionsTab()->SetStrokeAdaptative( overrides.SizeAdaptative );
        if( overrides.bOverride_PaintOnTick )   GetGUI()->GetStrokeOptionsTab()->SetStrokePaintOnTick( overrides.PaintOnTick );
        if( overrides.bOverride_Type )          GetGUI()->GetStrokeOptionsTab()->SetInterpolationType( (int32)overrides.Type );
        if( overrides.bOverride_Method )        GetGUI()->GetStrokeOptionsTab()->SetSmoothingMethod( (int32)overrides.Method );
        if( overrides.bOverride_Strength )      GetGUI()->GetStrokeOptionsTab()->SetSmoothingStrength( overrides.Strength );
        if( overrides.bOverride_Enabled )       GetGUI()->GetStrokeOptionsTab()->SetSmoothingEnabled( overrides.Enabled );
        if( overrides.bOverride_RealTime )      GetGUI()->GetStrokeOptionsTab()->SetSmoothingRealTime( overrides.RealTime );
        if( overrides.bOverride_CatchUp )       GetGUI()->GetStrokeOptionsTab()->SetSmoothingCatchUp( overrides.CatchUp );
        if( overrides.bOverride_Size )          GetGUI()->GetTopTab()->SetSize( overrides.Size );
        if( overrides.bOverride_Opacity )       GetGUI()->GetTopTab()->SetOpacity( overrides.Opacity );
        if( overrides.bOverride_Flow )          GetGUI()->GetTopTab()->SetFlow( overrides.Flow );
        if( overrides.bOverride_BlendingMode )  GetGUI()->GetTopTab()->SetBlendingMode( ( ::ul3::eBlendingMode )overrides.BlendingMode );
        if( overrides.bOverride_AlphaMode )     GetGUI()->GetTopTab()->SetAlphaMode( ( ::ul3::eAlphaMode )overrides.AlphaMode );

        GetData()->BrushInstance(brushInstance);
        GetData()->PaintEngine()->SetBrushInstance(brushInstance);
        GetGUI()->GetBrushExposedParametersTab()->Refresh(brushInstance);

        //---

        FOdysseyPainterEditorState* state = new FOdysseyPainterEditorState( GetGUI()->GetViewportTab()->GetZoom(), GetGUI()->GetViewportTab()->GetRotationInDegrees(), GetGUI()->GetViewportTab()->GetPan() );
        GetData()->BrushInstance()->AddOrReplaceState( FOdysseyPainterEditorState::GetId(), state );
    }
}

void
FOdysseyPainterEditorController::OnBrushChanged( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );
}

void
FOdysseyPainterEditorController::OnBrushCompiled( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );

    // Reload instance
    if( check_brush )
    {
        if( GetData()->BrushInstance() )
        {
            if(GetData()->BrushInstance()->IsValidLowLevel() )
				GetData()->BrushInstance()->RemoveFromRoot();

			GetData()->BrushInstance(NULL);
        }

        //brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorController::OnBrushCompiled );
		UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), GetData()->Brush()->GeneratedClass);
		brushInstance->AddToRoot();
		GetData()->BrushInstance(brushInstance);

		GetData()->PaintEngine()->SetBrushInstance(GetData()->BrushInstance());
        GetGUI()->GetBrushExposedParametersTab()->Refresh(GetData()->BrushInstance());

        //---

        FOdysseyPainterEditorState* state = new FOdysseyPainterEditorState( GetGUI()->GetViewportTab()->GetZoom(), GetGUI()->GetViewportTab()->GetRotationInDegrees(), GetGUI()->GetViewportTab()->GetPan() );
        GetData()->BrushInstance()->AddOrReplaceState( FOdysseyPainterEditorState::GetId(), state );
    }
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
void
FOdysseyPainterEditorController::HandleViewportParameterChanged()
{
    if( GetData()->BrushInstance() )
    {
        FOdysseyPainterEditorState* state = new FOdysseyPainterEditorState( GetGUI()->GetViewportTab()->GetZoom(), GetGUI()->GetViewportTab()->GetRotationInDegrees(), GetGUI()->GetViewportTab()->GetPan() );
        GetData()->BrushInstance()->AddOrReplaceState( FOdysseyPainterEditorState::GetId(), state );
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyPainterEditorController::HandleBrushParameterChanged()
{
    GetData()->PaintEngine()->TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers

void
FOdysseyPainterEditorController::HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture)
{
	if (!GetData()->DisplaySurface())
		return;

    ::ul3::FBlock* block = GetData()->DisplaySurface()->Block()->GetBlock();
    if (iPositionInTexture.X >= 0 && iPositionInTexture.X < block->Width() &&
        iPositionInTexture.Y >= 0 && iPositionInTexture.Y < block->Height())
    {
        const ::ul3::FPixelValue& color = block->PixelValue(iPositionInTexture.X, iPositionInTexture.Y);
        GetData()->PaintColor(color);
    }
        
    if (iEventState == eOdysseyEventState::kSet)
    {
        GetData()->PaintEngine()->SetColor(GetData()->PaintColor());
    }
}

void
FOdysseyPainterEditorController::HandlePaintColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor )
{
    GetData()->PaintColor(iColor);

    if (iEventState == eOdysseyEventState::kSet)
    {
	    GetData()->PaintEngine()->SetColor( iColor );
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
FOdysseyPainterEditorController::HandleSizeModifierChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetSizeModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleOpacityModifierChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetOpacityModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleFlowModifierChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetFlowModifier( iValue );
}

void
FOdysseyPainterEditorController::HandleBlendingModeModifierChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetBlendingModeModifier( static_cast<::ul3::eBlendingMode>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetAlphaModeModifier( static_cast<::ul3::eAlphaMode>(iValue) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
void
FOdysseyPainterEditorController::HandleStrokeStepChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetStrokeStep( iValue );
}

void
FOdysseyPainterEditorController::HandleStrokeAdaptativeChanged( bool iValue )
{
	GetData()->PaintEngine()->SetStrokeAdaptative( iValue );
}

void
FOdysseyPainterEditorController::HandleStrokePaintOnTickChanged( bool iValue )
{
	GetData()->PaintEngine()->SetStrokePaintOnTick( iValue );
}

void
FOdysseyPainterEditorController::HandleInterpolationTypeChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetInterpolationType( static_cast<EOdysseyInterpolationType>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleSmoothingMethodChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetSmoothingMethod( static_cast<EOdysseySmoothingMethod>( iValue ) );
}

void
FOdysseyPainterEditorController::HandleSmoothingStrengthChanged( int32 iValue )
{
	GetData()->PaintEngine()->SetSmoothingStrength( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingEnabledChanged( bool iValue )
{
	GetData()->PaintEngine()->SetSmoothingEnabled( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingRealTimeChanged( bool iValue )
{
	GetData()->PaintEngine()->SetSmoothingRealTime( iValue );
}

void
FOdysseyPainterEditorController::HandleSmoothingCatchUpChanged( bool iValue )
{
	GetData()->PaintEngine()->SetSmoothingCatchUp( iValue );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

void
FOdysseyPainterEditorController::HandlePerformanceDrawBrushPreviewChanged( bool iValue )
{
    GetData()->DrawBrushPreview( iValue );
    if (GetData()->DisplaySurface())
    {
        GetData()->DisplaySurface()->Invalidate();
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Shortcuts actions

FReply
FOdysseyPainterEditorController::OnClear()
{
	GetData()->PaintEngine()->AbortStroke();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnFill()
{
	GetData()->PaintEngine()->AbortStroke();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnUndoIliad()
{
	GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
    return FReply::Handled();
}


FReply
FOdysseyPainterEditorController::OnRedoIliad()
{
	GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
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
FOdysseyPainterEditorController::OnResetViewportPosition()
{
    GetGUI()->GetViewportTab()->SetRotationInDegrees( 0 );
    GetGUI()->GetViewportTab()->SetPan(FVector2D(0, 0));
}

void
FOdysseyPainterEditorController::OnResetViewportRotation()
{
    GetGUI()->GetViewportTab()->SetRotationInDegrees( 0 );
}

void
FOdysseyPainterEditorController::OnRotateViewportLeft()
{
    GetGUI()->GetViewportTab()->RotateLeft();
}

void
FOdysseyPainterEditorController::OnRotateViewportRight()
{
    GetGUI()->GetViewportTab()->RotateRight();
}

void
FOdysseyPainterEditorController::OnSetZoom(double iZoomValue)
{
    GetGUI()->GetViewportTab()->SetZoom(iZoomValue);
}

void
FOdysseyPainterEditorController::OnSetZoomFitScreen()
{
    GetGUI()->GetViewportTab()->ToggleFitToViewport();
}

void
FOdysseyPainterEditorController::OnZoomIn()
{
    GetGUI()->GetViewportTab()->ZoomIn();
}

void
FOdysseyPainterEditorController::OnZoomOut()
{
    GetGUI()->GetViewportTab()->ZoomOut();
}

void
FOdysseyPainterEditorController::OnRefreshBrush()
{
    if( GetData()->Brush() )
        OnBrushSelected( GetData()->Brush() );
}

void
FOdysseyPainterEditorController::OnSetAlphaMode(::ul3::eAlphaMode iAlphaMode)
{
    //PATCH: temporary solution, in the refactored version we should, as much as possible, change the data only, letting the GUI automatically refresh itself according to the Slate Attribute behaviour
    GetGUI()->GetTopTab()->SetAlphaMode(iAlphaMode);
}

void
FOdysseyPainterEditorController::OnAddBrushSize(int32 iValue)
{
    //PATCH: temporary solution, in the refactored version we should, as much as possible, change the data only, letting the GUI automatically refresh itself according to the Slate Attribute behaviour
    GetGUI()->GetTopTab()->SetSize(GetGUI()->GetTopTab()->GetSize() + iValue);
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
