#ifndef HTTPCOND_H
#define HTTPCOND_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "locker.h"

class http_cond {
// global variable
public:
static const int FILENAME_LEN = 200;
static const int READ_BUFFER_SIZE = 1024;
enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH};
enum CHECK_STATE {CHECK_STATE_REQUESTLINK, CHECK_STATE_HEADER, CHECK_STATE_CONTENT};
enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST,
                FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};
enum LINK_STATUS {LINK = 0, LINK_BAD, LINE_OPEN};

// public function
public:
    http_conn(){};
    ~http_conn(){};

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
    // the index of the next character which is going to be read
    int m_read_index;
    // the index of the character which is being analyzed
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
