/*
*+
*  Name:
*     fpolymap.c

*  Purpose:
*     Define a FORTRAN 77 interface to the AST PolyMap class.

*  Type of Module:
*     C source file.

*  Description:
*     This file defines FORTRAN 77-callable C functions which provide
*     a public FORTRAN 77 interface to the PolyMap class.

*  Routines Defined:
*     AST_ISAPOLYMAP
*     AST_POLYMAP

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     DSB: D.S. Berry (Starlink)

*  History:
*     27-SEP-2003 (DSB):
*        Original version.
*/

/* Define the astFORTRAN77 macro which prevents error messages from
   AST C functions from reporting the file and line number where the
   error occurred (since these would refer to this file, they would
   not be useful). */
#define astFORTRAN77

/* Header files. */
/* ============= */
#include "f77.h"                 /* FORTRAN <-> C interface macros (SUN/209) */
#include "c2f77.h"               /* F77 <-> C support functions/macros */
#include "error.h"               /* Error reporting facilities */
#include "memory.h"              /* Memory handling facilities */
#include "polymap.h"             /* C interface to the PolyMap class */

F77_LOGICAL_FUNCTION(ast_isapolymap)( INTEGER(THIS),
                                      INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   F77_LOGICAL_TYPE(RESULT);

   astAt( "AST_ISAPOLYMAP", NULL, 0 );
   astWatchSTATUS(
      RESULT = astIsAPolyMap( astI2P( *THIS ) ) ? F77_TRUE : F77_FALSE;
   )
   return RESULT;
}

F77_INTEGER_FUNCTION(ast_polymap)( INTEGER(NIN),
                                   INTEGER(NOUT),
                                   INTEGER(NCOEFF_F),
                                   DOUBLE_ARRAY(COEFF_F),
                                   INTEGER(NCOEFF_I),
                                   DOUBLE_ARRAY(COEFF_I),
                                   CHARACTER(OPTIONS),
                                   INTEGER(STATUS)
                                   TRAIL(OPTIONS) ) {
   GENPTR_INTEGER(NIN)
   GENPTR_INTEGER(NOUT)
   GENPTR_INTEGER(NCOEFF_F)
   GENPTR_DOUBLE_ARRAY(COEFF_F)
   GENPTR_INTEGER(NCOEFF_I)
   GENPTR_DOUBLE_ARRAY(COEFF_I)
   GENPTR_CHARACTER(OPTIONS)
   F77_INTEGER_TYPE(RESULT);
   char *options;
   int i;

   astAt( "AST_POLYMAP", NULL, 0 );
   astWatchSTATUS(
      options = astString( OPTIONS, OPTIONS_length );

/* Change ',' to '\n' (see AST_SET in fobject.c for why). */
      if ( astOK ) {
         for ( i = 0; options[ i ]; i++ ) {
            if ( options[ i ] == ',' ) options[ i ] = '\n';
         }
      }
      RESULT = astP2I( astPolyMap( *NIN, *NOUT, *NCOEFF_F, COEFF_F, *NCOEFF_I, 
                                   COEFF_I, "%s", options ) );
      astFree( options );
   )
   return RESULT;
}
