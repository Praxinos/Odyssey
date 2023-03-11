// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include <chrono>
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectPickTool::~UOdysseyPainterEditorVectorObjectPickTool()
{
    delete mSelectionHUD;
}

UOdysseyPainterEditorVectorObjectPickTool::UOdysseyPainterEditorVectorObjectPickTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mSelectionHUD = new FOdysseyVectorHUDSelection( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectPickTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        UTexture2D* texture = layerStack->GetTexture();

        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mSelectionHUD->Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );
        mSelectionHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( mSelectionHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorObjectPickTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mPointArray.clear();

    if( currentVectorLayer )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mSelectionHUD->SetSelecting( true, &mPointArray );

        mPointArray.push_back( point );
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        mPointArray.push_back( point );

        currentVectorLayer->RenderImageChanged(true);
    }
}

static bool
DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

static void
SetSelectionSpace( FOdysseyVectorEngine* iVectorEngine, UOdysseyVectorObject* iSelectedObject )
{
    if( DoubleClicked() == true )
    {
        // Note: due to the dynamic_cast, the argument will be NULL if the object
            // does not inherits of base class UOdysseyVectorGroup. That's on purpose.
        iVectorEngine->SetSelectionSpace(dynamic_cast<UOdysseyVectorGroup*>(iSelectedObject));
    }
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        if ( mPointArray.size() == 1 )
        {
            vectorEngine->Pick( *currentVectorLayer->GetScene(), mPointArray, UOdysseyVectorObject::PICK_MATH_BASED );
        }

        if ( mPointArray.size() > 1 )
        {
            vectorEngine->Pick( *currentVectorLayer->GetScene(), mPointArray, UOdysseyVectorObject::PICK_MASK_BASED );
        }

        SetSelectionSpace( vectorEngine, currentVectorLayer->GetScene()->GetLastSelected() );

        mSelectionHUD->SetSelecting( false, nullptr );
        mSelectionHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );

        mSelectionChanged.Broadcast();

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectPickTool::Commit()
{

}
