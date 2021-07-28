// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
        SLATE_ATTRIBUTE( FText, Text )
        /** The event called when the color is committed */
        SLATE_EVENT( FOnLinearColorValueChanged, OnColorCommitted )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& iArgs );

private:
    FLinearColor mColor;
    bool mUseAlpha;
    TAttribute<FText> mText;

    /** Invoked when a new value is selected on the color wheel */
    FOnLinearColorValueChanged mOnColorCommitted;
};

void
SColorPickerEntry::Construct( const FArguments& iArgs )
{
    mColor = iArgs._Color;
    mText = iArgs._Text;
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
                .Text( mText )
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 5.f, 0.f, 0.f, 0.f ) )
            [
                    SNew( SComboButton )
                    .ContentPadding( 0 )
                    .HasDownArrow( false )
                    .ButtonStyle( FEditorStyle::Get(), "Sequencer.AnimationOutliner.ColorStrip" )
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
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.drawing-material.section-label", "Material" ) );

    UMovieSceneSequence* root_sequence = iSequencer->GetRootMovieSceneSequence();

    //-

    iMenuBuilder.AddMenuEntry( FUIAction(
                                   FExecuteAction::CreateLambda( [root_sequence]() { MasterAssetTools::ToggleBackgroundVisibility( root_sequence ); } ),
                                   FCanExecuteAction(),
                                   FGetActionCheckState::CreateLambda( [root_sequence]() { return MasterAssetTools::GetBackgroundVisibility( root_sequence ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                               ),
                               SNew( SColorPickerEntry )
                                   .Text( LOCTEXT( "settings.drawing-material.background-label", "Background" ) )
                                   .Color( MasterAssetTools::GetBackgroundColor( root_sequence ) )
                                   .UseAlpha( true )
                                   .OnColorCommitted_Lambda( [root_sequence]( FLinearColor iColor ) { MasterAssetTools::SetBackgroundColor( root_sequence, iColor ); } ),
                               NAME_None,
                               LOCTEXT( "settings.drawing-material.background-tooltip", "Display background and select its color for the drawing materials" ),
                               EUserInterfaceActionType::Check );

    //---

    iMenuBuilder.AddMenuEntry( FUIAction(
                                   FExecuteAction::CreateLambda( [root_sequence]() { MasterAssetTools::ToggleGridVisibility( root_sequence ); } ),
                                   FCanExecuteAction(),
                                   FGetActionCheckState::CreateLambda( [root_sequence]() { return MasterAssetTools::GetGridVisibility( root_sequence ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                               ),
                               SNew( SColorPickerEntry )
                                   .Text( LOCTEXT( "settings.drawing-material.grid-label", "Grid" ) )
                                   .Color( MasterAssetTools::GetGridColor( root_sequence ) )
                                   .UseAlpha( false )
                                   .OnColorCommitted_Lambda( [root_sequence]( FLinearColor iColor ) { MasterAssetTools::SetGridColor( root_sequence, iColor ); } ),
                               NAME_None,
                               LOCTEXT( "settings.drawing-material.grid-tooltip", "Display grid and select its color for the drawing materials" ),
                               EUserInterfaceActionType::Check );

    //---

    auto grid_submenu = [root_sequence]( FMenuBuilder& iMenuBuilder )
    {
        auto CreateEntry = [root_sequence]( FMenuBuilder& iMenuBuilder, EGridType iGridType, FName iBrushName, FText iToolTip )
        {
            iMenuBuilder.AddMenuEntry( FUIAction(
                                           FExecuteAction::CreateLambda( [root_sequence, iGridType]() { MasterAssetTools::SetGridType( root_sequence, iGridType ); } ),
                                           FCanExecuteAction(),
                                           FGetActionCheckState::CreateLambda( [root_sequence, iGridType]() { return MasterAssetTools::GetGridType( root_sequence ) == iGridType ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                                       ),
                                       SNew( SHorizontalBox )
                                       + SHorizontalBox::Slot()
                                       .AutoWidth()
                                       [
                                           SNew( SImage )
                                           .Image( FEposSequenceEditorStyle::Get()->GetBrush( iBrushName ) )
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

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.advanced-settings.section-label", "Advanced Settings" ) );

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenTrackEditorSettings );

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

    iMenuBuilder.AddSeparator();

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenAboutWindow );
}

#undef LOCTEXT_NAMESPACE
