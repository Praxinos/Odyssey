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

    public:
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

        UOdysseyVectorObject* Pick( double iX, double iY, double iRadius );
        virtual UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ){ return nullptr; };

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
