#include "View/OdysseyVectorViewObject.h"

UOdysseyVectorViewObject::~UOdysseyVectorViewObject()
{

}

UOdysseyVectorViewObject::UOdysseyVectorViewObject()
    : mObject( nullptr )
{
}

void
UOdysseyVectorViewObject::ImportParam( FOdysseyVectorObject* iObject )
{
     ObjectParam = iObject->mObjectParam;
}

void
UOdysseyVectorViewObject::ExportParam( FOdysseyVectorObject* iObject )
{
    iObject->mObjectParam = ObjectParam;
}

void 
UOdysseyVectorViewObject::Update( FOdysseyVectorObject* iObject )
{
    mObject = iObject;

    ImportParam( mObject );
}

void
UOdysseyVectorViewObject::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }
}

void
UOdysseyVectorViewObject::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    if( mObject )
    {
        FOdysseyVectorScene* scene = mObject->GetScene();

        ExportParam( mObject );

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        // call delegates
        scene->mRefreshLayer.Broadcast(scene);
    }
}
