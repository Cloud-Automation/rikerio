# CPack

set(CPACK_PACKAGE_NAME "rikerio")
set(CPACK_PACKAGE_VENDOR "Cloud Automation")
set(CPACK_PACKAGE_VERSION_MAJOR VERSION_MAJOR)
set(CPACK_PACKAGE_VERSION_MINOR VERSION_MINOR)
set(CPACK_PACKAGE_VERSION_PATCH VERSION_PATCH)
set(CPACK_PACKAGE_EXECUTABLES "rio librio.so")

SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RikerIO Memory Server and Library.")
SET(CPACK_PACKAGE_VENDOR "Cloud Automation")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_CONTACT "Stefan Pöter <stage-automation@cloud-automation.de>")

#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libyaml-dev")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

include(CPack)

