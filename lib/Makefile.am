lib_LTLIBRARIES=libraidutil.la
libraidutil_la_SOURCES=ctlr_map.cpp dpt_buff.cpp engcalls.cpp engmsg.c findpath.c funcs.c hba_log.cpp lockunix.c scsi_log.cpp dptalign.c
libraidutil_la_CPPFLAGS=-O3 -DMESSAGES -D_DPT_ACTIVE_ALIGNMENT $(DEBUG_FLAGS) -Dtrue=1 -Dfalse=0 -DHORIZONTAL -DNEW_RSC_MGR -DNEW_RSC_HDR -D_DPT_FLASH -DNEW_LOGGER -DI_WANT_SNI_DEBUG -D__UNIX__
INCLUDES=-I../include
