// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImportImageSequenceSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "SPatternTextBox.h"
#include "Import/ImportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ImportImageSequenceSettingsCustomization"

//---

//static
TSharedRef<IPropertyTypeCustomization>
FImportImageSequenceOptionsCustomization::MakeInstance()
{
    return MakeShareable( new FImportImageSequenceOptionsCustomization() );
}

FText
FImportImageSequenceOptionsCustomization::GetTooltipText() const
{
    return FText::Format( LOCTEXT( "import-image-sequence-file-pattern-tooltip", "Each keywords will be replaced by its corresponding value.\n\nFor files like:\n- wonder-1-000.png\n- wonder-1-001.png\n- wonder-1-002.png\n- wonder-2-000.png\n- wonder-2-001.png\n- wonder-3-000.png\n- wonder-3-001.png\n- wonder-3-002.png\n- wonder-3-003.png\nIf the first number corresponds to the shot, and the second to each panel,\nthe pattern will look like:\n- wonder-{1}-{2}.png\n\nFor files like:\n- ful-bA-s01-00.png\n- ful-bA-s01-01.png\n- ful-bA-s01-02.png\n- ful-bA-s02-00.png\n- ful-bA-s02-01.png\n- ful-bB-s03-00.png\n- ful-bB-s03-01.png\n- ful-bB-s03-02.png\n- ful-bB-s03-03.png\nIf the first character (A or B) corresponds to the board, the next number to the shot, and the last to each panel,\nthe pattern will look like:\n- ful-b{0}-s{1}-{2}.png\n\n(Click on a keyword to Copy it)" )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EImportImageSequencePatternKeyword::BoardId ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EImportImageSequencePatternKeyword::ShotId ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EImportImageSequencePatternKeyword::PanelId ).mKeywordWithBraces )
                          , FText::FromString( mOptions->mPatternKeywordLists.GetKeyword( EImportImageSequencePatternKeyword::Duration ).mKeywordWithBraces )
    );
}

void
FImportImageSequenceOptionsCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FImportImageSequenceOptionsCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    mOptions = GetEditStruct( iStructPropertyHandle );
    check( mOptions );

    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FImportImageSequenceOptions, FilePattern ) )
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
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

FImportImageSequenceOptions*
FImportImageSequenceOptionsCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FImportImageSequenceOptions*> options;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( options ) );

    if( options.Num() == 1 )
        return options[0];

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
