/*
 * A utility file. A double linked list.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

#define DIMLIB
//#include <dim.h>
#include "feeclient/dim/dim/dim.h"

void dll_init( head )
register DLL* head;
{
	DISABLE_AST
	head->next = head;
	head->prev = head;
	ENABLE_AST
}


void dll_insert_queue( head, item )
register DLL *head, *item;
{
	register DLL *prevp;

	DISABLE_AST
	item->next = head;
	prevp = head->prev;
	item->prev = prevp;
	prevp->next = item;
	head->prev = item;
	ENABLE_AST
}	

void dll_insert_after( atitem, item )
register DLL *atitem, *item;
{
	register DLL *auxp;

	DISABLE_AST
	auxp = atitem->next;
	item->next = auxp;
	item->prev = atitem;
	atitem->next = item;
	auxp->prev = item;
	ENABLE_AST
}	

DLL *dll_search( head, data, size )
register DLL *head; 
register char *data;
register int size;
{
	register DLL *auxp= head->next;
 
	DISABLE_AST
	while( auxp!= head ) {
		if( !memcmp(auxp->user_info, data, size) ) {
			ENABLE_AST
			return(auxp);
		}
		auxp = auxp->next;
	}
	ENABLE_AST
	return((DLL *)0);
}


DLL *dll_get_next( head, item )
register DLL *head, *item;
{
	DISABLE_AST
	if( item->next != head ) {
		ENABLE_AST
		return(item->next);
	}
	ENABLE_AST
	return((DLL *) 0);
}

DLL *dll_get_prev( head, item )
register DLL *head, *item;
{
	DISABLE_AST
	if( item->prev != head ) {
		ENABLE_AST
		return(item->prev);
	}
	ENABLE_AST
	return((DLL *) 0);
}

int dll_empty( head )
register DLL *head;
{
	DISABLE_AST
	if( head->next != head ) {
		ENABLE_AST
		return(0);
	}
	ENABLE_AST
	return(1);
}


void dll_remove( item ) 
register DLL *item;
{
	register DLL *prevp, *nextp;

	DISABLE_AST
	prevp = item->prev;
	nextp = item->next;
	prevp->next = item->next;
	nextp->prev = prevp;
	ENABLE_AST
}	

DLL *dll_search_next_remove( head, offset, data, size )
register DLL *head; 
int offset;
register char *data;
register int size;
{
	register DLL *auxp= head->next;
	DLL *retp = 0;
 
	DISABLE_AST
	while( auxp!= head ) {
		if( !memcmp(&(auxp->user_info[offset]), data, size) ) {
			retp = auxp;
		}
		auxp = auxp->next;
	}
	if( retp)
	{
		dll_remove(retp);
		ENABLE_AST
		return(retp);
	}
	ENABLE_AST
	return((DLL *)0);
}

