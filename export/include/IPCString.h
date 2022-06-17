// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_IPCSTRING_H
#define _FMIPP_IPCSTRING_H

// Boost includes.
#ifdef WIN32
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#else
#include <boost/interprocess/managed_shared_memory.hpp>
#endif
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator; 
typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> IPCString;
typedef IPCString fmippIPCString;

#endif // _FMIPP_IPCSTRING_H
