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
#include "Import.h"
#include "Write.h"
#include "ViewerProgram.h"
#include "ConverterProgram.h"

int main( int argc, char** argv )
{
    local::ViewerProgram program;
//    local::ConverterProgram program;
    return program.start( argc, argv );
}
