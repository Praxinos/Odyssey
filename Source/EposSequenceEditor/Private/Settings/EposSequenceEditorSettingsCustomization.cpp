// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/EposSequenceEditorSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/SPatternTextBox.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettingsCustomization"

//---

namespace
{
static
TMap<EInfoBarPatternKeyword, FInfoBarPatternKeyword>
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
                TMap<EInfoBarPatternKeyword, FInfoBarPatternKeyword>* map = ( TMap<EInfoBarPatternKeyword, FInfoBarPatternKeyword>* )MapDataPtr;
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

    return TMap<EInfoBarPatternKeyword, FInfoBarPatternKeyword>();
}
}

//---
//---
//---

//static
TSharedRef<IPropertyTypeCustomization>
FInfoBarCustomization::MakeInstance()
{
    return MakeShareable( new FInfoBarCustomization() );
}

FText
FInfoBarCustomization::GetTooltipText() const
{
    return LOCTEXT( "infobar-pattern-tooltip",
R"(Each keywords will be replaced by its corresponding value.
The separator will be used to join each line of the pattern field)" );
}

void
FInfoBarCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FInfoBarCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FInfoBarSettings, Pattern ) )
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
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FInfoBarSettings, PatternKeywords ) )
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
