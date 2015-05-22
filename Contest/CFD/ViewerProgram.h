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

namespace local
{

class ViewerProgram : public kvs::Program
{
    int exec( int argc , char** argv );
};

} // end of namespace local
