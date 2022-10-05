// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Containers/Array.h"
#include "Math/IntPoint.h"

template<typename InElementType, typename InAllocator>
class TOdysseyArray2D
{
    template <typename OtherInElementType, typename OtherAllocator>
	friend class TOdysseyArray2D;

public:
	typedef InElementType ElementType;
	typedef InAllocator   Allocator;

public:
    /** Constructor. */
	FORCEINLINE TOdysseyArray2D()
		: mArray()
        , mSize()
	{
    }

    /** Constructor.
    *
    * @param iSize The 
    */
	FORCEINLINE TOdysseyArray2D(int iSizeX, int iSizeY)
		: mArray()
        , mSize()
	{
        Resize(iSizeX, iSizeY);
    }

    /**
	 * Copy constructor. Use the common routine to perform the copy.
	 *
	 * @param Other The source array to copy.
	 */
	FORCEINLINE TOdysseyArray2D(const TOdysseyArray2D& Other)
	{
		mArray = Other.mArray;
        mSize = Other.mSize;
	}

public:
    /**
	 * Assignment operator.
	 * @param Other The source array to assign from.
	 */
	TOdysseyArray2D& operator=(const TOdysseyArray2D& Other)
	{
        if (this != &Other)
		{
		    mArray = Other.mArray;
			mSize = Other.mSize;
        }
		return *this;
	}

    /**
	 * Returns number of elements in array in X and Y directions.
	 *
	 * @returns Number of elements in array.
	 */
	FORCEINLINE FIntPoint Num() const
	{
		return mSize;
	}

    /**
	 * An alias for Num()
	 *
	 * @returns Number of elements in array.
	 */
	FORCEINLINE FIntPoint Size() const
	{
		return mSize;
	}

    /**
	 * Resizes the Array dimensions
	 *
	 */
	FORCEINLINE void Resize(int iSizeX, int iSizeY)
	{
        mSize = FIntPoint(iSizeX, iSizeY);
		mArray.SetNum(iSizeY);

        for(int i = 0; i < iSizeY; i++)
        {
            mArray[i].SetNum(iSizeX);
        }
	}

    /**
	 * Sets the given element at the given index
	 *
	 * @returns Index of the element to get
	 */
	FORCEINLINE
    void Set(int iX, int iY, const ElementType& iElement)
	{
		mArray[iY][iX] = iElement;
	}

    /**
	 * Returns the element at the given index
	 *
	 * @returns Index of the element to get
	 */
	FORCEINLINE
    ElementType& Get(int iX, int iY)
	{
		return mArray[iY][iX];
	}

    /**
	 * Returns the element at the given index
	 *
	 * @returns Index of the element to get
	 */
	FORCEINLINE
    const ElementType& Get(int iX, int iY) const
	{
		return mArray[iY][iX];
	}

private:
    TArray<TArray<InElementType, InAllocator>, InAllocator> mArray; //[y][x]
    FIntPoint mSize;
};

template<typename T, typename Allocator = FDefaultAllocator> class TOdysseyArray2D;