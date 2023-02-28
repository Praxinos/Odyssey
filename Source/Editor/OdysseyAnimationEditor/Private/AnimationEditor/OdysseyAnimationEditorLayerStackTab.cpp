// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorLayerStackTab.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorLayerStackTab"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorLayerStackTab::~FOdysseyAnimationEditorLayerStackTab()
{
}

FOdysseyAnimationEditorLayerStackTab::FOdysseyAnimationEditorLayerStackTab(FOdysseyAnimationEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyAnimationEditor_LayerStack")
    , LOCTEXT( "OdysseyAnimationEditorLayerStackTab", "Layer Stack" )
    , FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

TSharedPtr<SWidget>
FOdysseyAnimationEditorLayerStackTab::CreateWidget()
{
    return SNew(SOdysseyAnimationLayerStack)
            .LayerStack(this, &FOdysseyAnimationEditorLayerStackTab::LayerStack);
}

void
FOdysseyAnimationEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyAnimationEditorCommands& AnimationEditorCommands = FOdysseyAnimationEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyAnimationEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(AnimationEditorCommands.CreateNewAnimationLayerImageRaster, CreateNewLayer )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity10, ChangeLayerOpacity, 0.1f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity20, ChangeLayerOpacity, 0.2f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity30, ChangeLayerOpacity, 0.3f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity40, ChangeLayerOpacity, 0.4f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity50, ChangeLayerOpacity, 0.5f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity60, ChangeLayerOpacity, 0.6f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity70, ChangeLayerOpacity, 0.7f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity80, ChangeLayerOpacity, 0.8f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity90, ChangeLayerOpacity, 0.9f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity100, ChangeLayerOpacity, 1.0f )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyLayerStack*
FOdysseyAnimationEditorLayerStackTab::LayerStack() const
{
    return mEditor->LayerStack();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyAnimationEditorLayerStackTab::CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
}

void
FOdysseyAnimationEditorLayerStackTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( !FOdysseyObjectEditorUtils::HasProperty(layerStack->CurrentLayer.Get(), "Opacity") )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), "Opacity", FMath::Clamp(iOpacity, 0.f, 1.f));
}

#undef LOCTEXT_NAMESPACE
