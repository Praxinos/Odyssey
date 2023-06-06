#include "OdysseyVectorGroup.h"

FOdysseyVectorGroup::FOdysseyVectorGroup( const FString& iName )
    : FOdysseyVectorObject( iName )
{
}

void
FOdysseyVectorGroup::Init( FString& iName )
{
    SetName( iName );
}

bool
FOdysseyVectorGroup::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

bool
FOdysseyVectorGroup::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{

    return false;
}

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape()
{
    return new FOdysseyVectorGroup( mObjectParam.Name );
}

void
FOdysseyVectorGroup::UpdateShape( uint32 iUpdateFlags )
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);
        ::ULIS::FRectD childBBox = child->GetBBox( true );

        bbox = ( init == 0 ) ? childBBox : bbox | childBBox;

        init = 1;
    }

    BLPoint p0 = mInverseWorldMatrix.mapPoint( bbox.x         , bbox.y          );
    BLPoint p1 = mInverseWorldMatrix.mapPoint( bbox.x + bbox.w, bbox.y          );
    BLPoint p2 = mInverseWorldMatrix.mapPoint( bbox.x + bbox.w, bbox.y + bbox.h );
    BLPoint p3 = mInverseWorldMatrix.mapPoint( bbox.x         , bbox.y + bbox.h );

    mBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                      , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                      , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                      , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
}
