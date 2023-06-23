#include "HUD/OdysseyVectorHUD.h"

FPointQuadTree::~FPointQuadTree()
{
    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            delete mChildren[i];
        }
    }
}

FPointQuadTree::FPointQuadTree( const ::ULIS::FRectD& iRect
                              , uint32 iMaxPointsPerQuad
                              , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray )
    : mRect( iRect )
    , mChildren { nullptr, nullptr, nullptr, nullptr }
{
    Build( iMaxPointsPerQuad, iPointQuadTreeEntryArray );
}

FOdysseyVectorHUD::~FOdysseyVectorHUD()
{
    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }
}

FOdysseyVectorHUD::FOdysseyVectorHUD()
    : mPointQuadTree( nullptr )
{
}

void
FPointQuadTree::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF )  );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeRect( mRect.x, mRect.y, mRect.w, mRect.h );
    blctx->restore();

    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            mChildren[i]->Draw( iScene, iFlags );
        }
    }
}

void
FPointQuadTree::Build( uint32 iMaxPointsPerQuad, std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray )
{
    mPointQuadTreeEntryArray.reserve( iParentPointQuadTreeEntryArray.size() );

    for( int i = 0; i < iParentPointQuadTreeEntryArray.size(); i++ )
    {
        if( mRect.HitTest( iParentPointQuadTreeEntryArray[i].worldCoords ) )
        {
            mPointQuadTreeEntryArray.push_back( iParentPointQuadTreeEntryArray[i] );
        }
    }

    if( mPointQuadTreeEntryArray.size() > iMaxPointsPerQuad )
    {
        uint32 minX =   mRect.x;
        uint32 minY =   mRect.y;
        uint32 maxX = ( mRect.x +   mRect.w );
        uint32 maxY = ( mRect.y +   mRect.h );
        uint32 avgX =   mRect.x + ( mRect.w * 0.5f );
        uint32 avgY =   mRect.y + ( mRect.h * 0.5f );

        mChildren[0] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, minY, avgX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[1] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, minY, maxX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[2] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, avgY, maxX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[3] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, avgY, avgX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );

        mPointQuadTreeEntryArray.clear();
    }
}

void
FPointQuadTree::PickPoints( double iWorldX
                          , double iWorldY
                          , double iSelectionRadius
                          , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
   ::ULIS::FRectD rect = ::ULIS::FRectD( mRect.x -   iSelectionRadius,
                                         mRect.y -   iSelectionRadius,
                                         mRect.w + ( iSelectionRadius * 2 ),
                                         mRect.h + ( iSelectionRadius * 2 ) );

    if( rect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        if( mChildren[0] == nullptr )
        {
            for( int i = 0; i < mPointQuadTreeEntryArray.size(); i++ )
            {
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( mPointQuadTreeEntryArray[i].worldCoords.x - iWorldX
                                                   , mPointQuadTreeEntryArray[i].worldCoords.y - iWorldY );

                if( dif.Distance() <= iSelectionRadius )
                {
                    oPickedPointArray.push_back( mPointQuadTreeEntryArray[i].point );
                }
            }
        }
        else
        {
            for( int i = 0; i < 4; i++ )
            {
                mChildren[i]->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
            }
        }
    }
}

static void
MapPoints( FOdysseyVectorObject* iObject, const ::ULIS::FRectD& iRect, std::vector<FPointQuadTreeEntry>& oPointQuadTreeEntryArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( iObject );
        std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();

        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex* vertex = (*it);
            ::ULIS::FVec2D& coords = vertex->GetCoords();
            BLPoint worldCoords = path->GetWorldMatrix().mapPoint( coords.x, coords.y );
            ::ULIS::FVec2D screenCoords = ::ULIS::FVec2D( worldCoords.x, worldCoords.y );

            if( iRect.HitTest( screenCoords ) )
            {
                oPointQuadTreeEntryArray.push_back( FPointQuadTreeEntry( vertex, screenCoords ) );
            }
        }
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        MapPoints( child, iRect, oPointQuadTreeEntryArray );
    }
}

void
FOdysseyVectorHUD::MakePointQuadTree( FOdysseyVectorScene *iScene )
{
    std::vector<FPointQuadTreeEntry> pointQuadTreeEntryArray;
    uint32 width, height;
    ::ULIS::FRectD screenRect;

    iScene->GetEngine()->GetColorImageSize( &width, &height );

    screenRect = ::ULIS::FRectD::FromXYWH( 0, 0, width, height );

    pointQuadTreeEntryArray.reserve( 200 );

    MapPoints( iScene, screenRect, pointQuadTreeEntryArray );

    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }

    mPointQuadTree = new FPointQuadTree( screenRect, 20, pointQuadTreeEntryArray );
}

void
FOdysseyVectorHUD::PickPoints( double iWorldX
                             , double iWorldY
                             , double iSelectionRadius
                             , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
    if( mPointQuadTree )
    {
        mPointQuadTree->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
    }
}

//static
void
FOdysseyVectorHUD::DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx )
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
