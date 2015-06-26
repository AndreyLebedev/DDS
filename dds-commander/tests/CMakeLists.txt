# Copyright 2014 GSI, Inc. All rights reserved.
#
#
#
project( dds-commander-tests )

include_directories(
   ${dds-commander_SOURCE_DIR}/src
   ${Boost_INCLUDE_DIRS}
   ${MiscCommon_LOCATION}
   ${SQLITE3_INCLUDE_DIRS}
)
#=============================================================================

##############################################################################
### TestScheduler ############################################################
##############################################################################
add_executable(dds-scheduler-tests
   TestScheduler.cpp
   ${dds-commander_SOURCE_DIR}/src/AgentChannel.cpp
   ${dds-commander_SOURCE_DIR}/src/SSHScheduler.cpp
)

target_link_libraries (
	dds-scheduler-tests
    dds-protocol-lib
    dds-topology-lib
	dds-user-defaults-lib
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
    ${Boost_THREAD_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
)

install(TARGETS dds-scheduler-tests DESTINATION tests)

target_include_directories(dds-scheduler-tests
	PRIVATE
		$<TARGET_PROPERTY:dds-user-defaults-lib,INTERFACE_INCLUDE_DIRECTORIES>
    INTERFACE
        ${dds-topology-lib_SOURCE_DIR}/src
    INTERFACE
        ${dds-protocol-lib_SOURCE_DIR}/src
)

if( BUILD_TESTS )
  install(FILES ${dds-commander-tests_SOURCE_DIR}/topology_scheduler_test_1.xml DESTINATION tests)
  install(FILES ${dds-commander-tests_SOURCE_DIR}/topology_scheduler_test_2.xml DESTINATION tests)
endif( BUILD_TESTS )

##############################################################################
### TestCommanderDB ##########################################################
##############################################################################
add_executable(dds-commander-db-tests
   TestCommanderDB.cpp
   ${dds-commander_SOURCE_DIR}/src/CommanderDB.cpp
)

target_link_libraries (
   dds-commander-db-tests
   dds-user-defaults-lib
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
   ${Boost_LOG_LIBRARY}
   ${Boost_LOG_SETUP_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${Boost_FILESYSTEM_LIBRARY}
   ${SQLITE3_LIBRARIES}
)

install(TARGETS dds-commander-db-tests DESTINATION tests)

target_include_directories(dds-commander-db-tests
   PRIVATE
      $<TARGET_PROPERTY:dds-user-defaults-lib,INTERFACE_INCLUDE_DIRECTORIES>
)
