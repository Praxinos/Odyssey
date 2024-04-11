// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathDrawingToolHUD( this ), true )
    , ColorSource( eBaseToolColorSource::ColorWheel )
    , Opacity( 1.0f )
    , TracingType( eTracingType::Organic )
    , TracingFidelity( eTracingFidelity::Highest )
    , Radius( 5.0f )
    , PressureSensitive( true )
    //, Absolute( true )
    , UpdatePaintGroups( true )
    , Stitch( false )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , Debug( false )
    , mStitchedVertex( nullptr )
    , mPathNumber( 0 )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathDrawing64");

    mPathDrawingHUD = static_cast<FOdysseyPainterEditorVectorPathDrawingToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // init pathTracer's raster image
    mPathTracer.Init( iScene );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}


FOdysseyVectorPathTracer&
UOdysseyPainterEditorVectorPathDrawingTool::GetPathTracer()
{
    return mPathTracer;
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorGroupPaint* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch || FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        FOdysseyVectorVertex* stitchCubicVertex = nullptr;
        // the HUD provides the hovered vertices
        std::vector<FOdysseyVectorPoint*>& pickedPointArray = mPathDrawingHUD->GetStitchedPointArray();

        pickedPointArray.reserve( 50 );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                return stitchCubicVertex;
            }
        }
    }

    return nullptr;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                               , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                               , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    // stays empty
    std::vector<FOdysseyVectorPath*> addedObjectArray;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-drawing-tool.transaction.draw-path-and-stitch","Vector Path Drawing Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , addedObjectArray
                                                                  , iAddedVertexArray
                                                                  , iAddedSegmentArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorVectorPathDrawingTool::RecordUndoPathAdd( FOdysseyVectorGroupPaint* iScene
                                                             , FOdysseyVectorPath* iPath
                                                             , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                             , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-drawing-tool.transaction.draw-path","Vector Path Drawing Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, iPath ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FKey& iKey )
{
    StitchAtKeyDown = Stitch;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Stitch = !Stitch; // flip the value
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKey& iKey )
{
    Stitch = StitchAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey );
}

FOdysseyVectorObject*
UOdysseyPainterEditorVectorPathDrawingTool::GetParentObject( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* parentObject = iScene;

    // Add the path to the current unique selected group
    if( vectorEngine->GetSelectedObjectList().size() == 1 )
    {
        FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();

        if(  selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            parentObject = selectedObject;
        }
    }

    return parentObject;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mPathTracer.Reset();
    mStitchedVertex = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorPath* path = nullptr;

        mAddedVertexArray.clear();
        mAddedSegmentArray.clear();

        if( Stitch )
        {
            mStitchedVertex = PickVertex( iScene
                                        , iPointInTexture.x
                                        , iPointInTexture.y
                                        , StitchingRadius );
            if( mStitchedVertex )
            {
                path = mStitchedVertex->GetOwnerAsPath();
            }
        }

        if( path == nullptr )
        {
            FOdysseyVectorObject* parentObject = GetParentObject( iScene );

            path = new FOdysseyVectorPath( FString( "Path_" ) + FString::FromInt( mPathNumber++ ) );

            parentObject->AppendChild( path );
            path->UpdateMatrix();

            SetPathColor( path, ColorSource );

            path->SetOpacity( Opacity );
        }

        path->SetBrush( Brush );

        mPathTracer.AttachPath( path );

        //iScene->ClearSelection();
        //iScene->Select( path );

        // update invalidated objects
        iScene->Update( 0 );

        vectorEngine->ResetHUD(); // re-creates the quadtree;
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    imageRegion.x = 0;
    imageRegion.y = 0;
    imageRegion.w = width;
    imageRegion.h = height;

    if( mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) == true )
    {
        redrawRegion = imageRegion; // needs full redraw
    }

    if( Stitch )
    {
        return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
             | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector);

    // Left mouse button clicked
    // For some reason, iPointInTexture.keysDown.Find does not find the left button click for the first few events
    // when using the stylus so we use mPathTracer.GetPath instead
    //if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    if( mPathTracer.GetPath() )
    {
        double pointRadius = PressureSensitive ? ( iPointInTexture.pressure * Radius ) : Radius;
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
        FOdysseyVectorSegment* newSegment;

        // mandatory for stitching vertices
        mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        newSegment = mPathTracer.Trace( mStitchedVertex, iPointInTexture.x, iPointInTexture.y, pointRadius );

        if( newSegment )
        {
            mAddedVertexArray.push_back( newSegment->GetVertex(1) );
            mAddedSegmentArray.push_back( newSegment );

            //mUndoPathExtend->RecordSegment( newSegment, newSegment->GetVertex(1) );
        }

        // dont redraw everything if no new segment was created
        if( newSegment == nullptr )
        {
            static double oldMouseX;
            static double oldMouseY;
            double x1 = oldMouseX
                 , y1 = oldMouseY
                 , x2 = iPointInTexture.x
                 , y2 = iPointInTexture.y;
            double xmin = ::ULIS::FMath::Min( x1, x2 ) - pointRadius
                 , ymin = ::ULIS::FMath::Min( y1, y2 ) - pointRadius
                 , xmax = ::ULIS::FMath::Max( x1, x2 ) + pointRadius
                 , ymax = ::ULIS::FMath::Max( y1, y2 ) + pointRadius;

            //vectorEngine->SetInvalidatedRect( mPathTracer.GetRedrawRect() );

            vectorEngine->InvalidateRect( ::ULIS::FRectD::FromMinMax( xmin
                                                                    , ymin
                                                                    , xmax
                                                                    , ymax ) );

            oldMouseX = iPointInTexture.x;
            oldMouseY = iPointInTexture.y;
        }
        else
        {
            //uint32 imgW = vectorEngine->GetPreferredWidth(),
            //       imgH = vectorEngine->GetPreferredHeight();

            //vectorEngine->SetInvalidatedRect( ::ULIS::FRectI( 0, 0, imgW, imgH ) );
            vectorEngine->InvalidateRect();
        }

        iScene->Update( 0 ); // update invalidated path after segment insertion
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 imgW = vectorEngine->GetPreferredWidth(),
           imgH = vectorEngine->GetPreferredHeight();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton)
    {
        // check path validity in case we get a UP without a DOWN first
        if( mPathTracer.GetPath() )
        {
            FOdysseyVectorPath* path = mPathTracer.GetPath();
            FOdysseyVectorSegment* newSegment;
            FOdysseyVectorVertex* endingVertex = PickVertex( iScene
                                                           , iPointInTexture.x
                                                           , iPointInTexture.y
                                                           , StitchingRadius );

            // stitching to another path at MouseUp is CURRENTLY not supported
            if( endingVertex && ( endingVertex->GetOwnerAsPath() != path ) )
            {
                endingVertex = nullptr;
            }

            newSegment = mPathTracer.Flush( endingVertex );

            if( newSegment )
            {
                if( endingVertex == nullptr )
                {
                    mAddedVertexArray.push_back( newSegment->GetVertex(1) );
                }

                mAddedSegmentArray.push_back( newSegment );
            }

            if( mStitchedVertex )
            {
                if( mAddedSegmentArray.size() )
                {
                    RecordUndoPathAlter( iScene
                                        , mAddedVertexArray
                                        , mAddedSegmentArray );
                }
            }
            else // mStitchedVertex = nullptr
            {
                // TODO: this is only for NEW path
                if ( path->GetVertexList().size() <= 1 )
                {
                    // delete any new path with single vertex that was create at mouse down
                    path->GetParent()->RemoveChild( path );
                    vectorEngine->UnselectObject( path );
                    delete path;
                }
                else
                {
                    // Note, newSegment can be NULL and we still have a valid path if the last vertex
                    // was create precisely at last drag before mouseup.
                    RecordUndoPathAdd( iScene
                                     , path
                                     , mAddedVertexArray
                                     , mAddedSegmentArray );
                }
            }

            iScene->Update( UpdatePaintGroups ? FOdysseyVectorObject::UPDATEPAINTGROUPS : 0 ); // update invalidated objects

            vectorEngine->ResetHUD(); // re-creates the quadtree;
        }
    }

    // in OnMouseDragVector() we are not guaranteed to get a viewport redraw from what I understand.
    // this means the Invalidation Rectangle is not resetted, so we force it.
    vectorEngine->InvalidateRect();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY // important to remove the path builder from the hierarchy widget
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

uint64
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                 , const FName& iPropertyName )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    if( iPropertyName == "TracingType" )
    {
        switch( TracingType )
        {
            case eTracingType::Organic :
                mPathTracer.SetDotLimit( 0.0f ); // cos 90deg
            break;

            case eTracingType::Mechanic :
                mPathTracer.SetDotLimit( 0.9659f ); // cos 15deg
            break;

            default :
            break;
        }
    }

    if( iPropertyName == "TracingFidelity" )
    {
        mPathTracer.SetTracingWidth( (double) TracingFidelity );
    }

    vectorEngine->ResetHUD(); // rebuilds quadtree if stitch mode changes

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathDrawingTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> radiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "Radius", defaultPropertyParams);
//    const TSharedPtr<ISinglePropertyView> opacityPropertyView = propertyEditorModule.CreateSingleProperty(this, "Opacity", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> fidelityPropertyView = propertyEditorModule.CreateSingleProperty(this, "TracingFidelity", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> radiusHandle = radiusPropertyView->GetPropertyHandle();
//    TSharedPtr<class IPropertyHandle> opacityHandle = opacityPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> fidelityHandle = fidelityPropertyView->GetPropertyHandle();

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
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(fidelityHandle, fidelityPropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorPathDrawingTool::GetTooltip() const
{
    return LOCTEXT("vector-path-drawing-tool.tooltip", "Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
