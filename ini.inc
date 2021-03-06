// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_ini
	#undef _inc_ini
#endif
// custom include-guard to ensure we don't duplicate
#if defined _ini_included
	#endinput
#endif
#define _ini_included


#include <a_samp>
#include <logger>
#include <map>


#define MAX_FILE_NAME       (256)
#define MAX_RECORDS         (999)
#define MAX_KEY_LENGTH      (64)
#define MAX_VAL_LENGTH      (512)
#define MAX_RECORD_LENGTH   (MAX_KEY_LENGTH + MAX_VAL_LENGTH + 2)

// Error values are non-zero positive
enum {
	INI_ERR_EXISTS = 1,
	INI_ERR_NOT_EXISTS,
	INI_ERR_OPEN,
	INI_ERR_NOT_OPEN,
	INI_ERR_IO_FAIL,
	INI_ERR_NO_KEY
}

forward ini_open(const filename[]);
forward ini_commit();
forward ini_close();
forward ini_isOpen();
forward ini_isKey(const key[]);
forward ini_remove(const key[]);

forward ini_getString(const key[], dest[], len = sizeof(dest));
forward ini_getInt(const key[], &dest);
forward ini_getFloat(const key[], &Float:dest);

forward ini_setString(const key[], const value[]);
forward ini_setInt(const key[], value);
forward ini_setFloat(const key[], Float:value);

enum E_CACHE_STRUCT {
	E_CACHE_KEY[MAX_KEY_LENGTH],
	E_CACHE_VALUE[MAX_VAL_LENGTH],
	E_CACHE_DELETED
}

new
	ini_fileName[MAX_FILE_NAME],
	Map:ini_cache,
	bool:ini_fileOpen;

#include "ini-parser.pwn"
#include "ini-writer.pwn"
#include "ini-access-get.pwn"
#include "ini-access-set.pwn"
#include "ini-misc.pwn"

stock ini_open(const filename[]) {
	if(ini_fileOpen) {
		Logger_Err("attempt to open ini while file already open", Logger_S("filename", filename));
		return INI_ERR_OPEN;
	}

	new ret;

	if(fexist(filename)) {
		Logger_Dbg("ini", "opened existing file", Logger_S("filename", filename));

		new File:handle = fopen(filename, io_read);
		if(!handle) {
			Logger_Err("failed to open file for reading", Logger_S("filename", filename));
			return INI_ERR_IO_FAIL;
		}

		ret = ini_parse(handle, ini_cache);
		fclose(handle);
	} else {
		Logger_Dbg("ini", "opened new file", Logger_S("filename", filename));
	}

	_ini_strcpy(ini_fileName, filename, MAX_FILE_NAME);
	ini_fileOpen = true;

	return ret;
}

stock ini_commit() {
	if(!ini_fileOpen) {
		Logger_Err("attempt to commit ini with no open file");
		return INI_ERR_NOT_OPEN;
	}

	new File:handle = fopen(ini_fileName, io_write);
	if(!handle) {
		Logger_Err("failed to open file for writing", Logger_S("filename", ini_fileName));
		return INI_ERR_IO_FAIL;
	}

	new ret = ini_write(handle, ini_cache);
	fclose(handle);

	Logger_Dbg("ini", "comitted file");

	ini_close();

	return ret;
}

stock ini_close() {
	if(!ini_fileOpen) {
		return INI_ERR_NOT_OPEN;
	}

	ini_fileOpen = false;

	Logger_Dbg("ini", "closed file");

	return 0;
}

stock ini_isOpen() {
	return ini_fileOpen;
}

stock ini_isKey(const key[]) {
	if(!ini_isOpen()) {
		return false;
	}

	return MAP_contains_str(ini_cache, key);
}

stock ini_remove(const key[]) {
	if(!ini_isOpen()) {
		return INI_ERR_NOT_OPEN;
	}

	if(!MAP_contains_str(ini_cache, key)) {
		return INI_ERR_NO_KEY;
	}

	MAP_remove_str(ini_cache, key);

	return 0;
}

stock _ini_printCache(const message[]) {
	Logger_Log(message);
	Logger_Log("begin ini cache");
	for(new i = 0, j = ini_recordsRead + ini_recordsAdded; i < j; ++i) {
		Logger_Log("--record",
			Logger_I("index", i),
			Logger_I("deleted", ini_cache[i][E_CACHE_DELETED]),
			Logger_S("key", ini_cache[i][E_CACHE_KEY]),
			Logger_S("value", ini_cache[i][E_CACHE_VALUE]));
	}
	Logger_Log("end ini cache");
}
