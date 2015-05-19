/*****************************************************************************/
/**
 *  @file   VTHB.cpp
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
#include "VTHB.h"
#include <kvs/Exception>
#include <kvs/XMLDocument>
#include <kvs/XMLDeclaration>
#include <kvs/XMLNode>
#include <kvs/XMLElement>
#include <kvs/XMLComment>
#include <kvs/String>
#include <kvs/Tokenizer>


namespace
{

inline void Throw( const std::string& message )
{
    KVS_THROW( kvs::FileReadFaultException, message );
}

inline int ToInt( const std::string& text )
{
    return kvs::String::To<int>( text );
}

inline kvs::Vec4i ToVec4i( const std::string& text )
{
    const std::string delim(" \n");
    kvs::Tokenizer t( text, delim );
    const int v0 = ToInt( t.token() );
    const int v1 = ToInt( t.token() );
    const int v2 = ToInt( t.token() );
    const int v3 = ToInt( t.token() );
    return kvs::Vec4i( v0, v1, v2, v3 );
}

}

namespace local
{

void VTHB::read( const std::string& filename )
{
    kvs::XMLDocument document;
    if ( !document.read( filename ) ) { ::Throw( "Cannot read " + filename + "." ); }

    // <VTKFile>
    const std::string VTKFile_tag("VTKFile");
    kvs::XMLNode::SuperClass* VTKFile_node = kvs::XMLDocument::FindNode( &document, VTKFile_tag );
    if ( !VTKFile_node ) { ::Throw( "Cannot find <" + VTKFile_tag + ">." ); }

    // <vtkHierarchicalBoxDataSet>
    const std::string vtkHBDS_tag("vtkHierarchicalBoxDataSet");
    kvs::XMLNode::SuperClass* vtkHBDS_node = kvs::XMLNode::FindChildNode( VTKFile_node, vtkHBDS_tag );
    if ( !vtkHBDS_node ) { ::Throw( "Cannot find <" + VTKFile_tag + ">." ); }

    // <DataSet>
    const std::string DataSet_tag("DataSet");
    kvs::XMLNode::SuperClass* DataSet_node = kvs::XMLNode::FindChildNode( vtkHBDS_node, DataSet_tag );
    if ( !DataSet_node ) { ::Throw( "Cannot find <" + DataSet_tag + ">." ); }
    while ( DataSet_node )
    {
        kvs::XMLElement::SuperClass* element = kvs::XMLNode::ToElement( DataSet_node );

        DataSet data_set;
        data_set.group = ::ToInt( kvs::XMLElement::AttributeValue( element, "group" ) );
        data_set.dataset = ::ToInt( kvs::XMLElement::AttributeValue( element, "dataset" ) );
        data_set.amr_box = ::ToVec4i( kvs::XMLElement::AttributeValue( element, "amr_box" ) );
        data_set.file = kvs::XMLElement::AttributeValue( element, "file" );
        m_data_set.push_back( data_set );

        DataSet_node = vtkHBDS_node->IterateChildren( DataSet_tag, DataSet_node );
    }
}

} // end of namespace local
