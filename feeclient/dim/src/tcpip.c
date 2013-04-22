/*
 * DNA (Delphi Network Access) implements the network layer for the DIM
 * (Delphi Information Managment) System.
 *
 * Started           : 10-11-91
 * Last modification : 29-07-94
 * Written by        : C. Gaspar
 * Adjusted by       : G.C. Ballintijn
 *
 */

/*
#define DEBUG
*/

#ifdef WIN32
#define FD_SETSIZE      1024
#define ioctl ioctlsocket

#define closesock closesocket
#define readsock recv
#define writesock send

#define EINTR WSAEINTR
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
/*
#define closesock(s) shutdown(s,2);
*/
#define closesock close
#define readsock(a,b,c,d) read(a,b,c)
#define writesock(a,b,c,d) write(a,b,c)

#ifdef solaris
#define BSD_COMP
/*
#include <thread.h>
*/
#endif

#ifdef LYNXOS
#ifdef RAID
typedef int pid_t;
#endif
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <time.h>
#define DIMLIB
//#include <dim.h>
#include "feeclient/dim/dim/dim.h"

#define ushort unsigned short

static int Threads_on = 0;

static int init_done = FALSE;		/* Is this module initialized? */
static int	queue_id;
static struct sockaddr_in DIM_sockname;

static int DIM_IO_path[2];
static int DIM_IO_valid = 1;

static int Write_timeout = 5;

void dim_set_write_timeout(secs)
     int secs;
{
  Write_timeout = secs;
}

int dim_get_write_timeout()
{
  return(Write_timeout);
}

#ifdef WIN32
int init_sock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	static int sock_init_done = 0;

	if(sock_init_done) return(1);
 	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) 
	{
    	return(0);
	}

	/* Confirm that the WinSock DLL supports 2.0.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) 
	{
	    WSACleanup( );
    	return(0); 
	}
	sock_init_done = 1;
	return(1);
}
#endif

int dim_tcpip_init(thr_flag)
int thr_flag;
{
#ifdef WIN32
	int tid;
/*
    void tcpip_task();
*/
	void create_io_thread();
#else
	struct sigaction sig_info;
	sigset_t set, set1;
	void io_sig_handler();
	void dummy_io_sig_handler();
	void tcpip_pipe_sig_handler();
#endif
	int ret, i, addr, flags = 1;

	if(init_done) return(1);
#ifdef WIN32
	init_sock();
	Threads_on = 1;
#else
	if(thr_flag)
	{
		Threads_on = 1;
	}
	else
	{
		sigemptyset(&set);

		sigaddset(&set,SIGALRM);
	    sig_info.sa_handler = io_sig_handler;
	    sig_info.sa_mask = set;
#ifndef LYNXOS
	    sig_info.sa_flags = SA_RESTART;
#else
	    sig_info.sa_flags = 0;
#endif
  
		if( sigaction(SIGIO, &sig_info, 0) < 0 ) 
		{
			perror( "sigaction(SIGIO)" );
			exit(1);
		}
	      
	    sigemptyset(&set);
	    sig_info.sa_handler = tcpip_pipe_sig_handler;
	    sig_info.sa_mask = set;
#ifndef LYNXOS 
	    sig_info.sa_flags = SA_RESTART;
#else
	    sig_info.sa_flags = 0;
#endif

	    if( sigaction(SIGPIPE, &sig_info, 0) < 0 ) {
			perror( "sigaction(SIGPIPE)" );
			exit(1);
	    }
	  
	}
#endif
	if(Threads_on)
	{
#ifdef WIN32
		if( (DIM_IO_path[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
		{
			perror("socket");
			return(0);
		}
		DIM_sockname.sin_family = PF_INET;
		addr = 0;
		DIM_sockname.sin_addr = *((struct in_addr *) &addr);
		DIM_sockname.sin_port = htons((ushort) 2000); 
		ioctl(DIM_IO_path[0], FIONBIO, &flags);
#else
		pipe(DIM_IO_path);
#endif
	}
	queue_id = dtq_create();

#ifdef WIN32
/*
#ifndef STDCALL
	tid = _beginthread((void *)(void *)tcpip_task,0,NULL);
#else
	tid = _beginthreadex(NULL, NULL,
			tcpip_task,0,0,NULL);
#endif
*/
	create_io_thread();
#endif
	init_done = 1;
	return(1);
}

static int enable_sig(conn_id)
int conn_id;
{
	int pid, ret = 1, flags = 1;
	int addr;

#ifdef DEBUG
	if(!Net_conns[conn_id].channel)
	{
	    printf("Enabling signals on channel 0\n");
	    fflush(stdout);
	}
#endif

	if(!init_done)
	{
		dim_tcpip_init(0);
	}
	if(Threads_on)
	{
#ifdef WIN32
		DIM_IO_valid = 0;
		ret = connect(DIM_IO_path[0], (struct sockaddr*)&DIM_sockname, sizeof(DIM_sockname));

		closesock(DIM_IO_path[0]);	  
		if( (DIM_IO_path[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
		{
			perror("socket");
			return(1);
		}		
		ret = ioctl(DIM_IO_path[0], FIONBIO, &flags);
		if(ret != 0)
		{
			perror("ioctlsocket");
		}
		DIM_IO_valid = 1;
#else
		write(DIM_IO_path[1], &flags, 4);
#endif
	}
#ifndef WIN32
	if(!Threads_on)
	{
	    pid = getpid();

#ifndef __linux__
		ret = ioctl(Net_conns[conn_id].channel, SIOCSPGRP, &pid );
#else
	    ret = fcntl(Net_conns[conn_id].channel,F_SETOWN, pid);
#endif
	    if(ret == -1)
	    {
#ifdef DEBUG
	        printf("ioctl returned -1\n");
#endif
			return(ret);
	    }
	}
	ret = ioctl(Net_conns[conn_id].channel, FIOASYNC, &flags );
	if(ret == -1)
	{
#ifdef DEBUG
		printf("ioctl1 returned -1\n");
#endif
		return(ret);
	}
	
    flags = fcntl(Net_conns[conn_id].channel,F_GETFD,0);
#ifdef DEBUG
    if(flags == -1)
    {
		printf("error\n");
    }
#endif
    ret = fcntl(Net_conns[conn_id].channel,F_SETFD, flags | FD_CLOEXEC );
    if(ret == -1)
    {
#ifdef DEBUG
		printf("ioctl2 returned -1\n");
#endif
		return(ret);
    }
#endif
	return(1);
}

static void dump_list()
{
	int	i;

	for( i = 1; i < Curr_N_Conns; i++ )
		if( Dna_conns[i].busy ) {
			printf( "dump_list: conn_id=%d reading=%d\n",
				i, Net_conns[i].reading );
		}
}

static int list_to_fds( fds )
fd_set *fds;
{
	int	i;
	int found = 0;

	FD_ZERO( fds ) ;
	for( i = 1; i < Curr_N_Conns; i++ )
    {
		if( Dna_conns[i].busy )
		{
			if(Net_conns[i].channel)
			{
				found = 1;
				FD_SET( Net_conns[i].channel, fds );

			}
		}
	}
	return(found);
}

static int fds_get_entry( fds, conn_id ) 
fd_set *fds;
int *conn_id;
{
	int	i;

	for( i = 1; i < Curr_N_Conns; i++ )
	{
		if( Dna_conns[i].busy &&
		    FD_ISSET(Net_conns[i].channel, fds) )
		{
			if(Net_conns[i].channel)
		    {
				*conn_id = i;
				return 1;
			}
		}
	}
	return 0;
}

static void tcpip_test_write( conn_id )
int conn_id;
{
	/* Write to every socket we use, which uses the TCPIP protocol,
	 * which has an established connection (reading), which is currently
	 * not writing data, so we can check if it is still alive.
	 */
	time_t cur_time;
	
	if(strcmp(Net_conns[conn_id].node,"MYNODE"))
	{
		cur_time = time(NULL);
		if( cur_time - Net_conns[conn_id].last_used > Net_conns[conn_id].timeout )
		{
			dna_test_write( conn_id );
		}
	}
}

tcpip_set_test_write(conn_id, timeout)
int conn_id, timeout;
{
	Net_conns[conn_id].timr_ent = dtq_add_entry( queue_id, timeout, 
		tcpip_test_write, conn_id );
	Net_conns[conn_id].timeout = timeout;
	Net_conns[conn_id].last_used = time(NULL);
}

tcpip_rem_test_write(conn_id)
int conn_id;
{
	if(Net_conns[conn_id].timr_ent)
	{
		dtq_rem_entry(queue_id, Net_conns[conn_id].timr_ent);
		Net_conns[conn_id].timr_ent = NULL;
	}
	Net_conns[conn_id].last_used = time(NULL);
}

void tcpip_pipe_sig_handler( num )
int num;
{
/*
	printf( "*** pipe_sig_handler called ***\n" );
*/
}

static int get_bytes_to_read(conn_id)
int conn_id;
{
	int i, ret, count;
	
	for(i = 0; i < 3; i++)
	{
		ret = ioctl( Net_conns[conn_id].channel, FIONREAD, &count );
	    if( ret != 0)
		{
			count = 0;
			break;
	    }
	    if(count > 0)
	    {
			break;
	    }
	}
	return(count);
}

static void do_read( conn_id )
int conn_id;
{
	/* There is 'data' pending, read it.
	 */
	int	len, ret, totlen, size, count;
	char	*p;
/*
	count = get_bytes_to_read(conn_id);
	if(!count)
	{
		printf("Connection closed %d\n", conn_id);
		Net_conns[conn_id].read_rout( conn_id, -1, 0 );
		return;
	}
*/
	size = Net_conns[conn_id].size;
	p = Net_conns[conn_id].buffer;
	totlen = 0;
	count = 1;
	while( size > 0 && count > 0 )
	{
		if( (len = readsock(Net_conns[conn_id].channel, p, size, 0)) <= 0 ) 
		{	/* Connection closed by other side. */
			Net_conns[conn_id].read_rout( conn_id, -1, 0 );
			return;
		} 
		else 
		{
			
			/*
			printf("tcpip: read %d bytes:\n",len); 
			printf( "buffer[0]=%d\n", vtohl((int *)p[0]));
			printf( "buffer[1]=%d\n", vtohl((int *)p[1]));
			printf( "buffer[2]=%x\n", vtohl((int *)p[2]));
			*/
			totlen += len;
			size -= len;
			p += len;
		}
		if(size)
			count = get_bytes_to_read(conn_id);
	}

	Net_conns[conn_id].last_used = time(NULL);
	Net_conns[conn_id].read_rout( conn_id, 1, totlen );
}


void do_accept( conn_id )
int conn_id;
{
	/* There is a 'connect' pending, serve it.
	 */
	struct sockaddr_in	other;
	int			othersize;

	othersize = sizeof(other);
	memset( (char *) &other, 0, othersize );
	Net_conns[conn_id].mbx_channel = accept( Net_conns[conn_id].channel,
						 (struct sockaddr*)&other, &othersize );
	if( Net_conns[conn_id].mbx_channel < 0 ) 
	{
		return;
	}
/*
	else
	{
			int all, a, b, c, d;
			char *pall;

			all = other.sin_addr.s_addr;
			pall = &all;
			a = pall[0];
			a &= 0x000000ff;
			b = pall[1];
			b &= 0x000000ff;
			c = pall[2];
			c &= 0x000000ff;
			d = pall[3];
			d &= 0x000000ff;
printf("TCPIP got %d.%d.%d.%d \n",
		a,b,c,d);
		if((a == 134) && (b == 79) && (c == 157) && (d == 40))
		{
			closesock(Net_conns[conn_id].mbx_channel);
			return;
		}
	}
*/

	Net_conns[conn_id].last_used = time(NULL);
	Net_conns[conn_id].read_rout( Net_conns[conn_id].mbx_channel,
				      conn_id, TCPIP );
}

void io_sig_handler(num)
int num;
{
    fd_set	rfds, wfds, efds;
    int	i, conn_id, ret, selret, count;
	struct timeval	timeout;

	do
	{
		timeout.tv_sec = 0;		/* Don't wait, just poll */
		timeout.tv_usec = 0;
		list_to_fds( &rfds );
		selret = select(FD_SETSIZE, &rfds, NULL, NULL, &timeout);
		if(selret > 0)
		{
			while( ret = fds_get_entry( &rfds, &conn_id ) > 0 ) 
			{
				if( Net_conns[conn_id].reading )
				{
					do
					{
						do_read( conn_id );
						count = 0;
						if(Net_conns[conn_id].channel)
						{
							count = get_bytes_to_read(conn_id);
						}
					}while(count > 0 );
				}
				else
				{
					do_accept( conn_id );
				}
				FD_CLR( Net_conns[conn_id].channel, &rfds );
	    	}
		}
	}while(selret > 0);
}

void tcpip_task( void *dummy)
{
	/* wait for an IO signal, find out what is happening and
	 * call the right routine to handle the situation.
	 */
	fd_set	rfds, efds, *pfds;
	int	conn_id, ret, rcount, count, data;

	while(1)
	{
		while(!DIM_IO_valid)
			dim_usleep(1000);

		list_to_fds( &rfds );
		FD_ZERO(&efds);
#ifdef WIN32
		pfds = &efds;
#else
		pfds = &rfds;
#endif
		FD_SET( DIM_IO_path[0], pfds );
		ret = select(FD_SETSIZE, &rfds, NULL, &efds, NULL);
		if(ret > 0)
		{
			if(FD_ISSET(DIM_IO_path[0], pfds) )
			  {
#ifndef WIN32
				read(DIM_IO_path[0], &data, 4);
#endif
				FD_CLR( DIM_IO_path[0], pfds );
			  }
			{
			DISABLE_AST
			while( ret = fds_get_entry( &rfds, &conn_id ) > 0 ) 
			{
				if( Net_conns[conn_id].reading )
				{
					do
					{
						do_read( conn_id );
						count = 0;
						if(Net_conns[conn_id].channel)
						{
							count = get_bytes_to_read(conn_id);
						}
					}while(count > 0 );
				}
				else
				{
					do_accept( conn_id );
				}
				FD_CLR( Net_conns[conn_id].channel, &rfds );
			}
			ENABLE_AST
			}
#ifndef WIN32
			return;
#endif
		}
	}
}

int tcpip_start_read( conn_id, buffer, size, ast_routine )
int size, conn_id;
char *buffer;
void (*ast_routine)();
{
	int pid, ret = 1, flags = 1;
	/* Install signal handler stuff on the socket, and record
	 * some necessary information: we are reading, and want size
	 * as size, and use buffer.
	 */

	Net_conns[conn_id].read_rout = ast_routine;
	Net_conns[conn_id].buffer = buffer;
	Net_conns[conn_id].size = size;
	if(Net_conns[conn_id].reading == -1)
	{
		if(enable_sig( conn_id ) == -1)
		{
#ifdef DEBUG
			printf("START_READ - enable_sig returned -1\n");
#endif
			return(0);
		}
	}
	Net_conns[conn_id].reading = TRUE;
	return(1);
}

int tcpip_open_client( conn_id, node, task, port )
int conn_id;
char *node, *task;
int port;
{
	/* Create connection: create and initialize socket stuff. Try
	 * and make a connection with the server.
	 */
	struct sockaddr_in sockname;
#ifndef VxWorks
	struct hostent *host;
#else
	int host_addr;
#endif
	int path, par, val, ret_code, ret;
	int a,b,c,d;
	unsigned char ipaddr[4], *ptr;
	int host_number = 0;
	char test[64];

    dim_tcpip_init(0);
	if(isdigit(node[0]))
	{
		sscanf(node,"%d.%d.%d.%d",&a, &b, &c, &d);
	    ipaddr[0] = a;
	    ipaddr[1] = b;
	    ipaddr[2] = c;
	    ipaddr[3] = d;
	    host_number = 1;
	}
#ifndef VxWorks
	else if( (host = gethostbyname(node)) == (struct hostent *)0 ) 
	{
          ptr = (unsigned char *)node+strlen(node)+1;
          ipaddr[0] = *ptr++;
          ipaddr[1] = *ptr++;
          ipaddr[2] = *ptr++;
          ipaddr[3] = *ptr++;
          host_number = 1;
		  if( (ipaddr[0] == 0xff) &&
			  (ipaddr[1] == 0xff) &&
			  (ipaddr[2] == 0xff) &&
			  (ipaddr[3] == 0xff) )
		  {
			  return(0);
		  }
	}
#else
	*(strchr(node,'.')) = '\0';
	host_addr = hostGetByName(node);
	printf("node %s addr: %x\n",node, host_addr);
#endif

	if( (path = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
	{
		perror("socket");
		return(0);
	}

	val = 1;
      
	if ((ret_code = setsockopt(path, IPPROTO_TCP, TCP_NODELAY, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set TCP_NODELAY\n");
#endif
		closesock(path); 
		return(0);
	}

	val = TCP_SND_BUF_SIZE;      
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_SNDBUF, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_SNDBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	val = TCP_RCV_BUF_SIZE;
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_RCVBUF, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_RCVBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	sockname.sin_family = PF_INET;
#ifndef VxWorks
    if(host_number)
		sockname.sin_addr = *((struct in_addr *) ipaddr);
    else
		sockname.sin_addr = *((struct in_addr *) host->h_addr);
#else
    if(host_number)
		sockname.sin_addr = *((struct in_addr *) ipaddr);
    else
		sockname.sin_addr = *((struct in_addr *) &host_addr);
#endif
	sockname.sin_port = htons((ushort) port); /* port number to send to */
	while((ret = connect(path, (struct sockaddr*)&sockname, sizeof(sockname))) == -1 )
	{
		if(errno != EINTR)
		{
			closesock(path);
			return(0);
		}
	}
	strcpy( Net_conns[conn_id].node, node );
	strcpy( Net_conns[conn_id].task, task );
	Net_conns[conn_id].channel = path;
	Net_conns[conn_id].port = port;
	Net_conns[conn_id].last_used = time(NULL);
	Net_conns[conn_id].reading = -1;
	Net_conns[conn_id].timr_ent = NULL;
	return(1);
}


int tcpip_open_server( conn_id, task, port )
int conn_id, *port;
char *task;
{
	/* Create connection: create and initialize socket stuff,
	 * find a free port on this node.
	 */
	struct sockaddr_in sockname;
	int path, par, val, ret_code, ret;

    dim_tcpip_init(0);
	if( (path = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
	{
		return(0);
	}

	val = 1;
	if ((ret_code = setsockopt(path, IPPROTO_TCP, TCP_NODELAY, 
			(char*)&val, sizeof(val))) == -1 ) 

	{
#ifdef DEBUG
		printf("Couln't set TCP_NODELAY\n");
#endif
		closesock(path); 
		return(0);
	}

	val = TCP_SND_BUF_SIZE;
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_SNDBUF, 
			(void *)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_SNDBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	val = TCP_RCV_BUF_SIZE;
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_RCVBUF, 
			(void *)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_RCVBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	if( *port == SEEK_PORT ) 
	{	/* Search a free one. */
		*port = START_PORT_RANGE - 1;
		do 
		{
			(*port)++;
			sockname.sin_family = AF_INET;
			sockname.sin_addr.s_addr = INADDR_ANY;
			sockname.sin_port = htons((ushort) *port);
			if( *port > STOP_PORT_RANGE ) {
				errno = EADDRNOTAVAIL;	/* fake an error code */
				closesock(path);
				return(0);
			}
		} while( bind(path, (struct sockaddr*)&sockname, sizeof(sockname)) == -1 );
	} else {
#ifndef WIN32
		val = 1;
		if( setsockopt(path, SOL_SOCKET, SO_REUSEADDR, (char*)&val, 
			sizeof(val)) == -1 )
		{
#ifdef DEBUG
			printf("Couln't set SO_REUSEADDR\n");
#endif
			closesock(path); 
			return(0);
		}
#endif
		sockname.sin_family = AF_INET;
		sockname.sin_addr.s_addr = INADDR_ANY;
		sockname.sin_port = htons((ushort) *port);
		if( (ret = bind(path, (struct sockaddr*) &sockname, sizeof(sockname))) == -1 )
		{
			closesock(path);
			return(0);
		}
	}

	if( (ret = listen(path, 5)) == -1 )
	{
		closesock(path);
		return(0);
	}

	strcpy( Net_conns[conn_id].node, "MYNODE" );
	strcpy( Net_conns[conn_id].task, task );
	Net_conns[conn_id].channel = path;
	Net_conns[conn_id].port = *port;
	Net_conns[conn_id].last_used = time(NULL);
	Net_conns[conn_id].reading = -1;
	Net_conns[conn_id].timr_ent = NULL;
	return(1);
}


int tcpip_start_listen( conn_id, ast_routine )
int conn_id;
void (*ast_routine)();
{
	int pid, ret=1, flags = 1;
	/* Install signal handler stuff on the socket, and record
	 * some necessary information: we are NOT reading, thus
	 * no size.
	 */

	Net_conns[conn_id].read_rout = ast_routine;
	Net_conns[conn_id].size = -1;
	if(Net_conns[conn_id].reading == -1)
	{
		if(enable_sig( conn_id ) == -1)
		{
#ifdef DEBUG
			printf("START_LISTEN - enable_sig returned -1\n");
#endif
			return(0);
		}
	}
	Net_conns[conn_id].reading = FALSE;
	return(1);
}


int tcpip_open_connection( conn_id, path )
int conn_id, path;
{
	/* Fill in/clear some fields, the node and task field
	 * get filled in later by a special packet.
	 */
	int par, val, ret_code;


	val = 1;
	if ((ret_code = setsockopt(path, IPPROTO_TCP, TCP_NODELAY, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set TCP_NODELAY\n");
#endif
		closesock(path); 
		return(0);
	}
	val = TCP_SND_BUF_SIZE;      
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_SNDBUF, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_SNDBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	val = TCP_RCV_BUF_SIZE;
	if ((ret_code = setsockopt(path, SOL_SOCKET, SO_RCVBUF, 
			(char*)&val, sizeof(val))) == -1 ) 
	{
#ifdef DEBUG
		printf("Couln't set SO_RCVBUF\n");
#endif
		closesock(path); 
		return(0);
	}

	Net_conns[conn_id].channel = path;
	Net_conns[conn_id].node[0] = 0;
	Net_conns[conn_id].task[0] = 0;
	Net_conns[conn_id].port = 0;
	Net_conns[conn_id].reading = -1;
	Net_conns[conn_id].timr_ent = NULL;
	return(1);
}


void tcpip_get_node_task( conn_id, node, task )
int conn_id;
char *node, *task;
{
	strcpy( node, Net_conns[conn_id].node );
	strcpy( task, Net_conns[conn_id].task );
}

int tcpip_write( conn_id, buffer, size )
int conn_id, size;
char *buffer;
{
	/* Do a (synchronous) write to conn_id.
	 */
	int	wrote;

	wrote = writesock( Net_conns[conn_id].channel, buffer, size, 0 );
	if( wrote == -1 ) {
/*
		Net_conns[conn_id].read_rout( conn_id, -1, 0 );
*/
		return(0);
	}
	return(wrote);
}

int set_non_blocking(channel)
     int channel;
{
  int ret, flags = 1;
	ret = ioctl(channel, FIONBIO, &flags );
	if(ret == -1)
	{
#ifdef DEBUG
	    printf("ioctl non block returned -1\n");
#endif
		return(ret);
	}
	return(1);
}

int set_blocking(channel)
     int channel;
{
  int ret, flags = 0;
	ret = ioctl(channel, FIONBIO, &flags );
	if(ret == -1)
	{
#ifdef DEBUG
	    printf("ioctl block returned -1\n");
#endif
		return(ret);
	}
	return(1);
}

int tcpip_write_nowait( conn_id, buffer, size )
int conn_id, size;
char *buffer;
{
	/* Do a (asynchronous) write to conn_id.
	 */
	int	wrote, ret, selret;

	struct timeval	timeout;
	fd_set wfds;
	
	set_non_blocking(Net_conns[conn_id].channel);
	wrote = writesock( Net_conns[conn_id].channel, buffer, size, 0 );
#ifndef WIN32
	ret = errno;
#else
	ret = WSAGetLastError();
#endif
	set_blocking(Net_conns[conn_id].channel);
	if(wrote == -1)
	{
		if(tcpip_would_block(ret))
		{
			timeout.tv_sec = Write_timeout;
			timeout.tv_usec = 0;
			FD_ZERO(&wfds);
			FD_SET( Net_conns[conn_id].channel, &wfds);
			selret = select(FD_SETSIZE, NULL, &wfds, NULL, &timeout);
			if(selret > 0)
			{
				wrote = writesock( Net_conns[conn_id].channel, buffer, size, 0 );
				if( wrote == -1 ) 
				{
					return(0);
				}
			}
		}
		else
			return(0);
	}
	return(wrote);
}

int tcpip_close( conn_id )
int conn_id;
{
	int channel;
	/* Clear all traces of the connection conn_id.
	 */
	if(Net_conns[conn_id].timr_ent)
	{
		dtq_rem_entry(queue_id, Net_conns[conn_id].timr_ent);
		Net_conns[conn_id].timr_ent = NULL;
	}
	channel = Net_conns[conn_id].channel;
	Net_conns[conn_id].channel = 0;
	Net_conns[conn_id].port = 0;
	Net_conns[conn_id].node[0] = 0;
	Net_conns[conn_id].task[0] = 0;
	if(channel)
		closesock(channel);
	return(1);
}


int tcpip_failure( code )
int code;
{
	return(!code);
}

int tcpip_would_block( code )
int code;
{
   if(code == EWOULDBLOCK)
		return(1);
    return(0);
}

void tcpip_report_error( code )
int code;
{
#ifndef WIN32
	perror("tcpip");
#else
	my_perror("tcpip", code);
#endif
}

#ifdef WIN32
int my_perror(str, error)
char *str;
int error;
{
int code;

	if(error <= 0)
		code = WSAGetLastError();
	else
		code = error;
	printf("new - %s\n",strerror(code));
	printf("%s: ",str);
	switch(code)
	{
		case WSAEWOULDBLOCK:
			printf("Operation would block");
			break;
		case WSAEINPROGRESS:
			printf("Operation now in progress");
			break;
		case WSAEALREADY:
			printf("Operation already in progress");
			break;
		case WSAENOTSOCK:
			printf("Socket operation on non-socket");
			break;
		case WSAEDESTADDRREQ:
			printf("Destination address required");
			break;
		case WSAEMSGSIZE:
			printf("Message too long");
			break;
		case WSAEPROTOTYPE:
			printf("Protocol wrong type for socket");
			break;
		case WSAENOPROTOOPT:
			printf("Protocol not available");
			break;
		case WSAEPROTONOSUPPORT:
			printf("Protocol not supported");
			break;
		case WSAESOCKTNOSUPPORT:
			printf("Socket type not supported");
			break;
		case WSAEOPNOTSUPP:
			printf("Operation not supported on transport endpoint");
			break;
		case WSAEPFNOSUPPORT:
			printf("Protocol family not supported");
			break;
		case WSAEAFNOSUPPORT:
			printf("Address family not supported by protocol");
			break;
		case WSAEADDRINUSE:
			printf("Address already in use");
			break;
		case WSAEADDRNOTAVAIL:
			printf("Cannot assign requested address");
			break;
		case WSAENETDOWN:
			printf("Network is down");
			break;
		case WSAENETUNREACH:
			printf("Network is unreachable");
			break;
		case WSAENETRESET:
			printf("Network dropped connection because of reset");
			break;
		case WSAECONNABORTED:
			printf("Software caused connection abort");
			break;
		case WSAECONNRESET:
			printf("Connection reset by peer");
			break;
		case WSAENOBUFS:
			printf("No buffer space available");
			break;
		case WSAEISCONN:
			printf("Transport endpoint is already connected");
			break;
		case WSAENOTCONN:
			printf("Transport endpoint is not connected");
			break;
		case WSAESHUTDOWN:
			printf("Cannot send after transport endpoint shutdown");
			break;
		case WSAETOOMANYREFS:
			printf("Too many references: cannot splice");
			break;
		case WSAETIMEDOUT:
			printf("Connection timed out");
			break;
		case WSAECONNREFUSED:
			printf("Connection refused");
			break;
		case WSAELOOP:
			printf("Too many symbolic links encountered");
			break;
		case WSAENAMETOOLONG:
			printf("File name too long");
			break;
		case WSAEHOSTDOWN:
			printf("Host is down");
			break;
		case WSAEHOSTUNREACH:
			printf("No route to host");
			break;
		case WSAENOTEMPTY:
			printf("Directory not empty");
			break;
		case WSAEUSERS:
			printf("Too many users");
			break;
		case WSAEDQUOT:
			printf("Quota exceeded");
			break;
		case WSAESTALE:
			printf("Stale NFS file handle");
			break;
		case WSAEREMOTE:
			printf("Object is remote");
			break;
		case WSAHOST_NOT_FOUND:
			printf("Host not found");
			break;
		case WSATRY_AGAIN:
			printf("Host not found, or SERVERFAIL");
			break;
		case WSANO_RECOVERY:
			printf("Non recoverable errors, FORMERR, REFUSED, NOTIMP");
			break;
		case WSANO_DATA:
			printf("Valid name, no data record of requested type");
			break;
		default:
			printf("Unknown error %d",code);
	}
	printf("\n");
	return(1);
}

void my_strerror(error, msg)
int error;
char *msg;
{
int code;
char str[128];

	if(error <= 0)
		code = WSAGetLastError();
	else
		code = error;
	switch(code)
	{
		case WSAEWOULDBLOCK:
			sprintf(str,"Operation would block");
			break;
		case WSAEINPROGRESS:
			sprintf(str,"Operation now in progress");
			break;
		case WSAEALREADY:
			sprintf(str,"Operation already in progress");
			break;
		case WSAENOTSOCK:
			sprintf(str,"Socket operation on non-socket");
			break;
		case WSAEDESTADDRREQ:
			sprintf(str,"Destination address required");
			break;
		case WSAEMSGSIZE:
			sprintf(str,"Message too long");
			break;
		case WSAEPROTOTYPE:
			sprintf(str,"Protocol wrong type for socket");
			break;
		case WSAENOPROTOOPT:
			sprintf(str,"Protocol not available");
			break;
		case WSAEPROTONOSUPPORT:
			sprintf(str,"Protocol not supported");
			break;
		case WSAESOCKTNOSUPPORT:
			sprintf(str,"Socket type not supported");
			break;
		case WSAEOPNOTSUPP:
			sprintf(str,"Operation not supported on transport endpoint");
			break;
		case WSAEPFNOSUPPORT:
			sprintf(str,"Protocol family not supported");
			break;
		case WSAEAFNOSUPPORT:
			sprintf(str,"Address family not supported by protocol");
			break;
		case WSAEADDRINUSE:
			sprintf(str,"Address already in use");
			break;
		case WSAEADDRNOTAVAIL:
			sprintf(str,"Cannot assign requested address");
			break;
		case WSAENETDOWN:
			sprintf(str,"Network is down");
			break;
		case WSAENETUNREACH:
			sprintf(str,"Network is unreachable");
			break;
		case WSAENETRESET:
			sprintf(str,"Network dropped connection because of reset");
			break;
		case WSAECONNABORTED:
			sprintf(str,"Software caused connection abort");
			break;
		case WSAECONNRESET:
			sprintf(str,"Connection reset by peer");
			break;
		case WSAENOBUFS:
			sprintf(str,"No buffer space available");
			break;
		case WSAEISCONN:
			sprintf(str,"Transport endpoint is already connected");
			break;
		case WSAENOTCONN:
			sprintf(str,"Transport endpoint is not connected");
			break;
		case WSAESHUTDOWN:
			sprintf(str,"Cannot send after transport endpoint shutdown");
			break;
		case WSAETOOMANYREFS:
			sprintf(str,"Too many references: cannot splice");
			break;
		case WSAETIMEDOUT:
			sprintf(str,"Connection timed out");
			break;
		case WSAECONNREFUSED:
			sprintf(str,"Connection refused");
			break;
		case WSAELOOP:
			sprintf(str,"Too many symbolic links encountered");
			break;
		case WSAENAMETOOLONG:
			sprintf(str,"File name too long");
			break;
		case WSAEHOSTDOWN:
			sprintf(str,"Host is down");
			break;
		case WSAEHOSTUNREACH:
			sprintf(str,"No route to host");
			break;
		case WSAENOTEMPTY:
			sprintf(str,"Directory not empty");
			break;
		case WSAEUSERS:
			sprintf(str,"Too many users");
			break;
		case WSAEDQUOT:
			sprintf(str,"Quota exceeded");
			break;
		case WSAESTALE:
			sprintf(str,"Stale NFS file handle");
			break;
		case WSAEREMOTE:
			sprintf(str,"Object is remote");
			break;
		case WSAHOST_NOT_FOUND:
			sprintf(str,"Host not found");
			break;
		case WSATRY_AGAIN:
			sprintf(str,"Host not found, or SERVERFAIL");
			break;
		case WSANO_RECOVERY:
			sprintf(str,"Non recoverable errors, FORMERR, REFUSED, NOTIMP");
			break;
		case WSANO_DATA:
			sprintf(str,"Valid name, no data record of requested type");
			break;
		default:
			sprintf(str,"Unknown error %d",code);
	}
	strcpy(msg, str);
}
#endif

void tcpip_get_error( str, code )
char *str;
int code;
{
	DISABLE_AST
#ifndef WIN32
	if((errno == ENOENT) && (h_errno == HOST_NOT_FOUND))
		strcpy(str,"Host not found");
	else
		strcpy(str, strerror(errno));
#else
	my_strerror(code, str);
#endif
	ENABLE_AST
}
