// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerStackAddLayerButton.h"

#include "EditorFontGlyphs.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerStackNew.h"

#define LOCTEXT_NAMESPACE "SOdysseyLayerStackAddLayerButton"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

SOdysseyLayerStackAddLayerButton::~SOdysseyLayerStackAddLayerButton()
{
}


void SOdysseyLayerStackAddLayerButton::Construct(const FArguments& InArgs)
{
    mLayerStack = InArgs._LayerStack;

    SComboButton::FArguments args;
    args.OnGetMenuContent(this, &SOdysseyLayerStackAddLayerButton::MakeMenu)
        .ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
        .ContentPadding(FMargin(0.0f, 5.0f))
        .HasDownArrow(true)
        .ButtonContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Bottom)
            .AutoWidth()
            [
                SNew(STextBlock)
                .TextStyle(FEditorStyle::Get(), "NormalText.Important")
                .Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
                .Text(FEditorFontGlyphs::Plus)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(4, 0, 0, 0)
            [
                SNew(STextBlock)
                .TextStyle(FEditorStyle::Get(), "NormalText.Important")
                .Text(LOCTEXT("AddLayer", "Add Layer"))
            ]
        ];

    SComboButton::Construct(args);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget> SOdysseyLayerStackAddLayerButton::MakeMenu()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return SNullWidget::NullWidget;

    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("AddLayers");
    {
        //Default Layers
        for( UClass* layerClass : layerStack->CompatibleLayers )
        {
            UOdysseyLayer* layerCDO = layerClass->GetDefaultObject<UOdysseyLayer>();
            if (!layerCDO)
                continue;
                
            menuBuilder.AddMenuEntry(
                layerCDO->LayerTypeName,
                layerCDO->Description,
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyLayerStackAddLayerButton::AddLayerFromClass, FAssetData(layerClass)))
                //We give a FAssetData(layerClass) instead of the layerClass directly, because layerClass could be detroyed
                //between the moment the user clicks on the button, and the one he clicks on the menu entry.
                //FAssetData will load the layerClass again if it's the case when calling FAssetData::GetAsset()
            );
        }

        //Custom Layers
        TArray<UClass*> supportedLayerClasses = layerStack->FindSupportedCustomLayerClasses();
        for (UClass* layerClass : supportedLayerClasses)
        {
            UOdysseyLayer* layerCDO = layerClass->GetDefaultObject<UOdysseyLayer>();
            if (!layerCDO)
                continue;

            menuBuilder.AddMenuEntry(
                layerCDO->LayerTypeName,
                layerCDO->Description,
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyLayerStackAddLayerButton::AddLayerFromClass, FAssetData(layerClass)))
                //We give a FAssetData(layerClass) instead of the layerClass directly, because layerClass could be detroyed
                //between the moment the user clicks on the button, and the one he clicks on the menu entry.
                //FAssetData will load the layerClass again if it's the case when calling FAssetData::GetAsset()
            );
        }
    }
    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

void
SOdysseyLayerStackAddLayerButton::AddLayerFromClass(FAssetData iAssetData)
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

	UObject* loadedAsset = iAssetData.FastGetAsset(true);
	if ( !loadedAsset )
		return;

	UClass* layerClass = Cast<UClass>(loadedAsset);
	if ( UBlueprint* blueprint = Cast<UBlueprint>(loadedAsset) )
		layerClass = blueprint->GeneratedClass;

	if ( !layerClass )
		return;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (currentLayer)
    {
        if (currentLayer->CanHaveChildren)
        {
			currentLayer = layerStack->AddLayer(layerClass, currentLayer);
        }
        else
        {
			UOdysseyLayer* parent = currentLayer->GetParent();
			int index = currentLayer->GetIndexInParent();
			currentLayer = layerStack->AddLayer(layerClass, parent, index);
        }
    }
    else
    {
		currentLayer = layerStack->AddLayer(layerClass);
    }

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(currentLayer));
}

#undef LOCTEXT_NAMESPACE
