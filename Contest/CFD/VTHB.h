/*****************************************************************************/
/**
 *  @file   VTHB.h
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

#include <kvs/Vector4>
#include <string>
#include <vector>


namespace local
{

class VTHB
{
public:

    struct DataSet
    {
        int group;
        int dataset;
        kvs::Vec4i amr_box;
        std::string file;
    };

private:

    std::vector<DataSet> m_data_set;

public:

    VTHB( const std::string& filename ) { this->read( filename ); }
    const DataSet& dataSet( const size_t index ) const { return m_data_set[index]; }
    size_t dataSetSize() const { return m_data_set.size(); }
    void read( const std::string& filename );
};

} // end of namespace local
