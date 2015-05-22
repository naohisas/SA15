/*****************************************************************************/
/**
 *  @file   Import.h
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
#include "VTHB.h"
#include "VTI.h"


namespace local
{

kvs::StructuredVolumeObject* Import( const local::VTI& vti, size_t index );
kvs::StructuredVolumeObject* Import( const local::VTHB& vthb, size_t index );

} // end of namespace local
