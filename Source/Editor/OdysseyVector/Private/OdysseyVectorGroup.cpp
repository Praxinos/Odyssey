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
FOdysseyVectorGroup::DrawShape( BLContext* iBLContext
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iCombinedOpacity
                              , uint64 iFlags )
{

}

bool
FOdysseyVectorGroup::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        if( mBBox.HitTest( ::ULIS::FVec2D( pt.x, pt.y ) ) )
        {
            return true;
        }
    }

    return false;
}

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape()
{
    return new FOdysseyVectorGroup( mName );
}

void
FOdysseyVectorGroup::UpdateShape( uint32 iUpdateFlags )
{
    ::ULIS::FRectD bbox;
    int init = 0;

    // Update the bounding box
    if( ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
     || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
     || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   )
     || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
     || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          ) )
    {
        for( FOdysseyVectorObject *child : mChildrenList )
        {
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
