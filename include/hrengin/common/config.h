/**
   Copyright (C) 2014  absurdworlds

   License LGPLv3-only:
   GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
   This is free software: you are free to change and redistribute it.
   There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_config_
#define _hrengin_config_

/* Windows platform */
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	#define _HR_�INDOWS
#endif

/* Determine compiler */
#if defined(__clang__)
	/* Clang/LLVM. ---------------------------------------------- */
	
#elif defined(__ICC) || defined(__INTEL_COMPILER)
	/* Intel ICC/ICPC. ------------------------------------------ */
	#define __HRCMP_ICC__
#elif defined(__GNUC__) || defined(__GNUG__)
	/* GNU GCC/G++. --------------------------------------------- */
	#define __HRCMP_GNU__
#elif defined(__HP_cc) || defined(__HP_aCC)
	/* Hewlett-Packard C/aC++. ---------------------------------- */

#elif defined(__IBMC__) || defined(__IBMCPP__)
	/* IBM XL C/C++. -------------------------------------------- */
	#define __HRCMP_IBM__

#elif defined(_MSC_VER)
	/* Microsoft Visual Studio. --------------------------------- */
	#define __HRCMP_MSC__
#elif defined(__PGI)
	/* Portland Group PGCC/PGCPP. ------------------------------- */

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
	/* Oracle Solaris Studio. ----------------------------------- */
#endif

#ifndef FORCEINLINE
	#ifdef __HRCMP_MSC__
		#define FORCEINLINE __forceinline
	#else
		#define FORCEINLINE inline
	#endif
#endif

#if defined(__HRCMP_MSC__)
	#pragma warning(disable: 4244) 
#endif


#define _HR_STR(arg) #arg

#endif //_hrengin_config_