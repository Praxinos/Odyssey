// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"

// for 3D HUDs
#include "CanvasTypes.h"
//#include "Engine/Texture.h"
//#include "Input/OdysseyPoint.h"
//#include "OdysseyHUDElement.h"
//#include "TextureResource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorSelectionToolHUD::~FOdysseyPainterEditorVectorSelectionToolHUD()
{
}

FOdysseyPainterEditorVectorSelectionToolHUD::FOdysseyPainterEditorVectorSelectionToolHUD( UOdysseyPainterEditorVectorSelectionTool* iSelectionTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iSelectionTool )
{
    mSelectionTool = iSelectionTool;
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Load()
{
    uint32 width = mSelectionTool->GetViewportWidth();
    uint32 height = mSelectionTool->GetViewportHeight();

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Unload()
{
    mBLSelectionContext.end();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Reset()
{
    uint64 hudFlags = mSelectionTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        hudFlags &= (~FOdysseyVectorHUD::HUD_MODE_INBETWEEN);

        hudFlags |= FOdysseyVectorHUD::HUD_MODE_OBJECT;
    }

    UpdateSelectionBox( false, hudFlags );
}

BLImage*
FOdysseyPainterEditorVectorSelectionToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::SelectObject( FOdysseyVectorGroupPaint* iScene
                                                         , std::vector<FOdysseyVectorObject*>& oPickedObjectArray )
{
    Pick( iScene, mBLSelectionMask, mROI, oPickedObjectArray );
}

//3D HUD
void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint64 hudFlags = mSelectionTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawHierarchy( iParams
                     , scene
                     , fgColor
                     , bgColor
                     , hcColor
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_PATH_VERTEX
                     | FOdysseyVectorHUD::HUD_PATH_SEGMENT );
    }

    if( ( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT    )
     || ( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) )
    {
        if( mBaseTool->GetWorkingCell()->GetSelectedObjectList().size() )
        {
            DrawSelectionBox( iParams, fgColor, bgColor, hcColor, hudFlags );
        }
    }

    // TODO
    //DrawSelectionSpace( iBLContext, iScene, hudFlags );

    DrawPickingArea( iParams, fgColor, bgColor, hcColor );

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    //FOdysseyHUDElement::DrawHUD( iParams );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawSelectionSpace( BLContext* iBLContext
                                                               , uint64 iFlags )
{
    BLPoint topLeft = { 0, 0 };

    mBLSelectionContext.save();

    if( mBaseTool->GetWorkingCell()->GetSelectionSpace() )
    {
        FOdysseyVectorGroup* selectionSpace = mBaseTool->GetWorkingCell()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false, false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        //iScene->GetEngine()->UseImage( mSelectionMask );

        mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );

        mBLSelectionContext.clear_all();
        mBLSelectionContext.set_fill_style( BLRgba32( 0x80FFFFFF ) );
        mBLSelectionContext.fill_rect( 0, 0, mBLSelectionMask.width(), mBLSelectionMask.height() );

        mBLSelectionContext.set_transform( worldMatrix );
        mBLSelectionContext.set_fill_style( BLRgba32( 0x00000000/*0x800000FF*/ ) );
        mBLSelectionContext.fill_rect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        //iScene->GetEngine()->UseImage( currentImage );
        // blit with current renderer
        iBLContext->blit_image( topLeft, mBLSelectionMask );
    }

    mBLSelectionContext.restore();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawPickingArea( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                            , const FLinearColor& fgColor
                                                            , const FLinearColor& bgColor
                                                            , const FLinearColor& hcColor )
{
    std::vector<::ULIS::FVec2D>& pointArray = mSelectionTool->GetPointArray();

    if( pointArray.size() > 1 )
    {
        FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

        switch( mSelectionTool->Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kEllipse:
            {
                double xmin = ::ULIS::FMath::Min( pointArray[0].x, pointArray[1].x );
                double ymin = ::ULIS::FMath::Min( pointArray[0].y, pointArray[1].y );
                double xmax = ::ULIS::FMath::Max( pointArray[0].x, pointArray[1].x );
                double ymax = ::ULIS::FMath::Max( pointArray[0].y, pointArray[1].y );
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( xmax, ymax ) - ::ULIS::FVec2D( xmin, ymin );
                double radius = diagonal.Distance();
                ::ULIS::TArray<::ULIS::FVec2I> points;

                ::ULIS::GenerateEllipsePoints( ::ULIS::FVec2I( pointArray[0].x, pointArray[0].y ), radius, radius, points );

                for( uint32 i = 1; i < points.Size(); i++ )
                {
                    uint32 n = ( i - 1 );
                    FVector2D p0 = iParams.mTextureToHUD.Execute( FVector2D( points[n].x, points[n].y ) );
                    FVector2D p1 = iParams.mTextureToHUD.Execute( FVector2D( points[i].x, points[i].y ) );

                    DrawPrimitiveLine( iParams, p0, p1, hcColor, 1.0f );
                }
            }
            break;

            case EOdysseyShapeType::kRectangle :
            case EOdysseyShapeType::kFreehand :
                for( int i = 0; i < pointArray.size(); i++ )
                {
                    int n = ( i + 1 ) % pointArray.size();
                    FVector2D p0 = iParams.mTextureToHUD.Execute( FVector2D( pointArray[n].x, pointArray[n].y ) );
                    FVector2D p1 = iParams.mTextureToHUD.Execute( FVector2D( pointArray[i].x, pointArray[i].y ) );

                    DrawPrimitiveLine( iParams, p0, p1, hcColor, 1.0f );
                }
            break;

            default:

            break;
        }
    }
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Draw( BLContext* iBLContext )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mSelectionTool->GetEditor()->GetVectorHUDFlags();

    DrawSelectionSpace( iBLContext, hudFlags );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::ClearMask()
{
    mBLSelectionContext.save();

    mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.set_fill_alpha( 0.0f );
    mBLSelectionContext.clear_all();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateMask( const std::vector<::ULIS::FVec2D>& iTexPointArray
                                                         , EOdysseyShapeType iActiveShapeType )
{
    ::ULIS::FRectD roi = ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );

    ClearMask();

    if( iTexPointArray.size() > 1 )
    {
        switch( iActiveShapeType )
        {
            case EOdysseyShapeType::kRectangle:
            {
                GenerateRectangleMask( iTexPointArray );
            }
            break;

            case EOdysseyShapeType::kEllipse:
            {
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( iTexPointArray[1] - iTexPointArray[0] );

                GenerateCircleMask( iTexPointArray[0].x, iTexPointArray[0].y, diagonal.Distance() );
            }
            break;

            case EOdysseyShapeType::kFreehand:
                GenerateFreehandMask( iTexPointArray );
            break;

            default:
            break;
        }
    }
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateCircleMask( double iTexX
                                                               , double iTexY
                                                               , double iRadius )
{
    FVector2D hudCoords = mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexX, iTexY ) );

    mBLSelectionContext.save();

    mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.set_fill_alpha( 1.0f );
    mBLSelectionContext.fill_circle( hudCoords.X, hudCoords.Y, iRadius );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    mROI = ::ULIS::FRectD::FromMinMax( hudCoords.X - iRadius, hudCoords.Y - iRadius
                                     , hudCoords.X + iRadius, hudCoords.Y + iRadius );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateRectangleMask( const std::vector<::ULIS::FVec2D>& iTexPointArray )
{
    FVector2D hudCoords[4] = { mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[0].x
                                                                                 , iTexPointArray[0].y ) )
                             , mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[1].x
                                                                                 , iTexPointArray[1].y ) )
                             , mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[2].x
                                                                                 , iTexPointArray[2].y ) )
                             , mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[3].x
                                                                                 , iTexPointArray[3].y ) ) };
    double xmin = ::ULIS::FMath::Min4( hudCoords[0].X, hudCoords[1].X, hudCoords[2].X, hudCoords[3].X );
    double ymin = ::ULIS::FMath::Min4( hudCoords[0].Y, hudCoords[1].Y, hudCoords[2].Y, hudCoords[3].Y );
    double xmax = ::ULIS::FMath::Min4( hudCoords[0].X, hudCoords[1].X, hudCoords[2].X, hudCoords[3].X );
    double ymax = ::ULIS::FMath::Min4( hudCoords[0].Y, hudCoords[1].Y, hudCoords[2].Y, hudCoords[3].Y );

    mBLSelectionContext.save();

    mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.set_fill_alpha( 0.0f );
    mBLSelectionContext.clear_all();
    mBLSelectionContext.set_fill_alpha( 1.0f );
    mBLSelectionContext.fill_rect( BLRect( xmin, ymin, xmax - xmin, ymax - ymin ) );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    mROI = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateFreehandMask( const std::vector<::ULIS::FVec2D>& iTexPointArray )
{
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };
    BLPath path;

    mBLSelectionContext.save();

    mBLSelectionContext.set_comp_op(BL_COMP_OP_SRC_COPY);
    /*blctx.set_fill_style( BLRgba32(0x00000000) );*/
    mBLSelectionContext.set_fill_alpha(0.0f);
    mBLSelectionContext.clear_all();

    if( iTexPointArray.size() )
    {
        FVector2D hudCoords = mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[0].x
                                                                                , iTexPointArray[0].y ) );

        double x1 = hudCoords.X, y1 = hudCoords.Y
             , x2 = hudCoords.X, y2 = hudCoords.Y;

        path.move_to( hudCoords.X, hudCoords.Y );

        for( uint32 i = 1; i < iTexPointArray.size(); i++ )
        {
            hudCoords = mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexPointArray[i].x
                                                                          , iTexPointArray[i].y ) );

            path.line_to( hudCoords.X, hudCoords.Y );

            if( hudCoords.X < x1 )
            {
                x1 = hudCoords.X;
            }

            if( hudCoords.Y < y1 )
            {
                y1 = hudCoords.Y;
            }

            if( hudCoords.X > x2 )
            {
                x2 = hudCoords.X;
            }

            if( hudCoords.Y > y2 )
            {
                y2 = hudCoords.Y;
            }
        }

        path.line_to( hudCoords.X, hudCoords.Y );

        rect = ::ULIS::FRectD::FromMinMax( x1, y1, x2, y2 );

        mBLSelectionContext.set_fill_alpha(1.0f);
        mBLSelectionContext.fill_path( path );
    }

    mBLSelectionContext.flush(BL_CONTEXT_FLUSH_SYNC);
    mBLSelectionContext.restore();

    mROI = rect;
}

#undef LOCTEXT_NAMESPACE
