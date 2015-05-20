/*****************************************************************************/
/**
 *  @file   VTI.h
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
#pragma once

#include <string>
#include <vector>
#include <kvs/Vector3>
#include <kvs/ValueArray>
#include <kvs/Type>


namespace local
{

class VTI
{
public:

    struct DataArray
    {
        std::string name;
        size_t ncomponents;
        size_t offset;
        kvs::ValueArray<kvs::Real32> values;
    };

private:

    kvs::Vec3 m_origin;
    kvs::Vec3 m_spacing;
    kvs::Vec3ui m_resolution;
    std::vector<DataArray> m_data_arrays;

public:

    VTI( const std::string& filename ) { this->read( filename ); }
    const kvs::Vec3 origin() const { return m_origin; }
    const kvs::Vec3 spacing() const { return m_spacing; }
    const kvs::Vec3ui& resolution() const { return m_resolution; }
    const DataArray& dataArray( const size_t index ) const { return m_data_arrays[index]; }
    size_t dataArraySize() const { return m_data_arrays.size(); }
    void read( const std::string& filename );
};

} // end of namespace local
