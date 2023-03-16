///////////////////////////////////////////////////////////////////////////////
//
//	Config.h
//
//	$Id: Config.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Copyright (C) Simon Collis 2000-2012
//
//-----------------------------------------------------------------------------
//
//	This file is part of a6.
//
//	a6 is free software: you can redistribute it and/or modify it under the
//	terms of the GNU General Public License as published by the Free Software
//	Foundation, either version 3 of the License, or (at your option) any later
//	version.
//
//	a6 is distributed in the hope that it will be useful, but WITHOUT ANY
//	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//	FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
//	details.
//
//	You should have received a copy of the GNU General Public License
//	along with a6.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

/* Standards */
#define AUTHOR_EMAIL	"simon@simoncollis.com"

// Versions - 0xAABBCCDD = AA.BB.CC.DD
#define G_VER			(0x00050000)
#define G_MINVER		(0x00040000)
#define G_MAXVER		(0x00060000)
#define A6_VER_TEXT		"0.5.0"
#define A6_VER_NAME		"Ignatz"

/* Win16 */
#ifndef SYSNAME
#ifdef _WINDOWS
#define PATH_SEPARATOR ';'
#define PATH_SLASH '\\'
#define SYSNAME "WIN16"
#define SYS_ENDIAN "I"
#endif
#endif

/* Win32 */
#ifndef SYSNAME
#ifdef __WINDOWS_386__
#define PATH_SEPARATOR ';'
#define PATH_SLASH '\\'
#define SYSNAME "WIN32"
#define SYS_ENDIAN "I"
#endif
#endif

/* OS/2 */
#ifndef SYSNAME
#ifdef __OS2__
#define PATH_SEPARATOR ';'
#define PATH_SLASH '\\'
#define SYSNAME "OS/2"
#define SYS_ENDIAN "I"
#endif
#endif

/* DOS */
#ifndef SYSNAME
#ifdef _DOS
#define PATH_SEPARATOR ';'
#define PATH_SLASH '\\'
#define SYSNAME "DOS"
#define SYS_ENDIAN "I"
#endif
#endif

/* Amiga */
#ifndef SYSNAME
/* Does any compiler actually #define this? */
#ifdef AMIGA
#define PATH_SEPARATOR ';'
#define PATH_SLASH '/'
#define SYSNAME "AmigaDOS"
#define SYS_ENDIAN "M"
#endif
#endif

/* Unix? */
#ifndef SYSNAME
#ifdef __UNIX__
#define PATH_SEPARATOR ':'
#define PATH_SLASH '/'
#define SYSNAME "Unix"
#define SYS_ENDIAN "?"
#endif
#endif

/* Failover case */
#ifndef SYSNAME
#define PATH_SEPARATOR ':'
#define PATH_SLASH '/'
#define SYSNAME "Unknown"
#define SYS_ENDIAN "?"
#endif

