/*****************************************************************************/
/**
 *  @file   ViewerProgram.h
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

#include <kvs/Program>
#include <kvs/StructuredVolumeObject>
#include <vector>


namespace local
{

class ViewerProgram : public kvs::Program
{
public:

    typedef kvs::StructuredVolumeObject Volume;
    typedef std::vector<Volume*> Volumes;

    struct Indices
    {
        int start;
        int end;
        int current;
    };

private:

    Indices m_indices;
    Volumes m_volumes;

public:

    int exec( int argc , char** argv );
};

} // end of namespace local
