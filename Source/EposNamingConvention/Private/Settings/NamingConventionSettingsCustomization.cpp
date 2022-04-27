// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/NamingConventionSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "Settings/NamingConventionSettings.h"
#include "Settings/SPatternTextBox.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettingsCustomization"

//---

namespace
{
static
TMap<FString, FNamingConventionPatternKeyword>
GetPatternKeywordsMap( TSharedRef<IPropertyHandle> iStructPropertyHandle, TArray<FString>& oValidKeywords, TArray<FText>& oKeywordLabels, TArray<FText>& oKeywordHelps )
{
    oValidKeywords.Empty();
    oKeywordLabels.Empty();
    oKeywordHelps.Empty();

    TSharedPtr<IPropertyHandle> child_handle = iStructPropertyHandle->GetChildHandle( "PatternKeywords" );
    if( child_handle.IsValid() )
    {
        TSharedPtr<IPropertyHandleMap> map_handle = child_handle->AsMap();
        if( map_handle.IsValid() )
        {
            void* MapDataPtr = nullptr;
            if( child_handle->GetValueData( MapDataPtr ) == FPropertyAccess::Success )
            {
                TMap<FString, FNamingConventionPatternKeyword>* map = ( TMap<FString, FNamingConventionPatternKeyword>* )MapDataPtr;
                if( map )
                {
                    for( auto pair : *map )
                    {
                        oValidKeywords.Add( pair.Value.mKeywordWithBraces );
                        oKeywordLabels.Add( FText::FromString( pair.Value.mKeywordWithBraces ) );
                        oKeywordHelps.Add( pair.Value.mHelp );
                    }
                    return *map;
                }
            }
        }
    }

    return TMap<FString, FNamingConventionPatternKeyword>();
}
}

//---
//---
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
    return LOCTEXT( "plane-pattern-tooltip",
R"(Some examples:

- plane_{plane-index} ->
    plane_10
    plane_20
    plane_30
    ...)" );
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

            mPatternHandle->SetToolTipText( GetTooltipText() );

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            GetPatternKeywordsMap( iStructPropertyHandle, keywords, keyword_labels, keyword_helps );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( mPatternHandle->GetToolTipText() )
                .Keywords( keywords )
                .KeywordLabels( keyword_labels )
                .KeywordHelps( keyword_helps )
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionBoard, PatternKeywords ) )
        {
            handle->MarkHiddenByCustomization();
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
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
    return LOCTEXT( "camera-pattern-tooltip",
R"(Some examples:

- camera_{camera-index} ->
    camera_10
    camera_20
    camera_30
    ...)" );
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

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            GetPatternKeywordsMap( iStructPropertyHandle, keywords, keyword_labels, keyword_helps );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( mPatternHandle->GetToolTipText() )
                .Keywords( keywords )
                .KeywordLabels( keyword_labels )
                .KeywordHelps( keyword_helps )
                //.MoreExplanation( LOCTEXT( "camera-pattern-info", "(both keys are not intended to be used at the same time)" ) )
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionBoard, PatternKeywords ) )
        {
            handle->MarkHiddenByCustomization();
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
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
    return LOCTEXT( "shot-pattern-tooltip",
R"(Some examples:

- shot_{shot-index} ->
    shot_10
    shot_20
    shot_30
    ...
- {studio-acronym}_shot_{shot-index}_{initials} ->
    MS_shot_0010_xy
    MS_shot_0020_xy
    MS_shot_0030_xy
    ...)" );
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

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            GetPatternKeywordsMap( iStructPropertyHandle, keywords, keyword_labels, keyword_helps );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( mPatternHandle->GetToolTipText() )
                .Keywords( keywords )
                .KeywordLabels( keyword_labels )
                .KeywordHelps( keyword_helps )
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionBoard, PatternKeywords ) )
        {
            handle->MarkHiddenByCustomization();
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
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
    return LOCTEXT( "board-pattern-tooltip",
R"(Some examples:

- board_{board-index} ->
    board_10
    board_20
    board_30
    ...
- {studio-acronym}_board_{board-index}_{initials} ->
    MS_board_0010_xy
    MS_board_0020_xy
    MS_board_0030_xy
    ...)" );
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

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            GetPatternKeywordsMap( iStructPropertyHandle, keywords, keyword_labels, keyword_helps );

            ioChildBuilder.AddCustomRow( LOCTEXT( "Pattern", "Pattern" ) )
            .NameContent()
            [
                mPatternHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .HAlign( HAlign_Fill )
            [
                SNew( SPatternTextBox, mPatternHandle )
                .ToolTipText( mPatternHandle->GetToolTipText() )
                .Keywords( keywords )
                .KeywordLabels( keyword_labels )
                .KeywordHelps( keyword_helps )
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionBoard, PatternKeywords ) )
        {
            handle->MarkHiddenByCustomization();
        }
        else
        {
            auto IsIndexPropertyEnabled = [=]() -> bool
            {
                FText pattern;
                mPatternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

#undef LOCTEXT_NAMESPACE
