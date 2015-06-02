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
#include <kvs/PolygonObject>
#include <kvs/PolygonImporter>
#include <kvs/File>
#include <fstream>

kvs::ValueArray<kvs::Real32> VertexNormals( const kvs::PolygonObject* polygon )
{
    if ( polygon->normals().size() == 0 )
    {
        return kvs::ValueArray<kvs::Real32>();
    }

    kvs::ValueArray<kvs::Real32> normals;
    switch ( polygon->normalType() )
    {
    case kvs::PolygonObject::VertexNormal:
    {
        normals = polygon->normals();
        break;
    }
    case kvs::PolygonObject::PolygonNormal:
    {
        // Same normal vectors are assigned for each vertex of the polygon.
        const size_t npolygons = polygon->normals().size() / 3;
        const size_t nnormals = npolygons * 3;
        normals.allocate( nnormals * 3 );
        kvs::Real32* pnormals = normals.data();
        for ( size_t i = 0; i < npolygons; i++ )
        {
            const kvs::Vec3 n = polygon->normal(i);
            for ( size_t j = 0; j < 3; j++ )
            {
                *(pnormals++) = n.x();
                *(pnormals++) = n.y();
                *(pnormals++) = n.z();
            }
        }
        break;
    }
    default: break;
    }

    return normals;
}

void WriteOBJ( const kvs::PolygonObject* object )
{
    const std::string filename = object->name();
    std::cout << filename << std::endl;

    std::ofstream ofs( filename );
    ofs << "# " << kvs::File( filename ).baseName() << std::endl;
    ofs << "o " << kvs::File( filename ).baseName() << std::endl;

    const size_t nvertices = object->numberOfVertices();
    for ( size_t i = 0; i < nvertices; i++ )
    {
        const kvs::Vec3 v = object->coord(i);
        ofs << "v " << v << std::endl;
    }

    const size_t nfaces = nvertices / 3;
    for ( size_t i = 0; i < nfaces; i++ )
//    for ( size_t i = 0; i < nvertices; i++ )
    {
        const kvs::Vec3 vn = object->normal(i);
        ofs << "vn " << vn << std::endl;
    }

    for ( size_t i = 0; i < nfaces; i++ )
    {
        const int id = i + 1;
        const int id0 = 3 * id;
        const int id1 = id0 + 1;
        const int id2 = id1 + 1;
//        ofs << "f " << kvs::Vec3i( id0, id1, id2 ) << std::endl;
        ofs << "f "
            << id0 << "//" << id << " "
            << id1 << "//" << id << " "
            << id2 << "//" << id << std::endl;
    }
}

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.show();

    kvs::PolygonObject* object = new kvs::PolygonImporter( argv[1] );
    object->setName( kvs::File( argv[1] ).baseName() + ".obj" );
//    object->setNormals( ::VertexNormals( object ) );
//    object->setNormalTypeToVertex();
    object->print( std::cout );

    WriteOBJ( object );

    screen.registerObject( object );

    return app.run();
}
