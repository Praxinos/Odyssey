// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTiledViewportTab.h"
#include "AssetEditorViewportLayout.h"
#include "Framework/Application/SlateApplication.h"
#include "SAssetEditorViewport.h"
#include "Slate/SceneViewport.h"

#include "OdysseyAnimation.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditorTiledViewportClient.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorTiledViewportTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_TiledViewport");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTiledViewportTab::~FOdysseyPainterEditorTiledViewportTab()
{
}

FOdysseyPainterEditorTiledViewportTab::FOdysseyPainterEditorTiledViewportTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "tiled-viewport-tab.name", "Tiled 2D Viewport" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

void
FOdysseyPainterEditorTiledViewportTab::Init()
{
    mEditor->OnSourceChanged().AddSP(SharedThis(this), &FOdysseyPainterEditorTiledViewportTab::OnEditorSourceChanged);

    FOdysseyEditorTab::Init();
}

const FName&
FOdysseyPainterEditorTiledViewportTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorTiledViewportTab::CreateWidget()
{

    mViewportWidget = SNew(SViewport)
        .EnableGammaCorrection(false)
        .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
        .ShowEffectWhenDisabled(false)
        .EnableBlending(true);

    //Setup Viewport
    mViewportClient = MakeShared<FOdysseyPainterEditorTiledViewportClient>();
    mSceneViewport = FSceneViewport::Create(mViewportClient, mViewportWidget);
    mViewportWidget->SetViewportInterface(mSceneViewport.ToSharedRef());

    UpdateViewportTextureRenderer();

    return mViewportWidget;
}

void
FOdysseyPainterEditorTiledViewportTab::UpdateViewportTextureRenderer()
{
    IOdysseyTextureRenderingAbility* renderer = nullptr;
    int width = 256;
    int height = 256;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (source)
    {
        if (source->Id() == FOdysseyPainterEditorTextureSource::StaticId())
        {
            TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
            renderer = source->GetLayerStack();
        }

        if (source->Id() == FOdysseyPainterEditorAnimationSource::StaticId())
        {
            TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);
            renderer = animationSource->GetAnimation();
        }

        width = source->Width();
        height = source->Height();
    }

    mViewportClient->SetTextureRenderer(renderer);
    mViewportClient->SetCanvasSize(width, height);
    mSceneViewport->Invalidate();
}

void
FOdysseyPainterEditorTiledViewportTab::OnEditorSourceChanged()
{
    UpdateViewportTextureRenderer();
}

void
FOdysseyPainterEditorTiledViewportTab::Tick( float DeltaTime )
{
    //mSceneViewport->Invalidate(); //Redraws the viewport each tick to display HUD animations
}

#undef LOCTEXT_NAMESPACE
