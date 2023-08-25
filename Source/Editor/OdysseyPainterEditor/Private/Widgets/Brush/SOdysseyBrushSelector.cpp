// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Brush/SOdysseyBrushSelector.h"

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
    current_brush = InArgs._Brush;
    OnBrushChanged = InArgs._OnBrushChanged;

    asset_thumbnail_pool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    ChildSlot
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
            .AllowCreate(           true )
            .ThumbnailSizeOverride( FIntPoint( 30, 30 ) )
    ];
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks
FString SOdysseyBrushSelector::ObjectPath() const
{
    if( !current_brush.Get() )
        return FString();

    return current_brush.Get()->GetPathName();
}


void SOdysseyBrushSelector::OnObjectChanged( const FAssetData& AssetData )
{
    UOdysseyBrush* brush = nullptr;
    if( AssetData.IsValid() )
        brush = CastChecked< UOdysseyBrush >( AssetData.GetAsset() );

    OnBrushChanged.ExecuteIfBound( brush );
}


#undef LOCTEXT_NAMESPACE
