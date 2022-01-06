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

//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionPlaneCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionPlaneCustomization() );
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
            FText tooltip_examples = LOCTEXT( "plane-pattern-info-label", "\
Some examples:\n\
\n\
- plane_{plane-index} ->\n\
    plane_10\n\
    plane_20\n\
    plane_30\n\
    ...\n\
- pl{plane-index}_{camera-name} ->\n\
    pl10_mycamera\n\
    pl20_mycamera\n\
    pl30_mycamera\n\
    ...\n\
- {shot-name}_plane{plane-index}_{camera-name} ->\n\
    shot40_plane10_mycamera\n\
    shot40_plane20_mycamera\n\
    shot40_plane30_mycamera\n\
    ..." );

            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( tooltip_examples );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
                .NameContent()
                [
                    mPatternHandle->CreatePropertyNameWidget()
                ]
                .ValueContent()
                .HAlign( HAlign_Fill )
                [
                    // (Nearly) Same as in D:\Epic Games\UE_4.27\Engine\Source\Editor\PropertyEditor\Private\UserInterface\PropertyEditor\SPropertyEditorText.cpp
                    SNew( SHorizontalBox )
                    + SHorizontalBox::Slot()
                    [
                        SAssignNew( mPatternWidget, SEditableTextBox )
                        .Text( this, &FNamingConventionPlaneCustomization::GetPatternText )
                        .Font( FEditorStyle::GetFontStyle( TEXT( "PropertyWindow.NormalFont" ) ) )
                        .SelectAllTextWhenFocused( true )
                        .ClearKeyboardFocusOnCommit( false )
                        .OnTextCommitted( this, &FNamingConventionPlaneCustomization::OnPatternTextCommited )
                        .OnTextChanged( this, &FNamingConventionPlaneCustomization::OnPatternTextChanged )
                        .SelectAllTextOnCommit( true )
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth( .1f )
                    [
                        SNew( SSpacer )
                    ]
                ];

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
                .ValueContent()
                .HAlign( HAlign_Fill )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "plane-pattern-info", "\
{plane-index} : an incremental index\n\
{camera-name} : the name of the shot camera\n\
{shot-name} : the name of the shot" ) )
                    .ToolTipText( tooltip_examples )
                ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                return GetPatternText().ToString().Contains( TEXT( "{plane-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FText
FNamingConventionPlaneCustomization::GetPatternText() const
{
    FText pattern;
    mPatternHandle->GetValueAsFormattedText( pattern );

    return pattern;
}

void
FNamingConventionPlaneCustomization::OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo )
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
FNamingConventionPlaneCustomization::OnPatternTextChanged( const FText& iNewText )
{
    if( CheckPatternValidity( iNewText.ToString() ) )
    {
        TAttribute<FSlateColor> empty;
        mPatternWidget->SetTextBoxBackgroundColor( empty ); // Remove the attribute to use the "real" background style of the widget
    }
    else
    {
        mPatternWidget->SetTextBoxBackgroundColor( FLinearColor( 1, 0, 0, 0.35f ) );
        //mPatternWidget->SetTextBoxBackgroundColor( FEditorStyle::GetColor( TEXT( "ErrorReporting.BackgroundColor" ) ) );
    }
}

bool
FNamingConventionPlaneCustomization::CheckPatternValidity( const FString& iPattern )
{
    static TArray<FString> valid_patterns{ TEXT( "{plane-index}" ), TEXT( "{camera-name}" ), TEXT( "{shot-name}" ) };

    return ::CheckPatternValidity( iPattern, valid_patterns );
}

//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionCameraCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionCameraCustomization() );
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
            FText tooltip_examples = LOCTEXT( "camera-pattern-info-label", "\
Some examples:\n\
\n\
- camera_{camera-index} ->\n\
    camera_10\n\
    camera_20\n\
    camera_30\n\
    ...\n\
- {shot-name}_mycamera ->\n\
    shot40_mycamera\n\
    shot40_mycamera\n\
    shot40_mycamera\n\
    ..." );

            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( tooltip_examples );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                // (Nearly) Same as in D:\Epic Games\UE_4.27\Engine\Source\Editor\PropertyEditor\Private\UserInterface\PropertyEditor\SPropertyEditorText.cpp
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                [
                    SAssignNew( mPatternWidget, SEditableTextBox )
                    .Text( this, &FNamingConventionCameraCustomization::GetPatternText )
                    .Font( FEditorStyle::GetFontStyle( TEXT( "PropertyWindow.NormalFont" ) ) )
                    .SelectAllTextWhenFocused( true )
                    .ClearKeyboardFocusOnCommit( false )
                    .OnTextCommitted( this, &FNamingConventionCameraCustomization::OnPatternTextCommited )
                    .OnTextChanged( this, &FNamingConventionCameraCustomization::OnPatternTextChanged )
                    .SelectAllTextOnCommit( true )
                ]
                + SHorizontalBox::Slot()
                .FillWidth( .1f )
                [
                    SNew( SSpacer )
                ]
            ];

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
                .ValueContent()
                .HAlign( HAlign_Fill )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "camera-pattern-info", "\
{camera-index} : an incremental index\n\
{shot-name} : the name of the shot\n\
(both keys are not intended to be used at the same time)" ) )
.ToolTipText( tooltip_examples )
                ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                return GetPatternText().ToString().Contains( TEXT( "{camera-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FText
FNamingConventionCameraCustomization::GetPatternText() const
{
    FText pattern;
    mPatternHandle->GetValueAsFormattedText( pattern );

    return pattern;
}

void
FNamingConventionCameraCustomization::OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo )
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
FNamingConventionCameraCustomization::OnPatternTextChanged( const FText& iNewText )
{
    //if( CheckPatternValidity( iNewText.ToString() ) )
    //{
    //    TAttribute<FSlateColor> empty;
    //    mPatternWidget->SetTextBoxBackgroundColor( empty ); // Remove the attribute to use the "real" background style of the widget
    //}
    //else
    //{
    //    mPatternWidget->SetTextBoxBackgroundColor( FLinearColor( 1, 0, 0, 0.35f ) );
    //    //mPatternWidget->SetTextBoxBackgroundColor( FEditorStyle::GetColor( TEXT( "ErrorReporting.BackgroundColor" ) ) );
    //}
}

bool
FNamingConventionCameraCustomization::CheckPatternValidity( const FString& iPattern )
{
    static TArray<FString> valid_patterns{ TEXT( "{camera-index}" ), TEXT( "{shot-name}" ) };

    return ::CheckPatternValidity( iPattern, valid_patterns );
}
