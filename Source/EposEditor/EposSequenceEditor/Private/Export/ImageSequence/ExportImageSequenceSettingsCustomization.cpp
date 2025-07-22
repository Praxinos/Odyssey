// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/ImageSequence/ExportImageSequenceSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "Export/ExportStruct.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Settings/NamingConventionSettings.h"
#include "SPatternTextBox.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceSettingsCustomization"

//---

//static
TSharedRef<IPropertyTypeCustomization>
FExportImageSequenceOptionsCustomization::MakeInstance()
{
    return MakeShareable( new FExportImageSequenceOptionsCustomization() );
}

FText
FExportImageSequenceOptionsCustomization::GetTooltipText() const
{
    return FText::Format( LOCTEXT( "export-image-sequence-file-pattern-tooltip", "Each keywords will be replaced by its corresponding value.\n\n- {0}-{1}-seq{2}-sh{3}---{4}\n    1-MyStoryboard-seq10-sh10---0000\n    2-MyStoryboard-seq10-sh10---0100\n    3-MyStoryboard-seq10-sh20---0175\n    4-MyStoryboard-seq10-sh20---0200\n    5-MyStoryboard-seq20-sh30---0250\n    6-MyStoryboard-seq20-sh40---0400\n    ...\n- {1}-pf{4}\n    MyStoryboard-pf0000\n    MyStoryboard-pf0100\n    MyStoryboard-pf0175\n    MyStoryboard-pf0200\n    MyStoryboard-pf0250\n    MyStoryboard-pf0400\n    ...\n\nThe extension will be automatically set according to the file format.\n\n(Click on a keyword to Copy it)" )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::PanelIndex ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::StoryboardName ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( ENamingConventionBoardPatternKeyword::BoardIndex ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::ShotIndex ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::PanelFrame ).mKeywordWithBraces )
);
}

void
FExportImageSequenceOptionsCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FExportImageSequenceOptionsCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    mOptions = GetEditStruct( iStructPropertyHandle );
    check( mOptions );

    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    TArray<TSharedPtr<IPropertyHandle>> advanced_handles;

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty()->HasAnyPropertyFlags( CPF_AdvancedDisplay ) )
        {
            advanced_handles.Add( handle );
            continue;
        }

        //---

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FExportImageSequenceOptions, Pattern ) )
        {
            mPatternHandle = handle;

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            for( auto keyword_list : mOptions->mPatternKeywordLists.mKeywordLists )
            {
                for( auto keyword : keyword_list->GetKeywordList() )
                {
                    keywords.Add( keyword.mKeywordWithBraces );
                    keyword_labels.Add( FText::FromString( keyword.mKeywordWithBraces ) );
                    keyword_helps.Add( keyword.mHelp );
                }
            }

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
                .ToolTipText( mPatternHandle->GetToolTipText() )
                .Keywords( keywords )
                .KeywordLabels( keyword_labels )
                .KeywordHelps( keyword_helps )
                .OnVerifyPattern_Raw( &mOptions->mPatternKeywordLists, &FPatternKeywordLists::IsValidPattern )
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FExportImageSequenceOptions, ImageSize ) )
        {
            mImageSizeHandle = handle;

            ioChildBuilder.AddProperty( mImageSizeHandle.ToSharedRef() );

            mCacheImageSize = FIntPoint::NoneValue;
            OnImageSizeChanged();

            mImageSizeHandle->SetOnChildPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FExportImageSequenceOptionsCustomization::OnImageSizeChanged ) );
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FExportImageSequenceOptions, AspectRatio ) )
        {
            mAspectRatioHandle = handle;

            //mAspectRatioHandle->MarkHiddenByCustomization();
        }
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }

    // Check D:\Epic Games\UE_5.0\Engine\Source\Editor\DetailCustomizations\Private\MeshProxySettingsCustomizations.cpp to see details of adding advanced row
    // This loop is not enough if advanced properties has customization (as customization is done above)
    // But the advanced group MUST be after all simple properties to be the last in the list
    IDetailGroup& advanced_group = ioChildBuilder.AddGroup( NAME_None, LOCTEXT( "advanced", "Advanced" ) );
    for( auto handle : advanced_handles )
    {
        advanced_group.AddPropertyRow( handle.ToSharedRef() );
    }
}

FExportImageSequenceOptions*
FExportImageSequenceOptionsCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FExportImageSequenceOptions*> options;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( options ) );

    if( options.Num() == 1 )
        return options[0];

    return nullptr;
}

void
FExportImageSequenceOptionsCustomization::OnImageSizeChanged()
{
    void* size_ptr = nullptr;
    if( mImageSizeHandle->GetValueData( size_ptr ) != FPropertyAccess::Success )
        return;

    FIntPoint* size = ( FIntPoint* )size_ptr;

    //---

    if( mCacheImageSize == FIntPoint::NoneValue )
    {
        mCacheImageSize = *size;
        return;
    }

    //---

    float aspect_ratio;
    mAspectRatioHandle->GetValue( aspect_ratio );

    if( mCacheImageSize.X == size->X )
    {
        size->X = size->Y * aspect_ratio;
    }
    else // same Y
    {
        size->Y = size->X / aspect_ratio;
    }

    size->X = FMath::Clamp( size->X, 128, 10000 );
    size->Y = FMath::Clamp( size->Y, 128, 10000 );

    mCacheImageSize = *size;
    mImageSizeHandle->NotifyFinishedChangingProperties();

    //UE_LOG( LogTemp, Warning, TEXT( "image size: %dx%d" ), size->X, size->Y );
}

#undef LOCTEXT_NAMESPACE
