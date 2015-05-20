/*****************************************************************************/
/**
 *  @file   main.cpp
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
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/Range>
#include "VTHB.h"
#include "VTI.h"

#include <kvs/StructuredVolumeObject>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>

kvs::StructuredVolumeObject* Import( local::VTI& vti, size_t index )
{
    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridTypeToUniform();
    volume->setResolution( vti.resolution() );
    volume->setVeclen( vti.dataArray(index).ncomponents );
    volume->setValues( kvs::AnyValueArray( vti.dataArray(index).values ) );
    return volume;
}

kvs::StructuredVolumeObject* Import( local::VTHB& vthb, size_t index )
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
    const kvs::Vec3ui resolution( dimx, dimy, dimz );
    const size_t veclen = local::VTI( vthb.dataSet(0).file ).dataArray(index).ncomponents;

    const size_t nnodes = dimx * dimy * dimz;
    kvs::ValueArray<kvs::Real32> values( nnodes * veclen );
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        const kvs::Real32* pvalues = local::VTI( vthb.dataSet(i).file ).dataArray(index).values.data();
        for ( size_t z = vthb.dataSet(i).amr_box[4]; z <= vthb.dataSet(i).amr_box[5]; z++ )
        {
            for ( size_t y = vthb.dataSet(i).amr_box[2]; y <= vthb.dataSet(i).amr_box[3]; y++ )
            {
                for ( size_t x = vthb.dataSet(i).amr_box[0]; x <= vthb.dataSet(i).amr_box[1]; x++ )
                {
                    const size_t index = ( dimx * dimy * z + dimx * y + x ) * veclen;
                    for ( size_t j = 0; j < veclen; j++ ) { values[ index + j ] = *(pvalues++); }
                }
            }
        }
    }

    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridTypeToUniform();
    volume->setResolution( resolution );
    volume->setVeclen( veclen );
    volume->setValues( kvs::AnyValueArray( values ) );
    return volume;
}

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.show();

    local::VTHB vthb( argv[1] );
    for ( size_t i = 0; i < vthb.dataSetSize(); i++ )
    {
        std::cout << vthb.dataSet(i).file << std::endl;
    }

    local::VTI vti( vthb.dataSet(0).file );
    for ( size_t i = 0; i < vti.dataArraySize(); i++ )
    {
        std::cout << vti.dataArray(i).name << std::endl;
    }

//    for ( size_t i = 0; i < vti.dataArray(0).values.size(); i++ )
//    {
//        std::cout << vti.dataArray(0).values[i] << std::endl;
//    }

//    kvs::StructuredVolumeObject* volume = Import( vti, 0 );
    kvs::StructuredVolumeObject* volume = Import( vthb, 0 );
    volume->print( std::cout );
    kvs::KVSMLObjectStructuredVolume* kvsml = new kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume>( volume );
    kvsml->write( "out.kvsml" );
    delete volume;
    delete kvsml;

    return app.run();
}
