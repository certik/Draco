//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/path_getFilenameComponent.cc
 * \brief  Encapsulate path information (path separator, etc.)
 * \note   Copyright (C) 2011-2014 Los Alamos National Security, LLC.
 *         All rights reserved.
 * \version $Id: path.cc 7388 2014-01-22 16:02:07Z kellyt $
 */
//---------------------------------------------------------------------------//

#include "path.hh"
#include <sstream>
#include <algorithm>    // std::replace()

namespace rtt_dsxx
{

//---------------------------------------------------------------------------//
/*!
 * \brief Helper function to extract path information from a filename.
 * \param fqName A fully qualified filename (/path/to/the/unit/test)
 * \return filename only, or path to file only.
 *
 * This function expects a fully qualfied name of a unit test (e.g.:
 * argv[0]).  It strips off the path and returns the name of the unit test.
 *
 * Options:
 *    FC_PATH        Return path portion only for fqName
 *    FC_ABSOLUTE    not implemented.
 *    FC_NAME        Return filename w/o path for fqName
 *    FC_EXT         not implemented.
 *    FC_NAME_WE     not implemented.
 *    FC_REALPATH    resolve all symlinks
 *    FC_LASTVALUE
 */
std::string getFilenameComponent( std::string const & fqName,
                                  FilenameComponent fc )
{
    using std::string;
    string retVal;
    string::size_type idx;
    std::string fullName( fqName );
    
    switch( fc )
    {
        case FC_PATH :
            // if fqName is a directory and ends with "/", trim the trailing
            // dirSep.
            if( fqName.rfind( rtt_dsxx::UnixDirSep ) == fqName.length()-1 )
                fullName=fqName.substr(0,fqName.length()-1);
            if( fqName.rfind( rtt_dsxx::WinDirSep ) == fqName.length()-1 )
                fullName=fqName.substr(0,fqName.length()-1);
            
            idx=fullName.rfind( rtt_dsxx::UnixDirSep );
            if( idx == string::npos ) 
            {
                // Didn't find directory separator, as 2nd chance look for
                // Windows directory separator.
                idx=fullName.rfind( rtt_dsxx::WinDirSep );
            }
            // If we still cannot find a path separator, return "./"
            if( idx == string::npos )
                retVal = string( string(".") + rtt_dsxx::dirSep );
            else
                retVal = fullName.substr(0,idx+1); 
            break;
            
        case FC_NAME :
            // if fqName is a directory and ends with "/", trim the trailing
            // dirSep.
            if( fqName.rfind( rtt_dsxx::UnixDirSep ) == fqName.length()-1 )
                fullName=fqName.substr(0,fqName.length()-1);
            if( fqName.rfind( rtt_dsxx::WinDirSep ) == fqName.length()-1 )
                fullName=fqName.substr(0,fqName.length()-1);
            
            idx=fullName.rfind( UnixDirSep );
            if( idx == string::npos )
            {
                // Didn't find directory separator, as 2nd chance look for
                // Windows directory separator.
                idx=fullName.rfind( WinDirSep );
            }
            // If we still cannot find a path separator, return the whole
            // string as the test name.
            if( idx == string::npos )
                retVal = fullName;
            else
                retVal = fullName.substr(idx+1);
            break;

        case FC_REALPATH :
            {
                std::string path( getFilenameComponent( fqName, FC_PATH ) );
                std::string name( getFilenameComponent( fqName, FC_NAME ) );
                if( ! draco_getstat(path).valid() )
                {
                    // On error, return empty string.
                    retVal = std::string();
                    // retVal = draco_getcwd();
                }
                else
                {
                    retVal = draco_getrealpath(path) + rtt_dsxx::dirSep + name;
                }
                break;
            }
        case FC_ABSOLUTE :
            Insist( false, "case for FC_ABSOLUTE not implemented." );
            break;
        case FC_EXT :
            Insist( false, "case for FC_EXT not implemented." );
            break;
        case FC_NAME_WE :
            Insist( false, "case for FC_NAME_WE not implemented." );
            break;
        case FC_NATIVE :
            if( dirSep == WinDirSep ) // Windows style.
                std::replace( fullName.begin(), fullName.end(), UnixDirSep, dirSep );
            else
                std::replace( fullName.begin(), fullName.end(), WinDirSep, dirSep );            
            retVal = fullName;
            break;

        default:
            std::ostringstream msg;
            msg << "Unknown mode for rtt_dsxx::setName(). fc = " << fc;
            Insist( false, msg.str() );
    }
    return retVal;
}

} // end namespace rtt_dsxx

//---------------------------------------------------------------------------//
// end of path_getFilenameComponent.cc
//---------------------------------------------------------------------------//