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

void
FOdysseyVectorGroup::DrawShape( uint64 iFlags )
{

}

bool
FOdysseyVectorGroup::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
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

    // Update the bounding box
    if( ( mInvalidationFlags & INVALIDATE_CHILD )
     || ( mInvalidationFlags & INVALIDATE_SHAPE ) )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            FOdysseyVectorObject *child = (*it);
            ::ULIS::FRectD childBBox = child->GetBBox( false );

            if( childBBox.Area() )
            {
                BLMatrix2D childLocalMatrix = child->GetLocalMatrix();
                BLPoint p0 = childLocalMatrix.mapPoint( childBBox.x              , childBBox.y               );
                BLPoint p1 = childLocalMatrix.mapPoint( childBBox.x + childBBox.w, childBBox.y               );
                BLPoint p2 = childLocalMatrix.mapPoint( childBBox.x + childBBox.w, childBBox.y + childBBox.h );
                BLPoint p3 = childLocalMatrix.mapPoint( childBBox.x              , childBBox.y + childBBox.h );

                childBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                      , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                      , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                      , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

                bbox = ( init == 0 ) ? childBBox : bbox | childBBox;

                init = 1;
            }
        }

        if( init )
        {
            mBBox = bbox;
        }
        else
        {
            mBBox = ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
        }
    }
}
