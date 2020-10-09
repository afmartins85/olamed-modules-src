CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
#PROJECT(server)
SET(PROJECT_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(COMMON_DIR ${PROJECT_TOP_DIR}/common)
SET(SRC_24_DIR ${PROJECT_TOP_DIR}/2.4)

FILE(GLOB_RECURSE SRC_24_DIR_HEADERS
    ${SRC_24_DIR}/*.h
    ${SRC_24_DIR}/*.hpp
    )

SET(HL7_COMMON_INCLUDE
    ${HL7_COMMON_INCLUDE}
    ${COMMON_DIR}/HL7Object.h
    ${COMMON_DIR}/ObjToPipe.h
    ${COMMON_DIR}/Util.h
    ${COMMON_DIR}/hl7socket.h
    ${COMMON_DIR}/hl7mllp.h
    ${COMMON_DIR}/dataencode.h
    ${COMMON_DIR}/validation.h
    )

SET(HL7_COMMON_SRC
        ${HL7_COMMON_SRC}
	${COMMON_DIR}/HL7Object.cpp
	${COMMON_DIR}/ObjToPipe.cpp
	${COMMON_DIR}/Util.cpp
	${COMMON_DIR}/dataencode.cpp
	${COMMON_DIR}/validation.cpp
        ${HL7_COMMON_INCLUDE}
        ${SRC_24_DIR_HEADERS}
        PARENT_SCOPE
	)
ADD_DEFINITIONS( -std=c++11 )
