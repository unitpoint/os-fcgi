#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include "win/os-fcgi/stdafx.h"
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#endif

#include "os/objectscript.h"
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "3rdparty/md5/md5.h"

#ifndef _MSC_VER
#include <pthread.h>
#endif

#ifndef OS_CURL_DISABLED
#include "os/ext-curl/os-curl.h"
#endif

#ifndef OS_SQLITE3_DISABLED
#include "os/ext-sqlite3/os-sqlite3.h"
#endif

#ifndef OS_REGEXP_DISABLED
#include "os/ext-regexp/os-regexp.h"
#endif

#ifndef OS_DATETIME_DISABLED
#include "os/ext-datetime/os-datetime.h"
#endif

#ifndef OS_ODBO_DISABLED
#include "os/ext-odbo/os-odbo.h"
#endif

#ifndef OS_ZLIB_DISABLED
#include "os/ext-zlib/os-zlib.h"
#endif

using namespace ObjectScript;

/* check that argument has no extra characters at the end */
#define noextrachars(x)		{if ((x)[2] != '\0') return -1;}

/* indices of various argument indicators in array args */
#define has_i		0	/* -i */
#define has_v		1	/* -v */
#define has_e		2	/* -e */
#define has_E		3	/* -E */

#define NUM_HAS		4	/* number of 'has_*' */

#ifndef OS_PROMPT
#define OS_PROMPT	"> "
#define OS_PROMPT2	">> "
#endif

#ifndef OS_MAXINPUT
#define OS_MAXINPUT	512
#endif

#define EOFMARK		"<eof>"
#define marklen		(sizeof(EOFMARK)/sizeof(char) - 1)

class ConsoleOS: public OS
{
protected:

	int shutdown_funcs_id;
	bool header_sent;
	Core::String * cache_path;

	virtual ~ConsoleOS()
	{
	}

	virtual bool init(MemoryManager * mem)
	{
		if(OS::init(mem)){
			// core->gc_start_used_bytes = 32*1024*1024;
			cache_path = new (malloc(sizeof(Core::String) OS_DBG_FILEPOS)) Core::String(this, 
#ifdef _MSC_VER
				"cache-osc"
#else
				"/tmp"
#endif
				);

#ifndef OS_CURL_DISABLED
			initCurlLibrary(this);
#endif

#ifndef OS_SQLITE3_DISABLED
			initSqlite3Library(this);
#endif

#ifndef OS_REGEXP_DISABLED
			initRegexpLibrary(this);
#endif

#ifndef OS_DATETIME_DISABLED
			initDateTimeLibrary(this);
#endif

#ifndef OS_ODBO_DISABLED
			initODBOLibrary(this);
#endif

#ifndef OS_ZLIB_DISABLED
			initZLibrary(this);
#endif
			return true;
		}
		return false;
	}

	virtual void shutdown()
	{
		deleteObj(cache_path);
		OS::shutdown();
	}

public:

	ConsoleOS()
	{
		header_sent = false;
	}

	void initPreScript()
	{
#if defined _MSC_VER && defined OS_DEBUG
		setSetting(OS_SETTING_CREATE_TEXT_EVAL_OPCODES, false);
		setSetting(OS_SETTING_CREATE_TEXT_OPCODES, true);
#else
		setSetting(OS_SETTING_CREATE_TEXT_EVAL_OPCODES, false);
		setSetting(OS_SETTING_CREATE_TEXT_OPCODES, false);
#endif
		setSetting(OS_SETTING_CREATE_DEBUG_INFO, true);
		setSetting(OS_SETTING_CREATE_COMPILED_FILE, true);

		OS::initPreScript();
	}

	void initEnv(const char * var_name, char ** envp)
	{
		newObject();
		for(; *envp; envp++){
			const char * value = *envp;
			const char * split = strchr(value, '=');
			OS_ASSERT(split);
			if(split){
				pushStackValue(-1);
				pushString(value, (int)(split - value));
				pushString(split + 1);
				setProperty();
			}
		}
		setGlobal(var_name);
	}

	OS_CHAR * md5(OS_CHAR * r, const String& buf)
	{
		MD5Context context;
		unsigned char digest[16];
		MD5Init(&context);
		MD5Update(&context, (md5byte*)buf.toChar(), buf.getDataSize());
		MD5Final(&context, digest);
		
		for(int i = 0; i < 16; i++){
			r[i*2+0] = OS_TEXT("0123456789ABCDEF")[(digest[i] >> 4) & 0xf];
			r[i*2+1] = OS_TEXT("0123456789ABCDEF")[(digest[i] >> 0) & 0xf];
		}
		r[32] = 0;
		return r;
	}

	String md5(const String& buf)
	{
		OS_CHAR r[34];
		return String(this, md5(r, buf), 32);
	}

	String getCompiledFilename(const String& resolved_filename)
	{
#if 1
		String ext = getFilenameExt(resolved_filename);
		if(ext == OS_EXT_COMPILED){
			return resolved_filename;
		}
#else
		String path = getFilenamePath(resolved_filename);
		if(path == *cache_path){
			return resolved_filename;
		}
#endif	
		OS_CHAR temp[34];
		Core::Buffer buf(this);
		buf.append(*cache_path);
		buf.append(OS_TEXT("/os-cache-"));
		buf.append(md5(temp, resolved_filename), 32);
		buf.append(OS_EXT_COMPILED);
		// buf.append(changeFilenameExt(md5(resolved_filename), OS_EXT_COMPILED));
		return buf.toStringOS(); 
	}

	String getTextOpcodesFilename(const String& resolved_filename)
	{
		return changeFilenameExt(getCompiledFilename(resolved_filename), OS_EXT_TEXT_OPCODES);
	}

	OS_EFileUseType checkFileUsage(const String& sourcecode_filename, const String& compiled_filename)
	{
		struct stat sourcecode_st, compiled_st;
		stat(sourcecode_filename, &sourcecode_st);
		stat(compiled_filename, &compiled_st);
		if(sourcecode_st.st_mtime >= compiled_st.st_mtime){
			return COMPILE_SOURCECODE_FILE;
		}
		return LOAD_COMPILED_FILE;
	}

	static int registerShutdownFunction(OS * p_os, int params, int, int, void*)
	{
		if(params > 0){
			ConsoleOS * os = dynamic_cast<ConsoleOS*>(p_os);
			int offs = os->getAbsoluteOffs(-params);
			os->pushValueById(os->shutdown_funcs_id);
			for(int i = params-1; i >= 0; i--){
				os->pushStackValue();
				os->pushStackValue(offs+i);
				os->pushStackValue();
				os->setProperty();
			}
		}
		return 0;
	}

	static int decodeHexChar(OS_CHAR c)
	{
		if(c >= '0' && c <= '9') return      c - '0';
		if(c >= 'A' && c <= 'F') return 10 + c - 'A';
		if(c >= 'a' && c <= 'f') return 10 + c - 'a';
		return 0;
	}

	static OS_CHAR decodeHexCode(const OS_CHAR * s)
	{
		// OS_ASSERT(s[0] && s[1]);
		int c = decodeHexChar(s[0]) * 16 + decodeHexChar(s[1]);
		return (OS_CHAR)c;
	}

	static int urlDecode(OS * os, int params, int, int, void*)
	{
		if(params >= 1){
			String str = os->toString(-params+0);
			const OS_CHAR * s = str;
			const OS_CHAR * end = s + str.getLen();
			
			Core::Buffer buf(os);
			for(; s < end;){
				if(*s == OS_TEXT('%')){
					if(s+3 <= end){
						buf.append(decodeHexCode(s+1));
					}
					s += 3;
				}else if(*s == OS_TEXT('+')){
					buf.append(OS_TEXT(' '));
					s++;
				}else{
					buf.append(*s++);
				}
			}
			os->pushString(buf);
			return 1;
		}
		return 0;
	}

	static int urlEncode(OS * os, int params, int, int, void*)
	{
		if(params >= 1){
			String str = os->toString(-params+0);
			const OS_CHAR * s = str;
			const OS_CHAR * end = s + str.getLen();
			
			Core::Buffer buf(os);
			for(; s < end; s++){
				if( (*s >= OS_TEXT('0') && *s <= OS_TEXT('9'))
					|| (*s >= OS_TEXT('A') && *s <= OS_TEXT('Z'))
					|| (*s >= OS_TEXT('a') && *s <= OS_TEXT('z')) )
				{
					buf.append(*s);
				}else{
					buf.append(OS_TEXT('%'));
					buf.append(OS_TEXT("0123456789ABCDEF")[((OS_BYTE)(*s) >> 4) & 0xf]);
					buf.append(OS_TEXT("0123456789ABCDEF")[((OS_BYTE)(*s) >> 0) & 0xf]);
				}
			}
			os->pushString(buf);
			return 1;
		}
		return 0;
	}

	void initUrlLibrary()
	{
		FuncDef funcs[] = {
			{"decode", ConsoleOS::urlDecode},
			{"encode", ConsoleOS::urlEncode},
			{}
		};
		getModule(OS_TEXT("url"));
		setFuncs(funcs);
		pop();
	}

	void triggerShutdownFunctions()
	{
		resetTerminated();
		String iter_func(this, "reverseIter");
		pushValueById(shutdown_funcs_id);
		while(nextIteratorStep(2, iter_func)){
			if(isFunction()){
				pushStackValue();
				pushNull();
				call();
			}
			pop(2);
		}
		pop();
		
		// reset shutdown_funcs_id
		pushValueById(shutdown_funcs_id);
		getProperty("clear");
		pushValueById(shutdown_funcs_id);
		call();
	}

	void initGlobalFunctions()
	{
		FuncDef funcs[] = {
			{"registerShutdownFunction", ConsoleOS::registerShutdownFunction},
			{}
		};
		pushGlobals();
		setFuncs(funcs);
		pop();
	}

	void printUsage(char ** argv, const char *badoption)
	{
		char * progname = argv[0] && argv[0][0] ? argv[0] : "unknown progname";
		printf("%s: ", progname);
		if(badoption[1] == 'e' || badoption[1] == 'l')
			printf("'%s' needs argument\n", badoption);
		else
			printf("unrecognized option '%s'\n", badoption);
		
		printf(
			"Usage: %s [options] [script [args]]\n"
			"available options are:\n"
			"  -e stat  execute string 'stat'\n"
			"  -i       enter interactive mode after executing 'script'\n"
			"  -l name  require library 'name'\n"
			"  -v       show version information\n"
			"  -E       ignore environment variables\n"
			"  --       stop handling options\n"
			"  -        stop handling options and execute stdin\n"
			"examples:\n"
			"%s -e 'print(\"2*5 - 1 = \", 2*5 - 1)'"
			,
			progname, progname);
	}

	void printVersion()
	{
		printf("%s\n", OS_COPYRIGHT);
		printf("%s\n", OS_OPENSOURCE);
	}

	OS::String parseQString(int& i, int argc, char **argv)
	{
		Core::Buffer buf(this);
		if(i < argc){
			char c = argv[i][0] == '\'' ? '\'' : argv[i][0] == '"' ? '"' : '\0';
			if(!c){
				buf.append(argv[i]);
			}else{
				int len = OS_STRLEN(argv[i]+1);
				if(!len){
					printUsage(argv, argv[i]);
					exit(1);
				}
				if(argv[i][1+len-1] == c){
					buf.append(argv[i]+1, len-1);
				}else{
					buf.append(argv[i]+1, len);
					for(i++; i < argc; i++){
						len = OS_STRLEN(argv[i]);
						if(!len){
							printUsage(argv, argv[i]);
							exit(1);
						}
						buf.append(" ");
						if(argv[i][len-1] == c){
							buf.append(argv[i], len-1);
							break;
						}
						buf.append(argv[i], len);
					}
				}
			}
		}
		return buf.toStringOS();
	}

	int collectArgs(int argc, char **argv, int *args)
	{
		for(int i = 1; i < argc && argv[i] != NULL; i++){
			if (argv[i][0] != '-')  /* not an option? */
				return i;
			switch (argv[i][1]) {  /* option */
			case '-':
				noextrachars(argv[i]);
				return (argv[i+1] != NULL ? i+1 : 0);
			case '\0':
				return i;
			case 'E':
				args[has_E] = 1;
				break;
			case 'i':
				noextrachars(argv[i]);
				args[has_i] = 1;  /* go through */
			case 'v':
				noextrachars(argv[i]);
				args[has_v] = 1;
				break;
			case 'e':
				args[has_e] = 1;
				if (argv[i][2] == '\0') {  /* no concatenated argument? */
					i++;  /* try next 'argv' */
					parseQString(i, argc, argv);
					if (argv[i] == NULL || argv[i][0] == '-')
						return -(i - 1);  /* no next argument or it is another option */
				}
				break;
			case 'l':  /* both options need an argument */
				if (argv[i][2] == '\0') {  /* no concatenated argument? */
					i++;  /* try next 'argv' */
					parseQString(i, argc, argv);
					if (argv[i] == NULL || argv[i][0] == '-')
						return -(i - 1);  /* no next argument or it is another option */
				}
				break;
			default:  /* invalid option; return its index... */
				return -i;  /* ...as a negative value */
			}
		}
		return 0;
	}

	bool runCommand(const OS::String& command)
	{
		eval(command, 0, 0, OS_SOURCECODE_PLAIN, false, false);
		if(isExceptionSet()){
			handleException();
			return false;
		}
		return true;
	}

	bool requireLibrary(const OS::String& filename)
	{
		require(filename, true, 0, OS_SOURCECODE_AUTO, true, false);
		if(isExceptionSet()){
			handleException();
			return false;
		}
		return true;
	}

	int runArgs(int n, char **argv)
	{
		for(int i = 1; i < n; i++){
			OS_ASSERT(argv[i][0] == '-');
			switch(argv[i][1]){  /* option */
			case 'e': 
				{
					i++;
					String str = parseQString(i, n, argv);
					if(!runCommand(str))
						return 0;
					break;
				}
			case 'l': 
				{
					i++;
					String filename = parseQString(i, n, argv);
					if(!requireLibrary(filename))
						return 0;  /* stop if file fails */
					break;
				}
			default: 
				break;
			}
		}
		return 1;
	}

	bool inComplete()
	{
		OS::String str = toString(-1);
		int lmsg = str.getLen();
		const char *msg = str;
		if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
			pop();
			return true;
		}
		return false;
	}

	bool readLine(char * buf, const char * promt)
	{
		fputs(promt, stdout); fflush(stdout); /* show prompt */
		return fgets(buf, OS_MAXINPUT, stdin) != NULL; /* get line */
	}

	/* void saveLine(const char * str)
	{
	}

	void freeLine(char * buf)
	{
	} */

	const char * getPrompt(bool firstline)
	{
		return firstline ? OS_PROMPT : OS_PROMPT2;
	}

	bool pushLine(bool firstline = false)
	{
		char buf[OS_MAXINPUT];
		if(!readLine(buf, getPrompt(firstline))){
			return false;  /* no input */
		}
		int len = OS_STRLEN(buf);
		for(; len > 0 && OS_IS_SPACE(buf[len-1]); len--){
			buf[len-1] = '\0';
		}
		if(firstline && buf[0] == '='){  /* first line starts with `=' ? */
			OS::String str = OS::String(this, buf+1);
			if(str.isEmpty()){
				return false;
			}
			pushString(OS::String(this, "return ") + str);  /* change it to `return' */
		}else{
			OS::String str = OS::String(this, buf);
			if(str.isEmpty()){
				return false;
			}
			pushString(str);
		}
		// freeLine(b);
		return true;
	}

	bool compileLine()
	{
		if(!pushLine(true)){
			return false;  /* no input */
		}
		for (;;) {  /* repeat until gets a complete line */
			if(compile(toString(), OS_SOURCECODE_PLAIN, false)){
				remove(-2);
				return true;
			}
			pop(); // pop null function
			handleException();
			if(!pushLine()){  /* no more input? */
				pop(); // pop prev string
				return false;
			}
			Core::Buffer buf(this);
			buf.append(toString(-2));
			buf.append("\n");
			buf.append(toString(-1));
			pushString(buf);
			remove(-3, 2);
		}
		return false; // shutup compiler
	}

	void dotty()
	{
		bool ok;
		while(ok = compileLine()){
			OS_ASSERT(isFunction());
			int offs = getAbsoluteOffs(-1);
			pushNull();
			int res_count = 10; // getAbsoluteOffs(-1) - offs;
			call(0, res_count);
			ok = !isExceptionSet();
			if(!ok){
				handleException();
			}
			if(res_count > 0){
				getGlobal("print");
				pushNull();
				int i = res_count-1;
				for(; i >= 0; i--){
					if(!isNull(offs + i)){
						break;
					}
				}
				int count = i+1;
				if(count > 0){
					for(i = 0; i < count; i++){
						pushStackValue(offs + i);
						// pushString("\n");
					}
					call(count * 1, 0);
				}else{
					// pop(2);
					call(0, 0);
				}
				pop(res_count);
			}
		}
	}

	void processRequest(int argc, char * argv[])
	{
		int args[NUM_HAS];
		args[has_i] = args[has_v] = args[has_e] = args[has_E] = 0;
		int script = collectArgs(argc, argv, args);
		// pushStackValue(OS_REGISTER_USERPOOL);
		if(script < 0){
			printUsage(argv, argv[-script]);
			return;
		}
		if(args[has_E]){  /* option '-E'? */
#if 0
			char * environ[] = {NULL};
			initEnv("_ENV", environ);
#endif
		}else{
			initEnv("_ENV", environ);
		}
		
		getGlobal("process");
		pushString("argv");
		newArray();

		pushStackValue();
		pushString(argv[0]);
		addProperty();

		script = script > 0 ? script : argc;
		for(int i = script; i < argc; i++){
			pushStackValue();
			pushString(argv[i]);
			addProperty();
		}
		setProperty();
		
		newObject();
		shutdown_funcs_id = getValueId();
		retainValueById(shutdown_funcs_id);
		pop();
		// addProperty();

		initGlobalFunctions();
		initUrlLibrary();

		char * server_env[] = {NULL};
		initEnv("_SERVER", server_env);

		if(args[has_v]){
			printVersion();
		}
		/* execute arguments -e and -l */
		if(!runArgs(script, argv)){
			return;
		}
		if(script < argc){
			String script_filename(this, argv[script]);
		
			getGlobal("_SERVER");
			pushString("SCRIPT_FILENAME");
			pushString(script_filename);
			setProperty();

			if(script_filename.isEmpty()){
				echo("Filename is not defined\n");
			}else{
#if 0
				if(getFilename(script_filename).isEmpty()){
					String new_script_filename = script_filename + OS_TEXT("index") + OS_EXT_TEMPLATE;
					if(!isFileExist(new_script_filename)){
						new_script_filename = script_filename + OS_TEXT("index") + OS_EXT_SOURCECODE;
						new_script_filename = resolvePath(new_script_filename);
					}
					script_filename = new_script_filename;
				}
#endif	
				String ext = getFilenameExt(script_filename);
				if(ext == OS_EXT_SOURCECODE || ext == OS_EXT_TEMPLATE || ext == OS_EXT_TEMPLATE_HTML || ext == OS_EXT_TEMPLATE_HTM){
					require(script_filename, true);
					// triggerShutdownFunctions();
				}else{
	#if 1
					printf("Error ObjectScript file: %s\n", script_filename.toChar());
	#else
					FileHandle * f = openFile(script_filename, "rb");
					if(f){
						const int BUF_SIZE = 1024*256;
						int size = getFileSize(f);
						void * buf = malloc(BUF_SIZE < size ? BUF_SIZE : size OS_DBG_FILEPOS);
						for(int i = 0; i < size; i += BUF_SIZE){
							int len = BUF_SIZE < size - i ? BUF_SIZE : size - i;
							readFile(buf, len, f);
							echo((const void*)buf, len);
						}
						free(buf);				
						closeFile(f);
					}else{
						// FCGX_PutS("Error open file: ", request->out);
						// FCGX_PutS(script_filename, request->out);
					}
	#endif
				}
			}
		}
		if(args[has_i]){ /* -i option? */
			dotty();
		}else if(script == 0 && !args[has_e] && !args[has_v]){  /* no arguments? */
			if(1){ // lua_stdin_is_tty()) {
				printVersion();
				dotty();
			}else{
				// dofile(L, NULL);  /* executes stdin as a file */
			}
		}

		triggerShutdownFunctions();
	}
};

void log(const char * msg)
{
	FILE * f = fopen("/tmp/os-fcgi.log", "wt");
	if(f){
		fwrite(msg, strlen(msg), 1, f);
		fclose(f);
	}
}

#ifdef _MSC_VER
int _tmain(int argc, _TCHAR* _argv[])
{
	char ** argv = new char*[argc];
	{ 
		for(int i = 0; i < argc; i++){
			int len = 0; for(; _argv[i][len]; len++);
			argv[i] = new char[len+1];
			for(int j = 0; j <= len; j++){
				argv[i][j] = (char)_argv[i][j];
			}
		}
	}
	struct ArgvFinalizer {
		int argc;
		char ** argv;
		~ArgvFinalizer(){
			for(int i = 0; i < argc; i++){
				delete [] argv[i];
			}
			delete [] argv;
		}
	} __argv_finalizer__ = {argc, argv};
#else
int main(int argc, char * argv[])
{
#endif
	ConsoleOS * os = OS::create(new ConsoleOS());
	os->processRequest(argc, argv);
    os->release();

	return 0;
}

