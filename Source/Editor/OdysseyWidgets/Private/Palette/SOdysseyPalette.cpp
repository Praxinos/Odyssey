// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

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
        /*
                SNew( SVerticalBox )
                    +SVerticalBox::Slot()
                    .Padding(5)
                    .AutoHeight()
                    [
                        SNew(SButton)
                        .Text(FText::FromString("Add new palette"))
                        .OnClicked(this, &SOdysseyPalette::HandleMeshColorBlockMouseButtonDown)
                    ]
                    + SVerticalBox::Slot()
                    .Padding(5)
                    .AutoHeight()
                    .Expose(mColorPaletteSlot)
                    [
                        SNullWidget::NullWidget
                        //mColorPalette->GetColorPalette()->CreateWidget().ToSharedRef()
                    ]
                    */
    ];

    if( mColorPalette->GetColorPalette() )
        mColorPaletteSlot->AttachWidget( CreateColorPaletteWidget());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

FOdysseyPalette* SOdysseyPalette::GetColorPalette()
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
    return
    
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(5)
        .AutoHeight()
        [
            SNew(SButton)
            .Text(FText::FromString("Add new palette"))
            .OnClicked(this, &SOdysseyPalette::HandleMeshColorBlockMouseButtonDown)
        ]
        + SVerticalBox::Slot()
        .Padding(5)
        .AutoHeight()
        [
            mColorPalette->GetColorPalette()->CreateWidget().ToSharedRef()
        ];
}

#undef LOCTEXT_NAMESPACE
