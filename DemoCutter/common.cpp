#include "demoCut.h"


// Code is 99%-100% from jomme, from various files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//


static	int			cmd_argc;
static	char* cmd_argv[MAX_STRING_TOKENS];		// points into cmd_tokenized
static	char		cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];	// will have 0 bytes inserted




int		Cmd_Argc(void) {
	return cmd_argc;
}
char* Cmd_Argv(int arg) {
	if ((unsigned)arg >= cmd_argc) {
		return "";
	}
	return cmd_argv[arg];
}

void Q_strncpyz(char* dest, const char* src, int destsize) {
	// bk001129 - also NULL dest
	if (!dest) {
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL dest");
	}
	if (!src) {
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL src");
	}
	if (destsize < 1) {
		Com_Error(ERR_FATAL, "Q_strncpyz: destsize < 1");
	}

	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = 0;
}

void QDECL Com_Printf(const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	std::cout << msg;
}
void QDECL Com_DPrintf(const char* fmt, ...) {
#ifdef DEBUG
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	std::cout << msg;
#endif
}
void QDECL Com_Error(int ignore, const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	std::cout << msg;
}
char* Cmd_ArgsFrom(int arg) {
	static	char		cmd_args[BIG_INFO_STRING];
	int		i;

	cmd_args[0] = 0;
	if (arg < 0)
		arg = 0;
	for (i = arg; i < cmd_argc; i++) {
		strcat(cmd_args, cmd_argv[i]);
		if (i != cmd_argc - 1) {
			strcat(cmd_args, " ");
		}
	}

	return cmd_args;
}


void Com_Memcpy(void* dest, const void* src, const size_t count)
{
	memcpy(dest, src, count);
}
void Com_Memset(void* dest, const int val, const size_t count)
{
	memset(dest, val, count);
}



int Q_vsnprintf(char* str, size_t size, const char* format, va_list ap) {
	int retval;
	retval = _vsnprintf(str, size, format, ap);
	if (retval < 0 || retval == size) {
		// Microsoft doesn't adhere to the C99 standard of vsnprintf,
		// which states that the return value must be the number of
		// bytes written if the output string had sufficient length.
		//
		// Obviously we cannot determine that value from Microsoft's
		// implementation, so we have no choice but to return size.
		str[size - 1] = '\0';
		return size;
	}
	return retval;
}
void QDECL Com_sprintf(char* dest, int size, const char* fmt, ...) {
	int		len;
	va_list		argptr;

	va_start(argptr, fmt);
	len = Q_vsnprintf(dest, size, fmt, argptr);
	va_end(argptr);
	if (len >= size) {
		Com_Printf("Com_sprintf: overflow of %i in %i\n", len, size);
#ifdef	_DEBUG
		__asm {
			int 3;
		}
#endif
	}
}

void Cmd_TokenizeString(const char* text_in) {
	const char* text;
	char* textOut;

	// clear previous args
	cmd_argc = 0;

	if (!text_in) {
		return;
	}

	text = text_in;
	textOut = cmd_tokenized;

	while (1) {
		if (cmd_argc == MAX_STRING_TOKENS) {
			return;			// this is usually something malicious
		}

		while (1) {
			// skip whitespace
			while (*text && *text <= ' ') {
				text++;
			}
			if (!*text) {
				return;			// all tokens parsed
			}

			// skip // comments
			if (text[0] == '/' && text[1] == '/') {
				return;			// all tokens parsed
			}

			// skip /* */ comments
			if (text[0] == '/' && text[1] == '*') {
				while (*text && (text[0] != '*' || text[1] != '/')) {
					text++;
				}
				if (!*text) {
					return;		// all tokens parsed
				}
				text += 2;
			}
			else {
				break;			// we are ready to parse a token
			}
		}

		// handle quoted strings
		if (*text == '"') {
			cmd_argv[cmd_argc] = textOut;
			cmd_argc++;
			text++;
			while (*text && *text != '"') {
				/*if (*text == '\\' && text[1] == '"') {
					text++; // Allow double quotes inside double quotes if they are escaped
				}*/ // This causes issues on some demos... sad.
				*textOut++ = *text++;
			}
			*textOut++ = 0;
			if (!*text) {
				return;		// all tokens parsed
			}
			text++;
			continue;
		}

		// regular token
		cmd_argv[cmd_argc] = textOut;
		cmd_argc++;

		// skip until whitespace, quote, or command
		while (*text > ' ') {
			if (text[0] == '"') {
				break;
			}

			if (text[0] == '/' && text[1] == '/') {
				break;
			}

			// skip /* */ comments
			if (text[0] == '/' && text[1] == '*') {
				break;
			}

			/*if (*text == '\\' && text[1] == '"') {
				text++; // Allow double quotes as string content if they are escaped (let's hope this does not interefere with other stuff)
			}*/ // This causes issues on some demos... sad.

			*textOut++ = *text++;
		}

		*textOut++ = 0;

		if (!*text) {
			return;		// all tokens parsed
		}
	}

}

char* QDECL va(const char* format, ...) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char* buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	return buf;
}