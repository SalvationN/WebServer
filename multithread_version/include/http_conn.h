#ifndef HTTPCOND_H
#define HTTPCOND_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "locker.h"

class http_conn {
// global variable
public:
static const int FILENAME_LEN = 200;
static const int READ_BUFFER_SIZE = 2048;
static const int WRITE_BUFFER_SIZE = 1024;
enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH};
enum CHECK_STATE {CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER, CHECK_STATE_CONTENT};
enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST,FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};
enum LINE_STATUS {LINE_OK = 0, LINE_BAD, LINE_OPEN};

// public function
public:
    http_conn(){};
    ~http_conn(){};
	
	// init connection
	void init(int sockfd, const sockaddr_in& addr);
	void close_conn(bool real_close = true);
	void process();
	bool read();
	bool write();

// private function
private:
	void init();
	// analyze http request
	HTTP_CODE process_read();
	// add http response
	bool process_write(HTTP_CODE ret);

	/* some functions for process_read() */
	HTTP_CODE parse_request_line(char* text);
	HTTP_CODE parse_headers(char* text);
	HTTP_CODE parse_content(char* text);
	HTTP_CODE do_request();
	char* get_line() { return m_read_buf + m_start_line; }
	LINE_STATUS parse_line();

	/* some functions for process_write()  */
	void unmap();
	bool add_response(const char* format, ...);
	bool add_content(const char* content);
	bool add_status_line(int status, const char* title);
	bool add_headers(int content_length);
	bool add_content_length(int content_length);
	bool add_linger();
	bool add_blank_line();

// public variable
public:
    // epoll file's fd
    static int m_epollfd;
    static int m_user_count;

// private variable
private:
    int m_sockfd;
    sockaddr_in m_address;
    char m_read_buf[READ_BUFFER_SIZE];
    
    /* readbuf  */
    // the index of the next character which is going to be read. Actually, it is the last character of the data.
    int m_read_index;
    // the index of the character which is being analyzed. 
    int m_checked_index;
    int m_start_line;
    
    /* writebuf  */
    char m_write_buf[READ_BUFFER_SIZE];
    int m_write_index;
    
    CHECK_STATE m_check_state;
    METHOD m_method;

    // users' target file;
    char m_real_file[FILENAME_LEN];
    char* m_url;
    // http version
    char* m_version;
    char* m_host;
    int m_content_length;
    // whether the connection is requested to be contained
    bool m_linger;

    // the real position of the file
    char* m_file_address;
    // whether the file is exist
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
};

#endif
