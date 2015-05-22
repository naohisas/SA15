/*****************************************************************************/
/**
 *  @file   Write.h
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

#include <kvs/StructuredVolumeObject>
#include <string>


namespace local
{

void Write( const kvs::StructuredVolumeObject* volume, const std::string filename, const bool binary = false );

} // end of namespace local
