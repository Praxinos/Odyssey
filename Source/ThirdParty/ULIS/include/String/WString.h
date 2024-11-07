// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         String.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FWString class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FWString
/// @brief      The FWString class provides a simple dynamic string class
/// @details    The need of a custom FWString class rose from the fact that std::
///             classes cannot be exported easily accross dll boundaries when
///             ULIS is compiled as a shared library.
///
///             \sa FString
class ULIS_API FWString
{
public:
    typedef wchar_t char_type;

public:
    /* Destructor. */
    ~FWString();

    /* Default constructor. */
    FWString();

    /* Constructor from C string. */
    FWString( const char_type* iStr );

    /* Copy constructor. */
    FWString( const FWString& iStr );

    /* Move constructor. */
    FWString( FWString&& iStr );

    /* Conversion constructor from WString. */
    FWString( const FString& iWStr );

    /* Copy assignment operator. */
    FWString& operator=( const FWString& iStr );

    /* Move assignment operator. */
    FWString& operator=( FWString&& iStr );

    /* Request explicit copy of string */
    FWString Copy() const;

    /* Request substring of string */
    FWString SubString( uint64 iPos, uint64 iCount ) const;

    /*! Returns the string capacity. */
    uint64 Capacity() const;

    /*! Returns the string size. */
    uint64 Size() const;

    /*! Returns the const raw storage. */
    const char_type* Data() const;

    /*! Access char_type at index. */
    char_type& operator[]( uint64 iIndex );

    /*! Access const char_type at index. */
    const char_type& operator[]( uint64 iIndex ) const;

    /*! Access char_type at index. */
    char_type& At( uint64 iIndex );

    /*! Access const char_type at index. */
    const char_type& At( uint64 iIndex ) const;

    /*! Access char_type at front. */
    char_type& Front();

    /*! Access const char_type at front. */
    const char_type& Front() const;

    /*! Access char_type at back. */
    char_type& Back();

    /*! Access const char_type at back. */
    const char_type& Back() const;

    /*! Convert to WString. */
    FString ToString() const;

    /*! Append a string. */
    FWString& Append( const FWString& iStr );

    /*! Append a string. */
    FWString& Append( const char_type* iStr );

    /*! Append a char. */
    FWString& Append( char_type iChar );

    /*! Prepend a string. */
    FWString& Prepend( const FWString& iStr );

    /*! Prepend a string. */
    FWString& Prepend( const char_type* iStr );

    /*! Prepend a char. */
    FWString& Prepend( char_type iChar );

    /* Equality operator */
    bool operator==( const FWString& iOther );

    /* Inequality operator */
    bool operator!=( const FWString& iOther );

    /* To Int */
    int ToInt() const;

    /* To Unsigned Int */
    unsigned int ToUnsignedInt() const;

    /* To Float */
    float ToFloat() const;

    /* To Double */
    double ToDouble() const;

    /* Static Maker from Int */
    static FWString FromInt( int iValue );

    /* Static Maker from Unsigned Int */
    static FWString FromUnsignedInt( unsigned int iValue );

    /* Static Maker from Float */
    static FWString FromFloat( float iValue );

    /* Static Maker from Double */
    static FWString FromDouble( double iValue );

    /* Static Maker from WString */
    static FWString FromString( const FString& iValue );

    /* Transform in place to uppercase */
    FWString& TransformUpperCase();

    /* Transform in place to lowercase */
    FWString& TransformLowerCase();

    /* Conversion to uppercase */
    FWString ToUpper() const;

    /* Conversion to lowercase */
    FWString ToLower() const;

    /*! Returns wether the array is empty or not. */
    bool IsEmpty() const;

    /*!
        Reserve some capacity for future usage, if iCapacity is bigger than the
        current capacity. If iCapacity is lower or equal to capacity, this
        function does nothing.
        This function might invalid the underlying buffer.
        The size doesn't change.
    */
    void Reserve( uint64 iCapacity );

    /*!
        Shrink the capacity to fit the size.
        If the capacity is already the same as the size, the function does
        nothing.
        This function might invalid the underlying buffer.
        The size doesn't change.
    */
    void Shrink();

    /*!
        Resize the actual usage size of the container.
        This function might invalid the underlying buffer.
    */
    void Resize( uint64 iSize, char_type iChar = char_type() );

    /*! Clear the string, deallocating everything. */
    void Clear();

    /*! Split the string. */
    TArray< FWString > Split( const FWString& iSep ) const;

    /* Erase part of string */
    FWString& Erase( uint64 iPos, uint64 iCount );

    /* Insert part of string */
    FWString& Insert( uint64 iPos, const FWString& iStr );

    /* Find First occurence */
    int64 FindFirst( const FWString& iStr ) const;

    /* Find Last occurence */
    int64 FindLast( const FWString& iStr ) const;

    /* Find occurence starting for position */
    int64 FindFrom( uint64 iPos, const FWString& iStr ) const;

    /* Replace First occurence */
    bool ReplaceFirst( const FWString& iA, const FWString& iB );

    /* Replace Last occurence */
    bool ReplaceLast( const FWString& iA, const FWString& iB );

    /* Replace occurence starting for position */
    bool ReplaceFrom( uint64 iPos, const FWString& iA, const FWString& iB );

    /* Replace all occurences */
    void ReplaceAll( const FWString& iA, const FWString& iB );

private:
    void CleanupBulk();
    void GrowBulk( uint64 iSize, uint64 iCopyOffset = 0 );
    FWString( uint64 iSize );

private:
    char_type* mBulk; ///< The main raw string buffer storage. A pascal string or C string with trailing null termination character.
    uint64 mCapacity; ///< The string capacity, may be bigger than size. The real size of the buffer storage, including the null termination character.
    uint64 mSize; ///< The string usage size, not including the null termination character, althoug other null characters can be inside the usable string.
};

ULIS_NAMESPACE_END

