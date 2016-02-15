#ifndef __INTERFACE_H_
#define __INTERFACE_H_

typedef int ident_t;

typedef enum {
  END_SESSION=0,
  GET_UNIT_IDENT_BY_NAME,
  ACCEPT_ITEM,
  FREE_ITEM,
  READ_BY_IDENT,
  WRITE_BY_IDENT,
  READ_ALL,
  READ_NEW,
  LINK_ITEM_BY_NAME,
  BEGIN_SESSION,
  STORE_ITEM,
  LOAD_ITEM,
  FREE_EXT_ITEM,
  INIT_DB
} command;

typedef struct
{
  int command; 
  int work;
} RD;

typedef struct
{
  ident_t Ident;
  short Reliability;
  float Value;
  float ELRange;
  float EHRange;
} GIV;

typedef struct
{
  float          DeadZone;
  unsigned short DeadTime;
} s_info;

#endif

