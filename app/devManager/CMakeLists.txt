cmake_minimum_required(VERSION 3.5)

include_directories("/home/dpdk/liqing/code/GW_handover/handover_Server/dependence/include/")

include_directories("/home/dpdk/liqing/common/include")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/app/devManager")

SET(BIN_NAME devManager) 

#通用源文件 
aux_source_directory(${PROJECT_SOURCE_DIR}/app/devManager BIN_SRCS) 

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRCS) 

aux_source_directory(${PROJECT_SOURCE_DIR}/src/regdev REG_SRCS)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/gpio GPIO_SRCS)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/spidev SPIDEV_SRCS)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/i2cdev I2CDEV_SRCS)  


#add_executable(${BIN_NAME} ${BIN_SRCS} ${SRCS} ${SPIDEV_SRCS})
add_executable(${BIN_NAME} ${BIN_SRCS} ${SRCS} ${REG_SRCS} ${GPIO_SRCS} ${SPIDEV_SRCS} ${I2CDEV_SRCS})

add_library(zlog SHARED IMPORTED)
add_library(cjson SHARED IMPORTED)
add_library(gwapp SHARED IMPORTED)
add_library(broker SHARED IMPORTED)
add_library(mosquitto SHARED IMPORTED)

# 设置第三方动态库属性
set_target_properties(zlog PROPERTIES IMPORTED_LOCATION
                      /home/dpdk/liqing/common/lib/libzlog.so)

set_target_properties(cjson PROPERTIES IMPORTED_LOCATION
                      /home/dpdk/liqing/common/lib/libcjson.so)

set_target_properties(gwapp PROPERTIES IMPORTED_LOCATION
					  /home/dpdk/liqing/code/GW_handover/handover_Server/dependence/lib_arm/libgwapp.so)
					  
set_target_properties(broker PROPERTIES IMPORTED_LOCATION
                      ${PROJECT_SOURCE_DIR}/lib/libbroker.so)
					  
set_target_properties(mosquitto PROPERTIES IMPORTED_LOCATION
                      ${PROJECT_SOURCE_DIR}/lib/libmosquitto.so)

					  
#link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(${BIN_NAME} cjson)
target_link_libraries(${BIN_NAME} zlog)
target_link_libraries(${BIN_NAME} -pthread)
target_link_libraries(${BIN_NAME} gwapp)

target_link_libraries(${BIN_NAME} broker)
target_link_libraries(${BIN_NAME} mosquitto)

install(TARGETS ${BIN_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
