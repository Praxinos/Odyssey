// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Texture/OdysseyTextureEditorExtension.h"

#include "LayerStack/OdysseyTextureLayer.h"
#include "Tools/RasterPaintBucketTool/OdysseyTextureEditorRasterPaintBucketToolSourceProvider.h"
#include "BrushContext/OdysseyLayerStackEditorBrushContext.h"
#include "Texture/OdysseyTextureEditorGUI.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyLayerStack.h"
#include "OdysseyTextureLayerStack.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Texture/OdysseyTextureEditorSource.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyTextureEditorExtension::~FOdysseyTextureEditorExtension()
{
}

FOdysseyTextureEditorExtension::FOdysseyTextureEditorExtension(FOdysseyPainterEditor* iEditor)
    : FOdysseyPainterEditorExtension(iEditor)
    , mTextureSource(nullptr)
    , mGUI(nullptr)
    , mLayerStackBrushEditorContext(MakeShared<FOdysseyLayerStackEditorBrushContext>(nullptr))
{
}

void
FOdysseyTextureEditorExtension::Initialize()
{
    mGUI = MakeShared<FOdysseyTextureEditorGUI>(this);
    mGUI->Init();

    mEditor->SetVectorHUDFlags( FOdysseyVectorHUD::HUD_MODE_OBJECT
                              | FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED
                              | FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED );

    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyTextureEditorExtension::OnSourceChanged);
}

void
FOdysseyTextureEditorExtension::Finalize()
{
    GetEditor()->OnSourceChanged().RemoveAll(this);
}

void
FOdysseyTextureEditorExtension::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    mGUI->ExtendMenu(iExtender);
}

void
FOdysseyTextureEditorExtension::BindShortcuts(FBaseToolkit* iToolkit)
{
    mGUI->BindShortcuts(iToolkit);
}

void
FOdysseyTextureEditorExtension::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    mGUI->BuildLayout(iBuilder);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

void
FOdysseyTextureEditorExtension::OnSourceChanged()
{
    mTextureSource = nullptr;
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);

    //Is the source an texture
    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
    {
        GetEditor()->GetBrushContexts().Remove(mLayerStackBrushEditorContext.Get());
        return;
    }

    mTextureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorExtension::OnCurrentLayerChanged);

    GetEditor()->GetBrushContexts().Add(mLayerStackBrushEditorContext.Get());
    mLayerStackBrushEditorContext->SetLayerStack(mTextureSource->GetLayerStack());

    ConfigureTools();
}

UTexture2D*
FOdysseyTextureEditorExtension::Texture() const
{
    if (!mTextureSource)
        return nullptr;

    return mTextureSource->GetTexture();
}

TSharedPtr<FOdysseyTextureEditorSource>
FOdysseyTextureEditorExtension::GetTextureSource() const
{
    return mTextureSource;
}

UOdysseyTextureLayerStack*
FOdysseyTextureEditorExtension::GetLayerStack() const
{
    if (!mTextureSource)
        return nullptr;

    return mTextureSource->GetLayerStack();
}

void
FOdysseyTextureEditorExtension::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if ( iLayerStack != mTextureSource->GetLayerStack() )
        return;

    // PATCH : We have to redraw all layers in order to draw all layers without the HUD of the tool.
    // This will be removed when we'll have a dedicated HUD layer.
    Cast<UOdysseyTextureLayerStack>(iLayerStack)->UpdateTexture(true);
}

void
FOdysseyTextureEditorExtension::ConfigureTools()
{
    TSharedPtr<FOdysseyTextureEditorRasterPaintBucketToolSourceProvider> provider = MakeShared<FOdysseyTextureEditorRasterPaintBucketToolSourceProvider>(this);
    GetEditor()->GetRasterPaintBucketTool()->SetSourceProvider(provider);
}
