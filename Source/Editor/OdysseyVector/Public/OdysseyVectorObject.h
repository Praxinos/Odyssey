#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorRoot;

class FOdysseyVectorObject : public UObject
{
    protected:
        ::ULIS::FVec2D mTranslation;
        double mRotation;
        ::ULIS::FVec2D mScaling;
        BLMatrix2D mLocalMatrix;
        BLMatrix2D mInverseLocalMatrix;
        BLMatrix2D mWorldMatrix;
        BLMatrix2D mInverseWorldMatrix;
        std::list<FOdysseyVectorObject*> mChildrenList;
        uint32 mStrokeColor;
        double mStrokeWidth;
        uint32 mFillColor;
        FOdysseyVectorObject* mParent;
        bool mIsFilled;
        bool mIsSelected;
        bool mIsInvalidated;
        ::ULIS::FRectD mBBox;
        std::string mName;

    public:
        ~FOdysseyVectorObject();
        FOdysseyVectorObject();
        FOdysseyVectorObject( std::string iName );
        void CopySettings( FOdysseyVectorObject& iDestinationObject );
        virtual void Update() final ; // cannot be overridden
        virtual void UpdateShape() = 0;
        virtual FOdysseyVectorObject* Copy() final ; // cannot be overridden
        virtual FOdysseyVectorObject* CopyShape() = 0;
        virtual void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags ) final; // cannot be overridden
        virtual void DrawShape( ::ULIS::FRectD &roi, uint64 iFlags ) = 0;
        virtual FOdysseyVectorObject* Pick( double iX, double iY, double iRadius ) final; // cannot be overridden
        virtual FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) = 0;
        /*virtual void UpdateBoundingBox() = 0;*/
        void DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void UpdateMatrix( );
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        void PrependChild( FOdysseyVectorObject* iChild );
        void AppendChild( FOdysseyVectorObject* iChild );
        void AddChild( FOdysseyVectorObject* iChild, bool iPrepend );
        void RemoveChild( FOdysseyVectorObject* iChild );
        void ImportChild( FOdysseyVectorObject* iChild, BLMatrix2D& iInverseWorldMatrix );
        static void ExtractTransformations( BLMatrix2D &iMatrix, ::ULIS::FVec2D* iTranslation, double* iRotation, ::ULIS::FVec2D* iScaling );
        double GetScalingX();
        double GetScalingY();
        double GetTranslationX();
        double GetTranslationY();
        double GetRotation();
        FOdysseyVectorObject* GetParent();
        void SetParent( FOdysseyVectorObject* iObject );
        void CopyTransformation( FOdysseyVectorObject& iObject );
        BLMatrix2D& GetLocalMatrix();
        BLMatrix2D& GetWorldMatrix();
        BLMatrix2D& GetInverseWorldMatrix();
        std::list<FOdysseyVectorObject*>& GetChildrenList();
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
        FOdysseyVectorRoot* GetRoot();
        bool IsInvalidated();
        bool IsSelected();
};
