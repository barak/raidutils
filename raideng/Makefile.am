bin_PROGRAMS=raideng
raideng_CPPFLAGS=$(INCLUDE_DIR) -D__UNIX__ -DMESSAGES -D_DPT_ACTIVE_ALIGNMENT -DI_WANT_SNI_DEBUG -DNEW_LOGGER
raideng_LDADD=../lib/libraidutil.la
raideng_SOURCES=addr_rng.cpp array.cpp connect.cpp core.cpp core_ccb.cpp core_con.cpp core_dev.cpp core_mgr.cpp core_obj.cpp debug.cpp device.cpp dptcbuff.c driver.cpp eata2i2o.c eng_sig.c eng_std.cpp eng_unix.cpp engine.cpp englists.cpp gbl_fns.cpp hba.cpp manager.cpp msg_str.cpp object.cpp osd_unix.cpp ptrarray.cpp raid.cpp raid_bcd.cpp raid_dev.cpp raid_hba.cpp raid_mgr.cpp raiddrvr.cpp scsi_bcd.cpp scsi_ccb.cpp scsi_con.cpp scsi_dev.cpp scsi_hba.cpp scsi_mgr.cpp scsi_obj.cpp scsidrvr.cpp semaphor.c stat_log.cpp swap_em.c threads.cpp unreslvd.cpp
INCLUDES=-I../include
