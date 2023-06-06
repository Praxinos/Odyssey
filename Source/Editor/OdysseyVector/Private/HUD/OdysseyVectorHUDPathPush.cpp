#include "HUD/OdysseyVectorHUDPathPush.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDPathPush::~FOdysseyVectorHUDPathPush()
{
}

FOdysseyVectorHUDPathPush::FOdysseyVectorHUDPathPush()
    : mRestrictToSelection ( false )
{
}

void
FOdysseyVectorHUDPathPush::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyVectorHUDPathPush::DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx )
{
    if( iObj->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
    {
        FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(iObj);
        std::list<FOdysseyVectorVertex*>& vertexList = cubicPath->GetVertexList();

        iBLCtx->setCompOp( BL_COMP_OP_SRC_OVER );
        iBLCtx->setFillStyle( BLRgba32( 0x80FFFFFF ) );

        // Points and Point size handles
        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex *cubicVertex = static_cast<FOdysseyVectorVertex*>(*it);
            BLMatrix2D& worldMatrix = cubicPath->GetWorldMatrix();
            BLPoint worldPoint = worldMatrix.mapPoint( cubicVertex->GetX(), cubicVertex->GetY() );

            iBLCtx->setFillStyle( BLRgba32( 0xFF000000 ) );
            iBLCtx->fillCircle( worldPoint.x, worldPoint.y, 2.0f );
            iBLCtx->setFillStyle( BLRgba32( 0xFFFFFFFF ) );
            iBLCtx->fillCircle( worldPoint.x, worldPoint.y, 1.0f );

            /*iBLCtx->fillRect( worldPoint.x + POINTRECT.x
                            , worldPoint.y + POINTRECT.y
                            , POINTRECT.w
                            , POINTRECT.h );*/
        }
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = iObj->GetChildrenList().begin(); it != iObj->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        DrawObjectRecursive( child, iBLCtx );
    }
}

void
FOdysseyVectorHUDPathPush::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    if( mRestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            DrawObjectRecursive( selectedObject, blctx );
        }
    }
    else
    {
        DrawObjectRecursive( iScene, blctx );
    }

    blctx->restore();
}
