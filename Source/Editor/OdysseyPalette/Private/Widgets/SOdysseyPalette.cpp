// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"
#include "SOdysseyPaletteAddEntryButton.h"
#include "SOdysseyPaletteEntryRow.h"
#include "PropertyCustomizationHelpers.h"
#include "SOdysseyPaletteFolderRow.h"
#include "SOdysseyPaletteColorRow.h"

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

    if( mColorPalette->GetPalette() )
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
        mColorPalette->SetPalette( CastChecked< UOdysseyPalette >(AssetData.GetAsset()));
    else
        mColorPalette->SetPalette(nullptr);

    mColorPaletteSlot->DetachWidget();

    if(mColorPalette->GetPalette())
        mColorPaletteSlot->AttachWidget(CreateColorPaletteWidget());
}


FString SOdysseyPalette::ObjectPath() const
{
    if (!mColorPalette)
        return FString();

    return mColorPalette->GetPalette()->GetPathName();
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
            .ToolTipText(LOCTEXT("OdysseyPaletteEntryIsActivatedButtonToolTip", "Toggle Entry Activation"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
            ]
        + SHeaderRow::Column("Color")
            .ToolTipText(LOCTEXT("OdysseyPaletteEntryColorToolTip", "Entry Color"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            //.DefaultTooltip(FText::FromName(GetColumnID()))
            //.HeaderContentPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
            [
                SNew(SColorBlock)
                .Color( FLinearColor::Black )
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
            .Palette(mColorPalette->GetPalette())
        ]
        + SVerticalBox::Slot()
        .Padding(5)
        .AutoHeight()
        [
            SAssignNew(mPaletteTreeView, SOdysseyPaletteTreeView)
            //.TreeItemsSource(&mColorPalette->GetColorPalette()->mPaletteEntries)
            .Palette(mColorPalette->GetPalette())
            .OnGenerateRow(this, &SOdysseyPalette::OnGenerateRow)
        ];
}

//TODO: MAKE OnHierarchyChanged instead
void SOdysseyPalette::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );
    if( mPaletteTreeView.IsValid() )
        mPaletteTreeView->RequestTreeRefresh();
}

TSharedRef<ITableRow> SOdysseyPalette::OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable)
{
    UE_LOG(LogTemp, Display, TEXT("GENERATE ROW"));
    check(iEntry);

    UClass* entryClass = iEntry->GetClass();
    
    if (entryClass == UOdysseyPaletteEntryColor::StaticClass())
    {
        return SNew(SOdysseyPaletteColorRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntryColor>(iEntry));
    }
    else if (entryClass == UOdysseyPaletteEntryFolder::StaticClass())
    {
        return SNew(SOdysseyPaletteFolderRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntryFolder>(iEntry));
    }

    return SNew(SOdysseyPaletteEntryRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntry>(iEntry)); //Default widget
}

#undef LOCTEXT_NAMESPACE
