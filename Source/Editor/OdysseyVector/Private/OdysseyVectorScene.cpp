#include "OdysseyVectorScene.h"

FOdysseyVectorScene::~FOdysseyVectorScene()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorScene::FOdysseyVectorScene( const FString& iName )
    : FOdysseyVectorGroupPaint( iName )
{
    mBackgroundBucket.SetSolidColor( 0, 0, 0, 0 );

    SetExpanded( true );
}

bool
FOdysseyVectorScene::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorGroupPaint::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorScene::SetEngine( FOdysseyVectorEngine* iEngine )
{
    mEngine = iEngine;
}

FOdysseyVectorEngine*
FOdysseyVectorScene::GetEngine()
{
    return mEngine;
}

void
FOdysseyVectorScene::ClearSelection()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorScene::Unselect( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorScene::Select( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

FOdysseyVectorObject*
FOdysseyVectorScene::CopyShape()
{
    FOdysseyVectorScene* rootCopy = new FOdysseyVectorScene( mObjectParam.Name );

    return static_cast<FOdysseyVectorObject*>(rootCopy);
}

FOdysseyVectorGroupPaint*
FOdysseyVectorScene::MakePaintGroupFromSelectedObjects( std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                      , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                      , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray )
{
    // this array will help us to transfer buckets as well
    std::vector<FOdysseyVectorGroupPaint*> parentPaintGroupArray;

    if( mSelectedObjectList.size() )
    {
        FOdysseyVectorGroupPaint* paintGroup = new FOdysseyVectorGroupPaint( "Paint Group" );

        AppendChild( paintGroup );

        paintGroup->UpdateMatrix();

        for( FOdysseyVectorObject* selectedObject : mSelectedObjectList )
        {
            if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* selectedCubicPath = static_cast<FOdysseyVectorPath*>( selectedObject );

                oCubicPathArray.push_back( selectedCubicPath );
            }
        }

        oCubicPathOldParentArray.resize( oCubicPathArray.size() );

        for( int i = 0; i < oCubicPathArray.size(); i++ )
        {
            FOdysseyVectorObject* parentObject = oCubicPathArray[i]->GetParent();

            oCubicPathOldParentArray[i] = parentObject;

            paintGroup->TransferChild( oCubicPathArray[i], paintGroup->GetLastChild() );

            // take andvantge of this loop to also extract buckets if over
            if( parentObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(parentObject);

                if( std::find( parentPaintGroupArray.begin(), parentPaintGroupArray.end(), parentPaintGroup ) == parentPaintGroupArray.end() )
                {
                    parentPaintGroupArray.push_back( parentPaintGroup );
                }
            }
        }

        // update paths and detect cycles for bucket matching
        paintGroup->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        // extract buckets if over
        for( FOdysseyVectorGroupPaint* parentPaintGroup : parentPaintGroupArray )
        {
            std::list<FOdysseyVectorBucket*>& bucketList = parentPaintGroup->GetBucketList();
            BLMatrix2D& parentWorldMatrix = parentPaintGroup->GetWorldMatrix();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                ::ULIS::FVec2D& bucketCoords = bucket->GetCoords();
                BLPoint bucketWorldCoords = parentWorldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );

                // TODO: that's a lot of conversion, kowing that both PickCycle 
                // and FOdysseyVectorBucket() convert to their own space.
                // We can optimize by creating a PickCycle
                // with local coordinates as parameter. Same for FOdysseyVectorBucket()
                if( paintGroup->PickCycle( bucketWorldCoords.x, bucketWorldCoords.y ) )
                {
                    FOdysseyVectorBucket* importedBucket = new FOdysseyVectorBucket( paintGroup, bucket );

                    paintGroup->AddBucket( importedBucket );

                    oRemovedBucketArray.push_back( bucket );
                }
            }
        }

        // we can't remove the bucket in the previous loop as it would alter the std:list
        // we are looping into (unless we copy the list, yes I know).
        for( int i = 0; i < oRemovedBucketArray.size(); i++ )
        {
            FOdysseyVectorObject* ownerObject = oRemovedBucketArray[i]->GetOwner();

            if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);
            
                parentPaintGroup->RemoveBucket( oRemovedBucketArray[i] );
            }
        }

        // re-colorize
        Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        ClearSelection();
        Select( paintGroup );

        return paintGroup;
    }

    return nullptr;
}

::ULIS::FVec2D
FOdysseyVectorScene::GetWorldPositionFromSelection()
{
    BLPoint averagePosition = BLPoint( 0.0f, 0.0f );

    if ( mSelectedObjectList.size() )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject *obj = (*it);
            ::ULIS::FRectD bbox = obj->GetBBox( true );
            BLPoint middle = BLPoint( bbox.x + bbox.w * 0.5f
                                    , bbox.y + bbox.h * 0.5f );

            averagePosition.x += middle.x;
            averagePosition.y += middle.y;
        }

        averagePosition.x /= mSelectedObjectList.size();
        averagePosition.y /= mSelectedObjectList.size();
    }

    return ::ULIS::FVec2D( averagePosition.x, averagePosition.y );
}

void
FOdysseyVectorScene::FlipSelectionHorizontal( bool iWorld )
{
    FlipSelection( iWorld, -1.0f, 1.0f );
}

void
FOdysseyVectorScene::FlipSelectionVertical( bool iWorld )
{
    FlipSelection( iWorld, 1.0f, -1.0f );
}

void
FOdysseyVectorScene::FlipSelection( bool iWorld, double iXFactor, double iYFactor )
{
    ::ULIS::FVec2D axisPosition = GetWorldPositionFromSelection();
    BLMatrix2D inverseAxisMatrix;
    BLMatrix2D axisMatrix;
    BLMatrix2D flippingMatrix;

    axisMatrix.reset();
    axisMatrix.translate( axisPosition.x, axisPosition.y );

    BLMatrix2D::invert( inverseAxisMatrix, axisMatrix );

    flippingMatrix.resetToScaling( iXFactor, iYFactor );

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *object = (*it);

        if( object->HasSelectedAncestor() == false )
        {
            BLMatrix2D& objectWorldMatrix = object->GetWorldMatrix();
            BLPoint objectWorldCenter = objectWorldMatrix.mapPoint( 0.0f, 0.0f );
            BLPoint objectLocalCenter = inverseAxisMatrix.mapPoint( objectWorldCenter ); 
            BLPoint objectLocalFlippedCenter = flippingMatrix.mapPoint( objectLocalCenter );
            BLPoint objectWorldFlippedCenter = axisMatrix.mapPoint( objectLocalFlippedCenter );

            objectLocalCenter = object->GetParent()->GetInverseWorldMatrix().mapPoint( objectWorldFlippedCenter );

            object->Translate( objectLocalCenter.x, objectLocalCenter.y );
            object->Scale( iXFactor * object->GetScalingX(), iYFactor * object->GetScalingY() );
            object->Rotate( -object->GetRotation() );

            object->UpdateMatrix();
        }
    }
            
/*
            BLMatrix2D objectAxisMatrix;
            BLMatrix2D finalLocalMatrix;
            BLMatrix2D finalWorldMatrix;

            FOdysseyVector::MatrixMultiply( inverseAxisMatrix, objectWorldMatrix, objectAxisMatrix );
            FOdysseyVector::MatrixMultiply( iFlippingMatrix, objectAxisMatrix, finalLocalMatrix );
            FOdysseyVector::MatrixMultiply( axisMatrix, finalLocalMatrix, finalWorldMatrix );

            objectWorldMatrix = finalWorldMatrix;

            object->Transfer( object->GetParent()->GetWorldMatrix() );
            object->UpdateMatrix();
        }
    }
*/
}

FOdysseyVectorGroup*
FOdysseyVectorScene::GroupSelectedObjects( std::vector<FOdysseyVectorObject*>& oObjectArray
                                         , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray )
{
    BLPoint averageTranslation = { 0.0f, 0.0f };
/*
    if ( mSelectedObjectList.size() )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject *obj = (*it);
            BLPoint origin = obj->GetWorldMatrix().mapPoint( 0.0f, 0.0f );

            averageTranslation.x += origin.x;
            averageTranslation.y += origin.y;
        }

        averageTranslation.x /= mSelectedObjectList.size();
        averageTranslation.y /= mSelectedObjectList.size();

        averageTranslation = this->GetInverseWorldMatrix().mapPoint ( averageTranslation.x, averageTranslation.y );
    }
*/
    if ( mSelectedObjectList.size() )
    {
        FOdysseyVectorGroup* group = new FOdysseyVectorGroup( FString("Group") );

        AppendChild ( group );

        //group->Translate( averageTranslation.x, averageTranslation.y );
        group->UpdateMatrix();

        oObjectArray.reserve( mSelectedObjectList.size() );
        oObjectOldParentArray.reserve( mSelectedObjectList.size() );

        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject *obj = (*it);

            oObjectArray.push_back( obj );
            oObjectOldParentArray.push_back( obj->GetParent() );

            group->TransferChild( obj, group->GetLastChild() );
        }

        group->Invalidate();
        group->Update( 0 );

        return group;
    }

    return nullptr;
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorScene::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

void
FOdysseyVectorScene::DrawShape( BLContext* iBLContext, double iCombinedOpacity, uint64 iDrawingFlags )
{
    static ::ULIS::FRectD zeroRectangle; // static variables are always zeroed by default
    BLRgba32 blFillColor;
    FColor fillColor = ( iDrawingFlags & FOdysseyVectorObject::DRAWING_IGNORECOLOR ) ? mGroupPaintParam.MonochromeColor
                                                                                     : mBackgroundBucket.GetColor();

    iBLContext->save();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

//UE_LOG(LogTemp, Warning, TEXT("Some warning message:%d %d %d %d"), roi.x, roi.y, roi.w, roi.h );

    blFillColor.setR( fillColor.R );
    blFillColor.setG( fillColor.G );
    blFillColor.setB( fillColor.B );
    blFillColor.setA( fillColor.A * iCombinedOpacity );

    iBLContext->setFillStyle( blFillColor );

    //iBLContext->resetMatrix();
    //blctx->clearAll();
    iBLContext->fillAll();


    FOdysseyVectorGroupPaint::DrawShape( iBLContext, iCombinedOpacity, iDrawingFlags );

    // view the updated zone ( testing purpose only )
    /*iBLContext.setStrokeStyle(BLRgba32(0xFFFF0000));
    iBLContext.setStrokeWidth(1.0f);
    iBLContext.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
    iBLContext->restore();
}

FOdysseyVectorObject*
FOdysseyVectorScene::GetLastSelected()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

uint32
FOdysseyVectorScene::GetType()
{
    return FOdysseyVectorObject::VECTORROOTTYPE;
}

void
FOdysseyVectorScene::RemoveSelectedObjects()
{
    for( FOdysseyVectorObject* selectedObject : mSelectedObjectList )
    {
        // prevent nested removal
        if( selectedObject->HasSelectedAncestor() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
