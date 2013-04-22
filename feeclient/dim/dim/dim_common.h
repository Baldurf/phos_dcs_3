#ifndef __COMMONDEFS
#define __COMMONDEFS

/* Service type definition */

#ifndef ONCE_ONLY
#define ONCE_ONLY	0x01
#define TIMED		0x02
#define MONITORED	0x04
#define COMMAND		0x08
#define DIM_DELETE	0x10
#define MONIT_ONLY	0x20
#define UPDATE 		0x40
#define TIMED_ONLY	0x80
#define MAX_TYPE_DEF    0x80
#define STAMPED       0x1000

typedef enum { SRC_NONE, SRC_DIS, SRC_DIC, SRC_DNS, SRC_DNA, SRC_USR }SRC_TYPES;
#ifdef __APPLE__
#ifndef unix
#define unix
#endif
#endif

#ifdef unix
#ifndef __unix__
#define __unix__
#endif
#endif

#ifdef linux
#ifndef __linux__
#define __linux__
#endif
#endif

#ifdef WIN32
typedef __int64		longlong;
#else
#include <sys/types.h> 
typedef int64_t	longlong;
#endif

#endif

#ifndef OSK
#	ifdef _OSK
#		define OSK
#	endif
#endif


#ifdef __VMS
#define VMS
#endif

#ifndef _DIM_PROTO
#ifndef OSK		/* Temorary hack */
#	if defined(__cplusplus) /* || (__STDC__ == 1) || defined(_ANSI_EXT) || defined(ultrix) */
#		define	_DIM_PROTO(func,param)	func param
#	else
#		define _DIM_PROTO(func,param)	func ()
#	endif
#else
#	define _DIM_PROTO(func,param)	func ()
#endif
#ifdef WIN32
#ifdef DIMLIB
#	define _DIM_PROTOE(func,param) __declspec(dllexport) _DIM_PROTO(func,param)
#	define DllExp __declspec(dllexport)
#else
#	define _DIM_PROTOE(func,param) __declspec(dllimport) _DIM_PROTO(func,param)
#	define DllExp __declspec(dllimport)
#endif
#else
#	define _DIM_PROTOE(func,param) _DIM_PROTO(func,param)
#	define DllExp
#endif
#endif

#if defined (hpux) || defined (__osf__) || defined(_AIX)  || defined(WIN32)
#ifndef NOTHREADS
#define NOTHREADS
#endif
#endif

#ifndef VMS
#ifndef WIN32
#ifdef NOTHREADS
#ifndef DIMLIB
#ifndef sleep
#define sleep(t) dtq_sleep(t)
#endif
#endif
#endif
#endif
#endif

#ifdef VMS
#include <ssdef.h>
#define DISABLE_AST     long int ast_enable = sys$setast(0);
#define ENABLE_AST      if (ast_enable == SS$_WASSET) sys$setast(1);
#define dim_enable()    sys$setast(1);
#endif

#ifdef __unix__
#include <signal.h>

#define DISABLE_AST     sigset_t set, oset; sigemptyset(&set);\
						sigaddset(&set,SIGIO);\
						sigaddset(&set,SIGALRM);\
						sigprocmask(SIG_BLOCK,&set,&oset);\
						DIM_LOCK
#define ENABLE_AST      DIM_UNLOCK sigprocmask(SIG_SETMASK,&oset,0);

#ifdef VxWorks
#define DIM_LOCK taskLock();
#define DIM_UNLOCK taskUnlock();
#else

#ifndef NOTHREADS
#include <pthread.h>

_DIM_PROTOE( void dim_lock,		() );
_DIM_PROTOE( void dim_unlock,	() );

#define DIM_LOCK 	dim_lock();
#define DIM_UNLOCK	dim_unlock();

#else
#define DIM_LOCK
#define DIM_UNLOCK
#endif
#endif
#endif
#ifdef OSK
#define INC_LEVEL               1
#define DEC_LEVEL               (-1)
#define DISABLE_AST     sigmask(INC_LEVEL);
#define ENABLE_AST      sigmask(DEC_LEVEL);
#endif


_DIM_PROTOE( int id_get,           (void *ptr, int type) );
_DIM_PROTOE( void id_free,         (int id, int type) );
_DIM_PROTOE( void *id_get_ptr,     (int id, int type) );

_DIM_PROTOE( int dtq_sleep,	(int secs) );
_DIM_PROTOE( void dtq_start_timer,      (int secs, void(*rout)(void*), void *tag) );
_DIM_PROTOE( int dtq_stop_timer,		(void *tag) );
_DIM_PROTOE( void dim_init,				() );
_DIM_PROTOE( void dim_no_threads,		() );
_DIM_PROTOE( void dna_set_test_write,	(int conn_id, int time) );
_DIM_PROTOE( void dna_rem_test_write,	(int conn_id) );
_DIM_PROTOE( int dim_set_dns_node,		(char *node) );
_DIM_PROTOE( int dim_get_dns_node,		(char *node) );
_DIM_PROTOE( int dim_set_dns_port,		(int port) );
_DIM_PROTOE( int dim_get_dns_port,		() );
_DIM_PROTOE( void dic_set_debug_on,		() );
_DIM_PROTOE( void dic_set_debug_off,	() );
_DIM_PROTOE( void dim_print_msg,		(char *msg, int severity) );
_DIM_PROTOE( void dim_print_date_time,		() );
_DIM_PROTOE( void dim_set_write_timeout,		(int secs) );
_DIM_PROTOE( int dim_get_write_timeout,		() );
_DIM_PROTOE( void dim_usleep,	(unsigned int t) );
_DIM_PROTOE( int dim_wait,		(void) );
_DIM_PROTOE( int dim_get_priority,		(int dim_thread, int prio) );
_DIM_PROTOE( int dim_set_priority,		(int dim_thread, int *prio) );
_DIM_PROTOE( int dim_set_scheduler_class,		(int sched_class) );
_DIM_PROTOE( int dim_get_scheduler_class,		(int *sched_class) );
_DIM_PROTOE( int dim_start_thread,    (void(*rout)(void*), void *tag) );

#ifdef WIN32
#define getpid _getpid
_DIM_PROTOE( void dim_pause,		() );
_DIM_PROTOE( void dim_wake_up,	() );
_DIM_PROTOE( void dim_lock,		() );
_DIM_PROTOE( void dim_unlock,	() );
_DIM_PROTOE( void dim_sleep,	(unsigned int t) );
_DIM_PROTOE( void dim_win_usleep,	(unsigned int t) );
#define sleep(t)	dim_sleep(t);
#define usleep(t)	dim_win_usleep(t);
#define pause() 	dim_pause();
#define wake_up()	dim_wake_up();
#define DIM_LOCK 	dim_lock();
#define DIM_UNLOCK	dim_unlock();
#define DISABLE_AST	DIM_LOCK
#define ENABLE_AST  DIM_UNLOCK
#endif

/* ctime usage */
#if defined (solaris) || defined (LYNXOS)
#define my_ctime(t,str,size) ctime_r(t,str,size)
#else 
#if defined (__linux__) 
#define my_ctime(t,str,size) ctime_r(t,str)
#else
#define my_ctime(t,str,size) strcpy(str,ctime(t))
#endif
#endif

/* DIM Error Severities*/
typedef enum { DIM_INFO, DIM_WARNING, DIM_ERROR, DIM_FATAL }DIM_SEVERITIES;
/* DIM Error codes */
#define DIMDNSUNDEF 0x1		/* DIM_DNS_NODE undefined			FATAL */
#define DIMDNSREFUS 0x2		/* DIM_DNS refuses connection		FATAL */
#define DIMDNSDUPLC 0x3		/* Service already exists in DNS	FATAL */
#define DIMDNSEXIT  0x4		/* DNS requests server to EXIT		FATAL */
#define DIMDNSTMOUT 0x5		/* Server failed sending Watchdog	WARNING */

#define DIMSVCDUPLC 0x10	/* Service already exists in Server	ERROR */
#define DIMSVCFORMT 0x11	/* Bat format string for service	ERROR */
#define DIMSVCINVAL 0x12	/* Service ID invalid				ERROR */
#define DIMSVCTOOLG 0x13	/* Service name too long			ERROR */

#define DIMTCPRDERR	0x20	/* TCP/IP read error				ERROR */
#define DIMTCPWRRTY	0x21	/* TCP/IP write	error - Retrying	WARNING */
#define DIMTCPWRTMO	0x22	/* TCP/IP write error - Disconnect	ERROR */
#define DIMTCPLNERR	0x23	/* TCP/IP listen error				ERROR */
#define DIMTCPOPERR	0x24	/* TCP/IP open server error			ERROR */
#define DIMTCPCNERR	0x25	/* TCP/IP connection error			ERROR */
#define DIMTCPCNEST	0x26	/* TCP/IP connection established	INFO */

#define DIMDNSCNERR	0x30	/* Connection to DNS failed			ERROR */
#define DIMDNSCNEST	0x31	/* Connection to DNS established	INFO */
		
#endif                         









