// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Brush/SOdysseyBrushSelector.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushSelector"


/////////////////////////////////////////////////////
// SOdysseyBrushSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyBrushSelector::~SOdysseyBrushSelector()
{
}


void SOdysseyBrushSelector::Construct(const FArguments& InArgs)
{
    OnBrushChanged = InArgs._OnBrushChanged;

    asset_thumbnail_pool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    ChildSlot
    [
        SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Padding( 2 )
            [
                SNew(SObjectPropertyEntryBox)
                    .AllowedClass(          UOdysseyBrush::StaticClass() )
                    .ObjectPath(            this, &SOdysseyBrushSelector::ObjectPath )
                    .ThumbnailPool(         asset_thumbnail_pool )
                    .OnObjectChanged(       this, &SOdysseyBrushSelector::OnObjectChanged )
                    .AllowClear(            true )
                    .DisplayUseSelected(    true )
                    .DisplayBrowse(         true )
                    .EnableContentPicker(   true )
                    .DisplayCompactSize(    true )
                    .DisplayThumbnail(      true )
                    .ThumbnailSizeOverride( FIntPoint( 30, 30 ) )
            ]
    ];
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks
FString SOdysseyBrushSelector::ObjectPath() const
{
    if( !current_brush )
        return FString();

    return current_brush->GetPathName();
}


void SOdysseyBrushSelector::OnObjectChanged( const FAssetData& AssetData )
{
    if( AssetData.IsValid() )
        current_brush = CastChecked< UOdysseyBrush >( AssetData.GetAsset() );

    OnBrushChanged.ExecuteIfBound( current_brush );
}

void
SOdysseyBrushSelector::SelectBrush(UOdysseyBrush* iBrush)
{
    current_brush = iBrush;
    OnBrushChanged.ExecuteIfBound(current_brush);
}


#undef LOCTEXT_NAMESPACE
