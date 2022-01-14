// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/NamingConventionSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettingsCustomization"

//---

namespace
{
static
bool
CheckPatternValidity( const FString& iPattern, const TArray<FString>& iValidPatterns )
{
    FRegexPattern key_pattern = TEXT( "\\{[^}]*\\}" ); // Mandatory as FRegexMatcher() takes a const reference
    FRegexMatcher matcher( key_pattern, iPattern );

    // This loop is to validate all {...} patterns
    while( matcher.FindNext() )
    {
        int32 full_begin = matcher.GetMatchBeginning();
        int32 full_end = matcher.GetMatchEnding();
        FTextRange full_range( full_begin, full_end );
        FString full_string = iPattern.Mid( full_range.BeginIndex, full_range.Len() );

        if( !iValidPatterns.Contains( full_string ) )
            return false;
    }

    // This loop is to check if a valid pattern appears ONLY 1 time
    for( auto valid_pattern : iValidPatterns )
    {
        int32 start_index = iPattern.Find( valid_pattern );
        // If the current valid pattern is NOT found, that's ok and let's check the next pattern
        if( start_index == INDEX_NONE )
            continue;

        // Here we find the first occurance of the current valid pattern

        // Try to find the same pattern another time
        start_index = iPattern.Find( valid_pattern, ESearchCase::IgnoreCase, ESearchDir::FromStart, start_index + 1 );
        // If the current valid pattern is found again, it's wrong because a valid pattern should only appear 1 time, so return false
        if( start_index != INDEX_NONE )
            return false;
    }

    return true;
}
}

class SPatternTextBox
    : public SCompoundWidget
    //: public SEditableTextBox
{
public:
    SLATE_BEGIN_ARGS( SPatternTextBox )
        {}
        SLATE_ATTRIBUTE( FText, AdvancedExplanation )
        SLATE_ARGUMENT( TArray<FString>, ValidPatterns )
    SLATE_END_ARGS()

    /**
     * Construct this widget
     *
     * @param   InArgs  The declaration data for this widget
     */
    void Construct( const FArguments& iArgs, TSharedPtr<IPropertyHandle> iPatternHandle );

private:
    FText GetPatternText() const;

    void OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo );
    void OnPatternTextChanged( const FText& iNewText );

    bool CheckPatternValidity( const FString& iPattern );

private:
    TSharedPtr<IPropertyHandle> mPatternHandle;

    TSharedPtr<SEditableTextBox> mTextBoxWidget;

    TAttribute<FText> mAdvancedExplanation;
    TArray<FString> mValidPatterns;
};

void
SPatternTextBox::Construct( const FArguments& iArgs, TSharedPtr<IPropertyHandle> iPatternHandle )
{
    mPatternHandle = iPatternHandle;

    mAdvancedExplanation = iArgs._AdvancedExplanation;
    mValidPatterns = iArgs._ValidPatterns;

    if( iArgs._ToolTipText.IsSet() )
        mPatternHandle->SetToolTipText( iArgs._ToolTipText.Get() );

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            [
                SAssignNew( mTextBoxWidget, SEditableTextBox )
                .Text( this, &SPatternTextBox::GetPatternText )
                .Font( FEditorStyle::GetFontStyle( TEXT( "PropertyWindow.NormalFont" ) ) )
                .SelectAllTextWhenFocused( true )
                .ClearKeyboardFocusOnCommit( false )
                .OnTextCommitted( this, &SPatternTextBox::OnPatternTextCommited )
                .OnTextChanged( this, &SPatternTextBox::OnPatternTextChanged )
                .SelectAllTextOnCommit( true )
            ]

            + SHorizontalBox::Slot()
            .FillWidth( .1f )
            [
                SNew( SSpacer )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0, 4, 0, 0 )
        [
            SNew( STextBlock )
            .Text( mAdvancedExplanation )
            .ToolTipText( iArgs._ToolTipText )
        ]
    ];
}

FText
SPatternTextBox::GetPatternText() const
{
    FText pattern;
    mPatternHandle->GetValueAsFormattedText( pattern );

    return pattern;
}

void
SPatternTextBox::OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo )
{
    FString new_pattern = iNewText.ToString();

    FText current_pattern;
    mPatternHandle->GetValueAsFormattedText( current_pattern );
    if( new_pattern.Equals( current_pattern.ToString() ) )
        return;

    if( !CheckPatternValidity( new_pattern ) )
        return;

    //-

    mPatternHandle->SetValueFromFormattedString( new_pattern );
}

void
SPatternTextBox::OnPatternTextChanged( const FText& iNewText )
{
    if( CheckPatternValidity( iNewText.ToString() ) )
    {
        TAttribute<FSlateColor> empty;
        mTextBoxWidget->SetTextBoxBackgroundColor( empty ); // Remove the attribute to use the "real" background style of the widget
    }
    else
    {
        mTextBoxWidget->SetTextBoxBackgroundColor( FLinearColor( 1, 0, 0, 0.35f ) );
        //mTextBoxWidget->SetTextBoxBackgroundColor( FEditorStyle::GetColor( TEXT( "ErrorReporting.BackgroundColor" ) ) );
    }
}

bool
SPatternTextBox::CheckPatternValidity( const FString& iPattern )
{
    return ::CheckPatternValidity( iPattern, mValidPatterns );
}

//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionPlaneCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionPlaneCustomization() );
}

FText
FNamingConventionPlaneCustomization::GetTooltipText() const
{
    return LOCTEXT( "plane-pattern-info-label",
R"(Some examples:

- plane_{plane-index} ->
    plane_10
    plane_20
    plane_30
    ...
- pl{plane-index}_{camera-name} ->
    pl10_mycamera
    pl20_mycamera
    pl30_mycamera
    ...
- {shot-name}_plane{plane-index}_{camera-name} ->
    shot40_plane10_mycamera
    shot40_plane20_mycamera
    shot40_plane30_mycamera
    ...)" );
}

FText
FNamingConventionPlaneCustomization::GetExplanationText() const
{
    return LOCTEXT( "plane-pattern-info",
R"({plane-index} : an incremental index
{camera-name} : the name of the shot camera
{shot-name} : the name of the shot)" );
}

void
FNamingConventionPlaneCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FNamingConventionPlaneCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionPlane, Pattern ) )
        {
            mPatternHandle = handle;

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( GetTooltipText() )
                .AdvancedExplanation( GetExplanationText() )
                .ValidPatterns( { TEXT( "{plane-index}" ), TEXT( "{camera-name}" ), TEXT( "{shot-name}" ) } )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "{plane-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

//---
//---
//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionCameraCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionCameraCustomization() );
}

FText
FNamingConventionCameraCustomization::GetTooltipText() const
{
    return LOCTEXT( "camera-pattern-info-label",
R"(Some examples:

- camera_{camera-index} ->
    camera_10
    camera_20
    camera_30
    ...
- {shot-name}_mycamera ->
    shot40_mycamera
    shot40_mycamera
    shot40_mycamera
    ...)" );
}

FText
FNamingConventionCameraCustomization::GetExplanationText() const
{
    return LOCTEXT( "camera-pattern-info",
R"({camera-index} : an incremental index
{shot-name} : the name of the shot
(both keys are not intended to be used at the same time))" );
}

void
FNamingConventionCameraCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FNamingConventionCameraCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionCamera, Pattern ) )
        {
            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( GetTooltipText() );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( GetTooltipText() )
                .AdvancedExplanation( GetExplanationText() )
                .ValidPatterns( { TEXT( "{camera-index}" ), TEXT( "{shot-name}" ) } )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "{camera-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

//---
//---
//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionShotCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionShotCustomization() );
}

FText
FNamingConventionShotCustomization::GetTooltipText() const
{
    return LOCTEXT( "shot-pattern-info-label",
R"(Some examples:

- shot_{shot-index} ->
    shot_10
    shot_20
    shot_30
    ...
- {studio-accronym}_shot_{shot-index}_{initials} ->
    MS_shot_0010_xy
    MS_shot_0020_xy
    MS_shot_0030_xy
    ...)" );
}

FText
FNamingConventionShotCustomization::GetExplanationText() const
{
    return LOCTEXT( "shot-pattern-info",
R"({shot-index} : an incremental index
{take-index} : an incremental index for take (not used)
{studio-name} : the full studio name
{studio-accronym} : the studio name accronym
{production-name} : the full production title
{production-accronym} : the production title accronym
{initials} : some initials)" );
}

void
FNamingConventionShotCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FNamingConventionShotCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionShot, Pattern ) )
        {
            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( GetTooltipText() );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( GetTooltipText() )
                .AdvancedExplanation( GetExplanationText() )
                .ValidPatterns( { TEXT( "{shot-index}" ), TEXT( "{take-index}" ), TEXT( "{studio-name}" ), TEXT( "{studio-accronym}" ), TEXT( "{production-name}" ), TEXT( "{production-accronym}" ), TEXT( "{initials}" ) } )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "{shot-index}" ) ) || pattern.ToString().Contains( TEXT( "{take-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

//---
//---
//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionBoardCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionBoardCustomization() );
}

FText
FNamingConventionBoardCustomization::GetTooltipText() const
{
    return LOCTEXT( "board-pattern-info-label",
R"(Some examples:

- board_{board-index} ->
    board_10
    board_20
    board_30
    ...
- {studio-accronym}_board_{board-index}_{initials} ->
    MS_board_0010_xy
    MS_board_0020_xy
    MS_board_0030_xy
    ...)" );
}

FText
FNamingConventionBoardCustomization::GetExplanationText() const
{
    return LOCTEXT( "board-pattern-info",
R"({board-index} : an incremental index
{studio-name} : the full studio name
{studio-accronym} : the studio name accronym
{production-name} : the full production title
{production-accronym} : the production title accronym
{initials} : some initials)" );
}

void
FNamingConventionBoardCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FNamingConventionBoardCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionBoard, Pattern ) )
        {
            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( GetTooltipText() );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( GetTooltipText() )
                .AdvancedExplanation( GetExplanationText() )
                .ValidPatterns( { TEXT( "{board-index}" ), TEXT( "{studio-name}" ), TEXT( "{studio-accronym}" ), TEXT( "{production-name}" ), TEXT( "{production-accronym}" ), TEXT( "{initials}" ) } )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "{board-index}" ) ) || pattern.ToString().Contains( TEXT( "{take-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}
