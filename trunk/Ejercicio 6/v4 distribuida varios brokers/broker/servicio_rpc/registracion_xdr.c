/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "registracion.h"

bool_t
xdr_par_broker_tester (XDR *xdrs, par_broker_tester *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->id_broker))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->id_tester))
		 return FALSE;
	return TRUE;
}
