#include "OdysseyVectorScene.h"

FOdysseyVectorScene::~FOdysseyVectorScene()
{

}

FOdysseyVectorScene::FOdysseyVectorScene()
{
    mFillBucket.SetColor( 0, 0, 0, 0 );
}

void FOdysseyVectorScene::Init( std::string iName )
{
    SetName( iName );
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

FOdysseyVectorScene::FSignalDelegate&
FOdysseyVectorScene::OnSignalDelegate()
{
    static FSignalDelegate onSignalDelegate;

    return onSignalDelegate;
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
    FOdysseyVectorScene* rootCopy = new FOdysseyVectorScene();

    rootCopy->Init( Name );

    return static_cast<FOdysseyVectorObject*>(rootCopy);
}

FOdysseyVectorGroupPaint*
FOdysseyVectorScene::MakePaintGroupFromSelectedObjects( std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                      , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                      , std::vector<FOdysseyVectorObject*>& oRemovedPaintGroupArray )
{
    if( mSelectedObjectList.size() )
    {
        FOdysseyVectorGroupPaint* paintGroup = new FOdysseyVectorGroupPaint( "Paint Group" );

        AppendChild( paintGroup );

        paintGroup->UpdateMatrix();

        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
            {
                FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);
                        
                for( std::list<FOdysseyVectorObject*>::iterator cit = selectedPaintGroup->GetChildrenList().begin(); cit != selectedPaintGroup->GetChildrenList().end(); ++cit )
                {
                    FOdysseyVectorObject* childObject = (*cit);

                    if( childObject->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
                    {
                        FOdysseyVectorPathCubic* childCubicPath = static_cast<FOdysseyVectorPathCubic*>( childObject );

                        oCubicPathArray.push_back( childCubicPath );
                    }
                }

                selectedPaintGroup->GetParent()->RemoveChild( selectedPaintGroup );

                selectedPaintGroup->CopyBuckets( paintGroup, true );

                oRemovedPaintGroupArray.push_back( selectedPaintGroup );
            }

            if( selectedObject->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
            {
                FOdysseyVectorPathCubic* selectedCubicPath = static_cast<FOdysseyVectorPathCubic*>( selectedObject );

                oCubicPathArray.push_back( selectedCubicPath );
            }
        }

        oCubicPathOldParentArray.resize( oCubicPathArray.size() );

        for( int i = 0; i < oCubicPathArray.size(); i++ )
        {
            oCubicPathOldParentArray[i] = oCubicPathArray[i]->GetParent();

            paintGroup->TransferChild( oCubicPathArray[i] );
        }

        // first update to update paths' segments.
        Update( 0 );

        ClearSelection();
        Select( paintGroup );

        return paintGroup;
    }

    return nullptr;
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
        FOdysseyVectorGroup* group = new FOdysseyVectorGroup();

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

            group->TransferChild( obj );
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
FOdysseyVectorScene::DrawShape( uint64 iFlags )
{
    BLContext* blctx = GetEngine()->GetBLContext();
    static ::ULIS::FRectD zeroRectangle; // static variables are always zeroed by default
    BLRgba32 blFillColor;
    FColor& fillColor = mFillBucket.GetColor();

    blctx->setCompOp( BL_COMP_OP_SRC_COPY );

//UE_LOG(LogTemp, Warning, TEXT("Some warning message:%d %d %d %d"), roi.x, roi.y, roi.w, roi.h );

    // Note: Blend2D color format is 0xAARRGGBB
    blFillColor.setR( fillColor.B );
    blFillColor.setG( fillColor.G );
    blFillColor.setB( fillColor.R );
    blFillColor.setA( fillColor.A );

    blctx->setFillStyle( blFillColor );

    blctx->save();
    blctx->resetMatrix();
    //blctx->clearAll();
    blctx->fillAll();
    blctx->restore();

    // view the updated zone ( testing purpose only )
    /*blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
    blctx.setStrokeWidth(1.0f);
    blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
}

void
FOdysseyVectorScene::UpdateShape( uint32 iUpdateFlags )
{
}

void
FOdysseyVectorScene::Update( uint32 iUpdateFlags )
{
    FOdysseyVectorObject::Update( iUpdateFlags );
}

void
FOdysseyVectorScene::Signal( uint64 iSignalFlags )
{
    OnSignalDelegate().Broadcast( this, iSignalFlags );
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
    for(std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it)
    {
        FOdysseyVectorObject* selectedObject = (*it);

        // prevent nested removal
        if( selectedObject->HasSelectedAncestor() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
