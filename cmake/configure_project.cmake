function(collect_sources_ext pathToCollect extToCollect)
    file(GLOB TEMP_FILES ${pathToCollect}/*.${extToCollect} )
    if( NOT TEMP_FILES EQUAL "" )
        set( SOURCES ${SOURCES} ${TEMP_FILES} PARENT_SCOPE )
    endif()
endfunction()

function(collect_includes_ext pathToCollect extToCollect)
    file(GLOB TEMP_FILES ${pathToCollect}/*.${extToCollect} )
    if( NOT TEMP_FILES EQUAL "" )
        set( INCS ${INCS} ${TEMP_FILES} PARENT_SCOPE )
    endif()
endfunction()

function(collect_sources pathToCollect)

    collect_sources_ext( ${pathToCollect} cpp )
    collect_sources_ext( ${pathToCollect} c )

    if( APPLE )
        collect_sources_ext( ${pathToCollect} mm )
        collect_sources_ext( ${pathToCollect} m )
    endif()

    collect_includes_ext( ${pathToCollect} h )
    collect_includes_ext( ${pathToCollect} hpp )

    set( SOURCES ${SOURCES} PARENT_SCOPE )
    set( INCS ${INCS} PARENT_SCOPE )

endfunction()
