cmake_minimum_required (VERSION 2.6)

set ( PROJECT run_test )

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(_DELIM_ ":")
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(_DELIM_ "\;")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")


set (JINTEROPDEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../dcom/jar/j-interopdeps.jar)
set (JCIFS_PATH        ${CMAKE_CURRENT_SOURCE_DIR}/../../dcom/jar/jcifs-1.2.19.jar )
set (JINTEROP_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/../../dcom/jar/j-interop.jar    )

set ( JAR_PATH "${JINTEROPDEPS_PATH}${_DELIM_}${JCIFS_PATH}${_DELIM_}${JINTEROP_PATH}" )
set ( LIB_GCJ_PATH /usr/share/java/libgcj-4.6.jar )
set ( OBJ_PATH ${CMAKE_CURRENT_BINARY_DIR}/obj )
set ( HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../include/opc )
set ( OUT_CLASS_PATH ${CMAKE_CURRENT_BINARY_DIR}/class )

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  MESSAGE("Linux....")
  set(_DELIM_ ":")
  set(GCJH_NAME gcjh)
  set(GCJ_NAME gcj)
  set(GCJH_CLASS_PATH "${JAR_PATH}${_DELIM_}${OUT_CLASS_PATH}${_DELIM_}${LIB_GCJ_PATH}${_DELIM_}${add_class_path}")
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  MESSAGE("Windows....")
  set(_DELIM_ "\;")
  #execute_process(COMMAND echo "gcjh %*\nexit 0" OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/run_gcjh.bat)
  set(LIBGCJ_JAR ${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/lib/gcc/mingw32/4.4.0/libgcj-4.4.0.jar)
  set(GCJH_NAME ${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/bin/gcjh.exe )
  set(GCJ_NAME  ${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/bin/gcj.exe -v -B /mingw/lib -B /mingw/lib/gcc/4.6.2/ 
                                -B ${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/bin/ -B ${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/lib/ 
								-fbootclasspath=${CMAKE_CURRENT_BINARY_DIR}/../../../gcj/lib/gcc/mingw32/4.4.0/libgcj-4.4.0.jar)
  set(GCJH_CLASS_PATH "${JAR_PATH}${_DELIM_}${OUT_CLASS_PATH}${_DELIM_}${LIB_GCJ_PATH}${_DELIM_}${add_class_path}${_DELIM_}${LIBGCJ_JAR}")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")





macro(obj_produce file_name class_path out full_o_file_list)
  set(o_file_path ${OBJ_PATH}/${file_name}.o )
  
  set(${out} ${GCJ_NAME} -c -CLASSPATH "${JAR_PATH}${_DELIM_}${class_path}${_DELIM_}${OUT_CLASS_PATH}" -o ${o_file_path} )
  
  set ( ${full_o_file_list} ${${full_o_file_list}} ${o_file_path} )
endmacro(obj_produce)

macro(class_produce file_name class_path out)
  
  set(${out} ${GCJ_NAME} -C -CLASSPATH "${JAR_PATH}${_DELIM_}${class_path}" -d ${OUT_CLASS_PATH})
endmacro(class_produce)

macro(header_produce file_name add_class_path out)
    set(${out} ${GCJH_NAME} -force -classpath "${GCJH_CLASS_PATH}" -d ${HEADER_PATH})
endmacro(header_produce)


macro (build_java_file java_lib_root additional_class_path java_file_dir_path java_file target full_o_file_list)
  STRING ( REPLACE "/" "." CLASS_FULL_NAME ${java_file_dir_path}/${java_file} )
  
  obj_produce   (${target}    "${additional_class_path}" OBJECT_BUILD ${full_o_file_list} )
  class_produce (${java_file} "${additional_class_path}" CLASS_BUILD  )
  header_produce(${java_file} empty_param  HEADER_BUILD )

  add_custom_target(${target}.class      ALL ${CLASS_BUILD}      "${java_lib_root}/${java_file_dir_path}/${java_file}.java"  )
  add_custom_target(${target}.ah_dir     ALL ${CMAKE_COMMAND} -E make_directory ${HEADER_PATH}/${java_file_dir_path}         )
  add_custom_target(${target}.h          ALL ${HEADER_BUILD}     "${CLASS_FULL_NAME}"                                        )
  
  add_custom_target(${target}.o          ALL ${OBJECT_BUILD}     "${java_lib_root}/${java_file_dir_path}/${java_file}.java"  )
  
  #add_custom_command(TARGET ${target}.h  PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory )
  #add_custom_target(${target}.o     ALL ${OBJECT_BUILD}     "${OUT_CLASS_PATH}/${java_file_dir_path}/${java_file}.class"  )
  
  add_custom_target(${target}.o_fix ALL objcopy -L '_ZGr8_$$_dummy' ${OBJ_PATH}/${target}.o )
  
  #MESSAGE("dir: " "${HEADER_PATH}/${java_file_dir_path}" )
  
  #add_custom_command(TARGET ${target}.h COMMAND ${CMAKE_COMMAND} -E make_directory ${HEADER_PATH}/${java_file_dir_path} )
  #add_custom_command(TARGET ${target}.h COMMAND ${CMAKE_COMMAND} -E "make_directory" "my_dir" )
  
#  set (VV objcopy -L '_ZGr8_$_dummy' ${OBJ_PATH}/${target}.o)
  
#  MESSAGE( "o_files_list: " ${VV} )
  
#  set (tmp ${tmp}  ${target}.o  )
#  set (full_o_file_list ${tmp}  )
#  set (full_o_file_list ${target}.o ${${full_o_file_list}} )
  #set (VVV ${VVV} ${target}.o  )
endmacro()

macro (build_additional_files full_o_files_list)
  #add_custom_target (jcifs.o         ALL ${OBJ_PRODUCE} -o ../../obj/jcifs.o         ../../lib/jcifs-1.2.19.jar  )
  #add_custom_target (j-interopdeps.o ALL ${OBJ_PRODUCE} -o ../../obj/j-interopdeps.o ../../lib/j-interopdeps.jar )
  #add_custom_target (j-interop.o     ALL ${OBJ_PRODUCE} -o ../../obj/j-interop.o     ../../lib/j-interop.jar     )

  obj_produce(jcifs-1.2.19 empty_param OBJECT_BUILD ${full_o_files_list})
  add_custom_target(gcjfs-1.2.19.o     ALL ${OBJECT_BUILD}     ${JCIFS_PATH}) #../../jar/jcifs-1.2.19.jar  )

  obj_produce(j-interopdeps empty_param OBJECT_BUILD ${full_o_files_list})
  add_custom_target(j-interopdeps.o     ALL ${OBJECT_BUILD}    ${JINTEROPDEPS_PATH} )#../../jar/j-interopdeps.jar  )
  
  obj_produce(j-interop empty_param OBJECT_BUILD ${full_o_files_list})
  add_custom_target(j-interop.o     ALL ${OBJECT_BUILD}     ${JINTEROP_PATH})#../../jar/j-interop.jar  )
  
  
  header_produce (JIVariant ${JINTEROP_PATH} HEADER_BUILD)#../../jar/j-interop.jar  HEADER_BUILD)  "org/jinterop/dcom/core"
  add_custom_target(JIVariant.ah_dir ALL ${CMAKE_COMMAND} -E make_directory "${HEADER_PATH}/org/jinterop/dcom/core" )
  add_custom_target(JIVariant.h      ALL ${HEADER_BUILD}    org.jinterop.dcom.core.JIVariant )

  
  header_produce (JISystem ${JINTEROP_PATH} HEADER_BUILD) #../../jar/j-interop.jar  HEADER_BUILD)  "org/jinterop/dcom/common"
  add_custom_target(JISystem.ah_dir ALL ${CMAKE_COMMAND} -E make_directory "${HEADER_PATH}/org/jinterop/dcom/common" )
  add_custom_target(JISystem.h     ALL ${HEADER_BUILD}    org.jinterop.dcom.common.JISystem )

endmacro()

set (OPC_DCOM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/org.openscada.opc.dcom/src)
set (OPC_LIB_SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/org.openscada.opc.lib/src)
set (ADDITIONAL_CLASS_PATH "${OPC_DCOM_SRC_DIR}${_DELIM_}${OPC_LIB_SRC_DIR}")

build_additional_files(full_o_file_list2)





#set (full_o_file_list AAA)

file (MAKE_DIRECTORY ${OBJ_PATH})

build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl BaseCOMObject BaseCOMObject full_o_file_list )
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl EnumGUID EnumGUID  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl EnumString EnumString  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl EventHandlerImpl EventHandlerImpl  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl Helper Helper  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common/impl OPCCommon OPCCommon  full_o_file_list)

build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common Categories Categories1  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common Constants Constants1  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common EventHandler EventHandler  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common FILETIME FILETIME  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common KeyedResult KeyedResult  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common KeyedResultSet KeyedResultSet  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common Result Result  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/common ResultSet ResultSet  full_o_file_list)

build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCAsyncIO2 OPCAsyncIO2  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCBrowse OPCBrowse full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCBrowseServerAddressSpace OPCBrowseServerAddressSpace full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCDataCallback OPCDataCallback full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCGroupStateMgt OPCGroupStateMgt full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCItemIO OPCItemIO full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCItemMgt OPCItemMgt full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCItemProperties OPCItemProperties full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCServer OPCServer full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da/impl OPCSyncIO OPCSyncIO full_o_file_list)

build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da Constants Constants2 full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da IOPCDataCallback IOPCDataCallback full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da IORequest IORequest full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCBROWSEDIRECTION OPCBROWSEDIRECTION full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCBROWSETYPE OPCBROWSETYPE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCDATASOURCE OPCDATASOURCE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCENUMSCOPE OPCENUMSCOPE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCGroupState OPCGroupState full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCITEMDEF OPCITEMDEF full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCITEMRESULT OPCITEMRESULT full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCITEMSTATE OPCITEMSTATE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCNAMESPACETYPE OPCNAMESPACETYPE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCSERVERSTATE OPCSERVERSTATE full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da OPCSERVERSTATUS OPCSERVERSTATUS full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da PropertyDescription PropertyDescription full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da ValueData ValueData  full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/da WriteRequest WriteRequest1 full_o_file_list)

build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/list/impl OPCServerList OPCServerList full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/list      ClassDetails  ClassDetails full_o_file_list)
build_java_file("${OPC_DCOM_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/dcom/list      Constants     Constants3 full_o_file_list)



build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/common      AlreadyConnectedException   AlreadyConnectedException full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/common      ConnectionInformation       ConnectionInformation     full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/common      NotConnectedException       NotConnectedException     full_o_file_list)

build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     Access       Access          full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     BaseBrowser  BaseBrowser     full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     Branch       Branch          full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     FlatBrowser  FlatBrowser     full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     Leaf         Leaf            full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da/browser     TreeBrowser  TreeBrowser     full_o_file_list)

build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AccessBase  AccessBase    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AccessStateListener  AccessStateListener    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AddFailedException  AddFailedException    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     Async20Access  Async20Access    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AutoReconnectController  AutoReconnectController    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AutoReconnectListener  AutoReconnectListener    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     AutoReconnectState  AutoReconnectState    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     DataCallback  DataCallback    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     DuplicateGroupException  DuplicateGroupException    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ErrorMessageResolver  ErrorMessageResolver    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     Group  Group    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     Item  Item    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ItemState  ItemState    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     Server  Server    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ServerConnectionStateListener  ServerConnectionStateListener    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ServerStateListener  ServerStateListener    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ServerStateOperation  ServerStateOperation    full_o_file_list)
#build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     ServerStateReader  ServerStateReader    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     SyncAccess  SyncAccess    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     UnknownGroupException  UnknownGroupException    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/da     WriteRequest  WriteRequest2    full_o_file_list)

build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/list     Categories  Categories2    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/list     Category  Category    full_o_file_list)
build_java_file("${OPC_LIB_SRC_DIR}" "${ADDITIONAL_CLASS_PATH}" org/openscada/opc/lib/list     ServerList ServerList    full_o_file_list)

#add_custom_target ( BAA ALL echo hello ${full_o_file_list}  )

add_custom_target(utgard.a ALL ar cr ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/utgard.a ${full_o_file_list} )
add_custom_target(jinterop.a ALL ar cr ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/j-interop.a ${full_o_file_list2} )

#message (STATUS "ofiles: "  full_o_file_list)
#add_executable ( SampleTestServer ../c++/run_test.cpp  ${full_o_file_list}  ../../obj/jcifs.o ../../obj/j-interop.o ../../obj/j-interopdeps.o  )
#target_link_libraries (SampleTestServer gcj )







