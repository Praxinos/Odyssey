// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SOdysseyPaletteAddEntryButton.h"
#include "SOdysseyPaletteEntryRow.h"

#define LOCTEXT_NAMESPACE "OdysseyPalette"


/////////////////////////////////////////////////////
// SOdysseyPalette
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyPalette::~SOdysseyPalette()
{
    mColorPalette.Reset();
}


void SOdysseyPalette::Construct(const FArguments& InArgs)
{
    mColorPalette = MakeShareable(new FOdysseyPalette());
    mAssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024));

    TArray<const UClass*> allowedClasses;
    allowedClasses.Add(UOdysseyPalette::StaticClass());

    ChildSlot
    [

        SNew(SScrollBox)
        .Orientation(Orient_Vertical)
        .ScrollBarAlwaysVisible(false)
        + SScrollBox::Slot()
        [
            SNew(SObjectPropertyEntryBox)
            .AllowedClass(UOdysseyPalette::StaticClass())
            .ObjectPath(this, &SOdysseyPalette::ObjectPath)
            .ThumbnailPool(mAssetThumbnailPool)
            .OnObjectChanged(this, &SOdysseyPalette::OnObjectChanged)
            .AllowClear(true)
            .DisplayUseSelected(false)
            .DisplayBrowse(false)
            .DisplayThumbnail(false)
            .EnableContentPicker(true)
            .DisplayCompactSize(true)
            .DisplayThumbnail(true)
            .ThumbnailSizeOverride(FIntPoint(32, 32))
        ]
        + SScrollBox::Slot()
        .Expose(mColorPaletteSlot)
        [
            SNullWidget::NullWidget
        ]
    ];

    if( mColorPalette->GetColorPalette() )
        mColorPaletteSlot->AttachWidget( CreateColorPaletteWidget());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

FOdysseyPalette* SOdysseyPalette::GetColorPalette() const
{
    return mColorPalette.Get();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks

void SOdysseyPalette::OnObjectChanged(const FAssetData& AssetData)
{
    if (AssetData.IsValid())
        mColorPalette->SetColorPalette( CastChecked< UOdysseyPalette >(AssetData.GetAsset()));
    else
        mColorPalette->SetColorPalette(nullptr);

    mColorPaletteSlot->DetachWidget();

    if(mColorPalette->GetColorPalette())
        mColorPaletteSlot->AttachWidget(CreateColorPaletteWidget());
}


FString SOdysseyPalette::ObjectPath() const
{
    if (!mColorPalette)
        return FString();

    return mColorPalette->GetColorPalette()->GetPathName();
}

FReply SOdysseyPalette::HandleMeshColorBlockMouseButtonDown()
{
    mColorPaletteSlot->DetachWidget();
    mColorPalette->AddNewPaletteEntry();
    mColorPaletteSlot->AttachWidget( CreateColorPaletteWidget() );
    return FReply::Handled();
}



TSharedRef<SWidget> SOdysseyPalette::CreateColorPaletteWidget()
{
    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .SplitterHandleSize(0.f) //Fixes alignment between header row and actual rows
        + SHeaderRow::Column("IsActivated")
            .ToolTipText(LOCTEXT("OdysseyLayerIsActivatedButtonToolTip", "Toggle Layer Activation"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            //.DefaultTooltip(FText::FromName(GetColumnID()))
            //.HeaderContentPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
            ]
        + SHeaderRow::Column("Header")
            .DefaultLabel(LOCTEXT("", ""))
            .VAlignCell(VAlign_Top)
            .FillWidth(true);

    return
    
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(5)
        .AutoHeight()
        [
            SNew(SOdysseyPaletteAddEntryButton)
            .Palette(mColorPalette->GetColorPalette())
        ]
        + SVerticalBox::Slot()
        .Padding(5)
        .AutoHeight()
        [
            SAssignNew(mPaletteTreeView, STreeView<UOdysseyPaletteEntry*>)
            .TreeItemsSource(&mColorPalette->GetColorPalette()->mPaletteEntries)
            .OnGenerateRow(this, &SOdysseyPalette::OnGenerateRow)
            .OnGetChildren(this, &SOdysseyPalette::OnGetChildren)
            //.OnExpansionChanged(this, &SOdysseyLayerStackTreeView::OnExpansionChanged)
            //.OnSelectionChanged( this, &SOdysseyLayerStackTreeView::OnSelectionChanged )
            //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
            //.OnContextMenuOpening(this, &SOdysseyLayerStackTreeView::OnContextMenuOpening)
            //.SelectionMode(ESelectionMode::Multi)
            .HeaderRow(headerRow)
        ];
}

//TODO: MAKE OnHierarchyChanged instead
void SOdysseyPalette::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );
    if( mPaletteTreeView.IsValid() )
        mPaletteTreeView->RequestTreeRefresh();
}

void
SOdysseyPalette::OnGetChildren(UOdysseyPaletteEntry* iParent, TArray<UOdysseyPaletteEntry*>& oChildren) const
{
    oChildren = mColorPalette->GetColorPalette()->mPaletteEntries;

    UE_LOG(LogTemp, Display, TEXT("oChildren: %d"), oChildren.Num());
    /*if (!iParent)
        return;

    oChildren = iParent->mChildren;*/
}

TSharedRef<ITableRow> SOdysseyPalette::OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable)
{
    UE_LOG(LogTemp, Display, TEXT("GENERATE ROW"));
    check(iEntry);

    UClass* entryClass = iEntry->GetClass();
    
    /*if (entryClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }*/

    return SNew(SOdysseyPaletteEntryRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntry>(iEntry)); //Default widget
}

#undef LOCTEXT_NAMESPACE
