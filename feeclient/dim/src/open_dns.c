/*
 * A utility file. 
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

#define DIMLIB
//#include <dim.h>
#include "../dim/dim.h"


typedef struct {
	char node_name[MAX_NODE_NAME];
	char task_name[MAX_TASK_NAME];
	void (*recv_rout)();
	void (*error_rout)();
	TIMR_ENT *timr_ent;
} PENDING_CONN;

static PENDING_CONN Pending_conns[MAX_CONNS];
static int Timer_q = 0;
static char DNS_nodes[255] = {'\0'};
static int DNS_port = 0;


_DIM_PROTO( void retry_dns_connection,    ( int conn_pend_id ) );
static int get_free_pend_conn(), find_pend_conn(), rel_pend_conn();

int dim_set_dns_node(node)
char *node;
{
	strcpy(DNS_nodes, node);
	return(1);
}

int dim_get_dns_node(node)
char *node;
{
	register int node_exists;

	if(DNS_nodes[0])
	{
		strcpy(node, DNS_nodes);
		node_exists = 1;
	}
	else
		node_exists = get_dns_node_name( node );
	return(node_exists);
}

int dim_set_dns_port(port)
int port;
{
	DNS_port = port;
	return(1);
}

int dim_get_dns_port()
{
int port;

	if(DNS_port)
		port = DNS_port;
	else
		port = get_dns_port_number();
	return(port);
}

int open_dns( recv_rout, error_rout, tmout_min, tmout_max )
void (*recv_rout)();
void (*error_rout)();
int tmout_min, tmout_max;
{
	char nodes[255],all_nodes[255];
	char str[132], node_info[MAX_NODE_NAME+4];
	register char *dns_node, *ptr; 
	register int conn_id, conn_pend_id;
	register PENDING_CONN *conn_pend;
	register int timeout, node_exists;
	int i, dns_port;

	conn_id = 0;
	if( !Timer_q )
		Timer_q = dtq_create();
/*
	dns_port = get_dns_port_number();
	if(DNS_nodes[0])
	{
		strcpy(nodes, DNS_nodes);
		node_exists = 1;
	}
	else
		node_exists = get_dns_node_name( nodes );
*/
	dns_port = dim_get_dns_port();
	node_exists = dim_get_dns_node(nodes);
	if( !(conn_pend_id = find_pend_conn(DNS_TASK, recv_rout)) ) 
	{
		if(!node_exists)
		{
/*
#ifdef unix
			dim_print_date_time();
			printf("DIM_DNS_NODE undefined -> Exiting...\n");
			fflush(stdout);
			exit(2);
#else

			conn_pend_id = get_free_pend_conn();
			conn_pend = &Pending_conns[conn_pend_id];
			strncpy(conn_pend->task_name, DNS_TASK, MAX_TASK_NAME); 
			conn_pend->recv_rout = recv_rout;
			timeout = rand_tmout( tmout_min, tmout_max );
			conn_pend->timr_ent = 
				dtq_add_entry( Timer_q, timeout,
					       retry_dns_connection,
					       conn_pend_id );
*/
			return(-2);
/*
#endif
*/
		}
/*
		strcpy(all_nodes, nodes);
*/
		ptr = nodes;			
		while(1)
		{
			dns_node = ptr;
			if(ptr = (char *)strchr(ptr,','))
			{
				*ptr = '\0';			
				ptr++;
			}
			strcpy(node_info,dns_node);
			for(i = 0; i < 4; i ++)
				node_info[strlen(node_info)+i+1] = 0xff;
			if( conn_id = dna_open_client( node_info, DNS_TASK, dns_port,
						 TCPIP, recv_rout, error_rout ))
				break;
			if( !ptr )
				break;
		}
		if(!conn_id)
		{
			conn_pend_id = get_free_pend_conn();
			conn_pend = &Pending_conns[conn_pend_id];
			strncpy(conn_pend->task_name, DNS_TASK, MAX_TASK_NAME); 
			conn_pend->recv_rout = recv_rout;
			conn_pend->error_rout = error_rout;
			timeout = rand_tmout( tmout_min, tmout_max );
			conn_pend->timr_ent = dtq_add_entry( Timer_q, timeout,
				retry_dns_connection,
				conn_pend_id );
			return( -1);
		}
	}
	else
		return(-1);
	return(conn_id);
}


void retry_dns_connection( conn_pend_id )
register int conn_pend_id;
{
	char nodes[255];
	char str[132], node_info[MAX_NODE_NAME+4];
	register char *dns_node, *ptr;
	register int conn_id, node_exists;
	register PENDING_CONN *conn_pend;
	static int retrying = 0;
	int i, dns_port;

	if( retrying ) return;
	retrying = 1;
	conn_pend = &Pending_conns[conn_pend_id];

/*
	dns_port = get_dns_port_number();
	if(DNS_nodes[0])
	{
		strcpy(nodes, DNS_nodes);
		node_exists = 1;
	}
	else
		node_exists = get_dns_node_name( nodes );
*/
	dns_port = dim_get_dns_port();
	node_exists = dim_get_dns_node(nodes);
	if(node_exists)
	{
		ptr = nodes;			
		while(1)
		{
			dns_node = ptr;
			if(ptr = (char *)strchr(ptr,','))
			{
				*ptr = '\0';			
				ptr++;
			}
			strcpy(node_info,dns_node);
			for(i = 0; i < 4; i ++)
				node_info[strlen(node_info)+i+1] = 0xff;
			if( conn_id = dna_open_client( node_info, conn_pend->task_name,
					 dns_port, TCPIP,
					 conn_pend->recv_rout, conn_pend->error_rout ) )
				break;
			if( !ptr )
				break;
		}
	}
	if(conn_id)
	{
		rel_pend_conn( conn_pend_id );
		dtq_rem_entry( Timer_q, conn_pend->timr_ent );
	}
	retrying = 0;
}	


static int get_free_pend_conn()
{
	register int i;
	register PENDING_CONN *pending_connp;

	for( i = 1, pending_connp = &Pending_conns[1]; i < MAX_CONNS; 
		i++, pending_connp++ )
	{
		if( pending_connp->task_name[0] == '\0' )
		{
			return(i);
		}
	}
	return(0);
}


static int find_pend_conn( task, rout )
register char *task;
register void (*rout)();
{
	register int i;
	register PENDING_CONN *pending_connp;

	for( i = 1, pending_connp = &Pending_conns[1]; i < MAX_CONNS; 
		i++, pending_connp++ )
	{
		if( (!strcmp(pending_connp->task_name, task)) &&
			(pending_connp->recv_rout == rout) )
		{
			return(i);
		}
	}
	return(0);
}


static int rel_pend_conn( conn_id )
register int conn_id;
{
	Pending_conns[conn_id].task_name[0] = '\0';
	return(1);
}	


int rand_tmout( min, max )
int min, max;
{
	int aux;

	aux = rand();
	aux %= (max - min);
	aux += min;
	return(aux);
}
