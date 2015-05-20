/*****************************************************************************/
/**
 *  @file   VTI.cpp
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
#include "VTI.h"
#include <kvs/Exception>
#include <kvs/XMLDocument>
#include <kvs/XMLDeclaration>
#include <kvs/XMLNode>
#include <kvs/XMLElement>
#include <kvs/XMLComment>
#include <kvs/String>
#include <kvs/Tokenizer>
#include <kvs/Vector>
#include <kvs/Endian>
#include <fstream>
#include <string>


namespace
{

inline void Throw( const std::string& message )
{
    KVS_THROW( kvs::FileReadFaultException, message );
}

template <typename T>
inline T ToValue( const std::string& text )
{
    return text.size() == 0 ? 1 : kvs::String::To<T>( text );
}

template <typename T, size_t N>
inline kvs::Vector<T> ToVec( const std::string& text )
{
    const std::string delim(" \n");
    kvs::Tokenizer t( text, delim );
    kvs::Vector<T> v( N );
    for ( size_t i = 0; i < N; i++ ) { v[i] = ToValue<T>( t.token() ); }
    return v;
}

template <typename T>
inline kvs::ValueArray<T> ReadData(
    std::ifstream& ifs,
    const std::string& tag,
    const size_t whole_byte_size,
    const size_t size,
    const size_t offset )
{
    std::string buffer;
    while ( ifs && std::getline( ifs, buffer ) )
    {
        if ( buffer.find( tag.c_str(), 0 ) != std::string::npos ) { break; }
    }

    kvs::ValueArray<char> whole_data( whole_byte_size );
    ifs.read( whole_data.data(), whole_data.byteSize() );

    const size_t header_offset = 1 + 4; // "_" + "NNNN"
    kvs::ValueArray<T> values( (T*)( whole_data.data() + header_offset + offset ), size );
    return values;
}

}


namespace local
{

void VTI::read( const std::string& filename )
{
    kvs::XMLDocument document( filename );

    // <VTKFile>
    const std::string VTKFile_tag("VTKFile");
    kvs::XMLNode::SuperClass* VTKFile_node = kvs::XMLDocument::FindNode( &document, VTKFile_tag );
    if ( !VTKFile_node ) { ::Throw( "Cannot find <" + VTKFile_tag + ">." ); }

    // <ImageData>
    const std::string ImageData_tag("ImageData");
    kvs::XMLNode::SuperClass* ImageData_node = kvs::XMLNode::FindChildNode( VTKFile_node, ImageData_tag );
    if ( !ImageData_node ) { ::Throw( "Cannot find <" + ImageData_tag + ">." ); }

    kvs::XMLElement::SuperClass* ImageData_element = kvs::XMLNode::ToElement( ImageData_node );
    kvs::Vector<int> extent = ::ToVec<int,6>( kvs::XMLElement::AttributeValue( ImageData_element, "WholeExtent" ) );
    size_t dimx = extent[1] - extent[0];
    size_t dimy = extent[3] - extent[2];
    size_t dimz = extent[5] - extent[4];
    size_t nnodes = dimx * dimy * dimz; // This is equal to the number of cells because the data may be a staggard grid.
    m_resolution = kvs::Vec3ui( dimx, dimy, dimz );

    kvs::Vector<float> origin = ::ToVec<float,3>( kvs::XMLElement::AttributeValue( ImageData_element, "Origin" ) );
    m_origin = kvs::Vec3( origin[0], origin[1], origin[2] );

    kvs::Vector<float> spacing = ::ToVec<float,3>( kvs::XMLElement::AttributeValue( ImageData_element, "Spacing" ) );
    m_spacing = kvs::Vec3( spacing[0], spacing[1], spacing[2] );

    // <Piece>
    const std::string Piece_tag("Piece");
    kvs::XMLNode::SuperClass* Piece_node = kvs::XMLNode::FindChildNode( ImageData_node, Piece_tag );
    if ( !Piece_node ) { ::Throw( "Cannot find <" + Piece_tag + ">." ); }

    // <CellData>
    const std::string CellData_tag("CellData");
    kvs::XMLNode::SuperClass* CellData_node = kvs::XMLNode::FindChildNode( Piece_node, CellData_tag );
    if ( !CellData_node ) { ::Throw( "Cannot find <" + CellData_tag + ">." ); }

    // <DataArray>
    const std::string DataArray_tag("DataArray");
    kvs::XMLNode::SuperClass* DataArray_node = kvs::XMLNode::FindChildNode( CellData_node, DataArray_tag );
    if ( !DataArray_node ) { ::Throw( "Cannot find <" + DataArray_tag + ">." ); }

    size_t whole_byte_size = 0;
    while ( DataArray_node )
    {
        kvs::XMLElement::SuperClass* element = kvs::XMLNode::ToElement( DataArray_node );

        DataArray data_array;
        data_array.name = kvs::XMLElement::AttributeValue( element, "Name" );
        data_array.ncomponents = ::ToValue<int>( kvs::XMLElement::AttributeValue( element, "NumberOfComponents" ) );
        data_array.offset = ::ToValue<int>( kvs::XMLElement::AttributeValue( element, "offset" ) );
        m_data_arrays.push_back( data_array );

        whole_byte_size += nnodes * data_array.ncomponents * sizeof(kvs::Real32) + 4;

        DataArray_node = CellData_node->IterateChildren( DataArray_tag, DataArray_node );
    }

    // <AppendedData>
    const std::string AppendedData_tag("AppendedData");
    for ( size_t i = 0; i < m_data_arrays.size(); i++ )
    {
        const size_t offset = m_data_arrays[i].offset;
        const size_t size = nnodes * m_data_arrays[i].ncomponents;
        std::ifstream ifs( filename.c_str(), std::ios_base::in | std::ios_base::binary );
        kvs::ValueArray<kvs::Real32> values = ::ReadData<kvs::Real32>( ifs, AppendedData_tag, whole_byte_size, size, offset );
        kvs::Endian::Swap( values.data(), values.size() );
        m_data_arrays[i].values = values;
    }
}

} // end of namespace local
