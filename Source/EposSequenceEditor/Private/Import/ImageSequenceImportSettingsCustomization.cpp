// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImageSequenceImportSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "Settings/SPatternTextBox.h"
#include "Import/ImageSequenceImportSettings.h"

#define LOCTEXT_NAMESPACE "ImageSequenceImportSettingsCustomization"

//---

namespace
{
static
TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>
GetPatternKeywordsMap2( TSharedRef<IPropertyHandle> iStructPropertyHandle, TArray<FString>& oValidKeywords, TArray<FText>& oKeywordLabels, TArray<FText>& oKeywordHelps )
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
                TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>* map = ( TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>* )MapDataPtr;
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

    return TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>();
}
}

//---
//---
//---

//static
TSharedRef<IPropertyTypeCustomization>
FImageSequenceImportSettingsCustomization::MakeInstance()
{
    return MakeShareable( new FImageSequenceImportSettingsCustomization() );
}

FText
FImageSequenceImportSettingsCustomization::GetTooltipText() const
{
    return LOCTEXT( "import-image-sequence-file-pattern-tooltip",
R"(Each keywords will be replaced by its corresponding value.

For files like:
- wonder-1-000.png
- wonder-1-001.png
- wonder-1-002.png
- wonder-2-000.png
- wonder-2-001.png
- wonder-3-000.png
- wonder-3-001.png
- wonder-3-002.png
- wonder-3-003.png
If the first number corresponds to the shot, and the second to each frame,
the pattern will look like:
- wonder-{shot}-{frame}.png

For files like:
- ful-bA-s01-00.png
- ful-bA-s01-01.png
- ful-bA-s01-02.png
- ful-bA-s02-00.png
- ful-bA-s02-01.png
- ful-bB-s03-00.png
- ful-bB-s03-01.png
- ful-bB-s03-02.png
- ful-bB-s03-03.png
If the first character (A or B) corresponds to the board, the next number to the shot, and the last to each frame,
the pattern will look like:
- ful-b{board}-s{shot}-{frame}.png
)" );
}

void
FImageSequenceImportSettingsCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FImageSequenceImportSettingsCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FImageSequenceImportSettings, FilePattern ) )
        {
            mPatternHandle = handle;

            mPatternHandle->SetToolTipText( GetTooltipText() );

            //ioChildBuilder.AddProperty( handle.ToSharedRef() );

            TArray<FString> keywords;
            TArray<FText> keyword_labels;
            TArray<FText> keyword_helps;
            GetPatternKeywordsMap2( iStructPropertyHandle, keywords, keyword_labels, keyword_helps );

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
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FImageSequenceImportSettings, PatternKeywords ) )
        {
            handle->MarkHiddenByCustomization();
        }
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

#undef LOCTEXT_NAMESPACE
