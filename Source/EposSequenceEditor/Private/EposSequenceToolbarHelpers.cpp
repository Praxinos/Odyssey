// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceToolbarHelpers.h"

#include "ISequencer.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

#include "EposSequenceEditorCommands.h"
#include "EposTracksToolbarHelpers.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceToolbarHelpers"

//---

//static
void
EposSequenceToolbarHelpers::MakeTextureSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    EposTracksToolbarHelpers::MakeTextureSettingsEntries( iMenuBuilder );
}

//static
void
EposSequenceToolbarHelpers::MakeCameraSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    EposTracksToolbarHelpers::MakeCameraSettingsEntries( iMenuBuilder );
}

//---

class SColorPickerEntry
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SColorPickerEntry )
        {}
        SLATE_ARGUMENT( FLinearColor, Color )
        SLATE_ARGUMENT( bool, UseAlpha )
        SLATE_ARGUMENT( FText, Text )
        /** The event called when the color is committed */
        SLATE_EVENT( FOnLinearColorValueChanged, OnColorCommitted )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& iArgs );

private:
    FLinearColor mColor;
    bool         mUseAlpha;

    /** Invoked when a new value is selected on the color wheel */
    FOnLinearColorValueChanged mOnColorCommitted;
};

void
SColorPickerEntry::Construct( const FArguments& iArgs )
{
    mColor = iArgs._Color;
    mUseAlpha = iArgs._UseAlpha;
    mOnColorCommitted = iArgs._OnColorCommitted;

    auto GetColor = [=]()
    {
        return mColor;
    };

    auto SetColor = [=]( FLinearColor iNewColor )
    {
        mColor = iNewColor;
        mOnColorCommitted.ExecuteIfBound( mColor );
    };

    auto OnGetMenuContent = [=]() -> TSharedRef<SWidget>
    {
        // Open a color picker
        return SNew( SColorPicker )
            .TargetColorAttribute_Lambda( GetColor )
            .UseAlpha( mUseAlpha )
            .DisplayInlineVersion( true )
            .OnColorCommitted_Lambda( SetColor );
    };

    ChildSlot
    [
        SNew( SBox )
        .MinDesiredWidth( 150.f )
        [
            SNew( SHorizontalBox )
            +SHorizontalBox::Slot()
            [
                SNew( STextBlock )
                .Visibility( iArgs._Text.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible )
                .Text( iArgs._Text )
            ]
            +SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 5.f, 0.f, 0.f, 0.f ) )
            [
                    SNew( SComboButton )
                    .ContentPadding( 0 )
                    .HasDownArrow( false )
                    .ButtonStyle( FAppStyle::Get(), "Sequencer.AnimationOutliner.ColorStrip" )
                    .OnGetMenuContent_Lambda( OnGetMenuContent )
                    .CollapseMenuOnParentFocus( true )
                    .ButtonContent()
                    [
                        SNew( SColorBlock )
                        .Color_Lambda( GetColor )
                        .ShowBackgroundForAlpha( true )
                        .Size( FVector2D( 50.0f, 16.0f ) )
                    ]
            ]
        ]
    ];
}

//---

//static
void
EposSequenceToolbarHelpers::MakeSettingsEntries( FMenuBuilder& iMenuBuilder, ISequencer* iSequencer )
{
    UMovieSceneSequence* root_sequence = iSequencer->GetRootMovieSceneSequence();

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.drawing-material.section-label", "Material" ) );

    //-

    iMenuBuilder.AddMenuEntry( FUIAction(
                                   FExecuteAction::CreateLambda( [iSequencer, root_sequence]() { MasterAssetTools::ToggleBackgroundVisibility( *iSequencer, root_sequence ); } ),
                                   FCanExecuteAction(),
                                   FGetActionCheckState::CreateLambda( [iSequencer, root_sequence]() { return MasterAssetTools::GetBackgroundVisibility( *iSequencer, root_sequence ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                               ),
                               SNew( SColorPickerEntry )
                                   .Text( LOCTEXT( "settings.drawing-material.background-label", "Background" ) )
                                   .Color( MasterAssetTools::GetBackgroundColor( *iSequencer, root_sequence ) )
                                   .UseAlpha( true )
                                   .OnColorCommitted_Lambda( [iSequencer, root_sequence]( FLinearColor iColor ) { MasterAssetTools::SetBackgroundColor( *iSequencer, root_sequence, iColor ); } ),
                               NAME_None,
                               LOCTEXT( "settings.drawing-material.background-tooltip", "Display background and select its color for the drawing materials" ),
                               EUserInterfaceActionType::Check );

    //-

    iMenuBuilder.AddMenuEntry( FUIAction(
                                   FExecuteAction::CreateLambda( [iSequencer, root_sequence]() { MasterAssetTools::ToggleGridVisibility( *iSequencer, root_sequence ); } ),
                                   FCanExecuteAction(),
                                   FGetActionCheckState::CreateLambda( [iSequencer, root_sequence]() { return MasterAssetTools::GetGridVisibility( *iSequencer, root_sequence ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                               ),
                               SNew( SColorPickerEntry )
                                   .Text( LOCTEXT( "settings.drawing-material.grid-label", "Grid" ) )
                                   .Color( MasterAssetTools::GetGridColor( *iSequencer, root_sequence ) )
                                   .UseAlpha( false )
                                   .OnColorCommitted_Lambda( [iSequencer, root_sequence]( FLinearColor iColor ) { MasterAssetTools::SetGridColor( *iSequencer, root_sequence, iColor ); } ),
                               NAME_None,
                               LOCTEXT( "settings.drawing-material.grid-tooltip", "Display grid and select its color for the drawing materials" ),
                               EUserInterfaceActionType::Check );

    //-

    auto grid_submenu = [iSequencer, root_sequence]( FMenuBuilder& iMenuBuilder )
    {
        auto CreateEntry = [iSequencer, root_sequence]( FMenuBuilder& iMenuBuilder, EGridType iGridType, FName iBrushName, FText iToolTip )
        {
            iMenuBuilder.AddMenuEntry( FUIAction(
                                           FExecuteAction::CreateLambda( [iSequencer, root_sequence, iGridType]() { MasterAssetTools::SetGridType( *iSequencer, root_sequence, iGridType ); } ),
                                           FCanExecuteAction(),
                                           FGetActionCheckState::CreateLambda( [iSequencer, root_sequence, iGridType]() { return MasterAssetTools::GetGridType( *iSequencer, root_sequence ) == iGridType ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                                       ),
                                       SNew( SHorizontalBox )
                                       + SHorizontalBox::Slot()
                                       .AutoWidth()
                                       [
                                           SNew( SImage )
                                           .Image( FEposSequenceEditorStyle::Get().GetBrush( iBrushName ) )
                                       ],
                                       NAME_None,
                                       iToolTip,
                                       EUserInterfaceActionType::RadioButton );
        };

        CreateEntry( iMenuBuilder, EGridType::kNone, "FilmOverlay.Disabled", LOCTEXT( "settings.drawing-material.grid-type-none-tooltip", "No grid" ) );
        CreateEntry( iMenuBuilder, EGridType::k2x2, "FilmOverlay.2x2Grid", LOCTEXT( "settings.drawing-material.grid-type-2x2-tooltip", "2x2" ) );
        CreateEntry( iMenuBuilder, EGridType::k3x3, "FilmOverlay.3x3Grid", LOCTEXT( "settings.drawing-material.grid-type-3x3-tooltip", "3x3" ) );
        CreateEntry( iMenuBuilder, EGridType::kCrosshair, "FilmOverlay.Crosshair", LOCTEXT( "settings.drawing-material.grid-type-crosshair-tooltip", "Crosshair" ) );
        CreateEntry( iMenuBuilder, EGridType::kAbatment, "FilmOverlay.Rabatment", LOCTEXT( "settings.drawing-material.grid-type-rabatment-tooltip", "Rabatment" ) );
    };

    iMenuBuilder.AddSubMenu( LOCTEXT( "settings.drawing-material.grid-type-label", "Grid Type" ), LOCTEXT( "settings.drawing-material.grid-type-tooltip", "Select the inner grid type" ), FNewMenuDelegate::CreateLambda( grid_submenu ) );

    iMenuBuilder.EndSection();

    //---

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.lighttable.section-label", "Lighttable" ) );

    auto OnGetPreviousColor = [iSequencer, root_sequence]() -> FLinearColor
    {
        FLinearColor color = MasterAssetTools::GetPreviousDrawingColor( *iSequencer, root_sequence );
        color.A = MasterAssetTools::GetPreviousDrawingOpacity( *iSequencer, iSequencer->GetRootMovieSceneSequence() );

        return color;
    };

    auto OnPreviousColorCommited = [iSequencer, root_sequence]( FLinearColor iColor )
    {
        MasterAssetTools::SetPreviousDrawingColor( *iSequencer, root_sequence, iColor );
        MasterAssetTools::SetPreviousDrawingOpacity( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iColor.A );
    };

    iMenuBuilder.AddWidget( SNew( SColorPickerEntry )
                                .ToolTipText( LOCTEXT( "settings.lighttable.previous-drawing-color-tooltip", "Select the color of the previous drawing when the lighttable is enabled" ) )
                                .Color( OnGetPreviousColor() )
                                .UseAlpha( true )
                                .OnColorCommitted_Lambda( OnPreviousColorCommited ),
                            LOCTEXT( "settings.lighttable.previous-drawing-color-label", "Previous Drawing Color" ) );

    //-

    auto OnGetNextColor = [iSequencer, root_sequence]() -> FLinearColor
    {
        FLinearColor color = MasterAssetTools::GetNextDrawingColor( *iSequencer, root_sequence );
        color.A = MasterAssetTools::GetNextDrawingOpacity( *iSequencer, iSequencer->GetRootMovieSceneSequence() );

        return color;
    };

    auto OnNextColorCommited = [iSequencer, root_sequence]( FLinearColor iColor )
    {
        MasterAssetTools::SetNextDrawingColor( *iSequencer, root_sequence, iColor );
        MasterAssetTools::SetNextDrawingOpacity( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iColor.A );
    };

    iMenuBuilder.AddWidget( SNew( SColorPickerEntry )
                                .ToolTipText( LOCTEXT( "settings.lighttable.next-drawing-color-tooltip", "Select the color of the next drawing when the lighttable is enabled" ) )
                                .Color( OnGetNextColor() )
                                .UseAlpha( true )
                                .OnColorCommitted_Lambda( OnNextColorCommited ),
                            LOCTEXT( "settings.lighttable.next-drawing-color-label", "Next Drawing Color" ) );

    iMenuBuilder.EndSection();

    //---

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.advanced-settings.section-label", "Advanced Settings" ) );

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenTrackEditorSettings );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenNamingConventionEditorSettings );

    iMenuBuilder.EndSection();
}

//static
void
EposSequenceToolbarHelpers::MakeHelpEntries( FMenuBuilder& iMenuBuilder )
{
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoPraxinos );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoForum );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoDiscord );

    iMenuBuilder.AddSeparator();

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoUserDocumentation );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoProjects );

    iMenuBuilder.AddSeparator();

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenAboutWindow );
}

#undef LOCTEXT_NAMESPACE
