# - Find mariadbclient
# Find the native MariaDb includes and library
#
#  MARIADB_INCLUDE_DIR - where to find mariadb.h, etc.
#  MARIADB_LIBRARIES   - List of libraries when using MariaDb.
#  MARIADB_FOUND       - True if MariaDb found.

IF (MARIADB_INCLUDE_DIR)
    # Already in cache, be silent
    SET(MARIADB_FIND_QUIETLY TRUE)
ENDIF (MARIADB_INCLUDE_DIR)

FIND_PATH(MARIADB_INCLUDE_DIR mysql.h
        /usr/local/include/mariadb
        /usr/include/mariadb
        )

SET(MARIADB_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MARIADB_LIBRARY
        NAMES ${MARIADB_NAMES}
        PATHS /usr/lib /usr/local/lib
        PATH_SUFFIXES mariadb
        )

IF (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARY)
    SET(MARIADB_FOUND TRUE)
    SET( MARIADB_LIBRARIES ${MARIADB_LIBRARY} )
ELSE (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARY)
    SET(MARIADB_FOUND FALSE)
    SET( MARIADB_LIBRARIES )
ENDIF (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARY)

IF (MARIADB_FOUND)
    IF (NOT MARIADB_FIND_QUIETLY)
        MESSAGE(STATUS "Found MariaDb: ${MARIADB_LIBRARY}")
    ENDIF (NOT MARIADB_FIND_QUIETLY)
ELSE (MARIADB_FOUND)
    IF (MARIADB_FIND_REQUIRED)
        MESSAGE(STATUS "Looked for MariaDb libraries named ${MARIADB_NAMES}.")
        MESSAGE(FATAL_ERROR "Could NOT find MariaDb library")
    ENDIF (MARIADB_FIND_REQUIRED)
ENDIF (MARIADB_FOUND)

MARK_AS_ADVANCED(
        MARIADB_LIBRARY
        MARIADB_INCLUDE_DIR
)