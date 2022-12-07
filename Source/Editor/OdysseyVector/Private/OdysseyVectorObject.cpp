#include "OdysseyVectorObject.h"

FOdysseyVectorObject::~FOdysseyVectorObject()
{
}

FOdysseyVectorObject::FOdysseyVectorObject()
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

FOdysseyVectorObject::FOdysseyVectorObject( std::string iName )
    : FOdysseyVectorObject()
{
    mName.assign( iName );
}

void
FOdysseyVectorObject::Update()
{
    UpdateShape();

    mIsInvalidated = false;
}

void
FOdysseyVectorObject::SetIsSelected( bool iIsSelected )
{
    mIsSelected = iIsSelected;
}

void
FOdysseyVectorObject::Translate( double iX, double iY )
{
    mTranslation.x = iX;
    mTranslation.y = iY;
}

void
FOdysseyVectorObject::Rotate( double iAngle )
{
    mRotation = iAngle;
}

void
FOdysseyVectorObject::Scale( double iX, double iY )
{
    mScaling.x = iX;
    mScaling.y = iY;
}

double
FOdysseyVectorObject::GetScalingX()
{
    return mScaling.x;
}

double
FOdysseyVectorObject::GetScalingY()
{
    return mScaling.y;
}

double
FOdysseyVectorObject::GetRotation()
{
    return mRotation;
}

FOdysseyVectorObject*
FOdysseyVectorObject::Copy() {
    FOdysseyVectorObject* objectCopy = CopyShape();

   // TODO, update matrices once we get a BLContext object

    if( objectCopy )
    {
        CopySettings( *objectCopy ); // we need the matrices to properly import the child

        // recurse
        for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            FOdysseyVectorObject *child = (*it);

            objectCopy->AppendChild( child->Copy() );
        }

        objectCopy->UpdateShape();
    }

    return objectCopy;
}

void
FOdysseyVectorObject::CopySettings( FOdysseyVectorObject& iDestinationObject )
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
FOdysseyVectorObject::GetTranslationX()
{
    return mTranslation.x;
}

double
FOdysseyVectorObject::GetTranslationY()
{
    return mTranslation.y;
}

void
FOdysseyVectorObject::UpdateMatrix( )
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
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        child->UpdateMatrix( );
    }

    blctx.restore();
}

::ULIS::FRectD
FOdysseyVectorObject::GetBBox( bool iWorld )
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
FOdysseyVectorObject::DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        child->Draw( iRoi, iFlags );
    }
}

void
FOdysseyVectorObject::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
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

void
FOdysseyVectorObject::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
}

FOdysseyVectorObject*
FOdysseyVectorObject::PickShape( double iX, double iY, double iRadius )
{
    return nullptr;
}

bool
FOdysseyVectorObject::IsFilled()
{
    return mIsFilled;
}

bool
FOdysseyVectorObject::IsInvalidated()
{
    return mIsInvalidated;
}

bool
FOdysseyVectorObject::IsSelected()
{
    return mIsSelected;
}

void
FOdysseyVectorObject::Invalidate()
{
    if ( mIsInvalidated == false )
    {
        FOdysseyVectorObject* obj = GetRoot();

        if ( obj && ( obj != this ) )
        {
            if ( typeid ( *obj ) == typeid ( FOdysseyVectorRoot ) )
            {
                FOdysseyVectorRoot* root = static_cast<FOdysseyVectorRoot*>(obj);

                root->InvalidateObject( this );

                mIsInvalidated = true;
            }
        }
    }
}

FOdysseyVectorRoot*
FOdysseyVectorObject::GetRoot()
{
    FOdysseyVectorObject* parent = mParent;
    FOdysseyVectorObject* root = nullptr;

    while ( parent )
    {
        root = parent;

        parent = parent->GetParent();
    }

    return static_cast<FOdysseyVectorRoot*>(root);
}

void
FOdysseyVectorObject::SetParent( FOdysseyVectorObject* iObject )
{
    mParent = iObject;
}

::ULIS::FVec2D
FOdysseyVectorObject::WorldCoordinatesToLocal( double iX, double iY )
{
    BLPoint localCoords;
    ::ULIS::FVec2D localPoint;

    localCoords = mInverseWorldMatrix.mapPoint( iX, iY );

    localPoint.x = localCoords.x;
    localPoint.y = localCoords.y;

    return localPoint;
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetParent()
{
    return mParent;
}

void
FOdysseyVectorObject::MoveBack()
{
    if ( mParent )
    {
        std::list<FOdysseyVectorObject*>::iterator it1 = mParent->mChildrenList.begin();
        std::list<FOdysseyVectorObject*>::iterator it2 = it1++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it1 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                FOdysseyVectorObject *child = static_cast<FOdysseyVectorObject*>(*it1);

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
FOdysseyVectorObject::MoveFront()
{
    if ( mParent )
    {
        std::list<FOdysseyVectorObject*>::iterator it2 = mParent->mChildrenList.begin();
        std::list<FOdysseyVectorObject*>::iterator it1 = it2++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it2 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                FOdysseyVectorObject *child = static_cast<FOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    return;
                }
            }
        }
    } 
}

FOdysseyVectorObject*
FOdysseyVectorObject::Pick( double iX, double iY, double iRadius )
{
    if ( this->mParent )
    {
        if ( typeid ( *this->mParent ) == typeid ( FOdysseyVectorGroup ) )
        {
            return this->mParent;
        }
    }

    return PickShape( iX, iY, iRadius );
}

void
FOdysseyVectorObject::ExtractTransformations( BLMatrix2D &iMatrix
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
FOdysseyVectorObject::AppendChild( FOdysseyVectorObject* iChild )
{
    AddChild ( iChild, false );
}

void
FOdysseyVectorObject::PrependChild( FOdysseyVectorObject* iChild )
{
    AddChild ( iChild, true );
}

void
FOdysseyVectorObject::AddChild( FOdysseyVectorObject* iChild, bool iPrepend )
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
FOdysseyVectorObject::RemoveChild( FOdysseyVectorObject* iChild )
{
    iChild->mParent = nullptr;

    mChildrenList.remove(iChild);
}

void
FOdysseyVectorObject::SetStrokeColor( uint32 iColor )
{
    mStrokeColor = iColor;
}

void
FOdysseyVectorObject::SetFillColor( uint32 iColor )
{
    mFillColor = iColor;
}

void
FOdysseyVectorObject::SetFilled( bool iIsFilled)
{
    mIsFilled = iIsFilled;
}

void
FOdysseyVectorObject::SetStrokeWidth( double iWidth )
{
    mStrokeWidth = iWidth;
}

double
FOdysseyVectorObject::GetStrokeWidth()
{
    return mStrokeWidth;
}

void 
FOdysseyVectorObject::CopyTransformation( FOdysseyVectorObject& iObject )
{
    iObject.mRotation    = mRotation;
    iObject.mScaling     = mScaling;
    iObject.mTranslation = mTranslation;
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorObject::GetChildrenList()
{
    return mChildrenList;
}

BLMatrix2D&
FOdysseyVectorObject::GetLocalMatrix()
{
    return mLocalMatrix;
}

BLMatrix2D&
FOdysseyVectorObject::GetWorldMatrix()
{
    return mWorldMatrix;
}

BLMatrix2D&
FOdysseyVectorObject::GetInverseWorldMatrix()
{
    return mInverseWorldMatrix;
}
