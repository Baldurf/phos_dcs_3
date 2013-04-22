/*
 * DNA (Delphi Network Access) implements the network layer for the DIM
 * (Delphi Information Managment) System.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <string.h>
#include <time.h>
#define DIMLIB
//#include <dim.h>
#include "feeclient/dim/dim/dim.h"


int get_proc_name(proc_name)
char *proc_name;
{
#ifndef VxWorks
	sprintf( proc_name, "%d", getpid() );
#else
	sprintf( proc_name, "%d", taskIdSelf() );      
#endif
	return(1);
}


int get_node_name(node_name)
char *node_name;
{
#ifndef VxWorks
struct hostent *host;
#endif
char *p;
int	i;

	DISABLE_AST
#ifdef WIN32
	init_sock();
#endif
	if( (p = getenv("DIM_HOST_NODE")) != NULL )
	{
		strcpy( node_name, p );
		ENABLE_AST
		return(1);
	}
	gethostname(node_name, MAX_NODE_NAME);
#ifndef VxWorks
#ifndef RAID
	if(!strchr(node_name,'.'))
	{
		if ((host = gethostbyname(node_name)) == (struct hostent *)0) 
		{
			ENABLE_AST
			return(0);
		}
		strcpy(node_name,host->h_name);
		if(!strchr(node_name,'.'))
		{
		    if(host->h_aliases)
		    {
				if(host->h_aliases[0])
				{
					for(i = 0; host->h_aliases[i]; i++)
					{
						p = host->h_aliases[i];
						if(strchr(p,'.'))
						{
							strcpy(node_name,p);
							break;
						}
					}
				}
		    }
		}
	}
#endif
#endif
	ENABLE_AST
	return(1);
}

int get_node_addr(node_addr)
char *node_addr;
{
#ifndef VxWorks
struct hostent *host;
#endif
char	*p;
char node_name[MAX_NODE_NAME];
unsigned int a,b,c,d;
unsigned char *ptr;

#ifdef WIN32
	init_sock();
#endif
	gethostname(node_name, MAX_NODE_NAME);
#ifndef VxWorks
	if ((host = (struct hostent *)gethostbyname(node_name)) == 
			(struct hostent *)0) 
		return(0);
    	ptr = (unsigned char *)host->h_addr;
    	node_addr[0] = *ptr++;
    	node_addr[1] = *ptr++;
    	node_addr[2] = *ptr++;
    	node_addr[3] = *ptr++;
    	return(1);
#else
    	node_addr[0] = 0;
    	node_addr[1] = 0;
    	node_addr[2] = 0;
    	node_addr[3] = 0;
		return(0);
#endif
}

void dim_print_date_time()
{
	time_t t;
	char str[128];

	t = time((time_t *)0);
/*
#ifdef WIN32
	strcpy(str, ctime(&t));
#else
#ifdef LYNXOS
	ctime_r(&t, str, 128);
#else
	ctime_r(&t, str);
#endif
#endif
*/
	my_ctime(&t, str, 128);
	str[strlen(str)-1] = '\0';
	printf("PID %d - ",getpid());
	printf("%s - ",str );
}

void dim_print_msg(msg, severity)
char *msg;
int severity;
{
	dim_print_date_time();
	switch(severity)
	{
		case 0: printf("(INFO) ");
			break;
		case 1: printf("(WARNING) ");
			break;
		case 2: printf("(ERROR) ");
			break;
		case 3: printf("(FATAL) ");
			break;
	}
	printf("%s\n",msg);
}

void dim_panic( s )
char *s;
{
	printf( "\n\nDNA library panic: %s\n\n", s );
	exit(0);
}

int get_dns_node_name( node_name )
char *node_name;
{
	char	*p;

	if( (p = getenv("DIM_DNS_NODE")) == NULL )
		return(0);
	else {
		strcpy( node_name, p );
		return(1);
	}
}

int get_dns_port_number()
{
	char	*p;

	if( (p = getenv("DIM_DNS_PORT")) == NULL )
		return(DNS_PORT);
	else {
		return(atoi(p));
	}
}

int get_dns_accepted_domains( domains )
char *domains;
{
	char	*p;

	if( (p = getenv("DIM_DNS_ACCEPTED_DOMAINS")) == NULL )
		return(0);
	else {
		strcpy( domains, p );
		return(1);
	}
}
