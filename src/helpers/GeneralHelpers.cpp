#include "GeneralHelpers.h"

void toCstat(Stat gstat, struct stat *st){
	st->st_dev = gstat.st_dev();
	st->st_ino = gstat.st_ino();
	st->st_mode = gstat.st_mode();
	st->st_nlink = gstat.st_nlink();
	st->st_uid = gstat.st_uid();
	st->st_gid = gstat.st_gid();
	st->st_rdev = gstat.st_rdev();
	st->st_size = gstat.st_size();
	// st->st_atime = gstat.st_atime();
	// st->st_mtime = gstat.st_mtime();
	//st->st_ctime = gstat.st_ctime();
	st->st_blksize = gstat.st_blksize();
	st->st_blocks = gstat.st_blocks();
	//st->st_attr = gstat.st_attr();
}

Stat toGstat(struct stat *st){
	Stat gstat;
	gstat.set_st_dev(st->st_dev);
	gstat.set_st_ino(st->st_ino);
	gstat.set_st_mode(st->st_mode);
	gstat.set_st_nlink(st->st_nlink);
	gstat.set_st_uid(st->st_uid);
	gstat.set_st_gid(st->st_gid);
	gstat.set_st_rdev(st->st_rdev);
	gstat.set_st_size(st->st_size);
	// gstat.set_st_atime(st->st_atime);
	// gstat.set_st_mtime(st->st_mtime);
	// gstat.set_st_ctime(st->st_ctime);
	gstat.set_st_blksize(st->st_blksize);
	gstat.set_st_blocks(st->st_blocks);
	// gstat.set_st_attr(st->st_attr);
	return gstat;
}

FuseFileInfo toGFileInfo(struct fuse_file_info *fi){
	FuseFileInfo fuseFileInfo;
	fuseFileInfo.set_fh(fi->fh);
	return fuseFileInfo;
}

void toCFileInfo(FuseFileInfo fuseFileInfo, struct fuse_file_info *fi){
	fi->fh = fuseFileInfo.fh();
}