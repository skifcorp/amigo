#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <cstddef>

typedef struct
{
  unsigned char version;
  unsigned char cmd;

  unsigned short length;
  unsigned short offset;
  unsigned short buf_len;
  unsigned short counter;
  unsigned short reserv;

  unsigned char buf[500];
} TCP_CMD;

const std::size_t cmd_tcp_header_len = sizeof(char)*2 + sizeof (unsigned short)*5;

//enum Commands  { MUST_WRITE=0, MUST_READ } ;

const unsigned char MustWrite = 0;
const unsigned char MustRead = 1;


#endif
