/*****************************************************************************/
/**
 *  @file   Import.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#include "Import.h"

#include <kvs/Range>
#include <kvs/StructuredVolumeObject>


namespace
{

inline kvs::Vec3 MinExtCoord( const local::VTI& vti )
{
    return vti.origin() + vti.spacing() * 0.5f;
}

inline kvs::Vec3 MaxExtCoord( const local::VTI& vti )
{
    return MinExtCoord( vti ) + vti.spacing() * kvs::Vec3( vti.resolution() - kvs::Vec3ui::All(1) );
}

inline kvs::Vec3ui Resolution( const local::VTHB& vthb )
{
    kvs::Range xrange;
    kvs::Range yrange;
    kvs::Range zrange;
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        xrange.extend( kvs::Range( vthb.dataSet(i).amr_box[0], vthb.dataSet(i).amr_box[1] ) );
        yrange.extend( kvs::Range( vthb.dataSet(i).amr_box[2], vthb.dataSet(i).amr_box[3] ) );
        zrange.extend( kvs::Range( vthb.dataSet(i).amr_box[4], vthb.dataSet(i).amr_box[5] ) );
    }

    const size_t dimx = size_t( xrange.upper() - xrange.lower() + 1 );
    const size_t dimy = size_t( yrange.upper() - yrange.lower() + 1 );
    const size_t dimz = size_t( zrange.upper() - zrange.lower() + 1 );
    return kvs::Vec3ui( dimx, dimy, dimz );
}

inline size_t Veclen( const local::VTHB& vthb, const size_t index )
{
    return local::VTI( vthb.dataSet(0).file ).dataArray(index).ncomponents;
}

inline kvs::AnyValueArray Values(
    const local::VTHB& vthb,
    const kvs::Vec3ui resolution,
    const size_t veclen,
    const size_t index )
{
    const size_t dimx = resolution.x();
    const size_t dimy = resolution.y();
    const size_t dimz = resolution.z();
    const size_t nnodes = dimx * dimy * dimz;

    kvs::ValueArray<kvs::Real32> values( nnodes * veclen );
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        const kvs::Real32* pvalues = local::VTI( vthb.dataSet(i).file ).dataArray( index ).values.data();
        const kvs::Range xrange( vthb.dataSet(i).amr_box[0], vthb.dataSet(i).amr_box[1] );
        const kvs::Range yrange( vthb.dataSet(i).amr_box[2], vthb.dataSet(i).amr_box[3] );
        const kvs::Range zrange( vthb.dataSet(i).amr_box[4], vthb.dataSet(i).amr_box[5] );
        for ( size_t z = zrange.lower(); z <= zrange.upper(); z++ )
        {
            for ( size_t y = yrange.lower(); y <= yrange.upper(); y++ )
            {
                for ( size_t x = xrange.lower(); x <= xrange.upper(); x++ )
                {
                    const size_t index = ( dimx * dimy * z + dimx * y + x ) * veclen;
                    for ( size_t j = 0; j < veclen; j++ ) { values[ index + j ] = *(pvalues++); }
                }
            }
        }
    }

    return kvs::AnyValueArray( values );
}

inline kvs::Vec3 MinVec( const kvs::Vec3& a, const kvs::Vec3& b )
{
    return kvs::Vec3(
        kvs::Math::Min( a.x(), b.x() ),
        kvs::Math::Min( a.y(), b.y() ),
        kvs::Math::Min( a.z(), b.z() ) );
}

inline kvs::Vec3 MaxVec( const kvs::Vec3& a, const kvs::Vec3& b )
{
    return kvs::Vec3(
        kvs::Math::Max( a.x(), b.x() ),
        kvs::Math::Max( a.y(), b.y() ),
        kvs::Math::Max( a.z(), b.z() ) );
}

inline kvs::Vec3 MinExtCoord( const local::VTHB& vthb )
{
    kvs::Vec3 min_coord = kvs::Vec3::All( kvs::Value<kvs::Real32>::Max() );
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        min_coord = MinVec( min_coord, ::MinExtCoord( local::VTI( vthb.dataSet(i).file ) ) );
    }
    return min_coord;
}

inline kvs::Vec3 MaxExtCoord( const local::VTHB& vthb )
{
    kvs::Vec3 max_coord = kvs::Vec3::All( kvs::Value<kvs::Real32>::Min() );
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        max_coord = MaxVec( max_coord, ::MaxExtCoord( local::VTI( vthb.dataSet(i).file ) ) );
    }
    return max_coord;
}

} // end of namespace


namespace local
{

kvs::StructuredVolumeObject* Import( const local::VTI& vti, size_t index )
{
    const kvs::Vec3 min_ext_coord = ::MinExtCoord( vti );
    const kvs::Vec3 max_ext_coord = ::MaxExtCoord( vti );

    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridTypeToUniform();
    volume->setResolution( vti.resolution() );
    volume->setVeclen( vti.dataArray(index).ncomponents );
    volume->setValues( kvs::AnyValueArray( vti.dataArray(index).values ) );
    volume->updateMinMaxValues();
    volume->updateMinMaxCoords();
    volume->setMinMaxExternalCoords( min_ext_coord, max_ext_coord );
    return volume;
}

kvs::StructuredVolumeObject* Import( const local::VTHB& vthb, size_t index )
{
    const kvs::Vec3ui resolution = ::Resolution( vthb );
    const size_t veclen = ::Veclen( vthb, index );
    const kvs::Vec3 min_ext_coord = ::MinExtCoord( vthb );
    const kvs::Vec3 max_ext_coord = ::MaxExtCoord( vthb );

    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridTypeToUniform();
    volume->setResolution( resolution );
    volume->setVeclen( veclen );
    volume->setValues( ::Values( vthb, resolution, veclen, index ) );
    volume->updateMinMaxValues();
    volume->updateMinMaxCoords();
    volume->setMinMaxExternalCoords( min_ext_coord, max_ext_coord );
    return volume;
}

} // end of namespace local
