// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Mesh/SOdysseyMeshSelector.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

#define LOCTEXT_NAMESPACE "OdysseyMeshSelector"


/////////////////////////////////////////////////////
// SOdysseyMeshSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyMeshSelector::~SOdysseyMeshSelector()
{
    MeshSelector.Reset();
    AssetThumbnailPool.Reset();
}


void SOdysseyMeshSelector::Construct(const FArguments& InArgs)
{
    MeshSelector = MakeShareable(new FOdysseyMeshSelector());
    OnMeshChanged = InArgs._OnMeshChanged;

    AssetThumbnailPool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    ChildSlot
    [
        SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			.ScrollBarAlwaysVisible(false)
			+SScrollBox::Slot()
            [
                SNew( SVerticalBox )
                    + SVerticalBox::Slot()
                    .Padding( 2 )
                    .AutoHeight()
                    [
                        SNew( SHorizontalBox )
                         + SHorizontalBox::Slot()
                         .HAlign(HAlign_Left)
                         .Padding( FMargin( 0.f, 0.f, 4.f, 0.f ) )
                         .AutoWidth()
                         [
                            SNew( STextBlock )
                            .Text( FText::FromString("Mesh Color"))
                         ]
                         + SHorizontalBox::Slot()
                         .HAlign(HAlign_Fill)
                         [
                            SAssignNew( ColorBlockWidget, SColorBlock )
                            .Color( this, &SOdysseyMeshSelector::GetMeshColor )
                            .OnMouseButtonDown(this, &SOdysseyMeshSelector::HandleMeshColorBlockMouseButtonDown )
                         ]

                    ]
                    + SVerticalBox::Slot()
                    .Padding( 2 )
                    .AutoHeight()
                    [
                        SNew(SObjectPropertyEntryBox)
                            .AllowedClass(          UStaticMesh::StaticClass() )
                            .ObjectPath(            this, &SOdysseyMeshSelector::ObjectPath )
                            .ThumbnailPool(         AssetThumbnailPool )
                            .OnObjectChanged(       this, &SOdysseyMeshSelector::OnObjectChanged )
                            .AllowClear(            true )
                            .DisplayUseSelected(    true )
                            .DisplayBrowse(         true )
                            .EnableContentPicker(   true )
                            .DisplayCompactSize(    true )
                            .DisplayThumbnail(      true )
                            .ThumbnailSizeOverride( FIntPoint( 30, 30 ) )
                    ]
                    + SVerticalBox::Slot()
                    .Padding( 5 )
                    .AutoHeight()
                    .Expose(LODSelectionMenu)
                    [
                        SNullWidget::NullWidget
                    ]
                    + SVerticalBox::Slot()
                    .Padding( 2 )
                    .AutoHeight()
                    .Expose(UVSelectionMenu)
                    [
                        SNullWidget::NullWidget
                    ]
             ]
    ];
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

FOdysseyMeshSelector* SOdysseyMeshSelector::GetMeshSelectorPtr()
{
    return MeshSelector.Get();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal Widget Creation

TSharedRef<SWidget> SOdysseyMeshSelector::CreateLODMenuWidget()
{
    if( MeshSelector->GetCurrentMesh() )
    {
        FMenuBuilder MenuBuilder(true, NULL);

        MenuBuilder.BeginSection("Select LOD Level");

        FUIAction LODAction;
        LODAction.ExecuteAction.BindSP(this, &SOdysseyMeshSelector::OnLODChanged, -1);
        LODAction.GetActionCheckState.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::GetLODCheckState, -1);

        MenuBuilder.AddMenuEntry(
            LOCTEXT("SelecLODToggle", "None"),
            LOCTEXT("SelectLODToggle_Tooltip", "Toggles selection of the static mesh's LOD."),
            FSlateIcon(),
            LODAction,
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );


        int32 MaxLODLevel = MeshSelector->GetMaxLOD();
        if( MaxLODLevel > 0)
            MenuBuilder.AddMenuSeparator();

        for(int LODLevel = 0; LODLevel < MaxLODLevel; ++LODLevel)
        {
            FUIAction MenuAction;
            MenuAction.ExecuteAction.BindSP(this, &SOdysseyMeshSelector::OnLODChanged, LODLevel);
            MenuAction.GetActionCheckState.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::GetLODCheckState, LODLevel);

            MenuBuilder.AddMenuEntry(
                FText::Format(LOCTEXT("LOD Level", "LOD Level {0}"), FText::AsNumber(LODLevel)),
                FText::Format(LOCTEXT("LODLevel_ToolTip", "Select LOD Level {0} for the UV Map"), FText::AsNumber(LODLevel)),
                FSlateIcon(),
                MenuAction,
                NAME_None,
                EUserInterfaceActionType::RadioButton
            );
        }

        MenuBuilder.EndSection();


        TSharedRef< SWidget > Widget =

        SNew( SVerticalBox )
            + SVerticalBox::Slot()
            .Padding( 2 )
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Mesh LOD", "Mesh LOD"))
            ]
            + SVerticalBox::Slot()
            .Padding( 2 )
            .AutoHeight()
            [
                MenuBuilder.MakeWidget()
            ];

        return Widget;

    }
    return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SOdysseyMeshSelector::CreateUVMenuWidget()
{
    if( MeshSelector->GetCurrentMesh() )
    {
        FMenuBuilder MenuBuilder(true, NULL);

        MenuBuilder.BeginSection("Display UV Map");

        FUIAction UVAction;
        UVAction.ExecuteAction.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::SetCurrentUVChannel, -1);
        UVAction.GetActionCheckState.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::GetUVChannelCheckState, -1);

        MenuBuilder.AddMenuEntry(
            LOCTEXT("ShowUVSToggle", "None"),
            LOCTEXT("ShowUVSToggle_Tooltip", "Toggles display of the static mesh's UVs."),
            FSlateIcon(),
            UVAction,
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );



        // Fill out the UV channels combo.
        int MaxUVChannels = MeshSelector->GetMaxUVChannelForCurrentLOD();

        if( MaxUVChannels > 0)
            MenuBuilder.AddMenuSeparator();

        for(int UVChannelID = 0; UVChannelID < MaxUVChannels; ++UVChannelID)
        {
            FUIAction MenuAction;
            MenuAction.ExecuteAction.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::SetCurrentUVChannel, UVChannelID);
            MenuAction.GetActionCheckState.BindSP(MeshSelector.Get(), &FOdysseyMeshSelector::GetUVChannelCheckState, UVChannelID);

            MenuBuilder.AddMenuEntry(
                FText::Format(LOCTEXT("UVChannel_ID", "UV Channel {0}"), FText::AsNumber(UVChannelID)),
                FText::Format(LOCTEXT("UVChannel_ID_ToolTip", "Overlay UV Channel {0} on the viewport"), FText::AsNumber(UVChannelID)),
                FSlateIcon(),
                MenuAction,
                NAME_None,
                EUserInterfaceActionType::RadioButton
            );
        }

        MenuBuilder.EndSection();

        return
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
            .Padding( 2 )
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Mesh UV Channel", "Mesh UV Channel"))
            ]
            + SVerticalBox::Slot()
            .Padding( 2 )
            .AutoHeight()
            [
                MenuBuilder.MakeWidget()
            ];
    }
    return SNullWidget::NullWidget;
}



//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks
FString SOdysseyMeshSelector::ObjectPath() const
{
    if( !MeshSelector->GetCurrentMesh() )
        return FString();

    return MeshSelector->GetCurrentMesh()->GetPathName();
}


void SOdysseyMeshSelector::OnObjectChanged( const FAssetData& AssetData )
{
    if( AssetData.IsValid() )
    {
        MeshSelector->SetCurrentMesh( CastChecked< UStaticMesh >( AssetData.GetAsset() ) );
        MeshSelector->SetCurrentUVChannel(-1);
        MeshSelector->SetCurrentLOD(-1);
        LODSelectionMenu->DetachWidget();
        LODSelectionMenu->AttachWidget( CreateLODMenuWidget() );
        UVSelectionMenu->DetachWidget();
        UVSelectionMenu->AttachWidget( CreateUVMenuWidget() );
    }

    OnMeshChanged.ExecuteIfBound( MeshSelector->GetCurrentMesh() );
}

void SOdysseyMeshSelector::OnLODChanged( int inNewLOD )
{
    MeshSelector->SetCurrentLOD( inNewLOD );
    if( MeshSelector->GetCurrentUVChannel() > MeshSelector->GetMaxUVChannelForCurrentLOD() )
        MeshSelector->SetCurrentUVChannel(-1);
    UVSelectionMenu->DetachWidget();
    UVSelectionMenu->AttachWidget( CreateUVMenuWidget() );
}

FReply SOdysseyMeshSelector::HandleMeshColorBlockMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = true;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.sRGBOverride = false;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SOdysseyMeshSelector::OnSetMeshColorFromColorPicker);
        PickerArgs.InitialColorOverride = MeshSelector->GetMeshColor();
        PickerArgs.ParentWidget = ColorBlockWidget;
        PickerArgs.OptionalOwningDetailsView = ColorBlockWidget;
        FWidgetPath ParentWidgetPath;
        if (FSlateApplication::Get().FindPathToWidget(ColorBlockWidget.ToSharedRef(), ParentWidgetPath))
        {
            PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
        }
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

void SOdysseyMeshSelector::OnSetMeshColorFromColorPicker(FLinearColor NewColor)
{
    MeshSelector->SetMeshColor( NewColor );
}

FLinearColor SOdysseyMeshSelector::GetMeshColor() const
{
    return MeshSelector->GetMeshColor();
}



#undef LOCTEXT_NAMESPACE
