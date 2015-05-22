/*****************************************************************************/
/**
 *  @file   ViewerProgram.cpp
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
#include "ViewerProgram.h"
#include "VTHB.h"
#include "VTI.h"
#include "Import.h"
#include "Write.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/PolygonObject>
#include <kvs/PolygonImporter>
#include <kvs/PolygonRenderer>
#include <kvs/OrthoSlice>
#include <kvs/Bounds>
#include <kvs/StructuredVolumeObject>
#include <kvs/StochasticUniformGridRenderer>
#include <kvs/StochasticRenderingCompositor>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>
#include <kvs/DivergingColorMap>
#include <kvs/Directory>


namespace local
{

int ViewerProgram::exec( int argc , char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.show();

    kvs::Directory dir( argv[1] );
    local::VTHB vthb( dir.fileList().at(1).filePath(true) );
    kvs::TransferFunction tfunc( kvs::DivergingColorMap::CoolWarm( 256 ) );
    kvs::StructuredVolumeObject* object = local::Import( vthb, 0 );

//        kvs::StochasticUniformGridRenderer* renderer = new kvs::StochasticUniformGridRenderer();
//        renderer->setRepetitionLevel( 10 );
//        renderer->setTransferFunction( tfunc );
//        renderer->enableLODControl();

    const float position = kvs::Math::Mix( object->minObjectCoord().y(), object->maxObjectCoord().y(), 0.5f );
    const kvs::OrthoSlice::AlignedAxis axis = kvs::OrthoSlice::YAxis;
    kvs::PolygonObject* slice = new kvs::OrthoSlice( object, position, axis, tfunc );

    kvs::glsl::PolygonRenderer* slice_renderer = new kvs::glsl::PolygonRenderer();
    slice_renderer->disableShading();

    kvs::PolygonObject* polygon = new kvs::PolygonImporter( argv[2] );
//        polygon->setMinMaxExternalCoords( slice->minExternalCoord(), slice->maxExternalCoord() );
    kvs::glsl::PolygonRenderer* polygon_renderer = new kvs::glsl::PolygonRenderer();

    screen.registerObject( object, new kvs::Bounds() );
    screen.registerObject( polygon, polygon_renderer );
    screen.registerObject( slice, slice_renderer );

    kvs::Light::SetModelTwoSide( true );

    return app.run();
}


} // end of namespace local
