/* File: stonithd_api.h
 * Description: Head file which define stonith deamon APIs for clients. 
 *
 * Author: Sun Jiang Dong <sunjd@cn.ibm.com>
 * Copyright (c) 2004 International Business Machines
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _STONITD_API_H_
#define _STONITD_API_H_
#include <uuid/uuid.h>
#include <clplumbing/ipc.h>

#define ST_OK 	0
#define ST_FAIL -1

typedef enum stonith_type
{	
	QUERY = 0,
	RESET = 1,
	POWERON = 2,
	POWEROFF = 3,
} stonith_type_t;
/* Now they (the above) are coresponding to the definition in stonith.h,
 * except the QUERY operation.
 */

/* need to translate the stonith operation value defined in stonith.h, 
 * the situation is more complex than a single node.
 */
typedef enum stonith_ret{
	STONITH_SUCCEEDED,
	STONITH_CANNOT,	
	STONITH_TIMEOUT,
	STONITH_GENERIC, /* Other generic or unspecified error */
} stonith_ret_t;

typedef struct
{
/* Input fields and output fields*/
	stonith_type_t 	optype;
	/* Temporarily only use node_name until having a good idea about it */
	char *		node_name;
	uuid_t		node_uuid;
	int 		timeout; /* its unit is millisecond */

/* Only output fields */
	int		call_id;
	stonith_ret_t	op_result;	
/*
 * Now just used for op==QUERY, then data is a GList *, contain the node name 
 * who can stonith a node whose name equals node_name/node_uuid. */
	void *		node_list;
} stonith_ops_t;

/* It's an asynchronus api */
typedef void (*stonith_ops_callback_t)(stonith_ops_t * op, void * private_data);

/*	return value: 	ST_OK or ST_FAIL.	*/
int stonithd_signon(const char * client);
int stonithd_signoff(void);

/*
 *	stonithd_node_fence: 
 *			Node fence operation. It's always an asynchronous 
 *			operation whose final reslut will return in a callback.
 *	op:		Contain the detailed information to stonith a node or
 *			query who can stonith a node.
 *	return value: 	ST_OK or ST_FAIL.
 */
int stonithd_node_fence(stonith_ops_t * op);

/*
 *	stonithd_set_stonith_callback: 
 *			Set callback for handling the stonith result.
 *	private_data:	Later passed to callback.
 */
int stonithd_set_stonith_ops_callback(stonith_ops_callback_t callback, 
				      void * private_data);

/*
 *	stonithd_input_IPC_channel : 
 *		Return IPC_Channel which can be given to select(2) or poll(2)
 *		for determining when input are ready to be handled.
 *	return value: return NULL if failed.
 */
IPC_Channel * stonithd_input_IPC_channel(void);

/*
 *	stonithd_op_result_ready:
 *			Returns TRUE when a operaton result is ready to be read.
 */
gboolean stonithd_op_result_ready(void);

/*
 *	stonithd_receive_ops_result:
 *		Cause the next stonith result to be handled - activating 
 *		the callback for processing the result.  If no callback processes
 *		the result, it will be ignored and disposed of.
 *		It returns ST_OK if a message was received.
 *	return value: 
 *		ST_OK or ST_FAIL.
 */
int stonithd_receive_ops_result(gboolean blocking);

/* The following is only used by the stonith RA executor. In other words, these
 * are only used internally in node fencing sub-system, other parts don't need
 * care of it.
 */
typedef struct stonithRA_ops 
{
/* Input fields and output fields*/
	char *	rsc_id;
	char *	ra_name;
	char * 	op_type;
	GHashTable *	params;

/* Only output fields */
	int		call_id;
	int		op_result;	/* exit code as the real OCF RA */

/* Internally use only */
	void *		private_data;
} stonithRA_ops_t;

/* It's an asynchronus api */
typedef void (*stonithRA_ops_callback_t)(stonithRA_ops_t * op, 
					 void * private_data);
/*
 *	stonithd_virtual_stonithRA_ops: 
 *			simulate the stonith RA functions such as start, stop
 *			and monitor. Only called in stonith RA plugin. 
 *			It's a synchronous operation now, that should be enough.
 *	op:		Contain the detailed information to activate a stonith
 *			plugin to simulate a stonith RA.
 *	return value:   ST_OK or ST_FAIL
 *			This operation's id will stored in call_id which should
 *			be used in callback function to distiguish different
 *			operations.
 */
int stonithd_virtual_stonithRA_ops( stonithRA_ops_t * op, int * call_id);

/*
 *	stonithd_set_stonith_callback: 
 *			Set callback for handling the stonith result.
 *	private_data:	Later passed to callback.
 */
int stonithd_set_stonithRA_ops_callback(stonithRA_ops_callback_t callback, 
				      void * private_data);

/*
 *	stonithd_list_stonith_types:
 *			List the valid stonith resource/devices types,
 *			This is a synchronous API.
 *	types:		Stored the returned types if succeed.
 *	return value:   ST_OK or ST_FAIL
 */
int stonithd_list_stonith_types(GList ** types);

/* Enable debug information output for  */
void enable_debug_mode(void);

#endif /* _STONITD_API_H_ */
