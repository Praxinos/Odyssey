// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
FNamingConventionPlaneCustomization::MakeInstance()
{
    return MakeShareable( new FNamingConventionPlaneCustomization() );
}

FText
FNamingConventionPlaneCustomization::GetTooltipText() const
{
    return FText::Format( LOCTEXT( "plane-pattern-tooltip",
R"(Some examples:

- plane_{0} ->
    plane_10
    plane_20
    plane_30
    ...)" )
                                   , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionPlanePatternKeyword::PlaneIndex].mKeywordWithBraces )
    );
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
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

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
            for( auto pair : mSettings->mPatternKeywords.mKeywordList )
            {
                keywords.Add( pair.Value.mKeywordWithBraces );
                keyword_labels.Add( FText::FromString( pair.Value.mKeywordWithBraces ) );
                keyword_helps.Add( pair.Value.mHelp );
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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywords, &TPatternKeywordList<ENamingConventionPlanePatternKeyword>::IsValidPattern )
            ];
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

FNamingConventionPlane*
FNamingConventionPlaneCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FNamingConventionPlane*> settings;

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
    return FText::Format( LOCTEXT( "camera-pattern-tooltip",
R"(Some examples:

- camera_{0} ->
    camera_10
    camera_20
    camera_30
    ...)" )
                                   , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionCameraPatternKeyword::CameraIndex].mKeywordWithBraces )
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
            for( auto pair : mSettings->mPatternKeywords.mKeywordList )
            {
                keywords.Add( pair.Value.mKeywordWithBraces );
                keyword_labels.Add( FText::FromString( pair.Value.mKeywordWithBraces ) );
                keyword_helps.Add( pair.Value.mHelp );
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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywords, &TPatternKeywordList<ENamingConventionCameraPatternKeyword>::IsValidPattern )
                //.MoreExplanation( LOCTEXT( "camera-pattern-info", "(both keys are not intended to be used at the same time)" ) )
            ];
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
    return FText::Format( LOCTEXT( "shot-pattern-tooltip",
R"(Some examples:

- shot_{0} ->
    shot_10
    shot_20
    shot_30
    ...
- {3}_shot_{0}_{11} ->
    MS_shot_0010_xy
    MS_shot_0020_xy
    MS_shot_0030_xy
    ...)" )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::ShotIndex].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::TakeIndex].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::StudioName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::StudioAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::LicenseName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::LicenseAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::ProductionName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::ProductionAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::Season].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::Episode].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::Part].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::Initials].mKeywordWithBraces )
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
            for( auto pair : mSettings->mPatternKeywords.mKeywordList )
            {
                keywords.Add( pair.Value.mKeywordWithBraces );
                keyword_labels.Add( FText::FromString( pair.Value.mKeywordWithBraces ) );
                keyword_helps.Add( pair.Value.mHelp );
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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywords, &TPatternKeywordList<ENamingConventionShotPatternKeyword>::IsValidPattern )
            ];
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
    return FText::Format( LOCTEXT( "board-pattern-tooltip",
R"(Some examples:

- board_{0} ->
    board_10
    board_20
    board_30
    ...
- {2}_board_{0}_{10} ->
    MS_board_0010_xy
    MS_board_0020_xy
    MS_board_0030_xy
    ...)" )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::BoardIndex].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::StudioName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::StudioAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::LicenseName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::LicenseAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::ProductionName].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::ProductionAcronym].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::Season].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::Episode].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::Part].mKeywordWithBraces )
                          , FText::FromString( mSettings->mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::Initials].mKeywordWithBraces )
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
            for( auto pair : mSettings->mPatternKeywords.mKeywordList )
            {
                keywords.Add( pair.Value.mKeywordWithBraces );
                keyword_labels.Add( FText::FromString( pair.Value.mKeywordWithBraces ) );
                keyword_helps.Add( pair.Value.mHelp );
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
                .OnVerifyPattern_Raw( &mSettings->mPatternKeywords, &TPatternKeywordList<ENamingConventionBoardPatternKeyword>::IsValidPattern )
            ];
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
