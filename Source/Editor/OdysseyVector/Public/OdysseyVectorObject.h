#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorBucket.h"

#include "OdysseyVectorObject.generated.h"

class FOdysseyVectorScene;
class FOdysseyVectorGroup;

USTRUCT()
struct FObjectParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Identity")
    FString Name;

    UPROPERTY(EditAnywhere,Category="Appearance", meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
    double Opacity;

    UPROPERTY(EditAnywhere, Category="Transform")
    double TranslationX;

    UPROPERTY(EditAnywhere, Category="Transform")
    double TranslationY;

    //UPROPERTY(EditAnywhere, Category="Transform")
    //double TranslationZ;

    UPROPERTY(EditAnywhere, Category="Transform")
    double Rotation;

    UPROPERTY(EditAnywhere, Category="Transform")
    double ScalingX;

    UPROPERTY(EditAnywhere, Category="Transform")
    double ScalingY;

    //UPROPERTY(EditAnywhere, Category="Coloring")
    //FColor Foreground;
};

class ODYSSEYVECTOR_API FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x84cd3d16; // value is crc32 FOdysseyVectorObject

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

        static const uint32 PICK_MATH_BASED = ( 1 << 0 );
        static const uint32 PICK_MASK_BASED = ( 1 << 3 );

        // DO NOT CHANGE ! These values are saved in the save file.
        static const uint32 VECTORROOTTYPE       = 0;
        static const uint32 VECTOROBJECTTYPE     = 1;
        static const uint32 VECTORGROUPTYPE      = 2;
        static const uint32 VECTORELLIPSETYPE    = 3;
        static const uint32 VECTORRECTANGLETYPE  = 4;
        static const uint32 VECTORPATHTYPE       = 5;
        static const uint32 VECTORGROUPPAINTTYPE = 6;

        // drawing mask
        static const uint32 DRAWING_IGNORECOLOR  = ( 1 << 0 );

        // update mask
        //static const uint32 FREQUENTUPDATES = ( 1 << 0 );
        static const uint32 KEEPINVALIDATED   = ( 1 << 1 );
        static const uint32 UPDATEPAINTGROUPS = ( 1 << 2 );

        // invalidation mask
        static const uint32 INVALIDATE_CHILD       = ( 1 << 0 );
        static const uint32 INVALIDATE_MATRIX      = ( 1 << 1 );
        //static const uint32 INVALIDATE_PARENT      = ( 1 << 2 ); // must not be set manually
        static const uint32 INVALIDATE_SHAPE       = ( 1 << 3 );
        static const uint32 INVALIDATE_COLOR       = ( 1 << 4 );
        static const uint32 INVALIDATE_ALL         = ( INVALIDATE_SHAPE | INVALIDATE_COLOR );

        static constexpr float BBOX_POINT_RADIUS = 4.0f;

    public:
        FObjectParam mObjectParam;

    protected:
        BLMatrix2D mLocalMatrix;
        BLMatrix2D mInverseLocalMatrix;
        BLMatrix2D mWorldMatrix;
        BLMatrix2D mInverseWorldMatrix;
        std::list<FOdysseyVectorObject*> mChildrenList;
        std::list<FOdysseyVectorObject*> mInvalidatedChildrenList;
        FOdysseyVectorObject* mParent;
        bool mIsSelected;
        bool mIsExpanded;
        ::ULIS::FRectD mBBox;

        FOdysseyVectorBucket mBackgroundBucket;
        FOdysseyVectorBucket mForegroundBucket;

        /*uint32 mStrokeColor;*/
        /*uint32 mFillColor;*/

        // used when saving
        uint32 mID;
        uint32 mInvalidationFlags;

    public:
        static uint32 TreeToList( FOdysseyVectorObject* iObject, std::list<FOdysseyVectorObject*>& iOutList );
        static uint32 TreeToArray( FOdysseyVectorObject* iObject, std::vector<FOdysseyVectorObject*>& iOutArray );
        static ::ULIS::FRectD GetBoundingBoxFromList( std::list<FOdysseyVectorObject*>& iObjectList );
        static uint32 GetCommonClass( std::list<FOdysseyVectorObject*>& iObjectList );

        virtual ~FOdysseyVectorObject();
        FOdysseyVectorObject( const FString& iName );
        void SetName( const FString& iName );
        void CopySettings( FOdysseyVectorObject& iDestinationObject );

        void Transfer( const BLMatrix2D& iMatrix );
        void GetTransform( double& oTranslationX
                         , double& oTranslationY
                         , double& oRotation
                         , double& oScalingX
                         , double& oScalingY );

        void SetTransform( double iTranslationX
                         , double iTranslationY
                         , double iRotation
                         , double iScalingX
                         , double iScalingY );

        virtual void Update( uint32 iUpdateFlags );
        virtual void UpdateShape( uint32 iUpdateFlags );

        virtual FOdysseyVectorObject* Copy();
        virtual FOdysseyVectorObject* CopyShape(){ return nullptr; };

        virtual void Draw( uint64 iFlags );
        virtual void DrawShape ( uint64 iFlags ){};

        virtual void DrawStructure ( uint64 iFlags ){};

        virtual uint32 GetType();

        FOdysseyVectorObject* Pick( FOdysseyVectorGroup* iSelectionSpace, const ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        virtual bool PickShape( const ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ){ return false; };

        FOdysseyVectorBucket& GetBackgroundBucket();
        FOdysseyVectorBucket& GetForegroundBucket();

        virtual void TransferChild( FOdysseyVectorObject* iFosterChild, FOdysseyVectorObject* iInsertAfter );
        /*virtual void UpdateBoundingBox() = 0;*/
        virtual void DrawChildren( uint64 iFlags );
        void UpdateMatrix( );
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        void PrependChild( FOdysseyVectorObject* iChild );
        void AppendChild( FOdysseyVectorObject* iChild );


        virtual void AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter );
        virtual void RemoveChild( FOdysseyVectorObject* iChild );

        void ImportChild( FOdysseyVectorObject* iChild, BLMatrix2D& iInverseWorldMatrix );
        double GetScalingX();
        double GetScalingY();
        double GetTranslationX();
        double GetTranslationY();
        double GetRotation();
        void ResetTransform();
        void SetID( uint32 iID );
        uint32 GetID();
        FOdysseyVectorObject* GetParent();
        void SetParent( FOdysseyVectorObject* iObject );
        void CopyTransformation( FOdysseyVectorObject& iObject );
        BLMatrix2D& GetLocalMatrix();
        BLMatrix2D& GetWorldMatrix();
        BLMatrix2D& GetInverseWorldMatrix();
        BLMatrix2D& GetInverseLocalMatrix();
        std::list<FOdysseyVectorObject*>& GetChildrenList();
        void SetForegroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetBackgroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetForegroundColor( FColor& iColor );
        void SetBackgroundColor( FColor& iColor );
        void SetFilled(bool iIsFilled);
        void SetStrokeWidth( double iWidth );
        double GetStrokeWidth( );
        ::ULIS::FVec2D WorldCoordinatesToLocal( double iX, double iY );
        void SetIsSelected( bool iIsSelected );
        ::ULIS::FRectD GetBBox( bool iWorld );
        void SendBackward();
        void BringForward();
        virtual void Invalidate( uint32 iInvalidationFlags );
        virtual void Invalidate();
        virtual void InvalidateChild( FOdysseyVectorObject* iChild );
        FOdysseyVectorScene* GetScene();
        bool IsInvalidated();
        bool IsSelected();
        void SetExpanded( bool iIsExpanded );
        bool IsExpanded();
        void DrawBBox( ::ULIS::FRectD& iRoi,uint64 iFlags );
        int32 PickBBox( double iX, double iY );
        bool HasSelectedAncestor();
        bool HasAncestor( FOdysseyVectorObject* iCandidateAncestor );
        FColor GetForegroundColor();
        FColor GetBackgroundColor();
        virtual void SwitchSpace( FOdysseyVectorObject& iNewSpace ){};
        //void PropertyChanged( const FName& iPropertyName );
        void UpdateMatrix( bool iRunTransformCallback );
        FString& GetName();
        FOdysseyVectorObject* GetLastChild();
        FOdysseyVectorObject* GetPreviousChild( FOdysseyVectorObject* iChild );
        virtual void ApplyTransformations();
        virtual void ApplyMatrix( BLMatrix2D& iMatrix );
        void SetOpacity( double iOpacity );
        double GetOpacity();
};
