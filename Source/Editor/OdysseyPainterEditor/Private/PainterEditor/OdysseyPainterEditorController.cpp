// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyPainterEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPaintEngine.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "UndoHistory/SOdysseyUndoHistory.h"

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
    
    // Register our commands. This will only register them if not previously registered
    FOdysseyPainterEditorCommands::Register();

    // Build commands
    BindCommands(iToolkitCommands);

	//Add PaintEngine Callbacks
	GetData()->PaintEngine()->OnStrokeChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeChanged);
	GetData()->PaintEngine()->OnStrokeWillEnd().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeWillEnd);
	GetData()->PaintEngine()->OnStrokeEnd().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeEnd);
	GetData()->PaintEngine()->OnStrokeAbort().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeAbort);
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
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::UndoIliad ),
        FCanExecuteAction() );
    
	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().Redo,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::RedoIliad ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosWebsite ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosForums,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosForums ),
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

    return MakeShareable(extender);
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

void
FOdysseyPainterEditorController::OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
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
    }

    if(GetData()->Brush())
    {
        //@todo: check
        //mBrush->OnChanged().AddSP( this, &FOdysseyPainterEditorController::OnBrushChanged );

		GetData()->Brush()->OnCompiled().AddSP( this, &FOdysseyPainterEditorController::OnBrushCompiled );
		UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), GetData()->Brush()->GeneratedClass);
		brushInstance->AddToRoot();
		GetData()->BrushInstance(brushInstance);

        GetData()->PaintEngine()->SetBrushInstance(GetData()->BrushInstance());
        GetGUI()->GetBrushExposedParametersTab()->Refresh(GetData()->BrushInstance());

        FOdysseyBrushPreferencesOverrides& overrides = GetData()->BrushInstance()->Preferences;
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
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyPainterEditorController::HandleBrushParameterChanged()
{
    GetData()->PaintEngine()->TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers

void
FOdysseyPainterEditorController::HandleViewportColorPicked(const ::ul3::FPixelValue& iColor)
{
	if (GetGUI()->GetColorSlidersTab())
		GetGUI()->GetColorSlidersTab()->SetColor(iColor);

	if (GetGUI()->GetColorSelectorTab())
		GetGUI()->GetColorSelectorTab()->SetColor(iColor);

	GetData()->PaintEngine()->SetColor(iColor);
}

void
FOdysseyPainterEditorController::HandleSelectorColorChanged( const ::ul3::FPixelValue& iColor )
{
    if( GetGUI()->GetColorSlidersTab() )
		GetGUI()->GetColorSlidersTab()->SetColor( iColor );

	GetData()->PaintEngine()->SetColor( iColor );
}

void
FOdysseyPainterEditorController::HandleSlidersColorChanged( const ::ul3::FPixelValue& iColor )
{
    if( GetGUI()->GetColorSelectorTab() )
		GetGUI()->GetColorSelectorTab()->SetColor( iColor );

	GetData()->PaintEngine()->SetColor( iColor );
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
    GetData()->DisplaySurface()->Invalidate();
}

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
FOdysseyPainterEditorController::OnClearUndo()
{
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorController::OnUndo()
{
    UndoIliad();
    return FReply::Handled();
}


FReply
FOdysseyPainterEditorController::OnRedo()
{
    RedoIliad();
    return FReply::Handled();
}

void
FOdysseyPainterEditorController::UndoIliad()
{
	GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
}


void
FOdysseyPainterEditorController::RedoIliad()
{
	GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
}

#undef LOCTEXT_NAMESPACE
