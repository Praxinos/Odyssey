#include "OdysseyVectorObject.h"

UOdysseyVectorObject::~UOdysseyVectorObject()
{
}

UOdysseyVectorObject::UOdysseyVectorObject()
    : mTranslation( 0.0f, 0.0f )
    , mRotation( 0.0f )
    , mScaling ( 1.0f, 1.0f )
    , mStrokeColor ( 0xFF000000 )
    , mFillColor ( 0xFF000000 )
    , mStrokeWidth ( 4.0f )
    , mParent ( nullptr )
    , mIsFilled ( false )
    , mIsSelected ( false )
    , mIsInvalidated ( false )
{
    UpdateMatrix();
}

void
UOdysseyVectorObject::SetName( std::string iName )
{
    mName.assign( iName );
}

void
UOdysseyVectorObject::Update()
{
    UpdateShape();

    mIsInvalidated = false;
}

void
UOdysseyVectorObject::SetIsSelected( bool iIsSelected )
{
    mIsSelected = iIsSelected;
}

void
UOdysseyVectorObject::Translate( double iX, double iY )
{
    mTranslation.x = iX;
    mTranslation.y = iY;
}

void
UOdysseyVectorObject::Rotate( double iAngle )
{
    mRotation = iAngle;
}

void
UOdysseyVectorObject::Scale( double iX, double iY )
{
    mScaling.x = iX;
    mScaling.y = iY;
}

double
UOdysseyVectorObject::GetScalingX()
{
    return mScaling.x;
}

double
UOdysseyVectorObject::GetScalingY()
{
    return mScaling.y;
}

double
UOdysseyVectorObject::GetRotation()
{
    return mRotation;
}

UOdysseyVectorObject*
UOdysseyVectorObject::Copy() {
    UOdysseyVectorObject* objectCopy = CopyShape();

   // TODO, update matrices once we get a BLContext object

    if( objectCopy )
    {
        CopySettings( *objectCopy ); // we need the matrices to properly import the child

        // recurse
        for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            UOdysseyVectorObject *child = (*it);

            objectCopy->AppendChild( child->Copy() );
        }

        objectCopy->UpdateShape();
    }

    return objectCopy;
}

void
UOdysseyVectorObject::CopySettings( UOdysseyVectorObject& iDestinationObject )
{
    iDestinationObject.mTranslation = mTranslation;
    iDestinationObject.mRotation = mRotation;
    iDestinationObject.mScaling = mScaling;

    iDestinationObject.mStrokeColor = mStrokeColor;
    iDestinationObject.mStrokeWidth = mStrokeWidth;
    iDestinationObject.mFillColor = mFillColor;
    iDestinationObject.mIsFilled = mIsFilled;

    iDestinationObject.mBBox = mBBox;

    iDestinationObject.mName = mName;
    iDestinationObject.mName.append("_Copy");

    iDestinationObject.mLocalMatrix = mLocalMatrix;
    iDestinationObject.mInverseLocalMatrix = mInverseLocalMatrix;
    iDestinationObject.mWorldMatrix = mWorldMatrix;
    iDestinationObject.mInverseWorldMatrix = mInverseWorldMatrix;
}

double
UOdysseyVectorObject::GetTranslationX()
{
    return mTranslation.x;
}

double
UOdysseyVectorObject::GetTranslationY()
{
    return mTranslation.y;
}

void
UOdysseyVectorObject::UpdateMatrix( )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    blctx.save();

    blctx.resetMatrix();
    blctx.translate( mTranslation.x, mTranslation.y );
    blctx.rotate( mRotation );
    blctx.scale( mScaling.x, mScaling.y );
    mLocalMatrix = blctx.userMatrix();

    BLMatrix2D::invert( mInverseLocalMatrix, mLocalMatrix );

    if( mParent)
    {
        blctx.setMatrix( mParent->mWorldMatrix );
        blctx.transform( mLocalMatrix );
        mWorldMatrix = blctx.userMatrix();

        BLMatrix2D::invert( mInverseWorldMatrix, mWorldMatrix );
    }
     else
    {
        memcpy( &mWorldMatrix       , &mLocalMatrix       , sizeof ( mLocalMatrix        ) );
        memcpy( &mInverseWorldMatrix, &mInverseLocalMatrix, sizeof ( mInverseLocalMatrix ) );
    }

    // recurse
    for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        child->UpdateMatrix( );
    }

    blctx.restore();
}

static void
MakeBBoxHandles( ::ULIS::FRectD& iBBox, ::ULIS::FRectD iHandles[4], double iRadius, double iXFactor, double iYFactor )
{
    double pointRadiusX = iRadius * iXFactor;
    double pointRadiusY = iRadius * iYFactor;
    double pointWidth = pointRadiusX * 2.0f;
    double pointHeight = pointRadiusY * 2.0f;
    double x1 = iBBox.x
         , y1 = iBBox.y;
    double x2 = iBBox.x + iBBox.w
         , y2 = iBBox.y + iBBox.h;

    iHandles[0] = ::ULIS::FRectD::FromXYWH( x1 - pointRadiusX, y1 - pointRadiusY, pointWidth, pointHeight );
    iHandles[1] = ::ULIS::FRectD::FromXYWH( x2 - pointRadiusX, y1 - pointRadiusY, pointWidth, pointHeight );
    iHandles[2] = ::ULIS::FRectD::FromXYWH( x2 - pointRadiusX, y2 - pointRadiusY, pointWidth, pointHeight );
    iHandles[3] = ::ULIS::FRectD::FromXYWH( x1 - pointRadiusX, y2 - pointRadiusY, pointWidth, pointHeight );
}

int32
UOdysseyVectorObject::PickBBox( double iLocalX, double iLocalY )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPoint vec = mInverseWorldMatrix.mapVector( 1.0f, 1.0f );
    ::ULIS::FVec2D size = { vec.x, vec.y };
    ::ULIS::FRectD handles[4];

    MakeBBoxHandles ( mBBox, handles, BBOX_POINT_RADIUS, size.x, size.y );

    // draw the handles
    for ( int i = 0; i < 4; i++ )
    {
        ::ULIS::FVec2D pt = { iLocalX, iLocalY };
 
        if ( handles[i].HitTest( pt ) == true )
        {
            return i;
        }
    }

    return -1;
}

void
UOdysseyVectorObject::DrawBBox( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPoint vec = mInverseWorldMatrix.mapVector( 1.0f, 1.0f );
    ::ULIS::FVec2D size = { vec.x, vec.y };
    ::ULIS::FRectD handles[4];

    MakeBBoxHandles ( mBBox, handles, BBOX_POINT_RADIUS, size.x, size.y );

    blctx.setStrokeStyle( BLRgba32(0xFF8B0000) );

    blctx.setStrokeWidth( vec.y );
    blctx.strokeLine( mBBox.x, mBBox.y          , mBBox.x + mBBox.w, mBBox.y           );
    blctx.strokeLine( mBBox.x, mBBox.y + mBBox.h, mBBox.x + mBBox.w, mBBox.y + mBBox.h );

    blctx.setStrokeWidth( vec.x );
    blctx.strokeLine( mBBox.x          , mBBox.y, mBBox.x          , mBBox.y + mBBox.h );
    blctx.strokeLine( mBBox.x + mBBox.w, mBBox.y, mBBox.x + mBBox.w, mBBox.y + mBBox.h );

    blctx.setFillStyle( BLRgba32(0xFF8B0000) );

    // draw the handles (squares at rectangle corners)
    for ( int i = 0; i < 4; i++ )
    {
        blctx.fillRect( handles[i].x, handles[i].y, handles[i].w, handles[i].h );
    }
}

::ULIS::FRectD
UOdysseyVectorObject::GetBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint origin = mWorldMatrix.mapPoint( mBBox.x, mBBox.y );
        BLPoint size = mWorldMatrix.mapVector( mBBox.w, mBBox.h );
        ::ULIS::FRectD worldBBox = { origin.x, origin.y, size.x, size.y };
        
        return worldBBox;
    }

    return mBBox;
}

void
UOdysseyVectorObject::DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        child->Draw( iRoi, iFlags );
    }
}

void
UOdysseyVectorObject::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    ::ULIS::FRectD localRoi = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Adapt the Region-Of-Interest to the local coordinates
    if( iRoi.Area() != 0.0f )
    {
        BLPoint roiCornerOrigin = { iRoi.x, iRoi.y };
        BLPoint roiCornerSize = { iRoi.w, iRoi.h };
        BLPoint localRoiCornerOrigin = mInverseLocalMatrix.mapPoint( roiCornerOrigin.x, roiCornerOrigin.y );
        BLPoint localRoiCornerSize = mInverseLocalMatrix.mapVector( roiCornerSize.x, roiCornerSize.y );

        localRoi.x = localRoiCornerOrigin.x;
        localRoi.y = localRoiCornerOrigin.y;
        localRoi.w = localRoiCornerSize.x;
        localRoi.h = localRoiCornerSize.y;
    }

            /*printf("%s : %f %f %f %f\n",mName.c_str(), iRoi.x,iRoi.y,iRoi.w,iRoi.h);*/

    blctx.save();
    blctx.transform( mLocalMatrix );
    DrawShape( localRoi, iFlags );

    DrawChildren( localRoi, iFlags );

    blctx.restore();
}

bool
UOdysseyVectorObject::IsFilled()
{
    return mIsFilled;
}

bool
UOdysseyVectorObject::IsInvalidated()
{
    return mIsInvalidated;
}

bool
UOdysseyVectorObject::IsSelected()
{
    return mIsSelected;
}

void
UOdysseyVectorObject::Invalidate()
{
    if ( mIsInvalidated == false )
    {
        UOdysseyVectorObject* obj = GetRoot();

        if ( obj && ( obj != this ) )
        {
            if ( obj->GetClass() == UOdysseyVectorRoot::StaticClass() )
            {
                UOdysseyVectorRoot* root = Cast<UOdysseyVectorRoot>(obj);

                root->InvalidateObject( this );

                mIsInvalidated = true;
            }
        }
    }
}

UOdysseyVectorRoot*
UOdysseyVectorObject::GetRoot()
{
    UOdysseyVectorObject* parent = mParent;
    UOdysseyVectorObject* root = nullptr;

    while ( parent )
    {
        root = parent;

        parent = parent->GetParent();
    }

    return Cast<UOdysseyVectorRoot>(root);
}

void
UOdysseyVectorObject::SetParent( UOdysseyVectorObject* iObject )
{
    mParent = iObject;
}

::ULIS::FVec2D
UOdysseyVectorObject::WorldCoordinatesToLocal( double iX, double iY )
{
    BLPoint localCoords;
    ::ULIS::FVec2D localPoint;

    localCoords = mInverseWorldMatrix.mapPoint( iX, iY );

    localPoint.x = localCoords.x;
    localPoint.y = localCoords.y;

    return localPoint;
}

UOdysseyVectorObject*
UOdysseyVectorObject::GetParent()
{
    return mParent;
}

void
UOdysseyVectorObject::MoveBack()
{
    if ( mParent )
    {
        std::list<UOdysseyVectorObject*>::iterator it1 = mParent->mChildrenList.begin();
        std::list<UOdysseyVectorObject*>::iterator it2 = it1++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it1 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                UOdysseyVectorObject *child = static_cast<UOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    return;
                }
            }
        }
    } 
}

void
UOdysseyVectorObject::MoveFront()
{
    if ( mParent )
    {
        std::list<UOdysseyVectorObject*>::iterator it2 = mParent->mChildrenList.begin();
        std::list<UOdysseyVectorObject*>::iterator it1 = it2++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it2 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                UOdysseyVectorObject *child = static_cast<UOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    return;
                }
            }
        }
    } 
}

UOdysseyVectorObject*
UOdysseyVectorObject::Pick( double iX, double iY, double iRadius )
{
    if ( this->mParent )
    {
        if ( this->mParent->GetClass() == UOdysseyVectorGroup::StaticClass() )
        {
            return this->mParent;
        }
    }

    return PickShape( iX, iY, iRadius );
}

void
UOdysseyVectorObject::ExtractTransformations( BLMatrix2D &iMatrix
                                     , ::ULIS::FVec2D* iTranslation
                                     , double* iRotation
                                     , ::ULIS::FVec2D* iScaling )
{
    if( iTranslation )
    {
        iTranslation->x = iMatrix.m20;
        iTranslation->y = iMatrix.m21;
    }

    if( iRotation )
    {
        *iRotation = atan( iMatrix.m01  / iMatrix.m11 );
    }

    if( iScaling )
    {
        iScaling->x = sqrt( ( iMatrix.m00 * iMatrix.m00 ) + ( iMatrix.m01 * iMatrix.m01 ) );
        iScaling->y = sqrt( ( iMatrix.m10 * iMatrix.m10 ) + ( iMatrix.m11 * iMatrix.m11 ) );
    }
}

void
UOdysseyVectorObject::AppendChild( UOdysseyVectorObject* iChild )
{
    AddChild ( iChild, false );
}

void
UOdysseyVectorObject::PrependChild( UOdysseyVectorObject* iChild )
{
    AddChild ( iChild, true );
}

void
UOdysseyVectorObject::AddChild( UOdysseyVectorObject* iChild, bool iPrepend )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLMatrix2D localMatrix = this->GetInverseWorldMatrix();

    iChild->mParent = this;

    localMatrix.transform( iChild->GetWorldMatrix() );

    ExtractTransformations ( localMatrix, &iChild->mTranslation, &iChild->mRotation, &iChild->mScaling );

    iChild->UpdateMatrix();

    if ( iPrepend == true )
    {
        mChildrenList.push_front( iChild );
    }
    else
    {
        mChildrenList.push_back( iChild );
    }
}

void
UOdysseyVectorObject::RemoveChild( UOdysseyVectorObject* iChild )
{
    iChild->mParent = nullptr;

    mChildrenList.remove(iChild);
}

void
UOdysseyVectorObject::SetStrokeColor( uint32 iColor )
{
    mStrokeColor = iColor;
}

void
UOdysseyVectorObject::SetFillColor( uint32 iColor )
{
    mFillColor = iColor;
}

void
UOdysseyVectorObject::SetFilled( bool iIsFilled)
{
    mIsFilled = iIsFilled;
}

void
UOdysseyVectorObject::SetStrokeWidth( double iWidth )
{
    mStrokeWidth = iWidth;
}

double
UOdysseyVectorObject::GetStrokeWidth()
{
    return mStrokeWidth;
}

void 
UOdysseyVectorObject::CopyTransformation( UOdysseyVectorObject& iObject )
{
    iObject.mRotation    = mRotation;
    iObject.mScaling     = mScaling;
    iObject.mTranslation = mTranslation;
}

std::list<UOdysseyVectorObject*>&
UOdysseyVectorObject::GetChildrenList()
{
    return mChildrenList;
}

BLMatrix2D&
UOdysseyVectorObject::GetLocalMatrix()
{
    return mLocalMatrix;
}

BLMatrix2D&
UOdysseyVectorObject::GetWorldMatrix()
{
    return mWorldMatrix;
}

BLMatrix2D&
UOdysseyVectorObject::GetInverseWorldMatrix()
{
    return mInverseWorldMatrix;
}
