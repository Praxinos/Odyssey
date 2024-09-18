// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorChartTool::~UOdysseyPainterEditorVectorChartTool()
{
}

UOdysseyPainterEditorVectorChartTool::UOdysseyPainterEditorVectorChartTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorChartToolHUD( this ), false )
    , PickingRadius( 10.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Chart64");

    mChartHUD = static_cast<FOdysseyPainterEditorVectorChartToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorChartTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN );
}

uint64
UOdysseyPainterEditorVectorChartTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorChartTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorChartTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    mPickedInbetween = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPickedInbetween = mChartHUD->PickInbetween( iPointInTexture.x
                                                   , iPointInTexture.y
                                                   , 10 );

        if( mPickedInbetween )
        {
            // needed for valid GUndo pointer
            /*GEditor->BeginTransaction(LOCTEXT("vector-chart-tool.transaction.edit-chart","Vector Chart Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                                          , inbetweenerTag
                                                                                          , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();*/
        }
    }

    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorChartTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    // TODO: highlight grid handles ?

    return 0;
}

uint64
UOdysseyPainterEditorVectorChartTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( iEngine->GetSelectedObjectList().size() )
        {
            FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( inbetweenerTag )
            {
                if( mPickedInbetween )
                {
                    mChartHUD->MoveInbetween( inbetweenerTag
                                               , mPickedInbetween
                                               , iPointInTexture.x
                                               , iPointInTexture.y
                                               , FSlateApplication::Get().GetModifierKeys().IsControlDown() );

                    iScene->Update( /*| FOdysseyVectorObject::UPDATE_INTERACTIVE*/0 );
                }
            }
        }
    }

    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorChartTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 retFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( iEngine->GetSelectedObjectList().size() )
        {
            FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( inbetweenerTag )
            {
                // we need to manually redraw because no object is modified
                inbetweenerTag->RedrawAnimationCells();

                iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            }
        }
    }

    return retFlags;
}

uint64
UOdysseyPainterEditorVectorChartTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    return 0;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorChartTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
/*
    const TSharedPtr<ISinglePropertyView> XDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsX", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> YDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsY", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> XDivHandle = XDivPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> YDivHandle = YDivPropertyView->GetPropertyHandle();
*/
    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
/*
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(XDivHandle, XDivPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(YDivHandle, YDivPropertyView).ToSharedRef()
        ]
*/;
}

FText
UOdysseyPainterEditorVectorChartTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Chart Tool");
}

#undef LOCTEXT_NAMESPACE
