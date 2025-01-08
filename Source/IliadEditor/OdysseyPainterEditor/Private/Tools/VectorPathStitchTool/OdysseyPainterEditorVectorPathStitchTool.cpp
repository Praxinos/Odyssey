// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathStitchTool::~UOdysseyPainterEditorVectorPathStitchTool()
{
}

UOdysseyPainterEditorVectorPathStitchTool::UOdysseyPainterEditorVectorPathStitchTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathStitchToolHUD( this ), false )
    , PickingRadius(20.0f)
//    , RestrictToSelection( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathStitch64");

    mPathStitchHUD = static_cast<FOdysseyPainterEditorVectorPathStitchToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathStitchTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* iEngine = iScene->GetEngine();
        FOdysseyVectorVertex* knotVertex;
        // for undos
        std::vector<FOdysseyVectorPath*> addedPathArray; // stays empty
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorPath*> removedPathArray; // receives the merged path if any
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
        std::vector<FOdysseyVectorVertex*> mergedVertexArray;
        FOdysseyVectorVertex** stitchableVertex = mPathStitchHUD->GetStitchableVertices();

        //pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.

    /*
        iEngine->PickPoints( iScene
                           , false
                           , iPointInTexture.x
                           , iPointInTexture.y
                           , PickingRadius
                           , pickedPointArray
                           , FOdysseyVectorPath::PICK_POINT );
    */
        if( stitchableVertex[0] && stitchableVertex[1] )
        {
            FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( stitchableVertex[0] );
            FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( stitchableVertex[1] );
            FOdysseyVectorPath* mergedPath = nullptr;

            // TODO: remove vertexB->GetPath() from selected objects.
            if( vertexA->GetOwnerAsPath() != vertexB->GetOwnerAsPath() )
            {
                mergedPath = vertexB->GetOwnerAsPath();

                vertexB->GetOwnerAsPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetOwnerAsPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );


                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = mergedVertexArray[vertexB->GetID()];

                if( mergedPath->IsSelected() )
                {
                    iEngine->UnselectObject( mergedPath );

                    iEngine->SelectObject( vertexA->GetOwnerAsPath() );
                }

                removedPathArray.push_back( mergedPath );
            }

            knotVertex = iEngine->Stitch( vertexA, vertexB, addedSegmentArray, removedSegmentArray, true );

            if( knotVertex )
            {
                addedVertexArray.push_back( knotVertex );
                removedVertexArray.push_back( vertexA );
                removedVertexArray.push_back( vertexB );

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-path-stitch-tool.transaction.stitch-path","Vector Path Stitch Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPathStitch( iScene
                                                                               , removedPathArray
                                                                               , removedVertexArray
                                                                               , removedSegmentArray
                                                                               , addedPathArray
                                                                               , addedVertexArray
                                                                               , addedSegmentArray
                                                                               , mergedVertexArray
                                                                               , mergedSegmentArray
                                                                               , notificationFlags );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
        }

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        mPathStitchHUD->Reset( iScene ); // rebuilds QuadTree after path alter.
    }

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , uint64& oSignalFlags )
{
    double diameter = PickingRadius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)PickingRadius
                          , (int)iPointInTexture.y - (int)PickingRadius
                          , (int)diameter
                          , (int)diameter };

    mPathStitchHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );


/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/
    /*}*/
    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathStitchHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
    }

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    return true;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    /* if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathStitchTool, RestrictToSelection) )
    {
        iEngine->ResetHUD(); // rebuild the quad tree
    } */

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathStitchTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> radiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "PickingRadius", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> radiusHandle = radiusPropertyView->GetPropertyHandle();

    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(radiusHandle, radiusPropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorPathStitchTool::GetTooltip() const
{
    return LOCTEXT("vector-path-stitch-tool.tooltip", "Path Stitch Tool");
}

#undef LOCTEXT_NAMESPACE
