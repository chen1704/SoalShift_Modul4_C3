#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/chen1704/Documents";

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

  	DIR *dp;
  	struct dirent *de;

	char ext;
	char temp[500];
	int temp1,temp2;	
	int panjang = strlen(fpath)-1;
	ext=fpath[panjang];

	if(ext == 'f'|| ext == 'c' || ext == 't'){
		char file[1000], command[1000];
		sprintf(file, "%s.ditandai", fpath);
		rename(fpath,file);
		sprintf(command, "chmod 000 %s.ditandai", fpath);
		system(command);
		system("notify-send \"Pesan Error: \" \"Terjadi Kesalahan! File berisi konten berbahaya.\" ");
		system("mkdir /home/chen1704/Documents/rahasia");
		system("mv /home/chen1704/Documents/*.ditandai /home/chen1704/Documents/rahasia");
		
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
		/*system(temp);
		temp2 = mkdir("/home/maile/Documents/rahasia", 0777);
		char before[200], after[200];
		strcpy(before,de->d_name);
		sprintf(before,"%s/%s", dirpath,de->d_name);
		sprintf(after,"%s/rahasia/%s.ditandai",dirpath,de->d_name);
		temp1 = rename(before,after);
		temp2 = chmod(after,0000);*/

}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
