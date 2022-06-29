// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/EposSequenceEditorSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "SPatternTextBox.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettingsCustomization"

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
The separator will be used to join each line of the pattern field.

(Click on a keyword to Copy it))" );
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
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

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
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

FInfoBarSettings*
FInfoBarCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FInfoBarSettings*> options;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( options ) );

    if( options.Num() == 1 )
        return options[0];

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
