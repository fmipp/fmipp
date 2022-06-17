// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_IPCSTRING_H
#define _FMIPP_IPCSTRING_H

// Boost includes.
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#ifdef WIN32

#include <boost/interprocess/managed_windows_shared_memory.hpp>
typedef boost::interprocess::allocator<char, boost::interprocess::managed_windows_shared_memory::segment_manager> CharAllocator;

#else

#include <boost/interprocess/managed_shared_memory.hpp>
typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;

#endif

/// FIXME This definition of IPCString and fmippIPCString works only for shared memory access.
typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> IPCString;
typedef IPCString fmippIPCString;

#endif // _FMIPP_IPCSTRING_H
