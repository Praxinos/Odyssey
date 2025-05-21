// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/NamingConventionSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "Settings/NamingConventionSettings.h"
#include "SPatternTextBox.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettingsCustomization"

//---

//static
TSharedRef<IPropertyTypeCustomization>
FNamingConventionAnimationCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionAnimationCustomization() );
}

FText
FNamingConventionAnimationCustomization::GetTooltipText() const
{
    return FText::Format( LOCTEXT( "animation-pattern-tooltip", "Some examples:\n\n- animation_{0} ->\n    animation_10\n    animation_20\n    animation_30\n    ...\n\n(Click on a keyword to Copy it)")
        , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionAnimationPatternKeyword::AnimationIndex ).mKeywordWithBraces )
    );
}

void
FNamingConventionAnimationCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FNamingConventionAnimationCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FNamingConventionAnimation, Pattern ) )
        {
            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( GetTooltipText() );

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            for( auto keyword_list : mSettings->mPatternKeywordLists.mKeywordLists )
            {
                for( auto keyword : keyword_list->GetKeywordList() )
                {
                    keywords.Add( keyword.mKeywordWithBraces );
                    keyword_labels.Add( FText::FromString( keyword.mKeywordWithBraces ) );
                    keyword_helps.Add( keyword.mHelp );
                }
            }

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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywordLists, &FPatternKeywordLists::IsValidPattern )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [patternHandle = mPatternHandle]() -> bool
            {
                FText pattern;
                patternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FNamingConventionAnimation*
FNamingConventionAnimationCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FNamingConventionAnimation*> settings;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( settings ) );

    if( settings.Num() == 1 )
        return settings[0];

    return nullptr;
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
    return FText::Format( LOCTEXT( "camera-pattern-tooltip", "Some examples:\n\n- camera_{0} ->\n    camera_10\n    camera_20\n    camera_30\n    ...\n\n(Click on a keyword to Copy it)" )
                                   , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCameraPatternKeyword::CameraIndex ).mKeywordWithBraces )
    );
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
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

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
            for( auto keyword_list : mSettings->mPatternKeywordLists.mKeywordLists )
            {
                for( auto keyword : keyword_list->GetKeywordList() )
                {
                    keywords.Add( keyword.mKeywordWithBraces );
                    keyword_labels.Add( FText::FromString( keyword.mKeywordWithBraces ) );
                    keyword_helps.Add( keyword.mHelp );
                }
            }

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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywordLists, &FPatternKeywordLists::IsValidPattern )
                //.MoreExplanation( LOCTEXT( "camera-pattern-info", "(both keys are not intended to be used at the same time)" ) )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [patternHandle = mPatternHandle]() -> bool
            {
                FText pattern;
                patternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FNamingConventionCamera*
FNamingConventionCameraCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FNamingConventionCamera*> settings;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( settings ) );

    if( settings.Num() == 1 )
        return settings[0];

    return nullptr;
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
    return FText::Format( LOCTEXT( "shot-pattern-tooltip", "Some examples:\n\n- shot_{0} ->\n    shot_10\n    shot_20\n    shot_30\n    ...\n- {3}_shot_{0}_{11} ->\n    MS_shot_0010_xy\n    MS_shot_0020_xy\n    MS_shot_0030_xy\n    ...\n\n(Click on a keyword to Copy it)" )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::ShotIndex ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::TakeIndex ).mKeywordWithBraces )

                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::StudioName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::StudioAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::LicenseName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::LicenseAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::ProductionName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::ProductionAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Season ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Episode ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Part ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Initials ).mKeywordWithBraces )
    );
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
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

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
            for( auto keyword_list : mSettings->mPatternKeywordLists.mKeywordLists )
            {
                for( auto keyword : keyword_list->GetKeywordList() )
                {
                    keywords.Add( keyword.mKeywordWithBraces );
                    keyword_labels.Add( FText::FromString( keyword.mKeywordWithBraces ) );
                    keyword_helps.Add( keyword.mHelp );
                }
            }

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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywordLists, &FPatternKeywordLists::IsValidPattern )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [patternHandle = mPatternHandle]() -> bool
            {
                FText pattern;
                patternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FNamingConventionShot*
FNamingConventionShotCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FNamingConventionShot*> settings;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( settings ) );

    if( settings.Num() == 1 )
        return settings[0];

    return nullptr;
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
    return FText::Format( LOCTEXT( "board-pattern-tooltip", "Some examples:\n\n- board_{0} ->\n    board_10\n    board_20\n    board_30\n    ...\n- {2}_board_{0}_{10} ->\n    MS_board_0010_xy\n    MS_board_0020_xy\n    MS_board_0030_xy\n    ...\n\n(Click on a keyword to Copy it)" )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionBoardPatternKeyword::BoardIndex ).mKeywordWithBraces )

                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::StudioName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::StudioAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::LicenseName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::LicenseAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::ProductionName ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::ProductionAcronym ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Season ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Episode ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Part ).mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywordLists.GetKeyword( ENamingConventionCommonPatternKeyword::Initials ).mKeywordWithBraces )
    );
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
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

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
            for( auto keyword_list : mSettings->mPatternKeywordLists.mKeywordLists )
            {
                for( auto keyword : keyword_list->GetKeywordList() )
                {
                    keywords.Add( keyword.mKeywordWithBraces );
                    keyword_labels.Add( FText::FromString( keyword.mKeywordWithBraces ) );
                    keyword_helps.Add( keyword.mHelp );
                }
            }

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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywordLists, &FPatternKeywordLists::IsValidPattern )
            ];
        }
        else
        {
            auto IsIndexPropertyEnabled = [patternHandle = mPatternHandle]() -> bool
            {
                FText pattern;
                patternHandle->GetValueAsFormattedText( pattern );

                return pattern.ToString().Contains( TEXT( "-index}" ) );
            };

            ioChildBuilder.AddProperty( handle.ToSharedRef() )
                .IsEnabled( MakeAttributeLambda( IsIndexPropertyEnabled ) ); // For the moment, every other properties (except Pattern) concern the index key
        }
    }
}

FNamingConventionBoard*
FNamingConventionBoardCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FNamingConventionBoard*> settings;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( settings ) );

    if( settings.Num() == 1 )
        return settings[0];

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
