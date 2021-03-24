#include "lfscache.h"
#include "network.h"
#include "acnode.h"

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
		wdog.feed();

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

	if(u.is_maintainer())
	{
		Serial.println("Setting maintainer card");
	}
	else
	{
		Serial.println("Setting user card");	
	}

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
		wdog.feed();

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
			if(currentUsr ==  u)
			{
				// no update
				return;
			}
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
		wdog.feed();
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
	if(!fp)
	{
		return;
	}

	if(lfs_file_seek(fs->getLfsHandle(), fp, 0, LFS_SEEK_SET) < 0)
	{
		Serial.println("LFSCache: Error seeking to beginning of file");
		return;
	}

	user usr;
	wdog.feed();
	while(lfs_file_read(fs->getLfsHandle(), fp, &usr, sizeof(usr)) == sizeof(usr))
	{
		wdog.feed();
		if(usr.invalid)
		{
			continue;
		}

		bool update = false;

		Card t(usr.uid, usr.uidlen, usr.status, usr.maintainer);
		int status = networking::querycard(t);
		if(status == 0)
		{
			Serial.print("User no longer valid: ");
			t.dump();
			update = true;
		}
		else if(status == 1 && usr.maintainer)
		{
			usr.maintainer = 0;
			update = true;
		}
		else if(status == 2 && !usr.maintainer)
		{
			usr.maintainer = 1;
			update = true;
		}
		if(update)
		{
			if(lfs_file_seek(fs->getLfsHandle(), fp, -sizeof(user), LFS_SEEK_CUR) < 0)
			{
				Serial.println("Error seeking while verifying");
				return;
			}

			if(lfs_file_write(fs->getLfsHandle(), fp, &usr, sizeof(user)) < 0)
			{
				Serial.println("Error updating card while verifying");
				return;
			}
		}
	}

	lfs_file_sync(fs->getLfsHandle(), fp);
}
