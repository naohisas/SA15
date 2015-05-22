/*****************************************************************************/
/**
 *  @file   Write.cpp
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
#include "Write.h"
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>


namespace local
{

void Write(
    const kvs::StructuredVolumeObject* volume,
    const std::string filename,
    const bool binary )
{
    typedef kvs::KVSMLObjectStructuredVolume KVSML;
    typedef kvs::StructuredVolumeExporter<KVSML> Exporter;
    KVSML* kvsml = new Exporter( volume );
    kvsml->setWritingDataType( binary ? Exporter::ExternalBinary : Exporter::Ascii );
    kvsml->write( filename );
    delete kvsml;
}

} // end of namespace local
