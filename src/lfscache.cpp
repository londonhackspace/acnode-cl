#include "lfscache.h"

#include <Energia.h>

static const char* filename = "CardCache";

LFSCache::LFSCache(LittleFS* fs) :
	fs(fs),
	fp(nullptr)
{

}

LFSCache::~LFSCache()
{
	if(fp)
	{
		lfs_file_close(fs->getLfsHandle(), fp);
		free(fp);
		fp = nullptr;
	}
}

void LFSCache::begin()
{
	fp = (lfs_file_t*)malloc(sizeof(lfs_file_t));
	int ret = lfs_file_open(fs->getLfsHandle(), fp, filename, LFS_O_RDWR | LFS_O_CREAT);
	if(ret != LFS_ERR_OK)
	{
		Serial.print("Error opening cache! Error ");
		Serial.println(ret, DEC);
		free(fp);
		fp = nullptr;
	}
}

Card LFSCache::get(Card u)
{
	if(!fp)
	{
		return Card();
	}

	user usr;

	// seek to beginning
	if(lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_SET) < 0)
	{
		Serial.println("LFSCache: Error seeking to beginning of file");
		return Card();
	}

	while(lfs_file_read(fs->getLfsHandle(), fp, &usr, sizeof(usr)) == sizeof(usr))
	{
		// skip invalid
		if(usr.invalid)
		{
			continue;
		}
		Card currentUsr(usr.uid, usr.uidlen, usr.status, usr.maintainer);
		if(currentUsr.compare_uid(u))
		{
			return currentUsr;
		}
	}

	return Card();
}

Card LFSCache::get(size_t n)
{
	if(!fp) return Card();

	user usr;
	int32_t pos = sizeof(user)*n;
	if(lfs_file_seek(fs->getLfsHandle(), fp, pos, LFS_SEEK_SET) == pos)
	{
		if(lfs_file_read(fs->getLfsHandle(), fp, &usr, sizeof(usr)) == sizeof(usr))
		{
			Card nc(usr.uid, usr.uidlen, usr.status, usr.maintainer);
			return nc;
		}
	}
	else
	{
		Serial.print("LFSCache: Error seeking to file position for card ");
		Serial.println(n);
	}
	return Card();
}

size_t LFSCache::count()
{
	if(!fp) return 0;
	int32_t pos = lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_END);
	return pos/sizeof(user);
}

void LFSCache::set(const Card u)
{
	int32_t spare_pos = -1;

	user usr;
	user newUser;

	memset(newUser.uid, 0, 7);
	u.get_uid(newUser.uid);

	newUser.status = u.is_user();
	newUser.maintainer = u.is_maintainer();
	newUser.uidlen = u.get_longuid();
	newUser.invalid = 0;

	// seek to beginning
	if(lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_SET) < 0)
	{
		Serial.println("LFSCache: Error seeking to beginning of file");
		return;
	}

	while(lfs_file_read(fs->getLfsHandle(), fp, &usr, sizeof(usr)) == sizeof(usr))
	{
		// skip invalid
		if(usr.invalid)
		{
			if(spare_pos < 0)
			{
				spare_pos = lfs_file_tell(fs->getLfsHandle(), fp) - sizeof(user);
			}
			continue;
		}

		Card currentUsr(usr.uid, usr.uidlen, usr.status, usr.maintainer);
		if(currentUsr.compare_uid(u))
		{
			if(lfs_file_seek(fs->getLfsHandle(), fp, -sizeof(user), LFS_SEEK_CUR) < 0)
			{
				Serial.println("Error seeking while updating card");
				return;
			}
			if(lfs_file_write(fs->getLfsHandle(), fp, &newUser, sizeof(user)) < 0)
			{
				Serial.println("LFSCache: Error writing while updating card");
			}
			else 
			{
				lfs_file_sync(fs->getLfsHandle(), fp);
			}
			return;
		}
	}

	// I guess we didn't find any existing file to update
	if(spare_pos > 0)
	{
		lfs_file_seek(fs->getLfsHandle(), fp, spare_pos, LFS_SEEK_SET);
	}
	else
	{
		lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_END);
	}

	if(lfs_file_write(fs->getLfsHandle(), fp, &newUser, sizeof(user)) < 0)
	{
		Serial.println("LFSCache: Error writing card");
	}
	else
	{
		lfs_file_sync(fs->getLfsHandle(), fp);
	}
}

void LFSCache::purge(void)
{
	if(lfs_file_truncate(fs->getLfsHandle(), fp, 0) < 0)
	{
		Serial.println("Error truncating file");
	}
	else
	{
		lfs_file_sync(fs->getLfsHandle(), fp);
	}
}

int LFSCache::each(void( *callback)(Card u))
{
	if(!fp)
	{
		return 0;
	}

	int count = 0;

	if(lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_SET) < 0)
	{
		Serial.println("LFSCache: Error seeking to beginning of file");
		return 0;
	}

	user usr;

	while(lfs_file_read(fs->getLfsHandle(), fp, &usr, sizeof(usr)) == sizeof(usr))
	{
		// skip invalid
		if(usr.invalid)
		{
			continue;
		}
		Card currentUsr(usr.uid, usr.uidlen, usr.status, usr.maintainer);
		callback(currentUsr);
		++count;
	}
	return count;
}

void LFSCache::verify(void)
{


}
