// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "OdysseyPaletteEntryColor.h"
#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPrimitiveDrawingTool::~UOdysseyPainterEditorVectorPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool::UOdysseyPainterEditorVectorPrimitiveDrawingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorBaseToolHUD( this ), true )
    , PrimitiveType ( EOdysseyVectorPrimitiveType::Ellipse )
    , ColorMode( eForegroundColorMode::SolidColor )
    , Opacity( 1.0f )
    , StrokeWidth( 4.0f )
    , Uniform( false )
    , mPrimitive( nullptr )
    , mRectangleNumber ( 0 )
    , mLineNumber ( 0 )
    , mEllipseNumber ( 0 )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Shapes64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKey& iKey
                                                                , uint64& oSignalFlags )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
        return true;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey
                                                              , uint64& oSignalFlags )
{
    Uniform = UniformAtKeyDown;

    return false;
}

FOdysseyVectorObject*
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetParentObject( FOdysseyVectorGroupPaint* iScene )
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

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey
                                                                  , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorObject* parentObject = GetParentObject( iScene );
        BLPoint localCoords = parentObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x
                                                                            , iPointInTexture.y );
        UOdysseyPaletteEntry* entry = nullptr;
        BLPoint widthVector = parentObject->GetInverseWorldMatrix().mapVector( 0.7071f * StrokeWidth
                                                                             , 0.7071f * StrokeWidth );
        ::ULIS::FVec2D width = ::ULIS::FVec2D( widthVector.x, widthVector.y );

        mMouseDown.x = iPointInTexture.x;
        mMouseDown.y = iPointInTexture.y;

        mPrimitive = nullptr;

        switch( PrimitiveType )
        {
            case EOdysseyVectorPrimitiveType::Rectangle:
                mPrimitive = new FOdysseyVectorRectangle( FString("Rectangle_") + FString::FromInt( mRectangleNumber++ ), 0.0f, 0.0f, width.Distance() );
            break;

            case EOdysseyVectorPrimitiveType::Line:
                mPrimitive = new FOdysseyVectorLine( FString("Line_") + FString::FromInt( mLineNumber++ ), 0.0f, 0.0f, width.Distance() );
            break;

            default:
                mPrimitive = new FOdysseyVectorEllipse( FString("Ellipse_" + FString::FromInt( mEllipseNumber++ )), 0.0f, 0.0f, width.Distance() );
            break;
        }

        parentObject->AppendChild( mPrimitive );

        SetPathColor( mPrimitive, ColorMode );
        mPrimitive->SetOpacity( Opacity );
        mPrimitive->SetBrush( Brush );
        //mPrimitive->SetForegroundColor( ueColor );
        mPrimitive->Translate( localCoords.x, localCoords.y );
        mPrimitive->UpdateMatrix();

        //iScene->GetEngine()->ClearObjectSelection();
        //iScene->GetEngine()->SelectObject( mPrimitive );

        //mSelectionChanged.Broadcast(iScene);

        iScene->Update( 0 ); // update invalidated objects
    }

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;

    return true;
}

double
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetLineRotationAngle( FOdysseyVectorLine* iLine
                                                                     , const FOdysseyPoint& iPointInTexture )
{
    BLPoint pt = iLine->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FVec2D hzVector = ::ULIS::FVec2D( 1.0f, 0.0f );
    ::ULIS::FVec2D ptVector = ::ULIS::FVec2D( pt.x, pt.y );
    double angle = 0.0f;

    if( ptVector.DistanceSquared() )
    {
        ptVector.Normalize();

        angle = fabs( acos( hzVector.DotProduct( ptVector ) ) );

        return FOdysseyVector::Cross2D( hzVector, ptVector ) >= 0.0f ? angle : ( M_PI * 2.0f ) - angle;
    }

    return 0.0f;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , uint64& oSignalFlags )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPrimitive )
        {
            BLPoint bldif = mPrimitive->GetInverseWorldMatrix().mapVector( iPointInTexture.x - mMouseDown.x
                                                                         , iPointInTexture.y - mMouseDown.y );
            ::ULIS::FVec2D size = ::ULIS::FVec2D( bldif.x, bldif.y );

            switch( PrimitiveType )
            {
                case EOdysseyVectorPrimitiveType::Ellipse:
                {
                    FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>(mPrimitive);

                    if( Uniform )
                    {
                        size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                        size.y = size.x;
                    }

                    ellipse->SetRadius( size.x, size.y );
                }
                break;

                case EOdysseyVectorPrimitiveType::Rectangle:
                {
                    FOdysseyVectorRectangle* rectangle = static_cast<FOdysseyVectorRectangle*>(mPrimitive);

                    if( Uniform )
                    {
                        size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                        size.y = size.x;
                    }

                    rectangle->SetSize( size.x, size.y );
                }
                break;

                case EOdysseyVectorPrimitiveType::Line:
                {
                    FOdysseyVectorLine* line = static_cast<FOdysseyVectorLine*>(mPrimitive);

                    if( Uniform )
                    {
                        BLMatrix2D& inverseMatrix = line->GetInverseWorldMatrix();
                        BLPoint pt = inverseMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
                        double distance = ::ULIS::FVec2D( pt.x, pt.y ).Distance();
                        int rotation = GetLineRotationAngle( line, iPointInTexture ) / M_PI * 180.0f;

                         // to force the tip of the line in the middle of the 45 degrees steps
                        rotation += 22;
                        // 45 deg by 45 deg
                        double angle = ( ( rotation / 45 ) * 45 ) * M_PI / 180.0f;

                        line->SetSize( cos( angle ) * distance, sin( angle ) * distance );
                    }
                    else
                    {
                        line->SetSize( size.x, size.y );
                    }
                }
                break;

                default:
                break;
            }
        }

        iScene->Update( /*FOdysseyVectorObject::FREQUENTUPDATES*/0 ); // update invalidated objects
    }

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FOdysseyPoint& iPointInTexture
                                                                , const FKey& iKey
                                                                , uint64& oSignalFlags )
{
    if( iKey != EKeys::LeftMouseButton )
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPrimitive )
        {
            FOdysseyVectorPath* path = mPrimitive->Convert();
            FOdysseyVectorObject* parentObject = mPrimitive->GetParent();

            //iScene->GetEngine()->ClearObjectSelection();
            parentObject->RemoveChild( mPrimitive );

            delete mPrimitive;
            mPrimitive = nullptr;

            parentObject->AppendChild( path );
            path->Invalidate();
            path->UpdateMatrix();

            //iScene->GetEngine()->SelectObject( path );

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-primitive-drawing-tool.transaction.draw-primitive","Vector Primitive Drawing Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectAdd( iScene, path );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidate objects
    }

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED;

    return true;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPrimitiveDrawingTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> strokeWidthPropertyView = propertyEditorModule.CreateSingleProperty(this, "StrokeWidth", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> primitiveTypePropertyView = propertyEditorModule.CreateSingleProperty(this, "PrimitiveType", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> strokeWidthHandle = strokeWidthPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> primitiveTypeHandle = primitiveTypePropertyView->GetPropertyHandle();

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
            CreatePropertyWidget(strokeWidthHandle, strokeWidthPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(primitiveTypeHandle, primitiveTypePropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetTooltip() const
{
    return LOCTEXT("vector-primitive-drawing-tool.tooltip", "Primitive Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
