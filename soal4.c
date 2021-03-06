#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/chen1704/Downloads";


static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	sprintf(fpath, "%s%s", dirpath,path);
	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  	int fd = 0 ;
	char ext[20];
	int panjang;
	panjang=strlen(fpath)-1;	
	strcpy(ext,fpath+panjang-4);
	if(strcmp(ext,".copy")==0){
		system("notify-send \"Pesan Error: \" \"File yang anda buka adalah file hasil salinan. File tidak bisa diubah maupun disalin kembali!\" ");
		return -errno;
	}
	else{
		(void) fi;
		fd = open(fpath, O_RDONLY);
		if (fd == -1)
			return -errno;

		res = pread(fd, buf, size, offset);
		if (res == -1)
			res = -errno;

		close(fd);
		return res;
	}
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	sprintf(fpath, "%s%s", dirpath,path);
	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char to2[256],from2[256];
	system("mkdir -p /home/chen1704/Downloads/simpanan");
	sprintf(to2,"%s/simpanan/%s.copy",dirpath,to);		
	sprintf(from2,"%s%s",dirpath,from);
	res = rename(from2, to2);
	
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    	int res;
 	char fpath[1000];
 	sprintf(fpath,"%s%s", dirpath, path);
    	res = mknod(fpath, mode, rdev);
    	if(res == -1)
        	return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.chmod 		= xmp_chmod,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write		= xmp_write,
	.rename 	= xmp_rename,
	.mknod		= xmp_mknod,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}


// gcc -Wall `pkg-config fuse --cflags` soal3.c -o soal3 `pkg-config fuse --libs`
