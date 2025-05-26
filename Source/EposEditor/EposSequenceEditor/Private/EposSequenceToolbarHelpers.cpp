// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceToolbarHelpers.h"

#include "ISequencer.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SComboButton.h"

#include "EposSequenceEditorCommands.h"
#include "EposTracksToolbarHelpers.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceToolbarHelpers"

//---

//static
void
EposSequenceToolbarHelpers::MakeAnimationSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    EposTracksToolbarHelpers::MakeAnimationSettingsEntries( iMenuBuilder );
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

    auto GetColor = [this]()
    {
        return mColor;
    };

    auto SetColor = [this]( FLinearColor iNewColor )
    {
        mColor = iNewColor;
        mOnColorCommitted.ExecuteIfBound( mColor );
    };

    auto OnGetMenuContent = [this, GetColor, SetColor]() -> TSharedRef<SWidget>
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
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();

    //---

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "settings.advanced-settings.section-label", "Advanced Settings" ) );

    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenTrackEditorSettings );
    iMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenNamingConventionEditorSettings );

    iMenuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
