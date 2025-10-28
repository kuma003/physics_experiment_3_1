/*
  nagiDAQ Device Driver for Kernel 2.6
  Define some parameters & symbolic name of ioctl command
  Written by Kohei Shoji
*/

#ifndef __NGDRVCOMMON_H__
#define __NGDRVCOMMON_H__

/* maximum size of 1 event data */
#define NGDRV_MAXEVLEN   1024

/* delimiters */
#define NGDRV_EOF        0xffff
#define NGDRV_DELIM      0x8001
#define CC7700_DELIM     0x8001
#define SBS620_DELIM     0x8001

/* on/off */
#define NGDRV_OFF        0
#define NGDRV_ON         1

/* symbols for sbs620 read/write */
/* size */
#define SBS620_BYTE      1
#define SBS620_WORD      2
#define SBS620_LONG      4
/* direction */
#define SBS620_READ      8
#define SBS620_WRITE     16
/* address modifier */
#define SBS620_AM_39             0x39   /* Std non-pri. data access */
#define SBS620_AM_29             0x29   /* Short non-pri. data access */
#define SBS620_AM_09             0x09   /* Ext non-pri. data access */
/* function code */
#define SBS620_FUNC_IO           0x1
#define SBS620_FUNC_RAM          0x2
#define SBS620_DPRAM             0x3
/* swap code */
#define SBS620_SWAP_NONE         0x0


typedef struct CC7700_CYCLE {
  int c;
  int n;
  int a;
  int f;
  unsigned long data;
} CC7700_CYCLE;

typedef struct SBS620_CYCLE {
  unsigned char byte;
  unsigned char direction;
  unsigned char am;
  unsigned long addr;
  unsigned long data;
} SBS620_CYCLE;

/* define symbolic name of ioctl command */
#define NGDRV_IOC_MAGIC 'd'

#define NGDRV_IOC_COMMON       0  /* from 0 to 31 */
#define NGDRV_IOC_INITIAL      _IO  ( NGDRV_IOC_MAGIC,  9                )
#define NGDRV_IOC_INTON        _IO  ( NGDRV_IOC_MAGIC, 10                )
#define NGDRV_IOC_INTOFF       _IO  ( NGDRV_IOC_MAGIC, 11                )
#define NGDRV_IOC_INTCNT       _IOR ( NGDRV_IOC_MAGIC, 12, unsigned long )
#define NGDRV_IOC_INTLOS       _IOR ( NGDRV_IOC_MAGIC, 13, unsigned long )
#define NGDRV_IOC_CLRCNT       _IO  ( NGDRV_IOC_MAGIC, 14                )
#define NGDRV_IOC_PUTEOF       _IO  ( NGDRV_IOC_MAGIC, 15                )

#define NGDRV_IOC_CC7700       1  /* from 32 to 63 */
#define CC7700_IOC_INITIAL     _IO  ( NGDRV_IOC_MAGIC, 32                )
#define CC7700_IOC_Z           _IO  ( NGDRV_IOC_MAGIC, 33                )
#define CC7700_IOC_C           _IO  ( NGDRV_IOC_MAGIC, 34                )
#define CC7700_IOC_I           _IO  ( NGDRV_IOC_MAGIC, 35                )
#define CC7700_IOC_LINT        _IO  ( NGDRV_IOC_MAGIC, 36                )
#define CC7700_IOC_L_P         _IOR ( NGDRV_IOC_MAGIC, 37, unsigned long )
#define CC7700_IOC_CYCLE       _IOWR( NGDRV_IOC_MAGIC, 38, CC7700_CYCLE  )

#define NGDRV_IOC_SBS620       2  /* from 64 to 96 */
#define SBS620_IOC_INITIAL     _IO  ( NGDRV_IOC_MAGIC, 64                )
#define SBS620_IOC_CYCLE       _IOWR( NGDRV_IOC_MAGIC, 65, SBS620_CYCLE  )

#define SBS620_IOC_LC_R        _IOR ( NGDRV_IOC_MAGIC, 66, unsigned char )
#define SBS620_IOC_LC_W        _IO  ( NGDRV_IOC_MAGIC, 67                )
#define SBS620_IOC_LS_R        _IOR ( NGDRV_IOC_MAGIC, 68, unsigned char )
#define SBS620_IOC_IC_R        _IOR ( NGDRV_IOC_MAGIC, 69, unsigned char )
#define SBS620_IOC_IC_W        _IO  ( NGDRV_IOC_MAGIC, 70                )
#define SBS620_IOC_IS_R        _IOR ( NGDRV_IOC_MAGIC, 71, unsigned char )
#define SBS620_IOC_PC_R        _IOR ( NGDRV_IOC_MAGIC, 72, unsigned char )
#define SBS620_IOC_PC_W        _IO  ( NGDRV_IOC_MAGIC, 73                )
#define SBS620_IOC_RC1_W       _IO  ( NGDRV_IOC_MAGIC, 74                )
#define SBS620_IOC_RS_R        _IOR ( NGDRV_IOC_MAGIC, 75, unsigned char )
#define SBS620_IOC_RC2_R       _IOR ( NGDRV_IOC_MAGIC, 76, unsigned char )
#define SBS620_IOC_RC2_W       _IO  ( NGDRV_IOC_MAGIC, 77                )
#define SBS620_IOC_IACK_B      _IOR ( NGDRV_IOC_MAGIC, 78, unsigned char )
#define SBS620_IOC_IACK_W      _IOR ( NGDRV_IOC_MAGIC, 79, unsigned short )


#endif /* __NGDRVCOMMON_H__ */
