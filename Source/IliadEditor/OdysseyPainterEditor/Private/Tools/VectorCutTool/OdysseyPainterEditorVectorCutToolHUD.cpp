// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorCutTool/OdysseyPainterEditorVectorCutToolHUD.h"
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

FOdysseyPainterEditorVectorCutToolHUD::~FOdysseyPainterEditorVectorCutToolHUD()
{
}

FOdysseyPainterEditorVectorCutToolHUD::FOdysseyPainterEditorVectorCutToolHUD( UOdysseyPainterEditorVectorCutTool* iSelectionTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iSelectionTool )
{
    mCutTool = iSelectionTool;
}

void
FOdysseyPainterEditorVectorCutToolHUD::Load()
{
    uint32 width = mScene->GetLayer()->GetWidth();
    uint32 height = mScene->GetLayer()->GetHeight();

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorCutToolHUD::Unload()
{
    mBLSelectionContext.end();
}

void
FOdysseyPainterEditorVectorCutToolHUD::Reset()
{
    uint64 hudFlags = mCutTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        hudFlags &= (~FOdysseyVectorHUD::HUD_MODE_INBETWEEN);

        hudFlags |= FOdysseyVectorHUD::HUD_MODE_OBJECT;
    }

    UpdateSelectionBox( false, hudFlags );
}

BLImage*
FOdysseyPainterEditorVectorCutToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

//3D HUD
void
FOdysseyPainterEditorVectorCutToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );

    uint32 selectedObjectCount = mScene->GetCell()->GetSelectedObjectList().size();
    uint64 hudFlags = mCutTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawHierarchy( iParams
                     , mScene
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
        if( mScene->GetCell()->GetSelectedObjectList().size() )
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
FOdysseyPainterEditorVectorCutToolHUD::DrawSelectionSpace( BLContext* iBLContext
                                                               , uint64 iFlags )
{
    BLPoint topLeft = { 0, 0 };

    mBLSelectionContext.save();

    if( mScene->GetCell()->GetSelectionSpace() )
    {
        FOdysseyVectorGroup* selectionSpace = mScene->GetCell()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false, false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        //iScene->GetEngine()->UseImage( mSelectionMask );

        mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );

        mBLSelectionContext.clearAll();
        mBLSelectionContext.setFillStyle( BLRgba32( 0x80FFFFFF ) );
        mBLSelectionContext.fillRect( 0, 0, mBLSelectionMask.width(), mBLSelectionMask.height() );

        mBLSelectionContext.setMatrix( worldMatrix );
        mBLSelectionContext.setFillStyle( BLRgba32( 0x00000000/*0x800000FF*/ ) );
        mBLSelectionContext.fillRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        //iScene->GetEngine()->UseImage( currentImage );
        // blit with current renderer
        iBLContext->blitImage( topLeft, mBLSelectionMask );
    }

    mBLSelectionContext.restore();
}

void
FOdysseyPainterEditorVectorCutToolHUD::DrawPickingArea( const FOdysseyHUD::FDrawHUDParams& iParams
                                                            , const FLinearColor& fgColor
                                                            , const FLinearColor& bgColor
                                                            , const FLinearColor& hcColor )
{
    std::vector<::ULIS::FVec2D>& pointArray = mCutTool->GetPointArray();

    if( pointArray.size() > 1 )
    {
        FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

        switch( mCutTool->Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kRectangle :
            {
                double xmin = ::ULIS::FMath::Min( pointArray[0].x, pointArray[1].x );
                double ymin = ::ULIS::FMath::Min( pointArray[0].y, pointArray[1].y );
                double xmax = ::ULIS::FMath::Max( pointArray[0].x, pointArray[1].x );
                double ymax = ::ULIS::FMath::Max( pointArray[0].y, pointArray[1].y );
                //::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                FVector2D p[4] = { iParams.mTextureToHUD.Execute( FVector2D( xmin, ymin ) )
                                 , iParams.mTextureToHUD.Execute( FVector2D( xmax, ymin ) )
                                 , iParams.mTextureToHUD.Execute( FVector2D( xmax, ymax ) )
                                 , iParams.mTextureToHUD.Execute( FVector2D( xmin, ymax ) ) };

                for( uint32 i = 0; i < 4; i++ )
                {
                    uint32 n = ( i + 1 ) % 4;

                    DrawPrimitiveLine( iParams, p[i], p[n], hcColor, 1.0f );
                }
            }
            break;

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

            case EOdysseyShapeType::kLine :
            {
                FVector2D p0 = iParams.mTextureToHUD.Execute( FVector2D( pointArray[0].x, pointArray[0].y ) );
                FVector2D p1 = iParams.mTextureToHUD.Execute( FVector2D( pointArray[1].x, pointArray[1].y ) );

                DrawPrimitiveLine( iParams, p0, p1, hcColor, 1.0f );
            }
            break;

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
FOdysseyPainterEditorVectorCutToolHUD::Draw( BLContext* iBLContext )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint32 selectedObjectCount = mScene->GetCell()->GetSelectedObjectList().size();
    uint64 hudFlags = mCutTool->GetEditor()->GetVectorHUDFlags();

    DrawSelectionSpace( iBLContext, hudFlags );
}

void
FOdysseyPainterEditorVectorCutToolHUD::ClearMask()
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

::ULIS::FRectD
FOdysseyPainterEditorVectorCutToolHUD::GenerateCircleMask( double iX, double iY, double iRadius, bool iStroke )
{
    mBLSelectionContext.save();
    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();

    if( iStroke )
    {
        mBLSelectionContext.setStrokeAlpha( 1.0f );
        mBLSelectionContext.setStrokeWidth( 1.0f );
        mBLSelectionContext.strokeCircle( iX, iY, iRadius );
    }
    else
    {
        mBLSelectionContext.setFillAlpha( 1.0f );
        mBLSelectionContext.fillCircle( iX, iY, iRadius );
    }

    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );
    mBLSelectionContext.restore();

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

::ULIS::FRectD
FOdysseyPainterEditorVectorCutToolHUD::GenerateRectangleMask( const ::ULIS::FRectD& iRect, bool iStroke )
{
    mBLSelectionContext.save();
    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();

    if( iStroke )
    {
        mBLSelectionContext.setStrokeAlpha( 1.0f );
        mBLSelectionContext.setStrokeWidth( 1.0f );
        mBLSelectionContext.strokeRect( iRect.x, iRect.y, iRect.w, iRect.h );
    }
    else
    {
        mBLSelectionContext.setFillAlpha( 1.0f );
        mBLSelectionContext.fillRect( iRect.x, iRect.y, iRect.w, iRect.h );
    }

    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );
    mBLSelectionContext.restore();

    return iRect;
}

::ULIS::FRectD
FOdysseyPainterEditorVectorCutToolHUD::GenerateLineMask( const ::ULIS::FVec2D& iPoint0
                                                       , const ::ULIS::FVec2D& iPoint1 )
{
    mBLSelectionContext.save();
    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();

    mBLSelectionContext.setStrokeAlpha( 1.0f );
    mBLSelectionContext.setStrokeWidth( 1.0f );
    mBLSelectionContext.strokeLine( iPoint0.x, iPoint0.y, iPoint1.x, iPoint1.y );

    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );
    mBLSelectionContext.restore();

    return ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( iPoint0.x, iPoint1.x )
                                     , ::ULIS::FMath::Min( iPoint0.y, iPoint1.y )
                                     , ::ULIS::FMath::Max( iPoint0.x, iPoint1.x )
                                     , ::ULIS::FMath::Max( iPoint0.y, iPoint1.y ) );
}

::ULIS::FRectD
FOdysseyPainterEditorVectorCutToolHUD::GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray, bool iStroke  )
{
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };
    BLPath path;

    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp(BL_COMP_OP_SRC_COPY);
    /*blctx.setFillStyle( BLRgba32(0x00000000) );*/
    mBLSelectionContext.setFillAlpha(0.0f);
    mBLSelectionContext.clearAll();

    if( iPointArray.size() )
    {
        double x1 = iPointArray[0].x, y1 = iPointArray[0].y
             , x2 = iPointArray[0].x, y2 = iPointArray[0].y;

        path.moveTo( iPointArray[0].x, iPointArray[0].y );

        for( uint32 i = 1; i < iPointArray.size(); i++ )
        {
            path.lineTo( iPointArray[i].x, iPointArray[i].y );

            if( iPointArray[i].x < x1 )
            {
                x1 = iPointArray[i].x;
            }

            if( iPointArray[i].y < y1 )
            {
                y1 = iPointArray[i].y;
            }

            if( iPointArray[i].x > x2 )
            {
                x2 = iPointArray[i].x;
            }

            if( iPointArray[i].y > y2 )
            {
                y2 = iPointArray[i].y;
            }
        }

        path.lineTo( iPointArray[0].x, iPointArray[0].y );

        rect = ::ULIS::FRectD::FromMinMax( x1, y1, x2, y2 );

        if( iStroke )
        {
            mBLSelectionContext.setStrokeAlpha(1.0f);
            mBLSelectionContext.setStrokeWidth( 1.0f );
            mBLSelectionContext.strokePath( path );
        }
        else
        {
            mBLSelectionContext.setFillAlpha(1.0f);
            mBLSelectionContext.fillPath( path );
        }
    }

    mBLSelectionContext.flush(BL_CONTEXT_FLUSH_SYNC);
    mBLSelectionContext.restore();

    return rect;
}

#undef LOCTEXT_NAMESPACE
