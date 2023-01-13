#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>

#include "OdysseyVectorObject.generated.h"

class UOdysseyVectorRoot;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorObject : public UObject
{
    public:
        GENERATED_BODY()

        static const uint32 PICK_POINT     = ( 1 << 0 );
        static const uint32 PICK_RECTANGLE = ( 1 << 1 );
        static const uint32 PICK_CIRCLE    = ( 1 << 2 );
        static const uint32 PICK_FREEHAND  = ( 1 << 3 );

        // DO NOT CHANGE !
        static const uint32 VECTORROOTTYPE      = 0;
        static const uint32 VECTOROBJECTTYPE    = 1;
        static const uint32 VECTORGROUPETYPE    = 2;
        static const uint32 VECTORCIRCLETYPE    = 3;
        static const uint32 VECTORRECTANGLETYPE = 4;
        static const uint32 VECTORPATHCUBICTYPE = 5;


        static constexpr float BBOX_POINT_RADIUS = 4.0f;

    protected:
        std::string Name;

        UPROPERTY()
        double TranslationX;

        UPROPERTY()
        double TranslationY;

        UPROPERTY()
        double Rotation;

        UPROPERTY()
        double ScalingX;

        UPROPERTY()
        double ScalingY;

    protected:
        BLMatrix2D mLocalMatrix;
        BLMatrix2D mInverseLocalMatrix;
        BLMatrix2D mWorldMatrix;
        BLMatrix2D mInverseWorldMatrix;
        std::list<UOdysseyVectorObject*> mChildrenList;
        UOdysseyVectorObject* mParent;
        bool mIsFilled;
        bool mIsSelected;
        bool mIsInvalidated;
        ::ULIS::FRectD mBBox;
        uint32 mStrokeColor;
        double mStrokeWidth;
        uint32 mFillColor;
        // used when saving
        uint32 mID;

    public:
        static uint32 TreeToList( UOdysseyVectorObject* iObject, std::list<UOdysseyVectorObject*>& iOutList );
        static uint32 TreeToArray( UOdysseyVectorObject* iObject, std::vector<UOdysseyVectorObject*>& iOutArray );

        ~UOdysseyVectorObject();
        UOdysseyVectorObject();
        void SetName( std::string iName );
        void CopySettings( UOdysseyVectorObject& iDestinationObject );

        void Update();
        virtual void UpdateShape() {};

        UOdysseyVectorObject* Copy();
        virtual UOdysseyVectorObject* CopyShape(){ return nullptr; };

        void Serialize(FArchive& Ar);
        virtual void SerializeShape(FArchive& Ar);

        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        virtual void DrawShape ( ::ULIS::FRectD &roi, uint64 iFlags ){};

        virtual void DrawStructure ( ::ULIS::FRectD &roi, uint64 iFlags ){};

        virtual uint32 GetType();

        UOdysseyVectorObject* Pick( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        virtual UOdysseyVectorObject* PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ){ return nullptr; };

        /*virtual void UpdateBoundingBox() = 0;*/
        void DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void UpdateMatrix( );
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        void PrependChild( UOdysseyVectorObject* iChild );
        void AppendChild( UOdysseyVectorObject* iChild );
        void AddChild( UOdysseyVectorObject* iChild, bool iPrepend );
        void RemoveChild( UOdysseyVectorObject* iChild );
        void ImportChild( UOdysseyVectorObject* iChild, BLMatrix2D& iInverseWorldMatrix );
        double GetScalingX();
        double GetScalingY();
        double GetTranslationX();
        double GetTranslationY();
        double GetRotation();
        void SetID( uint32 iID );
        uint32 GetID();
        UOdysseyVectorObject* GetParent();
        void SetParent( UOdysseyVectorObject* iObject );
        void CopyTransformation( UOdysseyVectorObject& iObject );
        BLMatrix2D& GetLocalMatrix();
        BLMatrix2D& GetWorldMatrix();
        BLMatrix2D& GetInverseWorldMatrix();
        std::list<UOdysseyVectorObject*>& GetChildrenList();
        void SetStrokeColor( uint32 iColor );
        void SetFillColor( uint32 iColor );
        void SetFilled(bool iIsFilled);
        void SetStrokeWidth( double iWidth );
        double GetStrokeWidth( );
        ::ULIS::FVec2D WorldCoordinatesToLocal( double iX, double iY );
        void SetIsSelected( bool iIsSelected );
        ::ULIS::FRectD GetBBox( bool iWorld );
        bool IsFilled();
        void MoveBack();
        void MoveFront();
        void Invalidate();
        UOdysseyVectorRoot* GetRoot();
        bool IsInvalidated();
        bool IsSelected();
        void DrawBBox( ::ULIS::FRectD& iRoi,uint64 iFlags );
        int32 PickBBox( double iX, double iY );
};
