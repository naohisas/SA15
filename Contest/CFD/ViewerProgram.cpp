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
#include <kvs/glut/Timer>
#include <kvs/PointObject>
#include <kvs/LineObject>
#include <kvs/PolygonObject>
#include <kvs/PolygonImporter>
#include <kvs/PolygonRenderer>
#include <kvs/OrthoSlice>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <kvs/StructuredVolumeObject>
#include <kvs/StochasticUniformGridRenderer>
#include <kvs/StochasticPolygonRenderer>
#include <kvs/StochasticLineRenderer>
#include <kvs/StochasticRenderingCompositor>
#include <kvs/ParticleBasedRenderer>
#include <kvs/CellByCellMetropolisSampling>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>
#include <kvs/DivergingColorMap>
#include <kvs/Directory>
#include <kvs/EventListener>
#include <kvs/Scene>
#include <iostream>
#include <fstream>


namespace
{

inline void WriteParticles( const kvs::PointObject* object, const int index )
{
    char num[4] = { 0, 0, 0, 0 };
    sprintf( num, "%03d", index );
    std::string filename = std::string("test_") + std::string( num ) + std::string(".dat");

    std::ofstream ofs( filename.c_str() );
    ofs << index << std::endl;
    ofs << object->numberOfVertices() << std::endl;
    for ( size_t i = 0; i < object->numberOfVertices(); i++ )
    {
        const kvs::Vec3 coord = object->coord(i);
        const kvs::Vec3i color = object->color(i).toVec3i();
        ofs << coord[0] << " " << coord[1] << " " << coord[2] << " ";
        ofs << color[0] << " " << color[1] << " " << color[2] << " ";
        ofs << "1" << " ";
        ofs << "0" << " ";
        ofs << "0" << " ";
        ofs << "1" << std::endl;
    }
}

inline void ExecPolygonObject(
    kvs::Scene* scene,
    const std::string filename )
{
    typedef kvs::PolygonObject Object;
    typedef kvs::StochasticPolygonRenderer Renderer;
    typedef kvs::PolygonImporter Importer;

    const std::string object_name("PolygonObject");
    Object* object = new Importer( filename );
    object->setName( object_name );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

inline void ExecOrthoSlice(
    kvs::Scene* scene,
    const kvs::StructuredVolumeObject* volume,
    const kvs::TransferFunction& tfunc )
{
    typedef kvs::PolygonObject Object;
    typedef kvs::StochasticPolygonRenderer Renderer;
    typedef kvs::OrthoSlice Mapper;

    const float position = kvs::Math::Mix( volume->minObjectCoord().y(), volume->maxObjectCoord().y(), 0.5f );
    const Mapper::AlignedAxis axis = Mapper::YAxis;

    const std::string object_name("OrthoSlice");
    Object* object = new Mapper( volume, position, axis, tfunc );
    object->setName( object_name );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        renderer->disableShading();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

inline void ExecBounds(
    kvs::Scene* scene,
    const kvs::ObjectBase* object_base )
{
    typedef kvs::LineObject Object;
    typedef kvs::StochasticLineRenderer Renderer;

    const std::string object_name("Bounds");
    Object* object = kvs::Bounds().outputLineObject( object_base );
    object->setName( object_name );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

inline void ExecIsosurface(
    kvs::Scene* scene,
    const kvs::StructuredVolumeObject* volume,
    const kvs::TransferFunction& tfunc )
{
    typedef kvs::PolygonObject Object;
    typedef kvs::StochasticPolygonRenderer Renderer;
    typedef kvs::Isosurface Mapper;

    const std::string object_name("Bounds");
    const double isovalue = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.4 );
    Object* object = new Mapper( volume, isovalue, kvs::Isosurface::VertexNormal, false, tfunc );
    object->setName( object_name );
    object->setOpacity( 128 );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

inline void ExecParticleRendering(
    kvs::Scene* scene,
    const kvs::StructuredVolumeObject* volume,
    const kvs::TransferFunction& tfunc )
{
    typedef kvs::PointObject Object;
    typedef kvs::glsl::ParticleBasedRenderer Renderer;
    typedef kvs::CellByCellMetropolisSampling Mapper;

    const size_t repetitions = 10;
    const size_t subpixels = 1; // fixed to '1'
    const size_t level = static_cast<size_t>( subpixels * std::sqrt( double( repetitions ) ) );
    const float step = 0.5f;
    const std::string object_name("Particle");
    Object* object = new Mapper( volume, level, step, tfunc );
    object->setName( object_name );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

inline void ExecVolumeRendering(
    kvs::Scene* scene,
    const kvs::StructuredVolumeObject* volume,
    const kvs::TransferFunction& tfunc )
{
    typedef kvs::StructuredVolumeObject Object;
    typedef kvs::StochasticUniformGridRenderer Renderer;

    const std::string object_name("Volume");
    Object* object = new Object();
    object->shallowCopy( *volume );
    object->setName( object_name );

    if ( !scene->hasObject( object_name ) )
    {
        Renderer* renderer = new Renderer();
        renderer->setTransferFunction( tfunc );
        renderer->disableShading();
        scene->registerObject( object, renderer );
    }
    else
    {
        scene->replaceObject( object_name, object );
    }
}

class Event : public kvs::EventListener
{
    local::ViewerProgram::Volumes& m_volumes;
    local::ViewerProgram::Indices& m_indices;
    kvs::glut::Timer m_timer; ///< timer
    int m_time_interval; ///< interval in msec
    kvs::TransferFunction m_tfunc;

public:

    Event(
        local::ViewerProgram::Volumes& volumes,
        local::ViewerProgram::Indices& indices ):
        m_volumes( volumes ),
        m_indices( indices ),
        m_time_interval( 100 )
    {
        setEventType( kvs::EventBase::AllEvents );
        m_timer.setInterval( m_time_interval );
        m_timer.setEventListener( this );
    }

    void initializeEvent()
    {
        std::cout << "initializeEvent" << std::endl;

        int index = m_indices.start;
        kvs::StructuredVolumeObject* object = m_volumes[index];

        kvs::OpacityMap omap( 256 );
        omap.addPoint(   0, 0.7 );
        omap.addPoint(  50, 0.5 );
        omap.addPoint( 100, 0.0 );
        omap.addPoint( 180, 0.0 );
        omap.addPoint( 190, 0.8 );
        omap.addPoint( 255, 0.9 );
        omap.create();

        m_tfunc = kvs::TransferFunction( kvs::DivergingColorMap::CoolWarm( 256 ), omap );
//        m_tfunc = kvs::TransferFunction( omap );

        ExecOrthoSlice( scene(), object, m_tfunc );
//    ExecIsosurface( screen, object, tfunc );
        ExecBounds( scene(), object );

//    object->setMinMaxExternalCoords( object->minObjectCoord(), object->maxObjectCoord() );
        ExecVolumeRendering( scene(), object, m_tfunc );

        m_timer.start();
    }

    void paintEvent() { std::cout << "paintEvent" << std::endl; }
    void resizeEvent( int, int ) { std::cout << "resizeEvent" << std::endl; }
    void mousePressEvent( kvs::MouseEvent* ) { std::cout << "mousePressEvent" << std::endl; }
    void mouseMoveEvent( kvs::MouseEvent* ) { std::cout << "mouseMoveEvent" << std::endl; }
    void mouseReleaseEvent( kvs::MouseEvent* ) { std::cout << "mouseReleaseEvent" << std::endl; }
    void mouseDoubleClickEvent( kvs::MouseEvent* ) { std::cout << "mouseDoubleClickEvent" << std::endl; }
    void wheelEvent( kvs::WheelEvent* ) { std::cout << "wheelEvent" << std::endl; }
    void keyPressEvent( kvs::KeyEvent* ) { std::cout << "keyPressEvent" << std::endl; }
    void timerEvent( kvs::TimeEvent* )
    {
        std::cout << "timerEvent" << std::endl;

        m_indices.current++;
        if ( m_indices.current > m_indices.end ) { m_indices.current = m_indices.start; }

        kvs::StructuredVolumeObject* object = m_volumes[m_indices.current];
        ExecOrthoSlice( scene(), object, m_tfunc );
        ExecVolumeRendering( scene(), object, m_tfunc );
        screen()->redraw();
    }
};

}


namespace local
{

int ViewerProgram::exec( int argc , char** argv )
{
    m_indices.start = 0;
    m_indices.end = 29;
    m_indices.current = m_indices.start;

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setSize( 800, 600 );
    screen.setBackgroundColor( kvs::RGBColor::White() );

    kvs::Directory dir( argv[1] );

    size_t index = m_indices.start + 1;
    while ( index <= m_indices.end + 1 )
    {
        local::VTHB vthb( dir.fileList().at( index++ ).filePath( true ) );
        m_volumes.push_back( local::Import( vthb, 0 ) );
    }

    ExecPolygonObject( screen.scene(), argv[2] );

    kvs::StochasticRenderingCompositor compositor( screen.scene() );
    compositor.setRepetitionLevel( 1 );
    compositor.setEnabledLODControl( true );
    screen.setEvent( &compositor );

    ::Event event( m_volumes, m_indices );
    screen.addEvent( &event );

    screen.show();

    kvs::Light::SetModelTwoSide( true );

    return app.run();
}


} // end of namespace local
