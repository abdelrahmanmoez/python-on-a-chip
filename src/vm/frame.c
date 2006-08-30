#undef __FILE_ID__
#define __FILE_ID__ 0x03
/**
 * VM Frame
 *
 * VM frame operations.
 *
 * @author      Dean Hall
 * @copyright   Copyright 2002 Dean Hall.  All rights reserved.
 * @file        frame.c
 *
 * Log
 * ---
 *
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/04/20   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include "py.h"


/***************************************************************
 * Constants
 **************************************************************/

/***************************************************************
 * Macros
 **************************************************************/

/***************************************************************
 * Types
 **************************************************************/

/***************************************************************
 * Globals
 **************************************************************/

/***************************************************************
 * Prototypes
 **************************************************************/

/***************************************************************
 * Functions
 **************************************************************/

PyReturn_t
frame_new(pPyObj_t pfunc, pPyObj_t * r_pobj)
{
    PyReturn_t retval = PY_RET_OK;
    S16 fsize = 0;
    S8 stacksz = 0;
    S8 nlocals = 0;
    pPyCo_t pco = C_NULL;
    pPyFrame_t pframe = C_NULL;
    P_U8 paddr = C_NULL;

    /* get fxn's code obj */
    pco = ((pPyFunc_t)pfunc)->f_co;

    /* TypeError if passed func's CO is not a true COB */
    if (pco->od.od_type != OBJ_TYPE_COB)
    {
        return PY_RET_EX_TYPE;
    }

    /* get sizes needed to calc frame size */
    paddr = pco->co_codeimgaddr + CI_STACKSIZE_FIELD;
    stacksz = mem_getByte(pco->co_memspace, &paddr);
    nlocals = mem_getByte(pco->co_memspace, &paddr);
    fsize = sizeof(PyFrame_t) + (stacksz + nlocals) *
            sizeof(pPyObj_t);
    /* allocate a frame */
    retval = heap_getChunk(fsize, (P_U8 *)&pframe);
    PY_RETURN_IF_ERROR(retval);

    /* set frame fields */
    pframe->od.od_type = OBJ_TYPE_FRM;
    pframe->fo_back = C_NULL;
    pframe->fo_func = (pPyFunc_t)pfunc;
    pframe->fo_memspace = pco->co_memspace;
    /* init instruction pointer, line number and block stack */
    pframe->fo_ip = pco->co_codeaddr;
    pframe->fo_line = 0;
    pframe->fo_blockstack = C_NULL;
    /* init globals dict to NULL, interpreter will set it */
    pframe->fo_globals = C_NULL;
    /* frame's attrs points to func/mod/class's attrs dict */
    pframe->fo_attrs = ((pPyFunc_t)pfunc)->f_attrs;
    /* empty stack points to one past locals */
    pframe->fo_sp = &(pframe->fo_locals[nlocals]);
    
    /* return ptr to frame */
    * r_pobj = (pPyObj_t)pframe;
    return retval;
}


/***************************************************************
 * Test
 **************************************************************/

/***************************************************************
 * Main
 **************************************************************/
