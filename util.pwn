#if defined _strcpy_included
	#endinput
#endif
#define _strcpy_included

// strcpy copies a string from source to destination
_ini_strcpy(dest[], const source[], maxlength = sizeof dest) {
	dest[0] = EOS;
	return strcat(dest, source, maxlength);
}

// isNull returns true if a string is empty
_ini_isNull(string[]) {
	return (
		string[0] == 0 || 
		(string[0] == 1 && string[1] == 0)
	);
}
