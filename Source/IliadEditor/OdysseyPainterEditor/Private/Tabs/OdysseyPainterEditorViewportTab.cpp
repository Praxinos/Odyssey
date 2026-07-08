// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorViewportTab.h"

#include "Engine/Texture2D.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/STextComboBox.h"

#include "OdysseyHUDElement.h"
#include "SOdysseyViewport.h"
#include "FOdysseySceneViewport.h"
#include "OdysseyAnimationNamingTokens.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportClient.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "OdysseyPainterEditorSettings.h"
#include "Toolkits/BaseToolkit.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "PaperFlipbook.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorViewportTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_Viewport"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportTab::~FOdysseyPainterEditorViewportTab()
{
}

FOdysseyPainterEditorViewportTab::FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "viewport-tab.name", "2D Viewport" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ))
    , mEditor(iEditor)
    , mViewport(nullptr)
    , mViewportClient(nullptr)
{
    SetDefaultTexture();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorViewportTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorViewportTab::CreateWidget()
{
    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddMenuExtension( "SettingsAdvancedSection", EExtensionHook::Before, nullptr, FMenuExtensionDelegate::CreateRaw( this, &FOdysseyPainterEditorViewportTab::BuildOptionsMenu ) );
    //ToolbarExtender->AddMenuExtension( "FlipMenu", EExtensionHook::Before, nullptr, FMenuExtensionDelegate::CreateRaw( this, &FOdysseyPainterEditorViewportTab::BuildOptionsMenu ) ); // It doesn't work
    //ToolbarExtender->AddMenuExtension( "ZoomMenu", EExtensionHook::First, nullptr, FMenuExtensionDelegate::CreateRaw( this, &FOdysseyPainterEditorViewportTab::BuildOptionsMenu ) );  // It doesn't work

    //TODO: store it somewhere ? here ? toolkit ?
    // But must be filled as lambda, as animation is not valid when the widget is created
    TStrongObjectPtr<UOdysseyAnimationNamingTokensContext> AnimationNamingTokenContext(NewObject<UOdysseyAnimationNamingTokensContext>());
    TStrongObjectPtr<UOdysseyAnimationPlayerNamingTokensContext> AnimationPlayerNamingTokenContext(NewObject<UOdysseyAnimationPlayerNamingTokensContext>());
    TStrongObjectPtr<UOdysseyViewportNamingTokensContext> ViewportNamingTokenContext(NewObject<UOdysseyViewportNamingTokensContext>());

    SAssignNew( mViewport, SOdysseyViewport )
        .Texture( this, &FOdysseyPainterEditorViewportTab::Texture )
        .OptionExtender( ToolbarExtender )
        .RotationStep_Lambda( []() -> float
                              {
                                  return GetDefault<UOdysseyPainterEditorSettings>()->ViewportRotationStep;
                              } )
        .ZoomStep_Lambda( []() -> float
                          {
                              return GetDefault<UOdysseyPainterEditorSettings>()->ViewportZoomStep;
                          } )
        .StatusbarTemplateString_Lambda( []() -> FTemplateString
                                         {
                                             return GetDefault<UOdysseyPainterEditorSettings>()->StatusBarTemplateString;
                                         } )
        .NamingTokensContexts_Lambda( [this, AnimationNamingTokenContext, AnimationPlayerNamingTokenContext, ViewportNamingTokenContext]() -> TArray<UObject*>
                                      {
                                          TArray<UObject*> contexts;

                                          AnimationNamingTokenContext->Animation = mEditor->GetAnimation();
                                          contexts.Add( AnimationNamingTokenContext.Get() );

                                          AnimationPlayerNamingTokenContext->AnimationPlayer = mEditor->GetAnimationPlayer();
                                          contexts.Add( AnimationPlayerNamingTokenContext.Get() );

                                          ViewportNamingTokenContext->ViewportWidget = mViewport;
                                          contexts.Add( ViewportNamingTokenContext.Get() );

                                          return contexts;
                                      } )
        ;

    mViewportClient = MakeShareable(new FOdysseyPainterEditorViewportClient(mEditor, mViewport, mEditor->GetMeshSelector().Get()));

    mViewportClient->OnPickColor().BindRaw(this, &FOdysseyPainterEditorViewportTab::HandleViewportColorPicked);
    mViewportClient->OnMouseDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDown);
    mViewportClient->OnMouseClick().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseClick);
    mViewportClient->OnMouseDoubleClick().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDoubleClick);
    mViewportClient->OnMouseUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseUp);
    mViewportClient->OnMouseHover().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseHover);
    mViewportClient->OnMouseDrag().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDrag);
    mViewportClient->OnKeyDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyDown);
    mViewportClient->OnKeyUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyUp);

    mViewport->SetViewportClient(mViewportClient);

    return mViewport;
}

void
FOdysseyPainterEditorViewportTab::BuildOptionsMenu( FMenuBuilder& ioMenuBuilder )
{
    ioMenuBuilder.AddSubMenu(
        LOCTEXT( "viewport.background.menu.name", "Background" ),
        LOCTEXT( "viewport.background.menu.tooltip", "Manage the background of the viewport and canvas" ),
        FNewMenuDelegate::CreateSP( this, &FOdysseyPainterEditorViewportTab::BuildBackgroundSubMenu )
    );
}

FReply
FOdysseyPainterEditorViewportTab::OnColorBlockClicked( TSharedPtr<SButton> iWidget, FLinearColor iInitialColor, FOnLinearColorValueChanged OnColorChanged, FSimpleDelegate OnColorCommitted ) const
{
    //AnimationOutlinerTreeNode::InitialTrackColor = GetTrackColorTint().GetSpecifiedColor();
    //AnimationOutlinerTreeNode::bFolderPickerWasCancelled = false;

    FColorPickerArgs PickerArgs;
    PickerArgs.ParentWidget = iWidget;
    PickerArgs.bUseAlpha = false;
    PickerArgs.bOpenAsMenu = true;
    PickerArgs.bClampValue = true;
    //PickerArgs.DisplayGamma = TAttribute<float>::Create( TAttribute<float>::FGetter::CreateUObject( GEngine, &UEngine::GetDisplayGamma ) );

    //PickerArgs.InitialColor = AnimationOutlinerTreeNode::InitialTrackColor;
    PickerArgs.InitialColor = iInitialColor;

    PickerArgs.OnColorCommitted = OnColorChanged;
    //PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP( this, &SOutlinerTrackColorPicker::OnColorPickerPicked );
    //PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP( this, &SOutlinerTrackColorPicker::OnColorPickerClosed );
    //PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP( this, &SOutlinerTrackColorPicker::OnColorPickerCancelled );
    PickerArgs.OnInteractivePickEnd = OnColorCommitted;

    OpenColorPicker( PickerArgs );

    return FReply::Handled();
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundViewportEntry( FMenuBuilder& ioMenuBuilder )
{
    FOnLinearColorValueChanged onColorChanged = FOnLinearColorValueChanged::CreateLambda(
        []( FLinearColor iColor )
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetInteractiveMode();
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetBackgroundColor( iColor.ToFColorSRGB() );
            GetMutableDefault<UOdysseyPainterEditorSettings>()->RemoveInteractiveMode();
        }
    );

    FSimpleDelegate onColorCommited = FSimpleDelegate::CreateLambda(
        []()
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SaveConfig();
        }
    );

    auto onGetColor = []() -> FLinearColor
        {
            return GetDefault<UOdysseyPainterEditorSettings>()->GetBackgroundColor();
        };

    TSharedPtr<SButton> buttonWidget = SNew( SButton )
        .ContentPadding( 0 )
        .VAlign( VAlign_Fill )
        .HAlign( HAlign_Right )
        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
        [
            SNew( SColorBlock )
            .AlphaDisplayMode( EColorBlockAlphaDisplayMode::Ignore )
            .Size( FVector2D( 80, 20 ) )
            .CornerRadius( FVector4( 4.0f, 4.0f, 4.0f, 4.0f ) )
            .Color_Lambda( onGetColor )
        ];

    buttonWidget->SetOnClicked( FOnClicked::CreateSP( SharedThis( this ), &FOdysseyPainterEditorViewportTab::OnColorBlockClicked, buttonWidget, onGetColor(), onColorChanged, onColorCommited ) );

    //-

    ioMenuBuilder.AddWidget(
        buttonWidget.ToSharedRef(),
        LOCTEXT( "viewport.background-color.label", "Color" ),
        true /* =bNoIndent */,
        true /* =bInSearchable */,
        LOCTEXT( "viewport.background-color.tooltip", "Modify the background color of the viewport." )
    );
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundCheckboardColorOneEntry( FMenuBuilder& ioMenuBuilder )
{
    FOnLinearColorValueChanged onColorChanged = FOnLinearColorValueChanged::CreateLambda(
        []( FLinearColor iColor )
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetInteractiveMode();
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetCheckerColorOne( iColor.ToFColorSRGB() );
            GetMutableDefault<UOdysseyPainterEditorSettings>()->RemoveInteractiveMode();
        }
    );

    FSimpleDelegate onColorCommited = FSimpleDelegate::CreateLambda(
        []()
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SaveConfig();
        }
    );

    auto onGetColor = []() -> FLinearColor
        {
            return GetDefault<UOdysseyPainterEditorSettings>()->GetCheckerColorOne();
        };

    TSharedPtr<SButton> buttonWidget = SNew( SButton )
        .ContentPadding( 0 )
        .VAlign( VAlign_Fill )
        .HAlign( HAlign_Right )
        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
        [
            SNew( SColorBlock )
            .AlphaDisplayMode( EColorBlockAlphaDisplayMode::Ignore )
            .Size( FVector2D( 80, 20 ) )
            .CornerRadius( FVector4( 4.0f, 4.0f, 4.0f, 4.0f ) )
            .Color_Lambda( onGetColor )
        ];

    buttonWidget->SetOnClicked( FOnClicked::CreateSP( SharedThis( this ), &FOdysseyPainterEditorViewportTab::OnColorBlockClicked, buttonWidget, onGetColor(), onColorChanged, onColorCommited ) );

    //-

    ioMenuBuilder.AddWidget(
        buttonWidget.ToSharedRef(),
        LOCTEXT( "viewport.checker-color-one.label", "Color One" ),
        true /* =bNoIndent */,
        true /* =bInSearchable */,
        LOCTEXT( "viewport.checker-color-one.tooltip", "Modify the checker color one of the canvas." )
    );
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundCheckboardColorTwoEntry( FMenuBuilder& ioMenuBuilder )
{
    FOnLinearColorValueChanged onColorChanged = FOnLinearColorValueChanged::CreateLambda(
        []( FLinearColor iColor )
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetInteractiveMode();
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SetCheckerColorTwo( iColor.ToFColorSRGB() );
            GetMutableDefault<UOdysseyPainterEditorSettings>()->RemoveInteractiveMode();
        }
    );

    FSimpleDelegate onColorCommited = FSimpleDelegate::CreateLambda(
        []()
        {
            GetMutableDefault<UOdysseyPainterEditorSettings>()->SaveConfig();
        }
    );

    auto onGetColor = []() -> FLinearColor
        {
            return GetDefault<UOdysseyPainterEditorSettings>()->GetCheckerColorTwo();
        };

    TSharedPtr<SButton> buttonWidget = SNew( SButton )
        .ContentPadding( 0 )
        .VAlign( VAlign_Fill )
        .HAlign( HAlign_Right )
        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
        [
            SNew( SColorBlock )
            .AlphaDisplayMode( EColorBlockAlphaDisplayMode::Ignore )
            .Size( FVector2D( 80, 20 ) )
            .CornerRadius( FVector4( 4.0f, 4.0f, 4.0f, 4.0f ) )
            .Color_Lambda( onGetColor )
        ];

    buttonWidget->SetOnClicked( FOnClicked::CreateSP( SharedThis( this ), &FOdysseyPainterEditorViewportTab::OnColorBlockClicked, buttonWidget, onGetColor(), onColorChanged, onColorCommited ) );

    //-

    ioMenuBuilder.AddWidget(
        buttonWidget.ToSharedRef(),
        LOCTEXT( "viewport.checker-color-two.label", "Color Two" ),
        true /* =bNoIndent */,
        true /* =bInSearchable */,
        LOCTEXT( "viewport.checker-color-two.tooltip", "Modify the checker color two of the canvas." )
    );
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundCheckboardSizeEntry( FMenuBuilder& ioMenuBuilder )
{
    TArray<TSharedPtr<FString>>* entries = new TArray<TSharedPtr<FString>>();
    entries->Add( MakeShareable( new FString( TEXT( "2" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "4" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "8" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "16" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "32" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "64" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "128" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "256" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "512" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "1024" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "2048" ) ) ) );
    entries->Add( MakeShareable( new FString( TEXT( "4096" ) ) ) );

    FString initial_size = FString::FromInt( GetDefault<UOdysseyPainterEditorSettings>()->GetCheckerSize() );
    TSharedPtr<FString>* selected_item = entries->FindByPredicate( [initial_size]( TSharedPtr<FString> iEntry )
                                                                  {
                                                                      return *iEntry == initial_size;
                                                                  } );

    TSharedRef<SWidget> widget = SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        [
            SNew( SSpacer )
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( SBox )
            .WidthOverride( 80 )
            .MaxDesiredWidth( 80 )
            .MinDesiredWidth( 80 )
            [
                SNew( STextComboBox )
                .OptionsSource( entries )
                .InitiallySelectedItem( selected_item ? *selected_item : nullptr )
                .OnGetTextLabelForItem_Lambda( [entries]( TSharedPtr<FString> iStringEntry ) -> FString
                                               {
                                                   return *iStringEntry + TEXT( "px" ); // There is no "pixel" EUnit
                                               } )
                .OnSelectionChanged_Lambda( [entries]( TSharedPtr<FString> iStringSelected, ESelectInfo::Type iType )
                                            {
                                                GetMutableDefault<UOdysseyPainterEditorSettings>()->SetCheckerSize( FCString::Atoi( **iStringSelected ) );
                                            } )
            ]
        ];

    ioMenuBuilder.AddWidget(
        widget,
        LOCTEXT( "viewport.checker-size.label", "Size" ),
        true /* =bNoIndent */,
        true /* =bInSearchable */,
        LOCTEXT( "viewport.checker-size.tooltip", "Modify the checker size of the canvas." )
    );
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundCheckboardPresets( FMenuBuilder& ioMenuBuilder )
{
    TArray<FCheckboardPreset> presets = GetDefault<UOdysseyPainterEditorSettings>()->GetCheckerPresets();
    for( FCheckboardPreset preset : presets )
    {
        TSharedPtr<SHorizontalBox> widget = SNew( SHorizontalBox )

            + SHorizontalBox::Slot()
            .Padding( 3, 0 )
            [
                SNew( STextBlock )
                .Text( FText::FromString( preset.Name ) )
            ]

            + SHorizontalBox::Slot()
            .Padding( 3, 0, 1, 0 )
            .AutoWidth()
            [
                SNew( SColorBlock )
                .AlphaDisplayMode( EColorBlockAlphaDisplayMode::Ignore )
                .Size( FVector2D( 20, 20 ) )
                .CornerRadius( FVector4( 4.0f, 4.0f, 4.0f, 4.0f ) )
                .Color( preset.ColorOne )
            ]

            + SHorizontalBox::Slot()
            .Padding( 1, 0, 3, 0 )
            .AutoWidth()
            [
                SNew( SColorBlock )
                .AlphaDisplayMode( EColorBlockAlphaDisplayMode::Ignore )
                .Size( FVector2D( 20, 20 ) )
                .CornerRadius( FVector4( 4.0f, 4.0f, 4.0f, 4.0f ) )
                .Color( preset.ColorTwo )
            ]

            + SHorizontalBox::Slot()
            .Padding( 3, 0 )
            .AutoWidth()
            [
                SNew( SBox )
                .WidthOverride( 50 ) // To have enough space to display XXXXpx
                .MaxDesiredWidth( 50 )
                .MinDesiredWidth( 50 )
                .HAlign( HAlign_Right )
                [
                    SNew( STextBlock )
                    .Text( FText::FromString( FString::FromInt( preset.Size ) + TEXT( "px" ) ) ) // There is no "pixel" EUnit to be able to use `TNumericUnitTypeInterface<int> degrees( EUnit::Degrees );` to convert to `FString`
                    //.Text( FText::Format( LOCTEXT( "viewport.checker-preset.entry.size", "{0}px" ), preset.Size ) )
                ]
            ];

        //-

        FText tooltip = FText::Format( LOCTEXT( "viewport.checker-preset.entry.tooltip", "Preset \"{3}\":\n- Color 1: {0}\n- Color 2: {1}\n- Size: {2}px" )
                                        , FText::FromString( FString::Printf( TEXT( "%d %d %d" ), preset.ColorOne.R, preset.ColorOne.G, preset.ColorOne.B ) )
                                        , FText::FromString( FString::Printf( TEXT( "%d %d %d" ), preset.ColorTwo.R, preset.ColorTwo.G, preset.ColorTwo.B ) )
                                        , preset.Size
                                        , FText::FromString( preset.Name )
        );

        FMenuEntryParams params;
        params.ToolTipOverride = tooltip;
        params.EntryWidget = widget;
        params.DirectActions = FUIAction(
            FExecuteAction::CreateLambda( [preset]()
                                          {
                                              GetMutableDefault<UOdysseyPainterEditorSettings>()->SetCheckerColor( preset.ColorOne, preset.ColorTwo );
                                              GetMutableDefault<UOdysseyPainterEditorSettings>()->SetCheckerSize( preset.Size );
                                          } )
        );
        ioMenuBuilder.AddMenuEntry( params );
    }

    ioMenuBuilder.AddSeparator();

    FMenuEntryParams params;
    params.LabelOverride = LOCTEXT( "viewport.checker-preset-open-settings.entry.label", "Open Preset Settings..." );
    params.ToolTipOverride = LOCTEXT( "viewport.checker-preset-open-settings.entry.tooltip", "Open the preset settings panel" );
    params.DirectActions = FUIAction(
        FExecuteAction::CreateLambda( []()
                                      {
                                          FModuleManager::LoadModuleChecked<ISettingsModule>( "Settings" ).ShowViewer( "Editor", "Plugins", "OdysseyPainterEditor" );
                                      } )
    );
    ioMenuBuilder.AddMenuEntry( params );
}

void
FOdysseyPainterEditorViewportTab::BuildBackgroundSubMenu( FMenuBuilder& ioMenuBuilder )
{
    ioMenuBuilder.SetSearchable( false );

    //---

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "section.viewport.background.label", "Viewport" ) );

    BuildBackgroundViewportEntry( ioMenuBuilder );

    ioMenuBuilder.EndSection();

    //---

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "section.canvas.background-checker.label", "Canvas Checkboard" ) );

    BuildBackgroundCheckboardColorOneEntry( ioMenuBuilder );
    BuildBackgroundCheckboardColorTwoEntry( ioMenuBuilder );
    BuildBackgroundCheckboardSizeEntry( ioMenuBuilder );

    ioMenuBuilder.EndSection();

    //---

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "section.canvas.background-checker-presets.label", "Presets" ) );

    BuildBackgroundCheckboardPresets( ioMenuBuilder );

    ioMenuBuilder.EndSection();
}

void
FOdysseyPainterEditorViewportTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();

    TSharedPtr<FUICommandList> commands = mViewport->GetCommandList();
    if( commands.IsValid() )
        toolkitCommands->Append( mViewport->GetCommandList().ToSharedRef() );
}

void FOdysseyPainterEditorViewportTab::OnTabClosed(TSharedRef<SDockTab> iDockTab)
{
    if( mViewportClient )
        mViewportClient->UnregisterWindow();

    FOdysseyEditorTab::OnTabClosed(iDockTab);
}

void FOdysseyPainterEditorViewportTab::CloseTab()
{
    if (mViewportClient)
        mViewportClient->UnregisterWindow();

    FOdysseyEditorTab::CloseTab();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyViewport>
FOdysseyPainterEditorViewportTab::GetViewport()
{
    return mViewport;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Setters

void
FOdysseyPainterEditorViewportTab::SetDefaultTexture()
{
    mTexture = TAttribute<UTexture*>::CreateLambda(
        [this]() -> UTexture*
        {
            if (!mEditor->GetSource())
                return nullptr;

            return mEditor->GetSource()->DisplayTexture();
        }
    );
}

void
FOdysseyPainterEditorViewportTab::SetTexture(const TAttribute<UTexture*>& iTexture )
{
    mTexture = iTexture;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UTexture*
FOdysseyPainterEditorViewportTab::Texture() const
{
    UObject* editedObject = mEditor->GetEditedObject();
    if (!editedObject || !editedObject->IsA<UPaperFlipbook>())
    {
        TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
        if (!source)
            return nullptr;

        return source->DisplayTexture();
    }

    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(editedObject);

    TSharedPtr<FOdysseyPainterEditorFlipbookTimelineTab> timelineTab = mEditor->FindTab<FOdysseyPainterEditorFlipbookTimelineTab>();
    if (timelineTab->Timeline() && timelineTab->Timeline()->IsScrubbing())
    {
        int32 index = timelineTab->Timeline()->GetCurrentKeyframeIndex();
        UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, index);
        return texture;
    }

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source)
        return nullptr;

    return source->DisplayTexture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorViewportTab::HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture)
{
    if (iEventState == eOdysseyEventState::kSet)
        mEditor->GetTemporaryColorPickerTool() ? mEditor->GetTemporaryColorPickerTool()->PickColorUp(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y)) : mEditor->GetColorPickerTool()->PickColorUp(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y));
    else
        mEditor->GetTemporaryColorPickerTool() ? mEditor->GetTemporaryColorPickerTool()->PickColorMove(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y)) : mEditor->GetColorPickerTool()->PickColorMove(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y));
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->ProcessMouseDown(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->ProcessMouseClick(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->ProcessMouseDoubleClick(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->ProcessMouseUp(iPointInTexture, iKey);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseHover(const FOdysseyPoint& iPointInTexture)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    tool->ProcessMouseHover(iPointInTexture);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    tool->ProcessMouseDrag(iPointInTexture);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyDown(const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if ( !tool )
        return false;

    return tool->ProcessKeyDown(iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyUp(const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if ( !tool )
        return false;

    return tool->ProcessKeyUp(iKey);
}

#undef LOCTEXT_NAMESPACE
