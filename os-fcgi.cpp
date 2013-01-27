// os-insight.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#ifdef _MSC_VER
#include "win32/stdafx.h"
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#endif

#include "os/objectscript.h"
#include "fcgi-2.4.1/include/fcgi_stdio.h"
#include "MPFDParser-1.0/Parser.h"
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#ifndef _MSC_VER
#include <pthread.h>
#endif

#define PID_FILE "/var/run/os-fcgi.pid"

using namespace ObjectScript;

class FCGX_OS: public OS
{
protected:

	FCGX_Request * request;
	int shutdown_funcs_id;
	bool header_sent;
	Core::Buffer * buffer;

	virtual ~FCGX_OS()
	{
		OS_ASSERT(!buffer);
	}

	virtual bool init(MemoryManager * mem)
	{
		if(OS::init(mem)){
			buffer = new (malloc(sizeof(Core::Buffer) OS_DBG_FILEPOS)) Core::Buffer(this);
			return true;
		}
		return false;
	}

	virtual void shutdown()
	{
		deleteObj(buffer);
		OS::shutdown();
	}

public:

	FCGX_OS()
	{
		request = NULL;
		header_sent = false;
		buffer = NULL;
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

	void initPreScript()
	{
#if defined _MSC_VER && defined OS_DEBUG
		setSetting(OS_SETTING_CREATE_DEBUG_EVAL_OPCODES, false);
		setSetting(OS_SETTING_CREATE_DEBUG_OPCODES, true);
#else
		setSetting(OS_SETTING_CREATE_DEBUG_EVAL_OPCODES, false);
		setSetting(OS_SETTING_CREATE_DEBUG_OPCODES, false);
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

	void flushBuffer()
	{
		if(buffer->buffer.count > 0){
			FCGX_PutStr((const char*)buffer->buffer.buf, buffer->buffer.count, request->out);
			buffer->buffer.count = 0;
			buffer->pos = 0;
		}
	}

	void appendBuffer(const void * buf, int size)
	{
		OS_ASSERT(buffer);
		const int MAX_BUFFER = 32*1024;
		if(buffer->buffer.count + size > MAX_BUFFER){
			flushBuffer();
			if(size > MAX_BUFFER){
				FCGX_PutStr((char*)buf, size, request->out);
				return;
			}
		}
		buffer->append(buf, size);
	}

	void appendBuffer(const OS_CHAR * str)
	{
		appendBuffer((const char*)str, (int)OS_STRLEN(str) * sizeof(OS_CHAR));
	}

	void echo(const void * buf, int size)
	{
		if(!header_sent){
			header_sent = true;
			appendBuffer("Content-type: text/html\r\n\r\n");
		}
		appendBuffer(buf, size);
	}

	static int triggerHeaderSent(OS * p_os, int params, int, int, void*)
	{
		FCGX_OS * os = (FCGX_OS*)p_os;
		os->header_sent = true;
		return 0;
	}

	static int registerShutdownFunction(OS * p_os, int params, int, int, void*)
	{
		if(params > 0){
			FCGX_OS * os = (FCGX_OS*)p_os;
			int offs = os->getAbsoluteOffs(-params);
			os->pushValueById(os->shutdown_funcs_id);
			for(int i = 0; i < params; i++){
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
			{"decode", FCGX_OS::urlDecode},
			{"encode", FCGX_OS::urlEncode},
			{}
		};
		getModule(OS_TEXT("url"));
		setFuncs(funcs);
		pop();
	}

	void triggerShutdownFunctions()
	{
		pushValueById(shutdown_funcs_id);
		Core::Value list = core->getStackValue(-1);
		Core::GCValue * obj = list.getGCValue();
		OS_ASSERT(OS_VALUE_TYPE(list) == OS_VALUE_TYPE_OBJECT && obj);
		if(obj->table && obj->table->count){
			Core::Property * prop = obj->table->last;
			for(; prop; prop = prop->prev){
				if(prop->value.isFunction()){
					core->pushValue(prop->value);
					pushNull();
					call();
				}
			}
		}
		pop();
	}

	void initGlobalFunctions()
	{
		FuncDef funcs[] = {
			{"registerShutdownFunction", FCGX_OS::registerShutdownFunction},
			{"triggerHeaderSent", FCGX_OS::triggerHeaderSent},
			{}
		};
		pushGlobals();
		setFuncs(funcs);
		pop();
	}

	bool gcStepIfNeeded()
	{
		if(getAllocatedBytes() > 32*1024*1024){
			return OS::gcStepIfNeeded();
		}
		return false;
	}

	void processRequest(FCGX_Request * p_request)
	{
		request = p_request;

		pushStackValue(OS_REGISTER_USERPOOL);
		newObject();
		shutdown_funcs_id = getValueId();
		addProperty();

		initGlobalFunctions();
		initUrlLibrary();

		initEnv("_SERVER", request->envp);

		newObject();
		setGlobal("_POST");
		
		newObject();
		setGlobal("_GET");
		
		newObject();
		setGlobal("_FILES");
		
		newObject();
		setGlobal("_COOKIE");

		getGlobal("_SERVER");
		getProperty("CONTENT_LENGTH");
		int content_length = popInt();

		int post_max_size = 1024*1024*8;
		if(content_length > post_max_size){
			FCGX_FPrintF(request->out, "POST Content-Length of %d bytes exceeds the limit of %d bytes", content_length, post_max_size);
			return;
		}

		getGlobal("_SERVER");
		getProperty("CONTENT_TYPE");
		String content_type = popString();

		const char * multipart_form_data = "multipart/form-data;";
		int multipart_form_data_len = (int)strlen(multipart_form_data);

		MPFD::Parser POSTParser = MPFD::Parser();
		if(content_length > 0 && content_type.getLen() > 0 && strncmp(content_type.toChar(), multipart_form_data, multipart_form_data_len) == 0){
			POSTParser.SetTempDirForFileUpload("/tmp");
			// POSTParser.SetMaxCollectedDataLength(20*1024);
			POSTParser.SetContentType(content_type.toChar());

			int max_temp_buf_size = (int)(1024*1024*0.1);
			int temp_buf_size = content_length < max_temp_buf_size ? content_length : max_temp_buf_size;
			char * temp_buf = new char[temp_buf_size + 1];
			for(int cur_len; (cur_len = FCGX_GetStr(temp_buf, temp_buf_size, request->in)) > 0;){
				POSTParser.AcceptSomeData(temp_buf, cur_len);
			}
			delete [] temp_buf;
			temp_buf = NULL;
			
			// POSTParser.SetExternalDataBuffer(buf, len);
			POSTParser.FinishData();

			std::map<std::string, MPFD::Field *> fields = POSTParser.GetFieldsMap();
			// FCGX_FPrintF(request->out, "Have %d fields<p>\n", fields.size());

			std::map<std::string, MPFD::Field *>::iterator it;
			for(it = fields.begin(); it != fields.end(); it++){
				MPFD::Field * field = fields[it->first];
				if(field->GetType() == MPFD::Field::TextType){
					getGlobal("_POST");
					pushString(field->GetTextTypeContent().c_str());
					setSmartProperty(it->first.c_str());
				}else{
					getGlobal("_FILES");
					newObject();
					{
						pushStackValue();
						pushString(field->GetFileName().c_str());
						setProperty("name");
						
						pushStackValue();
						pushString(field->GetFileMimeType().c_str());
						setProperty("type");
						
						pushStackValue();
						pushString(field->GetTempFileNameEx().c_str());
						setProperty("tmp_name");
						
						pushStackValue();
						pushNumber(getFileSize(field->GetTempFileNameEx().c_str()));
						setProperty("size");
					}
					setSmartProperty(it->first.c_str());
				}
			}
		}
		
		initEnv("_ENV", environ);
		
		getGlobal("_SERVER");
		getProperty("SCRIPT_FILENAME");
		String script_filename = popString();
		if(script_filename.getLen() == 0){
			if(!header_sent){
				header_sent = true;
				FCGX_PutS("Content-type: text/plain\r\n\r\n", request->out);
			}
			FCGX_PutS("Filename is not defined", request->out);
		}else{
			if(getFilename(script_filename).getLen() == 0){
				String new_script_filename = script_filename + OS_TEXT("index") + OS_EXT_TEMPLATE;
				if(!isFileExist(new_script_filename)){
					new_script_filename = script_filename + OS_TEXT("index") + OS_EXT_SOURCECODE;
					new_script_filename = resolvePath(new_script_filename);
				}
				script_filename = new_script_filename;
			}
		
			String ext = getFilenameExt(script_filename);
			if(ext == OS_EXT_SOURCECODE || ext == OS_EXT_TEMPLATE || ext == OS_EXT_TEMPLATE_HTML || ext == OS_EXT_TEMPLATE_HTM){
				require(script_filename, true);
				if(!header_sent){
					header_sent = true;
					FCGX_PutS("Content-type: text/plain\r\n\r\n", request->out);
				}
			}else{
				if(!header_sent){
					header_sent = true;
					FCGX_PutS("Content-type: ", request->out);
					FCGX_PutS(getContentType(ext), request->out);
					FCGX_PutS("\r\n", request->out);
					FCGX_PutS("\r\n", request->out);
				}
				void * f = openFile(script_filename, "rb");
				if(f){
					const int BUF_SIZE = 1024*256;
					int size = getFileSize(f);
					void * buf = malloc(BUF_SIZE < size ? BUF_SIZE : size OS_DBG_FILEPOS);
					for(int i = 0; i < size; i += BUF_SIZE){
						int len = BUF_SIZE < size - i ? BUF_SIZE : size - i;
						readFile(buf, len, f);
						FCGX_PutStr((const char*)buf, len, request->out);
					}
					free(buf);				
					closeFile(f);
				}else{
					// FCGX_PutS("Error open file: ", request->out);
					// FCGX_PutS(script_filename, request->out);
				}
			}
		}

		triggerShutdownFunctions();
		flushBuffer();
		
		// FCGX_Finish_r(request);
		FCGX_FFlush(request->out);
	}

	const OS_CHAR * getContentType(const OS_CHAR * ext)
	{
		if(ext[0] == OS_TEXT('.')){
			ext++;
		}
		static const OS_CHAR * mime_types[][2] = {
			{"html", "text/html"},
			{"htm", "text/html"},
			{"js", "text/javascript"},
			{"css", "text/css"},
			{"png", "image/png"},
			{"jpeg", "image/jpeg"},
			{"jpg", "image/jpeg"},
			{"gif", "image/gif"},
			{"ico", "image/x-icon"},
			{"txt", "text/plain"},
			{"log", "text/plain"},
			{OS_EXT_SOURCECODE, "text/os"},
			{OS_EXT_TEMPLATE, "text/osh"},
			{}
		};
		for(int i = 0;; i++){
			const OS_CHAR ** mime = mime_types[i];
			if(!mime[0]){
				break;
			}
			if(OS_STRCMP(ext, mime[0]) == 0){
				return mime[1];
			}
		}
		return "application/octet-stream";
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

void * doit(void * a)
{
    int listen_socket = (int)(ptrdiff_t)a;

    FCGX_Request request;
    if(FCGX_InitRequest(&request, listen_socket, 0)){
		printf("error init request \n");
		exit(1);
	}

#ifndef _MSC_VER
	static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

    for(;;){
#ifndef _MSC_VER
		pthread_mutex_lock(&accept_mutex);
#endif		
		int rc = FCGX_Accept_r(&request);
#ifndef _MSC_VER
		pthread_mutex_unlock(&accept_mutex);
#endif
		if(rc){
			printf("error accept code: %d\n", rc);
			exit(1);
		}

		FCGX_OS * os = OS::create(new FCGX_OS());
		os->processRequest(&request);
        os->release();

		FCGX_Finish_r(&request);
    }
}

#ifndef _MSC_VER
void signalHandler(int sig)
{
	unlink(PID_FILE);
	exit(EXIT_SUCCESS);
}

void setPidFile(const char * filename)
{
	FILE * f = fopen(filename, "w+");
	if (f) {
		fprintf(f, "%u", getpid());
		fclose(f);
	}
}

void demonize()
{
	pid_t pid = fork();
	if (pid < 0) {
		printf("Error: Start Daemon failed (%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);

	if (setsid() < 0)
		exit(EXIT_FAILURE);

	if ((chdir("/")) < 0)
		exit(EXIT_FAILURE);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);

	setPidFile(PID_FILE);
}
#endif

#ifdef _MSC_VER
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{
#ifndef _MSC_VER
	demonize();
#endif

	if(FCGX_Init()){
		exit(1); 
	}

	const char * port = ":9000";
    int listen_queue_backlog = 400;

    int listen_socket = FCGX_OpenSocket(port, listen_queue_backlog);
    if(listen_socket < 0){
		log("listen_socket < 0 \n");
		exit(1);
	}

#ifndef _MSC_VER
	const int THREAD_COUNT = 8;
	pthread_t id[THREAD_COUNT];
	for(int i = 1; i < THREAD_COUNT; i++){
        pthread_create(&id[i], NULL, doit, (void*)listen_socket);
	}
#endif
	doit((void*)listen_socket);

	return 0;
}

