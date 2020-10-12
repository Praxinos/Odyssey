// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SOdysseyViewportDrawingEditorWidget.h"

#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SHeader.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "OdysseyViewportDrawingEditorPainter.h"
#include "OdysseyViewportDrawingEditorSettings.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "OdysseyTextureAssetUserData.h"



#include "Modules/ModuleManager.h"
#include "OdysseyViewportDrawingEditorCommands.h"
#include "DetailLayoutBuilder.h"

#include "EditorModeManager.h"
#include "EditorModes.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

#define LOCTEXT_NAMESPACE "PaintModePainter"

void SOdysseyViewportDrawingEditorWidget::Construct(const FArguments& InArgs, FOdysseyViewportDrawingEditorPainter* InPainter)
{
    FMargin StandardPadding(6.f, 3.f);

	mMeshPainter = InPainter;
	mPaintModeSettings = Cast<UOdysseyViewportDrawingEditorSettings>(mMeshPainter->GetPainterSettings());
	mSettingsObjects.Add(mPaintModeSettings);
	CreateDetailsView();

	mPaintEngine.OnStrokeChanged().AddRaw(this, &SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeChanged);
	mPaintEngine.OnStrokeWillEnd().AddRaw(this, &SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeWillEnd);
	mPaintEngine.OnStrokeEnd().AddRaw(this, &SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeEnd);
	mPaintEngine.OnStrokeAbort().AddRaw(this, &SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeAbort);

    mBrushSelector = SNew( SOdysseyBrushSelector )
        .OnBrushChanged( this, &SOdysseyViewportDrawingEditorWidget::OnBrushSelected );

    mPaintModifiers = SNew( SOdysseyPaintModifiers )
        .OnSizeChanged          ( this, &SOdysseyViewportDrawingEditorWidget::HandleSizeModifierChanged )
        .OnOpacityChanged       ( this, &SOdysseyViewportDrawingEditorWidget::HandleOpacityModifierChanged )
        .OnFlowChanged          ( this, &SOdysseyViewportDrawingEditorWidget::HandleFlowModifierChanged )
        .OnBlendingModeChanged  ( this, &SOdysseyViewportDrawingEditorWidget::HandleBlendingModeModifierChanged )
        .OnAlphaModeChanged     ( this, &SOdysseyViewportDrawingEditorWidget::HandleAlphaModeModifierChanged );

    mBrushExposedParameters = SNew( SOdysseyBrushExposedParameters )
        .OnParameterChanged( this, &SOdysseyViewportDrawingEditorWidget::HandleBrushParameterChanged );

    mColorSelector = SNew( SOdysseyColorSelector )
        .OnColorChanged( this, &SOdysseyViewportDrawingEditorWidget::HandleSelectorColorChanged );

    mColorSliders = SNew( SOdysseyColorSliders )
        .OnColorChanged( this, &SOdysseyViewportDrawingEditorWidget::HandleSlidersColorChanged );

    mStrokeOptions = SNew( SOdysseyStrokeOptions )
        .OnStrokeStepChanged        ( this, &SOdysseyViewportDrawingEditorWidget::HandleStrokeStepChanged )
        .OnStrokeAdaptativeChanged  ( this, &SOdysseyViewportDrawingEditorWidget::HandleStrokeAdaptativeChanged )
        .OnStrokePaintOnTickChanged ( this, &SOdysseyViewportDrawingEditorWidget::HandleStrokePaintOnTickChanged )
        .OnInterpolationTypeChanged ( this, &SOdysseyViewportDrawingEditorWidget::HandleInterpolationTypeChanged )
        .OnSmoothingMethodChanged   ( this, &SOdysseyViewportDrawingEditorWidget::HandleSmoothingMethodChanged )
        .OnSmoothingStrengthChanged ( this, &SOdysseyViewportDrawingEditorWidget::HandleSmoothingStrengthChanged )
        .OnSmoothingEnabledChanged  ( this, &SOdysseyViewportDrawingEditorWidget::HandleSmoothingEnabledChanged )
        .OnSmoothingRealTimeChanged ( this, &SOdysseyViewportDrawingEditorWidget::HandleSmoothingRealTimeChanged )
        .OnSmoothingCatchUpChanged  ( this, &SOdysseyViewportDrawingEditorWidget::HandleSmoothingCatchUpChanged );

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SAssignNew(mErrorTextWidget, SErrorText)
		]
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			.IsEnabled(this, &SOdysseyViewportDrawingEditorWidget::GetMeshPaintEditorIsEnabled)
		    + SVerticalBox::Slot()
		    .AutoHeight()
		    [
			    SNew(SHorizontalBox)
			    + SHorizontalBox::Slot()
			    .AutoWidth()
			    .Padding(1.f, 5.f, 0.f, 5.f)
			    [
				    CreateTabSelectorWidget()->AsShared()
			    ]
			    + SHorizontalBox::Slot()
			    [
				    SNew(SBorder)
				    .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
				    [
					    SNew(SVerticalBox)	
			            /** Texture paint action buttons widget */
			            + SVerticalBox::Slot()
			            .AutoHeight()
			            [
				            CreateTexturePaintWidget()->AsShared()
			            ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
						    SNew(SVerticalBox)
                            .Visibility(this, &SOdysseyViewportDrawingEditorWidget::GetBrushSettingsWidgetVisibility )
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("BrushSelector", "Brush Selector"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mBrushSelector.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("PaintModifiers", "Paint Modifiers"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mPaintModifiers.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("ExposedParameters", "Exposed Parameters"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mBrushExposedParameters.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("ColorSelector", "Color Selector"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                SNew(SBox)
                                .HeightOverride(256)
                                [
                                    mColorSelector.ToSharedRef()
                                ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mColorSliders.ToSharedRef()
                            ]
                        ]
                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SHeader)
                            .Visibility(this, &SOdysseyViewportDrawingEditorWidget::GetStrokeOptionsWidgetVisibility )
						    [
							    SNew(STextBlock)
							    .Text(LOCTEXT("StrokeOptions", "Stroke Options"))
						    ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
						    SNew(SHorizontalBox)
                            .Visibility(this, &SOdysseyViewportDrawingEditorWidget::GetStrokeOptionsWidgetVisibility )
                   		    + SHorizontalBox::Slot()
						    [
                                mStrokeOptions.ToSharedRef()
                            ]
                        ]
                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SHeader)
                            .Visibility( this, &SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility )
						    [
							    SNew(STextBlock)
							    .Text(LOCTEXT("LayerStack", "Layer Stack"))
						    ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Expose( mLayerStackView )
                        [
                            SNullWidget::NullWidget
                        ]
                        + SVerticalBox::Slot()
                        .Padding(StandardPadding)
                        .AutoHeight()
                        [
                            SNew(SHeader)
                            .Visibility(this,&SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility)
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Texture to edit","Texture to edit"))
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            .Visibility(this,&SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility)
                            + SHorizontalBox::Slot()
                            [
                                SNew(SObjectPropertyEntryBox)
                                .ObjectPath(this, &SOdysseyViewportDrawingEditorWidget::PaintTexturePath )
                                .AllowedClass(UTexture2D::StaticClass())
                                .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(mMeshPainter,&FOdysseyViewportDrawingEditorPainter::ShouldFilterTextureAsset))
                                .OnObjectChanged(FOnSetObject::CreateRaw(mMeshPainter,&FOdysseyViewportDrawingEditorPainter::PaintTextureChanged))
                                .DisplayUseSelected(false)
                                //.ThumbnailPool(iCustomizationUtils.GetThumbnailPool())
                            ]
                        ]
		            ]
                ]
	        ]
        ]
    ];

    if( mPaintModeSettings->mTexturePaintSettings.mPaintTexture )
    {
        //Check if this texture already has Iliad User Data or not, if yes, we get its LayerStack
        UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mPaintModeSettings->mTexturePaintSettings.mPaintTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
        if( !userData )
        {
            userData = NewObject< UOdysseyTextureAssetUserData >(mPaintModeSettings->mTexturePaintSettings.mPaintTexture, NAME_None, RF_Public);
            mPaintModeSettings->mTexturePaintSettings.mPaintTexture->AddAssetUserData( userData );
            FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( mPaintModeSettings->mTexturePaintSettings.mPaintTexture );
            userData->GetLayerStack()->InitFromData( textureData );
            delete textureData;
            mPaintModeSettings->mTexturePaintSettings.mPaintTexture->PostEditChange();
        }
    
        mLayerStack = userData->GetLayerStack();
        mLayerStack->ComputeResultBlock();

        mLayerStackView->DetachWidget();
        mLayerStackView->AttachWidget( 
			SNew(SHorizontalBox)
            .Visibility( this, &SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility )
            + SHorizontalBox::Slot()
		    [
                SNew( SOdysseyLayerStackView )
                .LayerStackData( mLayerStack )
            ]
        );
    }

    mPaintEngine.SetBrushInstance( NULL );
    mPaintEngine.SetColor( ::ul3::FPixelValue::FromRGBA8(0,255,255) );
    mPaintEngine.SetSizeModifier( 20.f );
}

void SOdysseyViewportDrawingEditorWidget::CreateDetailsView()
{
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs(
		/*bUpdateFromSelection=*/ false,
		/*bLockable=*/ false,
		/*bAllowSearch=*/ false,
		FDetailsViewArgs::HideNameArea,
		/*bHideSelectionTip=*/ true,
		/*InNotifyHook=*/ this,
		/*InSearchInitialKeyFocus=*/ false,
		/*InViewIdentifier=*/ NAME_None);
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bAllowMultipleTopLevelObjects = true;

	mSettingsDetailsView = EditModule.CreateDetailView(DetailsViewArgs);
	mSettingsDetailsView->SetObjects(mSettingsObjects);
}
 
TSharedPtr<SWidget> SOdysseyViewportDrawingEditorWidget::CreateTexturePaintWidget()
{
	TSharedPtr<SWidget> TexturePaintWidget;
	TSharedPtr<SHorizontalBox> ActionBox;

	SAssignNew(TexturePaintWidget, SVerticalBox)
    .Visibility(EVisibility::Visible)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Center)
	[
		SAssignNew(ActionBox, SHorizontalBox)
	];
	 
	FToolBarBuilder TexturePaintToolbarBuilder(mMeshPainter->GetUICommandList(), FMultiBoxCustomization::None);
	TexturePaintToolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);
	TexturePaintToolbarBuilder.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SaveTexturePaint, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Save"));

	ActionBox->AddSlot()
	.FillWidth(1.0f)
	[
		TexturePaintToolbarBuilder.MakeWidget()
	];

	return TexturePaintWidget->AsShared();
}

TSharedPtr<SWidget> SOdysseyViewportDrawingEditorWidget::CreateTabSelectorWidget()
{
	FToolBarBuilder Toolbar(mMeshPainter->GetUICommandList(), FMultiBoxCustomization::None, nullptr, Orient_Vertical);
	Toolbar.SetLabelVisibility(EVisibility::Collapsed);
	Toolbar.SetStyle(&FEditorStyle::Get(), "FoliageEditToolbar");
	{
		Toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyBrushSettingsView);
		Toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyStrokeOptions);
		Toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyLayerStackView);
	}
	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.Padding(0)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
				.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
				[
					Toolbar.MakeWidget()
				]
			]
		];
}

void SOdysseyViewportDrawingEditorWidget::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		for (UObject* Settings : mSettingsObjects)
		{
			Settings->SaveConfig();
		}
	}
}

bool SOdysseyViewportDrawingEditorWidget::GetMeshPaintEditorIsEnabled() const
{
	FOdysseyViewportDrawingEditorEdMode* MeshPaintMode = (FOdysseyViewportDrawingEditorEdMode*)GLevelEditorModeTools().GetActiveMode(FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId);
	if (MeshPaintMode)
	{
		bool bEnabled = MeshPaintMode->IsEditingEnabled();
		FText ErrorText = bEnabled ? FText::GetEmpty() : LOCTEXT("MeshPaintSM5Only", "Mesh Paint mode can only be used in SM5.");
		mErrorTextWidget->SetError(ErrorText);
		return bEnabled;
	}
	return false;
}

//Delegates
void
SOdysseyViewportDrawingEditorWidget::OnBrushSelected( UOdysseyBrush* iBrush )
{
    mPaintEngine.SetBrushInstance( mBrushInstance );

    mBrush = iBrush;

    if( mBrushInstance )
    {
        mBrushInstance->RemoveFromRoot();
        mBrushInstance = NULL;
    }

    if( mBrush )
    {
        //@todo: check
        //mBrush->OnChanged().AddSP( this, &SOdysseyViewportDrawingEditorWidget::OnBrushChanged );

        mBrush->OnCompiled().AddSP( this, &SOdysseyViewportDrawingEditorWidget::OnBrushCompiled );
        mBrushInstance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), mBrush->GeneratedClass );
        mBrushInstance->AddToRoot();

        mPaintEngine.SetBrushInstance( mBrushInstance );
        mBrushExposedParameters->Refresh( mBrushInstance );

        FOdysseyBrushPreferencesOverrides& overrides = mBrushInstance->Preferences;
        if( overrides.bOverride_Step )          mStrokeOptions->SetStrokeStep( overrides.Step );
        if( overrides.bOverride_Adaptative )    mStrokeOptions->SetStrokeAdaptative( overrides.SizeAdaptative );
        if( overrides.bOverride_PaintOnTick )   mStrokeOptions->SetStrokePaintOnTick( overrides.PaintOnTick );
        if( overrides.bOverride_Type )          mStrokeOptions->SetInterpolationType( (int32)overrides.Type );
        if( overrides.bOverride_Method )        mStrokeOptions->SetSmoothingMethod( (int32)overrides.Method );
        if( overrides.bOverride_Strength )      mStrokeOptions->SetSmoothingStrength( overrides.Strength );
        if( overrides.bOverride_Enabled )       mStrokeOptions->SetSmoothingEnabled( overrides.Enabled );
        if( overrides.bOverride_RealTime )      mStrokeOptions->SetSmoothingRealTime( overrides.RealTime );
        if( overrides.bOverride_CatchUp )       mStrokeOptions->SetSmoothingCatchUp( overrides.CatchUp );
        if( overrides.bOverride_Size )          mPaintModifiers->SetSize( overrides.Size );
        if( overrides.bOverride_Opacity )       mPaintModifiers->SetOpacity( overrides.Opacity );
        if( overrides.bOverride_Flow )          mPaintModifiers->SetFlow( overrides.Flow );
        if( overrides.bOverride_BlendingMode )  mPaintModifiers->SetBlendingMode( ( ::ul3::eBlendingMode )overrides.BlendingMode );
        if( overrides.bOverride_AlphaMode )     mPaintModifiers->SetAlphaMode( ( ::ul3::eAlphaMode )overrides.AlphaMode );
    }
}

void
SOdysseyViewportDrawingEditorWidget::OnBrushCompiled( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );

    // Reload instance
    if( check_brush )
    {
        if( mBrushInstance )
        {
            if( mBrushInstance->IsValidLowLevel() )
                mBrushInstance->RemoveFromRoot();

            mBrushInstance = NULL;
        }

        //brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        mBrushInstance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), mBrush->GeneratedClass );
        mBrushInstance->AddToRoot();

        mPaintEngine.SetBrushInstance( mBrushInstance );
        mBrushExposedParameters->Refresh( mBrushInstance );
    }
}

void
SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mLayerStack->ComputeResultBlockWithTempBuffer(iChangedTiles[i], mPaintEngine.TempBuffer(), mPaintEngine.GetOpacity(), mPaintEngine.GetBlendingMode(), mPaintEngine.GetAlphaMode());
	}
}

void
SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
	mLayerStack->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mLayerStack->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
        mLayerStack->BlendTempBufferOnCurrentBlock(iChangedTiles[i], mPaintEngine.TempBuffer(), mPaintEngine.GetOpacity(), mPaintEngine.GetBlendingMode(), mPaintEngine.GetAlphaMode());
	}
}

void
SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
	mLayerStack->mDrawingUndo->EndRecord();
	mLayerStack->ComputeResultBlock();
}

void
SOdysseyViewportDrawingEditorWidget::OnPaintEngineStrokeAbort()
{
	mLayerStack->ComputeResultBlock();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
SOdysseyViewportDrawingEditorWidget::HandleBrushParameterChanged()
{
    mPaintEngine.TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers
void
SOdysseyViewportDrawingEditorWidget::HandleSelectorColorChanged( const ::ul3::FPixelValue& iColor )
{
    if( mColorSliders )
        mColorSliders->SetColor( iColor );

    mPaintEngine.SetColor( iColor );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSlidersColorChanged( const ::ul3::FPixelValue& iColor )
{
    if( mColorSelector )
        mColorSelector->SetColor( iColor );

    mPaintEngine.SetColor( iColor );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
SOdysseyViewportDrawingEditorWidget::HandleSizeModifierChanged( int32 iValue )
{
    mPaintEngine.SetSizeModifier( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleOpacityModifierChanged( int32 iValue )
{
    mPaintEngine.SetOpacityModifier( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleFlowModifierChanged( int32 iValue )
{
    mPaintEngine.SetFlowModifier( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleBlendingModeModifierChanged( int32 iValue )
{
    mPaintEngine.SetBlendingModeModifier( static_cast<::ul3::eBlendingMode>( iValue ) );
}

void
SOdysseyViewportDrawingEditorWidget::HandleAlphaModeModifierChanged( int32 iValue )
{
    mPaintEngine.SetAlphaModeModifier( static_cast<::ul3::eAlphaMode>( iValue ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
void
SOdysseyViewportDrawingEditorWidget::HandleStrokeStepChanged( int32 iValue )
{
    mPaintEngine.SetStrokeStep( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleStrokeAdaptativeChanged( bool iValue )
{
    mPaintEngine.SetStrokeAdaptative( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleStrokePaintOnTickChanged( bool iValue )
{
    mPaintEngine.SetStrokePaintOnTick( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleInterpolationTypeChanged( int32 iValue )
{
    mPaintEngine.SetInterpolationType( static_cast<EOdysseyInterpolationType>( iValue ) );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSmoothingMethodChanged( int32 iValue )
{
    mPaintEngine.SetSmoothingMethod( static_cast<EOdysseySmoothingMethod>( iValue ) );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSmoothingStrengthChanged( int32 iValue )
{
    mPaintEngine.SetSmoothingStrength( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSmoothingEnabledChanged( bool iValue )
{
    mPaintEngine.SetSmoothingEnabled( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSmoothingRealTimeChanged( bool iValue )
{
    mPaintEngine.SetSmoothingRealTime( iValue );
}

void
SOdysseyViewportDrawingEditorWidget::HandleSmoothingCatchUpChanged( bool iValue )
{
    mPaintEngine.SetSmoothingCatchUp( iValue );
}

FString 
SOdysseyViewportDrawingEditorWidget::PaintTexturePath() const
{
    if(!mPaintModeSettings->mTexturePaintSettings.mPaintTexture)
        return FString();

    return mPaintModeSettings->mTexturePaintSettings.mPaintTexture->GetPathName();
}


//---

EVisibility
SOdysseyViewportDrawingEditorWidget::GetBrushSettingsWidgetVisibility() const
{
    if( mMeshPainter->GetSelectedMode() == EOdysseyViewportSelectedMode::kBrushSettings )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

EVisibility
SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility() const
{
    if( mMeshPainter->GetSelectedMode() == EOdysseyViewportSelectedMode::kLayerStack )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

EVisibility
SOdysseyViewportDrawingEditorWidget::GetStrokeOptionsWidgetVisibility() const
{
    if( mMeshPainter->GetSelectedMode() == EOdysseyViewportSelectedMode::kStrokeOptions )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

FOdysseyPaintEngine* 
SOdysseyViewportDrawingEditorWidget::GetPaintEngine()
{
    return &mPaintEngine;
}

void
SOdysseyViewportDrawingEditorWidget::RefreshLayerStackView()
{
    mLayerStackView->DetachWidget();

    if( mPaintModeSettings->mTexturePaintSettings.mPaintTexture )
    {
        //Check if this texture already has Iliad User Data or not, if yes, we get its LayerStack
        UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mPaintModeSettings->mTexturePaintSettings.mPaintTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
        if( !userData )
        {
            userData = NewObject< UOdysseyTextureAssetUserData >(mPaintModeSettings->mTexturePaintSettings.mPaintTexture, NAME_None, RF_Public);
            mPaintModeSettings->mTexturePaintSettings.mPaintTexture->AddAssetUserData( userData );
            FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( mPaintModeSettings->mTexturePaintSettings.mPaintTexture );
            userData->GetLayerStack()->InitFromData( textureData );
            delete textureData;
            mPaintModeSettings->mTexturePaintSettings.mPaintTexture->PostEditChange();
        }
    
        mLayerStack = userData->GetLayerStack();
        mLayerStack->ComputeResultBlock();

        mLayerStackView->DetachWidget();
        mLayerStackView->AttachWidget( 
			SNew(SHorizontalBox)
            .Visibility( this, &SOdysseyViewportDrawingEditorWidget::GetLayerStackWidgetVisibility )
            + SHorizontalBox::Slot()
		    [
                SNew( SOdysseyLayerStackView )
                .LayerStackData( mLayerStack )
            ]
        );


        if( mLayerStack->GetCurrentLayer()->GetNodeContent()->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStack->GetCurrentLayer()->GetNodeContent() );
	        mPaintEngine.Block( imageLayer->GetBlock());
        }
        else
        {
            mPaintEngine.Block( NULL );
        }
    }
}


#undef LOCTEXT_NAMESPACE // "PaintModePainter"

/*
		BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(MipLevel, LayerIndex, bIsNormalMap, bIsSingleChannel, bSingleVTPhysicalSpace, bIsVirtualTexture, false);
		FCanvasTileItem TileItem( FVector2D( XPos, YPos ), Texture->Resource, FVector2D( Width, Height ), FLinearColor(Exposure, Exposure, Exposure) );
		TileItem.BlendMode = TextureEditorPtr.Pin()->GetColourChannelBlendMode();
		TileItem.BatchedElementParameters = BatchedElementParameters;
		Canvas->DrawItem( TileItem );
        */