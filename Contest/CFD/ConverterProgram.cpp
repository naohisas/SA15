/*****************************************************************************/
/**
 *  @file   ConverterProgram.cpp
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
#include "ConverterProgram.h"
#include "VTHB.h"
#include "VTI.h"
#include "Import.h"
#include "Write.h"
#include <kvs/Directory>
#include <kvs/StructuredVolumeObject>
#include <string>


namespace local
{

int ConverterProgram::exec( int argc, char** argv )
{
    const kvs::Directory dir( argv[1] );
    const size_t nfiles = dir.fileList().size();
    for ( size_t i = 0; i < nfiles; i++ )
    {
        const std::string filename = dir.fileList().at(i).fileName();
        const std::string filepath = dir.fileList().at(i).filePath( true );
        const std::string basename = dir.fileList().at(i).baseName();
        if ( filename[0] == '.' ) { continue; }

        const local::VTHB vthb( filepath );
        const local::VTI vti0 = local::VTI( vthb.dataSet(0).file );
        const size_t nvars = vti0.dataArraySize();
        for ( size_t j = 0; j < nvars; j++ )
        {
            const std::string varname = vti0.dataArray(j).name;
            const std::string outputfile = basename + "-" + varname + ".kvsml";
            kvs::StructuredVolumeObject* volume = local::Import( vthb, j );
            local::Write( volume, outputfile, true );
            delete volume;
            std::cout << outputfile << std::endl;
        }
    }

    return 0;
}

} // end of namespace local
