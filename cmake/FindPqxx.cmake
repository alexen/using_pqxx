#.rst:
# FindPqxx
# --------
#
# Find libpqxx library - official PostgreSQL C++ client
#
# This will define the following variables::
#
#     Pqxx_FOUND         - True if the system has libpqxx library
#     Pqxx_INCLUDE_DIRS  - List of include directories that contain headers of the library
#     Pqxx_LIBRARIES     - List of libraries which was found
#

find_package(PkgConfig)
pkg_check_modules(PC_Pqxx QUIET Pqxx)

find_path(Pqxx_INCLUDE_DIRS
     NAMES pqxx
     PATHS ${PC_Pqxx_INCLUDE_DIRS}
)

find_library(Pqxx_LIBRARIES
     NAMES pqxx
     PATHS ${PC_Pqxx_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Pqxx
     FOUND_VAR Pqxx_FOUND
     REQUIRED_VARS
          Pqxx_INCLUDE_DIRS
          Pqxx_LIBRARIES
)

mark_as_advanced(
     Pqxx_INCLUDE_DIRS
     Pqxx_LIBRARIES
)