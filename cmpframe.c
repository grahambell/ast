/*
*class++
*  Name:
*     CmpFrame

*  Purpose:
*     Compound Frame.

*  Constructor Function:
c     astCmpFrame
f     AST_CMPFRAME

*  Description:
*     A CmpFrame is a compound Frame which allows two component Frames
*     (of any class) to be merged together to form a more complex
*     Frame. The axes of the two component Frames then appear together
*     in the resulting CmpFrame (those of the first Frame, followed by
*     those of the second Frame).
*
*     Since a CmpFrame is itself a Frame, it can be used as a
*     component in forming further CmpFrames. Frames of arbitrary
*     complexity may be built from simple individual Frames in this
*     way.

*  Inheritance:
*     The CmpFrame class inherits from the Frame class.

*  Attributes:
*     The CmpFrame class does not define any new attributes beyond
*     those which are applicable to all Frames.

*  Functions:
c     The CmpFrame class does not define any new functions beyond those
f     The CmpFrame class does not define any new routines beyond those
*     which are applicable to all Frames.

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     RFWS: R.F. Warren-Smith (Starlink)

*  History:
*     4-MAR-1996 (RFWS):
*        Original version.
*     27-FEB-1997 (RFWS):
*        Improved public prologues.
*     25-FEB-1998 (RFWS):
*        Over-ride the astUnformat method.
*     6-APR-1998 (RFWS):
*        Fixed bug in returned value of GenAxisSelection.
*     22-SEP-1998 (RFWS):
*        Fixed bug in Match function - was not checking Domain values
*        for equality.
*     11-JUN-1999 (RFWS):
*        Fixed bug in GenAxisSelection-  some selections were being omitted.
*class--
*/

/* Module Macros. */
/* ============== */
/* Set the name of the class we are implementing. This indicates to
   the header files that define class interfaces that they should make
   "protected" symbols available. */
#define astCLASS CmpFrame

/* Define numerical constants for use in this module. */
#define LABEL_BUFF_LEN 100       /* Max length of default axis Label string */
#define SYMBOL_BUFF_LEN 50       /* Max length of default axis Symbol string */

/* Define macros to implement member functions for accessing axis
   attributes. */
/*
*  Name:
*     MAKE_CLEAR

*  Purpose:
*     Implement a function to clear an attribute value for a CmpFrame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "cmpframe.h"
*     MAKE_CLEAR(attribute)

*  Class Membership:
*     Defined by the CmpFrame class.

*  Description:
*     This macro expands to an implementation of a private member
*     function of the form:
*
*        static void Clear<Attribute>( AstFrame *this, int axis )
*
*     which clears an attribute value for a specified axis of a CmpFrame.

*  Parameters:
*      attribute
*         The name of the attribute to be cleared, as it appears in the
*         function name (e.g. Label in "ClearLabel").

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           void astClear<Attribute>( AstFrame *this, int axis )
*
*     which clears the required attribute for a Frame object.
*     - To avoid problems with some compilers, you should not leave
*     any white space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_CLEAR(attribute) \
static void Clear##attribute( AstFrame *this_frame, int axis ) { \
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */ \
   int naxes1;                   /* Number of axes in frame1 */ \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Obtain a pointer to the CmpFrame structure. */ \
   this = (AstCmpFrame *) this_frame; \
\
/* Validate and permute the axis index supplied. */ \
   axis = astValidateAxis( this, axis, "astSet" #attribute ); \
\
/* Determine the number of axes in the first component Frame. */ \
   naxes1 = astGetNaxes( this->frame1 ); \
   if ( astOK ) { \
\
/* Decide which Frame contains the axis and invoke its astClear... method to \
   clear the attribute value. */ \
      if ( axis < naxes1 ) { \
         astClear##attribute( this->frame1, axis ); \
      } else { \
         astClear##attribute( this->frame2, axis - naxes1 ); \
      } \
   } \
}

/*
*  Name:
*     MAKE_GET

*  Purpose:
*     Implement a function to get an attribute value for a CmpFrame axis.

*  Type:
*     Private macro.

*  Synopsis:
#     #include "cmpframe.h"
*     MAKE_GET(attribute,type,bad_value,default,assign_default)

*  Class Membership:
*     Defined by the CmpFrame class.

*  Description:
*     This macro expands to an implementation of a private member
*     function of the form:
*
*        static <Type> Get<Attribute>( AstFrame *this, int axis )
*
*     which gets an attribute value for a specified axis of a
*     CmpFrame.

*  Parameters:
*     attribute
*        The name of the attribute whose value is to be obtained, as
*        it appears in the function name (e.g. Label in "GetLabel").
*     type
*        The C type of the attribute.
*     bad_value
*        A constant value to return if the global error status is set,
*        or if the function fails.
*     default
*        A boolean (int) value that indicates whether a new default
*        value should be returned if the requested attribute has not
*        been set for the appropriate axis of the appropriate
*        component Frame. If this value is zero, the component Frame's
*        default (for the appropriate axis) will be used instead.
*     assign_default
*        An expression that evaluates to the new default value to be
*        assigned. This value is ignored if "default" is zero, but a
*        valid (e.g. constant) value should nevertheless be supplied.

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           <Type> astGet<Attribute>( AstFrame *this, int axis )
*
*     which gets the required attribute for a Frame object.
*     - To avoid problems with some compilers, you should not leave
*     any white space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_GET(attribute,type,bad_value,default,assign_default) \
static type Get##attribute( AstFrame *this_frame, int axis ) { \
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */ \
   AstFrame *frame;              /* Pointer to Frame containing axis */\
   int axis_p;                   /* Permuted axis index */ \
   int naxes1;                   /* Number of axes in frame1 */ \
   int set;                      /* Digits attribute set? */ \
   type result;                  /* Result value to return */ \
 \
/* Initialise. */ \
   result = (bad_value); \
\
/* Check the global error status. */ \
   if ( !astOK ) return result; \
\
/* Obtain a pointer to the CmpFrame structure. */ \
   this = (AstCmpFrame *) this_frame; \
\
/* Validate and permute the axis index supplied. */ \
   axis_p = astValidateAxis( this, axis, "astGet" #attribute ); \
\
/* Determine the number of axes in the first component Frame. */ \
   naxes1 = astGetNaxes( this->frame1 ); \
   if ( astOK ) { \
\
/* Decide which Frame contains the axis and adjust the axis index if \
   necessary. */ \
      frame = ( axis_p < naxes1 ) ? this->frame1 : this->frame2; \
      axis_p = ( axis_p < naxes1 ) ? axis_p : axis_p - naxes1; \
\
/* Since the component Frame is "managed" by the enclosing CmpFrame, we next \
   test if any Frame attributes which may affect the result are undefined \
   (i.e. have not been explicitly set). If so, we over-ride them, giving \
   them temporary values dictated by the CmpFrame. Only the Digits attribute \
   is relevant here. */ \
      set = astTestDigits( frame ); \
      if ( !set ) astSetDigits( frame, astGetDigits( this ) ); \
\
/* If the default value is to be over-ridden, test if the Frame's axis \
   attribute has been set. Then, if required, obtain the attribute value from \
   the Frame. */ \
      if ( !(default) || astTest##attribute( frame, axis_p ) ) { \
         result = astGet##attribute( frame, axis_p ); \
\
/* If required, assign the new default value. */ \
      } else { \
         result = (assign_default); \
      } \
\
/* Clear Frame attributes which were temporarily over-ridden. */ \
      if ( !set ) astClearDigits( frame ); \
   } \
\
/* If an error occurred, clear the result value. */ \
   if ( !astOK ) result = (bad_value); \
\
/* Return the result. */ \
   return result; \
}

/*
*  Name:
*     MAKE_SET

*  Purpose:
*     Implement a function to set an attribute value for a CmpFrame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "cmpframe.h"
*     MAKE_SET(attribute,type)

*  Class Membership:
*     Defined by the CmpFrame class.

*  Description:
*     This macro expands to an implementation of a private member
*     function of the form:
*
*        static void Set<Attribute>( AstFrame *this, int axis, <Type> value )
*
*     which sets an attribute value for a specified axis of a CmpFrame.

*  Parameters:
*      attribute
*         The name of the attribute to be set, as it appears in the
*         function name (e.g. Label in "SetLabel").
*      type
*         The C type of the attribute.

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           void astSet<Attribute>( AstFrame *this, int axis, <Type> value )
*
*     which sets the required attribute for a Frame object.
*     - To avoid problems with some compilers, you should not leave
*     any white space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_SET(attribute,type) \
static void Set##attribute( AstFrame *this_frame, int axis, type value ) { \
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */ \
   int naxes1;                   /* Number of axes in frame1 */ \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Obtain a pointer to the CmpFrame structure. */ \
   this = (AstCmpFrame *) this_frame; \
\
/* Validate and permute the axis index supplied. */ \
   axis = astValidateAxis( this, axis, "astSet" #attribute ); \
\
/* Determine the number of axes in the first component Frame. */ \
   naxes1 = astGetNaxes( this->frame1 ); \
   if ( astOK ) { \
\
/* Decide which Frame contains the axis and invoke its astSet... method to \
   set the attribute value. */ \
      if ( axis < naxes1 ) { \
         astSet##attribute( this->frame1, axis, value ); \
      } else { \
         astSet##attribute( this->frame2, axis - naxes1, value ); \
      } \
   } \
}

/*
*  Name:
*     MAKE_TEST

*  Purpose:
*     Implement a function to test if an attribute is set for a CmpFrame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "cmpframe.h"
*     MAKE_TEST(attribute)

*  Class Membership:
*     Defined by the CmpFrame class.

*  Description:
*     This macro expands to an implementation of a private member
*     function of the form:
*
*        static int Test<Attribute>( AstFrame *this, int axis )
*
*     which tests whether an attribute value is set for a specified
*     axis of a CmpFrame.

*  Parameters:
*      attribute
*         The name of the attribute to be tested, as it appears in the
*         function name (e.g. Label in "TestLabel").

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           int astTest<Attribute>( AstFrame *this, int axis )
*
*     which tests the required attribute for a Frame object.
*     - To avoid problems with some compilers, you should not leave
*     any white space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_TEST(attribute) \
static int Test##attribute( AstFrame *this_frame, int axis ) { \
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */ \
   int naxes1;                   /* Number of axes in frame1 */ \
   int result;                   /* Result value to return */ \
\
/* Initialise. */ \
   result = 0; \
\
/* Check the global error status. */ \
   if ( !astOK ) return result; \
\
/* Obtain a pointer to the CmpFrame structure. */ \
   this = (AstCmpFrame *) this_frame; \
\
/* Validate and permute the axis index supplied. */ \
   axis = astValidateAxis( this, axis, "astSet" #attribute ); \
\
/* Determine the number of axes in the first component Frame. */ \
   naxes1 = astGetNaxes( this->frame1 ); \
   if ( astOK ) { \
\
/* Decide which Frame contains the axis and invoke its astTest... method to \
   test the attribute. */ \
      if ( axis < naxes1 ) { \
         result = astTest##attribute( this->frame1, axis ); \
      } else { \
         result = astTest##attribute( this->frame2, axis - naxes1 ); \
      } \
   } \
\
/* Return the result. */ \
   return result; \
}

/* Include files. */
/* ============== */
/* Interface definitions. */
/* ---------------------- */
#include "error.h"               /* Error reporting facilities */
#include "memory.h"              /* Memory allocation facilities */
#include "object.h"              /* Base Object class */
#include "mapping.h"             /* Coordinate Mappings */
#include "unitmap.h"             /* Unit Mappings */
#include "permmap.h"             /* Coordinate permutation Mappings */
#include "cmpmap.h"              /* Compound Mappings */
#include "axis.h"                /* Coordinate axes */
#include "frame.h"               /* Parent Frame class */
#include "cmpframe.h"            /* Interface definition for this class */

/* Error code definitions. */
/* ----------------------- */
#include "ast_err.h"             /* AST error codes */

/* C header files. */
/* --------------- */
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Module Variables. */
/* ================= */
/* Define the class virtual function table and its initialisation flag
   static variables. */
static AstCmpFrameVtab class_vtab; /* Virtual function table */
static int class_init = 0;       /* Virtual function table initialised? */

/* Pointers to parent class methods which are extended by this class. */
static const char *(* parent_getdomain)( AstFrame * );
static const char *(* parent_gettitle)( AstFrame * );

/* Pointer to axis index array accessed by "qsort". */
static int *qsort_axes;

/* Define other static variables. */
static char label_buff[ LABEL_BUFF_LEN + 1 ]; /* Default Label string buffer */
static char symbol_buff[ SYMBOL_BUFF_LEN + 1 ]; /* Default Symbol buffer */

/* External Interface Function Prototypes. */
/* ======================================= */
/* The following functions have public prototypes only (i.e. no
   protected prototypes), so we must provide local prototypes for use
   within this module. */
AstCmpFrame *astCmpFrameId_( void *, void *, const char *, ... );

/* Prototypes for Private Member Functions. */
/* ======================================== */
static AstAxis *GetAxis( AstFrame *, int );
static const char *Abbrev( AstFrame *, int, const char *, const char * );
static const char *Format( AstFrame *, int, double );
static const char *GetDomain( AstFrame * );
static const char *GetFormat( AstFrame *, int );
static const char *GetLabel( AstFrame *, int );
static const char *GetSymbol( AstFrame *, int );
static const char *GetTitle( AstFrame * );
static const char *GetUnit( AstFrame *, int );
static const int *GetPerm( AstFrame * );
static double Distance( AstFrame *, const double[], const double[] );
static double Gap( AstFrame *, int, double, int * );
static int GenAxisSelection( int, int, int [] );
static int GetDirection( AstFrame *, int );
static int GetMaxAxes( AstFrame * );
static int GetMinAxes( AstFrame * );
static int GetNaxes( AstFrame * );
static int GoodPerm( int, const int [], int, const int [] );
static int Match( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
static int PartMatch( AstCmpFrame *, AstFrame *, int, const int [], int, const int [], int **, int **, AstMapping **, AstFrame ** );
static int QsortCmpAxes( const void *, const void * );
static int SubFrame( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
static int TestDirection( AstFrame *, int );
static int TestFormat( AstFrame *, int );
static int TestLabel( AstFrame *, int );
static int TestMaxAxes( AstFrame * );
static int TestMinAxes( AstFrame * );
static int TestSymbol( AstFrame *, int );
static int TestUnit( AstFrame *, int );
static int Unformat( AstFrame *, int, const char *, double * );
static void AddExtraAxes( int, int [], int, int, int );
static void ClearDirection( AstFrame *, int );
static void ClearFormat( AstFrame *, int );
static void ClearLabel( AstFrame *, int );
static void ClearMaxAxes( AstFrame * );
static void ClearMinAxes( AstFrame * );
static void ClearSymbol( AstFrame *, int );
static void ClearUnit( AstFrame *, int );
static void Copy( const AstObject *, AstObject * );
static void Delete( AstObject * );
static void Dump( AstObject *, AstChannel * );
static void InitVtab( AstCmpFrameVtab * );
static void Norm( AstFrame *, double [] );
static void Offset( AstFrame *, const double [], const double [], double, double [] );
static void PartitionSelection( int, const int [], const int [], int, int, int [], int );
static void PermAxes( AstFrame *, const int[] );
static void PrimaryFrame( AstFrame *, int, AstFrame **, int * );
static void RenumberAxes( int, int [] );
static void SetAxis( AstFrame *, int, AstAxis * );
static void SetDirection( AstFrame *, int, int );
static void SetFormat( AstFrame *, int, const char * );
static void SetLabel( AstFrame *, int, const char * );
static void SetMaxAxes( AstFrame *, int );
static void SetMinAxes( AstFrame *, int );
static void SetSymbol( AstFrame *, int, const char * );
static void SetUnit( AstFrame *, int, const char * );

/* Member functions. */
/* ================= */
static const char *Abbrev( AstFrame *this_frame, int axis,
                           const char *str1, const char *str2 ) {
/*
*  Name:
*     Abbrev

*  Purpose:
*     Abbreviate a formatted CmpFrame axis value by skipping leading fields.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     const char *Abbrev( AstFrame *this, int axis,
*                         const char *str1, const char *str2 )

*  Class Membership:
*     CmpFrame member function (over-rides the protected astAbbrev
*     method inherited from the Frame class).

*  Description:
*     This function compares two CmpFrame axis values that have been
*     formatted (using astFormat) and determines if they have any
*     redundant leading fields (i.e. leading fields in common which
*     can be suppressed when tabulating the values or plotting them on
*     the axis of a graph).

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The number of the CmpFrame axis for which the values have
*        been formatted (axis numbering starts at zero for the first
*        axis).
*     str1
*        Pointer to a constant null-terminated string containing the
*        first formatted value.
*     str1
*        Pointer to a constant null-terminated string containing the
*        second formatted value.

*  Returned Value:
*     A pointer into the "str2" string which locates the first
*     character in the first field that differs between the two
*     formatted values.
*
*     If the two values have no leading fields in common, the returned
*     value will point at the start of string "str2". If the two
*     values are equal, it will point at the terminating null at the
*     end of this string.

*  Notes:
*     - This function assumes that the format specification used was
*     the same when both values were formatted and that they both
*     apply to the same CmpFrame axis.
*     - A pointer to the start of "str2" will be returned if this
*     function is invoked with the global error status set, or if it
*     should fail for any reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   AstFrame *frame;              /* Pointer to Frame containing axis */
   const char *result;           /* Pointer value to return */
   int naxes1;                   /* Number of axes in frame1 */
   int set;                      /* Digits attribute set? */

/* Initialise. */
   result = str2;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astAbbrev" );

/* Determine the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which component Frame contains the axis and adjust the axis
   index if necessary. */
      frame = ( axis < naxes1 ) ? this->frame1 : this->frame2;
      axis = ( axis < naxes1 ) ? axis : axis - naxes1;

/* Since the component Frame is "managed" by the enclosing CmpFrame,
   we next test if any Frame attributes which may affect the result
   are undefined (i.e. have not been explicitly set). If so, we
   over-ride them, giving them temporary values dictated by the
   CmpFrame. Only the Digits attribute is relevant here. */
      set = astTestDigits( frame );
      if ( !set ) astSetDigits( frame, astGetDigits( this ) );

/* Invoke the Frame's astAbbrev method to perform the processing. */
      result = astAbbrev( frame, axis, str1, str2 );

/* Clear Frame attributes which were temporarily over-ridden. */
      if ( !set ) astClearDigits( frame );
   }

/* If an error occurred, clear the returned value. */
   if ( !astOK ) result = str2;

/* Return the result. */
   return result;
}

static void AddExtraAxes( int naxes, int axes[], int i1, int i2,
                          int following ) {
/*
*  Name:
*     AddExtraAxes

*  Purpose:
*     Add extra axis indices in place of missing ones.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void AddExtraAxes( int naxes, int axes[], int i1, int i2,
*                        int following )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function takes an array of axis indices that refer to the
*     axes of a Frame, and which may have values missing (denoted by
*     an index of -1). It replaces each occurrence of -1 by a new axis
*     index (and re-numbers the others to avoid duplication) in such a
*     way that the new indices introduced are "associated" with
*     certain of the pre-existing indices, by virtue of being numbered
*     consecutively with them.
*
*     The purpose of this operation is to establish the relative
*     location of new axes in relation to the pre-existing ones.
*
*     Normally, each new axis will be associated with (i.e. numbered
*     one more than) the pre-existing index which precedes
*     it. However, if the "following" parameter is non-zero, it will
*     instead be associated with (numbered one less than) the one
*     which follows it. If there is no preceding (or following)
*     pre-existing index, the following (or preceding) one is used
*     instead. If several adjacent occurrences of -1 must be replaced,
*     they are numbered consecutively in their order of occurrence.

*  Parameters:
*     naxes
*        The number of axis indices in the array.
*     axes
*        The array containing the axis indices.
*     i1
*        Index of the first element of the array to be processed.
*     i2
*        Index of the last element of the array to be processed.
*     following
*        Boolean flag to determine if new indices are associated with
*        the preceding index (if zero) or the following index (if
*        non-zero).

*  Notes:
*     - The values of "i1" and "i2" dictate the range of array
*     elements where values of -1 will be replaced, but all array
*     elements are candidates for renumbering in order to avoid
*     duplicate axis indices.
*     - This function aims to establish the location of new axes only
*     by means of the relative numerical value of the indices assigned
*     to them. It does not constrain the actual indices assigned in
*     any further way.
*     - Because axis indices are always incremented (never
*     decremented) in order to avoid duplicates, where a number of new
*     indices have been introduced, the maximum index present in the
*     result array may exceed the original maximum.
*     - Some axis indices may remain unused (i.e. not present) in the
*     result array.
*/

/* Local Variables: */
   int end;                      /* Loop termination value */
   int extra;                    /* Index to apply to next "extra" axis */
   int found;                    /* Default value found? */
   int i;                        /* Main loop counter */
   int inc;                      /* Loop increment value */
   int j;                        /* Loop counter for eliminating duplicates */
   int start;                    /* Loop starting value */

/* Check the global error status. */
   if ( !astOK ) return;

/* Initialise the default index of the next extra axis to add. This
   will apply only if there are no valid axis indices from which to
   obtain a better default. */
   extra = 0;

/* Initialise loop parameters so as to scan the axis indices provided
   in either the forward or reverse direction, according to the value
   of "following". Start with the section of the array being processed,
   but continue looking for a default right up to the end of the array
   (this prevents the current section being numbered inconsistently
   with respect to adjacent ones that may already have been
   processed). */
   start = following ? i2 : i1;
   end = following ? -1 : naxes;
   inc = following ? -1 : 1;

/* Search for the first (in whichever direction this is) valid axis
   index and use it to set a new default index for the next extra axis
   to add. If scanning forward, use the valid axis index (causing any
   preceding extra axis to displace it upwards). If scanning
   backwards, use one more than the valid axis index (causing any
   following extra axis to tag on the end). */
   found = 0;
   for ( i = start; i != end; i += inc ) {
      if ( axes[ i ] != -1 ) {
         found = 1;
         extra = axes[ i ] + ( following ? 1 : 0 );
         break;
      }
   }

/* If no default has yet been found, repeat the above process by
   scanning in the opposite direction (by inverting the "following"
   value used). Again, this prevents inconsistency with neighbouring
   regions. This time a default must be found unless the entire array
   is filled with -1's (in which case a default of zero is used). */
   if ( !found ) {
      start = !following ? i2 : i1;
      end = !following ? -1 : naxes;
      inc = !following ? -1 : 1;
      for ( i = start; i != end; i += inc ) {
         if ( axes[ i ] != -1 ) {
            extra = axes[ i ] + ( !following ? 1 : 0 );
            break;
         }
      }
   }

/* Reset the loop parameters to scan just the region of interest in
   the original (correct) direction. */
   start = following ? i2 : i1;
   end = following ? i1 - 1 : i2 + 1;
   inc = following ? -1 : 1;

/* Identify those indices which are not valid. */
   for ( i = start; i != end; i += inc ) {
      if ( axes[ i ] == -1 ) {

/* We wish to assign the value "extra" in place of this invalid axis
   index.  However, this may duplicate an index already present, so
   increment by one all valid indices which are not less than the new
   index. This eliminates any possibility duplication, although it may
   leave an axis index value unused (if no duplication would actually
   have occurred). */
         for ( j = 0; j < naxes; j++ ) {
            if ( axes[ j ] != -1 ) {
               if ( axes[ j ] >= extra ) axes[ j ]++;
            }
         }

/* We can now assign the new axis index. */
         axes[ i ] = extra;

/* Assign the value to be used for the next extra axis index. If
   scanning forward, this will be one more than the last one used (so
   it will follow it).  If scanning backwards, it is equal to the last
   one (so that it will displace the last one upwards). */
         extra += ( following ? 0 : 1 );

/* When a valid axis index is encountered, reset the value to be used
   for the next extra axis index. If scanning forward, this is one
   more than the last valid index (so the extra axis will follow
   it). If scanning backwards, it is equal to the last valid index (so
   it will displace the valid index upwards). */
      } else {
         extra = axes[ i ] + ( following ? 0 : 1 );
      }
   }
}

static void ClearMaxAxes( AstFrame *this_frame ) {
/*
*  Name:
*     ClearMaxAxes

*  Purpose:
*     Clear the value of the MaxAxes attribute for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void ClearMaxAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astClearMaxAxes method
*     inherited from the Frame class).

*  Description:
*     This function does nothing because the MaxAxes attribute for a
*     CmpFrame is determined by the Frames it contains and cannot be
*     altered independently.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     void.
*/

/* Do nothing. */
}

static void ClearMinAxes( AstFrame *this_frame ) {
/*
*  Name:
*     ClearMinAxes

*  Purpose:
*     Clear the value of the MinAxes attribute for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void ClearMinAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astClearMinAxes method
*     inherited from the Frame class).

*  Description:
*     This function does nothing because the MinAxes attribute for a
*     CmpFrame is determined by the Frames it contains and cannot be
*     altered independently.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     void.
*/

/* Do nothing. */
}

static double Distance( AstFrame *this_frame,
                        const double point1[], const double point2[] ) {
/*
*  Name:
*     Distance

*  Purpose:
*     Calculate the distance between two points.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     double Distance( AstFrame *this,
*                      const double point1[], const double point2[] )

*  Class Membership:
*     CmpFrame member function (over-rides the astDistance method
*     inherited from the Frame class).

*  Description:
*     This function finds the distance between two points whose
*     CmpFrame coordinates are given. The distance calculated is that
*     along the geodesic curve that joins the two points. This is
*     computed as the Cartesian sum of the distances between the
*     points when their coordinates are projected into each of the
*     CmpFrame's component Frames.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     point1
*        An array of double, with one element for each CmpFrame axis,
*        containing the coordinates of the first point.
*     point2
*        An array of double, with one element for each CmpFrame axis,
*        containing the coordinates of the second point.

*  Returned Value:
*     The distance between the two points.

*  Notes:
*     - This function will return a "bad" result value (AST__BAD) if
*     any of the input coordinates has this value.
*     - A "bad" value will also be returned if this function is
*     invoked with the AST error status set or if it should fail for
*     any reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   const int *perm;              /* Axis permutation array */
   double *p1;                   /* Pointer to permuted point1 coordinates */
   double *p2;                   /* Pointer to permuted point2 coordinates */
   double dist1;                 /* Distance in frame1 */
   double dist2;                 /* Distance in frame2 */
   double result;                /* Value to return */
   int axis;                     /* Loop counter for axes */
   int naxes1;                   /* Number of axes in frame1 */
   int naxes;                    /* Number of axes in CmpFrame */
   int ok;                       /* No "bad" coordinates found? */

/* Initialise. */
   result = AST__BAD;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain a pointer to the CmpFrame's axis permutation array. */
   perm = astGetPerm( this );

/* Obtain the number of axes in the CmpFrame and in the first
   component Frame. */
   naxes = astGetNaxes( this );
   naxes1 = astGetNaxes( this->frame1 );

/* Allocate memory to hold the permuted coordinates of each point. */
   p1 = astMalloc( sizeof( double ) * (size_t) naxes );
   p2 = astMalloc( sizeof( double ) * (size_t) naxes );
   if ( astOK ) {

/* Examine the coordinates of both points and note if any coordinate
   value is "bad". */
      ok = 1;
      for ( axis = 0; axis < naxes; axis++ ) {
         if ( ( point1[ axis ] == AST__BAD ) ||
              ( point2[ axis ] == AST__BAD ) ) {
            ok = 0;
            break;

/* Permute good coordinates using the CmpFrame's axis permutation
   array to put them into the order required internally (i.e. by the
   two component Frames). */
         } else {
            p1[ perm[ axis ] ] = point1[ axis ];
            p2[ perm[ axis ] ] = point2[ axis ];
         }
      }

/* If no "bad" coordinates were found, obtain the distance between the
   two points when their coordinates are projected into each component
   Frame. */
      if ( ok ) {
         dist1 = astDistance( this->frame1, p1, p2 );
         dist2 = astDistance( this->frame2, p1 + naxes1, p2 + naxes1 );

/* If the distances found were OK, compute the distance between the
   two points as the Cartesian sum of the two component distances. */
         if ( astOK && ( dist1 != AST__BAD ) && ( dist2 != AST__BAD ) ) {
            result = sqrt( ( dist1 * dist1 ) + ( dist2 * dist2 ) );
         }
      }
   }

/* Free the memory used for the permuted coordinates. */
   p1 = astFree( p1 );
   p2 = astFree( p2 );

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = AST__BAD;

/* Return the result. */
   return result;
}

static const char *Format( AstFrame *this_frame, int axis, double value ) {
/*
*  Name:
*     Format

*  Purpose:
*     Format a coordinate value for a CmpFrame axis.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     const char *Format( AstFrame *this, int axis, double value )

*  Class Membership:
*     CmpFrame member function (over-rides the astFormat method
*     inherited from the Frame class).

*  Description:
*     This function returns a pointer to a string containing the
*     formatted (character) version of a coordinate value for a
*     CmpFrame axis. The formatting applied is that specified by a
*     previous invocation of the astSetFormat method (or a default
*     format appropriate to the axis in question).

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The number of the axis (zero-based) for which formatting is
*        to be performed.
*     value
*        The coordinate value to be formatted.

*  Returned Value:
*     A pointer to a null-terminated string containing the formatted
*     value.

*  Notes:
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   AstFrame *frame;              /* Pointer to Frame containing axis */
   const char *result;           /* Pointer value to return */
   int naxes1;                   /* Number of axes in frame1 */
   int set;                      /* Digits attribute set? */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astFormat" );

/* Determine the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which component Frame contains the axis and adjust the axis
   index if necessary. */
      frame = ( axis < naxes1 ) ? this->frame1 : this->frame2;
      axis = ( axis < naxes1 ) ? axis : axis - naxes1;

/* Since the component Frame is "managed" by the enclosing CmpFrame,
   we next test if any Frame attributes which may affect the result
   are undefined (i.e. have not been explicitly set). If so, we
   over-ride them, giving them temporary values dictated by the
   CmpFrame. Only the Digits attribute is relevant here. */
      set = astTestDigits( frame );
      if ( !set ) astSetDigits( frame, astGetDigits( this ) );

/* Invoke the Frame's astFormat method to format the value. */
      result = astFormat( frame, axis, value );

/* Clear Frame attributes which were temporarily over-ridden. */
      if ( !set ) astClearDigits( frame );
   }

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = NULL;

/* Return the result. */
   return result;
}

static double Gap( AstFrame *this_frame, int axis, double gap, int *ntick ) {
/*
*  Name:
*     Gap

*  Purpose:
*     Find a "nice" gap for tabulating CmpFrame axis values.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     double Gap( AstFrame *this, int axis, double gap, int *ntick )

*  Class Membership:
*     CmpFrame member function (over-rides the protected astGap method
*     inherited from the Frame class).

*  Description:
*     This function returns a gap size which produces a nicely spaced
*     series of formatted values for a CmpFrame axis, the returned gap
*     size being as close as possible to the supplied target gap
*     size. It also returns a convenient number of divisions into
*     which the gap can be divided.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The number of the axis (zero-based) for which a gap is to be found.
*     gap
*        The target gap size.
*     ntick
*        Address of an int in which to return a convenient number of
*        divisions into which the gap can be divided.

*  Returned Value:
*     The nice gap size.

*  Notes:
*     - A value of zero is returned if the target gap size is zero.
*     - A negative gap size is returned if the supplied gap size is negative.
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   AstFrame *frame;              /* Pointer to Frame containing axis */
   double result;                /* Result value to return */
   int naxes1;                   /* Number of axes in frame1 */
   int set;                      /* Digits attribute set? */

/* Initialise. */
   result = 0.0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astGap" );

/* Determine the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which component Frame contains the axis and adjust the axis
   index if necessary. */
      frame = ( axis < naxes1 ) ? this->frame1 : this->frame2;
      axis = ( axis < naxes1 ) ? axis : axis - naxes1;

/* Since the component Frame is "managed" by the enclosing CmpFrame,
   we next test if any Frame attributes which may affect the result
   are undefined (i.e. have not been explicitly set). If so, we
   over-ride them, giving them temporary values dictated by the
   CmpFrame. Only the Digits attribute is relevant here. */
      set = astTestDigits( frame );
      if ( !set ) astSetDigits( frame, astGetDigits( this ) );

/* Invoke the Frame's astGap method to find the gap size. */
      result = astGap( frame, axis, gap, ntick );

/* Clear Frame attributes which were temporarily over-ridden. */
      if ( !set ) astClearDigits( frame );
   }

/* If an error occurred, clear the returned value. */
   if ( !astOK ) result = 0.0;

/* Return the result. */
   return result;
}

static int GenAxisSelection( int naxes, int nselect, int axes[] ) {
/*
*  Name:
*     GenAxisSelection

*  Purpose:
*     Generate a sequence of axis selections.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int GenAxisSelection( int naxes, int nselect, int axes[] )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function generates a sequence of axis selections covering
*     all possible ways of selecting a specified number of axes from a
*     Frame.

*  Parameters:
*     naxes
*        The number of axes in the Frame.
*     nselect
*        The number of axes to be selected (between zero and "naxes").
*     axes
*        An array with "nselect" elements. On entry it should contain
*        the (zero-based) indices of the initial set of distinct axes
*        to be selected, in increasing order (initiallly this should
*        just be the sequence [0,1,...nselect-1]). On exit, these
*        indices will be updated to identify the next possible axis
*        selection.
*
*        By invoking the function repeatedly, and passing this array
*        each time, all possible selections will be covered.

*  Returned Value:
*     One if a new axis selection has been returned. Zero if all
*     possible selections have already been returned (in which case
*     the selection returned this time is not valid and should not be
*     used).

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   int i;                        /* Loop counter for axes */
   int iselect;                  /* Selection index */

/* Check the global error status. */
   if ( !astOK ) return 0;

/* Start with the first axis index and loop until the selection has
   been updated. */
   iselect = 0;
   while ( 1 ) {

/* Increment the current axis index if it is the final one or it can
   be incremented without equalling the one which follows (this ensures
   the indices remain in increasing order). */
      if ( ( iselect == ( nselect - 1 ) ) ||
           ( axes[ iselect + 1 ] > ( axes[ iselect ] + 1 ) ) ) {
         axes[ iselect ]++;

/* After incrementing an index, reset all previous indices to their
   starting values. */
         for ( i = 0; i < iselect; i++ ) axes[ i ] = i;
         break;

/* If this axis index can't be incremented, consider the next one.
   Quit if we go beyond the end of the selection array. */
      } else if ( ++iselect >= nselect ) {
         break;
      }
   }   

/* Return a result to indicate if we've reached the final selection
   (when the final axis index goes out of range). */
   return ( nselect > 0 ) && ( axes[ nselect - 1 ] < naxes );
}

static AstAxis *GetAxis( AstFrame *this_frame, int axis ) {
/*
*  Name:
*     GetAxis

*  Purpose:
*     Obtain a pointer to a specified Axis from a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     AstAxis *GetAxis( AstFrame *this, int axis )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetAxis method
*     inherited from the Frame class).

*  Description:
*     This function returns a pointer to the Axis object associated
*     with one of the axes of a CmpFrame.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The number of the axis (zero-based) for which an Axis pointer
*        is required.

*  Returned Value:
*     A pointer to the requested Axis object.

*  Notes:
*     - The reference count of the requested Axis object will be
*     incremented by one to reflect the additional pointer returned by
*     this function.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Vaiables: */
   AstAxis *result;              /* Pointer value to return */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int naxes1;                   /* Number of axes for frame1 */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astGetAxis" );

/* Obtain the number of axes for frame1. */
   naxes1 = astGetNaxes( this->frame1 );

/* Decide which Frame the axis belongs to and obtain the required
   Axis pointer. */
   if ( axis < naxes1 ) {
      result = astGetAxis( this->frame1, axis );
   } else {
      result = astGetAxis( this->frame2, axis - naxes1 );
   }

/* Return the result. */
   return result;
}

static const char *GetDomain( AstFrame *this_frame ) {
/*
*  Name:
*     GetDomain

*  Purpose:
*     Obtain a pointer to the Domain attribute string for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     const char *GetDomain( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetDomain protected
*     method inherited from the Frame class).

*  Description:
*    This function returns a pointer to the Domain attribute string
*    for a CmpFrame.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     A pointer to a constant null-terminated string containing the
*     Domain value.

*  Notes:
*     - The returned pointer or the string it refers to may become
*     invalid following further invocation of this function or
*     modification of the CmpFrame.
*     - A NULL pointer is returned if this function is invoked with
*     the global error status set or if it should fail for any reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   const char *result;           /* Pointer value to return */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* If a Domain attribute string has been set, invoke the parent method
   to obtain a pointer to it. */
   if ( astTestDomain( this ) ) {
      result = (*parent_getdomain)( this_frame );

/* Otherwise, provide a pointer to a suitable default string. */
   } else {
      result = "CMP";
   }

/* Return the result. */
   return result;
}

static int GetMaxAxes( AstFrame *this_frame ) {
/*
*  Name:
*     GetMaxAxes

*  Purpose:
*     Get a value for the MaxAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int GetMaxAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetMaxAxes method
*     inherited from the Frame class).

*  Description:
*     This function returns a value for the MaxAxes attribute of a
*     CmpFrame.  This will be the sum of the MaxAxes attributes for
*     each of its component Frames.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     The MaxAxes attribute value.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int max_axes1;                /* MaxAxes attribute for frame1 */
   int max_axes2;                /* MaxAxes attribute for frame2 */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain the MaxAxes attribute value for each component Frame. */
   max_axes1 = astGetMaxAxes( this->frame1 );
   max_axes2 = astGetMaxAxes( this->frame2 );

/* If OK, sum these values. */
   if ( astOK ) result = max_axes1 + max_axes2;

/* Return the result. */
   return result;
}

static int GetMinAxes( AstFrame *this_frame ) {
/*
*  Name:
*     GetMinAxes

*  Purpose:
*     Get a value for the MinAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int GetMinAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetMinAxes method
*     inherited from the Frame class).

*  Description:
*     This function returns a value for the MinAxes attribute of a
*     CmpFrame.  This will be the sum of the MinAxes attributes for
*     each of its component Frames.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     The MinAxes attribute value.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int min_axes1;                /* MinAxes attribute for frame1 */
   int min_axes2;                /* MinAxes attribute for frame2 */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain the MinAxes attribute value for each component Frame. */
   min_axes1 = astGetMinAxes( this->frame1 );
   min_axes2 = astGetMinAxes( this->frame2 );

/* If OK, sum these values. */
   if ( astOK ) result = min_axes1 + min_axes2;

/* Return the result. */
   return result;
}

static int GetNaxes( AstFrame *this_frame ) {
/*
*  Name:
*     GetNaxes

*  Purpose:
*     Determine how many axes a CmpFrame has.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int GetNaxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetNaxes method
*     inherited from the Frame class).

*  Description:
*     This function returns the number of axes for a CmpFrame.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     The number of CmpFrame axes.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int naxes1;                   /* Number of axes for frame1 */
   int naxes2;                   /* Number of axes for frame2 */
   int result;                   /* Number of CmpFrame axes */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain the number of axes for each component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   naxes2 = astGetNaxes( this->frame2 );

/* If OK, calculate the total number of axes. */
   if ( astOK ) result = naxes1 + naxes2;

/* Return the result. */
   return result;
}

static const int *GetPerm( AstFrame *this_frame ) {
/*
*  Name:
*     GetPerm

*  Purpose:
*     Access the axis permutation array for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     const int *astGetPerm( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the protected astGetPerm
*     method inherited from the Frame class).

*  Description:
*     This function returns a pointer to the axis permutation array
*     for a CmpFrame. This array constitutes a lookup-table that
*     converts between an axis number supplied externally and the
*     corresponding index in the CmpFrame's internal data.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     Pointer to the CmpFrame's axis permutation array (a constant
*     array of int).  Each element of this contains the (zero-based)
*     internal axis index to be used in place of the external index
*     which is used to address the permutation array. If the CmpFrame
*     has zero axes, this pointer will be NULL.

*  Notes:
*     - This protected method is provided to assist class
*     implementations which need to implement axis-dependent
*     extensions to CmpFrame methods, and which therefore need to know
*     how a CmpFrames's external axis index is converted for internal
*     use.
*     - The pointer returned by this function gives direct access to
*     data internal to the CmpFrame object. It remains valid only so
*     long as the CmpFrame exists. The permutation array contents may
*     be modified by other functions which operate on the CmpFrame and
*     this may render the returned pointer invalid.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.

*  Implementation Notes:
*     - This function performs essentially the same operation as the
*     Frame member function which it over-rides. However, it returns a
*     pointer to the "perm" array held in the CmpFrame structure
*     (rather than the one in the parent Frame structure). This
*     duplication of the array is necessary because the one in the
*     Frame structure is of zero length, the number of axes in the
*     Frame structure having been set to zero to prevent unnecessary
*     allocation of Axis objects which are not needed by the CmpFrame.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Return a pointer to the axis permutation array. */
   return this->perm;
}

static const char *GetTitle( AstFrame *this_frame ) {
/*
*  Name:
*     GetTitle

*  Purpose:
*     Obtain a pointer to the Title attribute string for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     const char *GetTitle( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astGetTitle protected
*     method inherited from the Frame class).

*  Description:
*    This function returns a pointer to the Title attribute string for
*    a CmpFrame.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     A pointer to a constant null-terminated string containing the
*     Title value.

*  Notes:
*     - The returned pointer or the string it refers to may become
*     invalid following further invocation of this function or
*     modification of the CmpFrame.
*     - A NULL pointer is returned if this function is invoked with
*     the global error status set or if it should fail for any reason.
*/

/* Local Constants: */
#define BUFF_LEN 100             /* Length of formatting buffer */

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   const char *result;           /* Pointer value to return */
   static char buff[ BUFF_LEN + 1 ]; /* Formatting buffer */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* If a Title attribute string has been set, invoke the parent method
   to obtain a pointer to it. */
   if ( astTestTitle( this ) ) {
      result = (*parent_gettitle)( this_frame );

/* Otherwise, create a suitable default string and return a pointer to
   this. */
   } else {
      (void) sprintf( buff, "%d-d compound coordinate system",
                      astGetNaxes( this ) );
      if ( astOK ) result = buff;
   }

/* Return the result. */
   return result;

/* Undefine macros local to this function. */
#undef BUFF_LEN
}

static int GoodPerm( int ncoord_in, const int inperm[],
                     int ncoord_out, const int outperm[] ) {
/*
*  Name:
*     GoodPerm

*  Purpose:
*     Test if a PermMap will be non-null.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int GoodPerm( int ncoord_in, const int inperm[],
*                   int ncoord_out, const int outperm[] )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function tests if a pair of permutation arrays will, when
*     used to create a PermMap, result in a PermMap which has a
*     non-null effect (i.e. one which is not simply equivalent to a
*     unit Mapping).

*  Parameters:
*     ncoord_in
*        The number of input coordinates for the PermMap.
*     inperm
*        The input permutation array for the PermMap (with "ncoord_in"
*        elements).
*     ncoord_out
*        The number of output coordinates for the PermMap.
*     outperm
*        The output permutation array for the PermMap (with
*        "ncoord_out" elements).

*  Returned Value:
*     Zero if the PermMap would be equivalent to a unit Mapping,
*     otherwise one.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   int axis;                     /* Loop counter for axes */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* First test if the number of input and output coordinates are
   different. */
   result = ( ncoord_in != ncoord_out );

/* If they are not, examine the contents of the "inperm" array. */
   if ( !result ) {
      for ( axis = 0; axis < ncoord_in; axis++ ) {

/* We have a non-null Mapping if any element of this array selects an
   output axis with a different index to the input axis (or selects an
   invalid axis or a constant). */
         if ( inperm[ axis ] != axis ) {
            result = 1;
            break;
         }
      }
   }

/* If the Mapping still appears to be null, also examine the "outperm"
   array in the same way. */
   if ( !result ) {
      for ( axis = 0; axis < ncoord_out; axis++ ) {
         if ( outperm[ axis ] != axis ) {
            result = 1;
            break;
         }
      }
   }

/* Return the result. */
   return result;
}

static void InitVtab( AstCmpFrameVtab *vtab ) {
/*
*  Name:
*     InitVtab

*  Purpose:
*     Initialise a virtual function table for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void InitVtab( AstCmpFrameVtab *vtab )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function initialises the component of a virtual function
*     table which is used by the CmpFrame class.

*  Parameters:
*     vtab
*        Pointer to the virtual function table. The components used by
*        all ancestral classes should already have been initialised.
*/

/* Local Variables: */
   AstFrameVtab *frame;          /* Pointer to Frame component of Vtab */

/* Check the local error status. */
   if ( !astOK ) return;

/* Store a unique "magic" value in the virtual function table. This
   will be used (by astIsACmpFrame) to determine if an object belongs
   to this class.  We can conveniently use the address of the (static)
   class_init variable to generate this unique value. */
   vtab->check = &class_init;

/* Initialise member function pointers. */
/* ------------------------------------ */
/* Store pointers to the member functions (implemented here) that
   provide virtual methods for this class. */

/* Save the inherited pointers to methods that will be extended, and
   replace them with pointers to the new member functions. */
   frame = (AstFrameVtab *) vtab;

   parent_getdomain = frame->GetDomain;
   frame->GetDomain = GetDomain;
   parent_gettitle = frame->GetTitle;
   frame->GetTitle = GetTitle;

/* Store replacement pointers for methods which will be over-ridden by
   new member functions implemented here. */
   frame->Abbrev = Abbrev;
   frame->ClearDirection = ClearDirection;
   frame->ClearFormat = ClearFormat;
   frame->ClearLabel = ClearLabel;
   frame->ClearMaxAxes = ClearMaxAxes;
   frame->ClearMinAxes = ClearMinAxes;
   frame->ClearSymbol = ClearSymbol;
   frame->ClearUnit = ClearUnit;
   frame->Distance = Distance;
   frame->Format = Format;
   frame->Gap = Gap;
   frame->GetAxis = GetAxis;
   frame->GetDirection = GetDirection;
   frame->GetFormat = GetFormat;
   frame->GetLabel = GetLabel;
   frame->GetMaxAxes = GetMaxAxes;
   frame->GetMinAxes = GetMinAxes;
   frame->GetNaxes = GetNaxes;
   frame->GetPerm = GetPerm;
   frame->GetSymbol = GetSymbol;
   frame->GetUnit = GetUnit;
   frame->Match = Match;
   frame->Norm = Norm;
   frame->Offset = Offset;
   frame->PermAxes = PermAxes;
   frame->PrimaryFrame = PrimaryFrame;
   frame->SetAxis = SetAxis;
   frame->SetDirection = SetDirection;
   frame->SetFormat = SetFormat;
   frame->SetLabel = SetLabel;
   frame->SetMaxAxes = SetMaxAxes;
   frame->SetMinAxes = SetMinAxes;
   frame->SetSymbol = SetSymbol;
   frame->SetUnit = SetUnit;
   frame->SubFrame = SubFrame;
   frame->TestDirection = TestDirection;
   frame->TestFormat = TestFormat;
   frame->TestLabel = TestLabel;
   frame->TestMaxAxes = TestMaxAxes;
   frame->TestMinAxes = TestMinAxes;
   frame->TestSymbol = TestSymbol;
   frame->TestUnit = TestUnit;
   frame->Unformat = Unformat;

/* Declare the copy constructor, destructor and class dump
   function. */
   astSetCopy( vtab, Copy );
   astSetDelete( vtab, Delete );
   astSetDump( vtab, Dump, "CmpFrame",
               "Compound coordinate system description" );
}

static int Match( AstFrame *template_frame, AstFrame *target,
                  int **template_axes, int **target_axes,
                  AstMapping **map, AstFrame **result ) {
/*
*  Name:
*     Match

*  Purpose:
*     Determine if conversion is possible between two coordinate systems.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int Match( AstFrame *template, AstFrame *target,
*                int **template_axes, int **target_axes,
*                AstMapping **map, AstFrame **result )

*  Class Membership:
*     CmpFrame member function (over-rides the protected astMatch
*     method inherited from the Frame class).

*  Description:
*     This function matches a "template" CmpFrame to a "target" Frame
*     and determines whether it is possible to convert coordinates
*     between them.  If it is, a Mapping that performs the
*     transformation is returned along with a new Frame that describes
*     the coordinate system that results when this Mapping is applied
*     to the "target" coordinate system. In addition, information is
*     returned to allow the axes in this "result" Frame to be
*     associated with the corresponding axes in the "target" Frame and
*     "template" CmpFrame from which they are derived.

*  Parameters:
*     template
*        Pointer to the template CmpFrame. This describes the
*        coordinate system (or set of possible coordinate systems)
*        into which we wish to convert our coordinates.
*     target
*        Pointer to the target Frame. This describes the coordinate
*        system in which we already have coordinates.
*     template_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        template CmpFrame axis from which it is derived. If it is not
*        derived from any template axis, a value of -1 will be
*        returned instead.
*     target_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        target Frame axis from which it is derived. If it is not
*        derived from any target axis, a value of -1 will be returned
*        instead.
*     map
*        Address of a location where a pointer to a new Mapping will
*        be returned if the requested coordinate conversion is
*        possible. If returned, the forward transformation of this
*        Mapping may be used to convert coordinates between the
*        "target" Frame and the "result" Frame (see below) and the
*        inverse transformation will convert in the opposite
*        direction.
*     result
*        Address of a location where a pointer to a new Frame will be
*        returned if the requested coordinate conversion is
*        possible. If returned, this Frame describes the coordinate
*        system that results from applying the returned Mapping
*        (above) to the "target" coordinate system. In general, this
*        Frame will combine attributes from (and will therefore be
*        more specific than) both the target Frame and the template
*        CmpFrame. In particular, when the template allows the
*        possibility of transformaing to any one of a set of
*        alternative coordinate systems, the "result" Frame will
*        indicate which of the alternatives was used.

*  Returned Value:
*     A non-zero value is returned if the requested coordinate
*     conversion is possible. Otherwise zero is returned (this will
*     not in itself result in an error condition).

*  Notes:
*     - By default, the "result" Frame will have its number of axes
*     and axis order determined by the "template" CmpFrame. However,
*     if the PreserveAxes attribute of the template CmpFrame is
*     non-zero, then the axis count and axis order of the "target"
*     Frame will be used instead.
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *template;        /* Pointer to template CmpFrame structure */
   char *template_domain;        /* Pointer to copy of template domain */
   const char *ptr;              /* Pointer to domain string */
   const char *target_domain;    /* Pointer to target domain string */
   int *axes1;                   /* Pointer to axis selection 1 */
   int *axes2;                   /* Pointer to axis selection 2 */
   int *used;                    /* Pointer to flags array */
   int axis2;                    /* Index for axis selection 2 */
   int axis;                     /* Index for axis arrays */
   int last_target;              /* Last target axis association */
   int last_template;            /* Last template axis associateion */
   int match;                    /* Match obtained (returned result)? */
   int maxax1;                   /* MaxAxes attribute for component 1 */
   int maxax2;                   /* MaxAxes attribute for component 2 */
   int minax1;                   /* MinAxes attribute for component 1 */
   int minax2;                   /* MinAxes attribute for component 2 */
   int naxes1;                   /* Number of axes assigned to component 1 */
   int naxes2;                   /* Number of axes assigned to component 2 */
   int naxes;                    /* Total number of target axes */
   int naxes_max1;               /* First estimate of naxes_max */
   int naxes_max2;               /* Second estimate of naxes_max */
   int naxes_max;                /* Max number of axes to match component 1 */
   int naxes_min1;               /* First estimate of naxes_min */
   int naxes_min2;               /* Second estimate of naxes_min */
   int naxes_min;                /* Min number of axes to match component 1 */
   int permute;                  /* Permute attribute for template */
   int result_naxes;             /* Number of result Frame axes */

/* Initialise the returned values. */
   *template_axes = NULL;
   *target_axes = NULL;
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;
   
/* Obtain a pointer to the template CmpFrame structure. */
   template = (AstCmpFrame *) template_frame;

/* Obtain the minimum and maximum number of axes that each component
   Frame of the template CmpFrame can match. */
   minax1 = astGetMinAxes( template->frame1 );
   minax2 = astGetMinAxes( template->frame2 );
   maxax1 = astGetMaxAxes( template->frame1 );
   maxax2 = astGetMaxAxes( template->frame2 );

/* Obtain the number of axes in the target Frame and test to see if it
   is possible for the template to match it on the basis of axis
   counts. */
   naxes = astGetNaxes( target );
   match = ( naxes >= ( minax1 + minax2 ) && naxes <= ( maxax1 + maxax2 ) );

/* The next requirement is that if the template CmpFrame has its
   Domain attribute defined, then the target Frame must also have the
   same Domain (although it need not be set - the default will
   do). First check if the template has a domain. */
   if ( astOK && match ) {
      if ( astTestDomain( template ) ) {

/* Obtain a pointer to the template domain. Then allocate memory and
   make a copy of it (this is necessary as we will next inquire the
   domain of the target and may over-write the buffer holding the
   template's domain). */
         ptr = astGetDomain( template );
         if ( astOK ) {
            template_domain = astStore( NULL, ptr,
                                        strlen( ptr ) + (size_t) 1 );

/* Obtain a pointer to the target domain. */
            target_domain = astGetDomain( target );
            
/* Compare the domain strings for equality. Then free the memory
   allocated above. */
            match = astOK && !strcmp( template_domain, target_domain );
            template_domain = astFree( template_domain );
         }
      }
   }

/* If a match still appears possible, determine the minimum number of
   target axes that will have to match the first component Frame of
   the template CmpFrame. */
   if ( astOK && match ) {
      naxes_min1 = minax1;
      naxes_min2 = naxes - maxax2;
      naxes_min = ( naxes_min1 > naxes_min2 ) ? naxes_min1 : naxes_min2;

/* Also determine the maximum number of target axes that may match
   this component of the template. */
      naxes_max1 = maxax1;
      naxes_max2 = naxes - minax2;
      naxes_max = ( naxes_max1 < naxes_max2 ) ? naxes_max1 : naxes_max2;

/* Allocate workspace. */
      axes1 = astMalloc( sizeof( int ) * (size_t) naxes );
      axes2 = astMalloc( sizeof( int ) * (size_t) naxes );
      used = astMalloc( sizeof( int ) * (size_t) naxes );

/* Obtain the value of the template's Permute attribute. */
      permute = astGetPermute( template );
      if ( astOK ) {

/* Loop to consider all possible choices of the number of template
   axes that might match the first component Frame of the template,
   and derive the corresponding number of axes that must match the
   second component at the same time. */
         for ( naxes1 = naxes_max; naxes1 >= naxes_min; naxes1-- ) {
            naxes2 = naxes - naxes1;

/* Initialise the selection of target axes that we will attempt to
   match against the first template component (to [0,1,2,...]). */
            for ( axis = 0; axis < naxes1; axis++ ) axes1[ axis ] = axis;

/* Loop to consider all possible selections with this number of axes,
   until a match is found. */
            while ( 1 ) {

/* Initialise an array of flags to zero for each target axis. Then set
   the flag to 1 for each axis which is in the first selection.*/
               for ( axis = 0; axis < naxes; axis++ ) used[ axis ] = 0;
               for( axis = 0; axis < naxes1; axis++ ) {
                  used[ axes1[ axis ] ] = 1;
               }

/* Generate the second selection by including all target axes that are
   not in the first selection. */
               axis2 = 0;
               for ( axis = 0; axis < naxes; axis++ ) {
                  if ( !used[ axis ] ) axes2[ axis2++ ] = axis;
               }

/* Attempt to match the target axes partitioned in this way to the two
   template components. */
               match = PartMatch( template, target,
                                  naxes1, axes1, naxes2, axes2,
                                  template_axes, target_axes, map, result );

/* If a match was obtained but the template's Permute attribute is zero,
   then we must check to see if the match involves permuting the target
   axes. */
               if ( astOK && match && !permute ) {

/* Obtain the number of result Frame axes. */
                  result_naxes = astGetNaxes( *result );

/* Loop to check the target and template axis associations for all the
   result Frame axes. The match will only be accepted if both of these
   are monotonically increasing (indicating no axis permutation) after
   allowing for any absent associations . */
                  last_template = -1;
                  last_target = -1;
                  for ( axis = 0; axis < result_naxes; axis++ ) {

/* Check the template axis association against the previous value,
   omitting any axes witout valid associations. */
                     if ( ( *template_axes )[ axis ] != -1 ) {
                        if ( ( *template_axes )[ axis ] <= last_template ) {
                           match = 0;
                           break;

/* Update the previous association value. */
                        } else {
                           last_template = ( *template_axes )[ axis ];
                        }
                     }

/* Repeat this process for the target axis associations. */
                     if ( ( *target_axes )[ axis ] != -1 ) {
                        if ( ( *target_axes )[ axis ] <= last_target ) {
                           match = 0;
                           break;
                        } else {
                           last_target = ( *target_axes )[ axis ];
                        }
                     }
                  }

/* If the match was rejected because it involves an axis permutation,
   then free the allocated memory and annul the Object pointers
   associated with the match. */
                  if ( !match ) {
                     *template_axes = astFree( *template_axes );
                     *target_axes = astFree( *target_axes );
                     *map = astAnnul( *map );
                     *result = astAnnul( *result );
                  }
               }

/* If an error occurred or a match was found, quit searching,
   otherwise generate the next axis selection and try that
   instead. Quit if there are no more selections to try. */
               if ( !astOK || match || 
                    !GenAxisSelection( naxes, naxes1, axes1 ) ) break;
            }

/* Quit the outer loop if an error occurs or a match is found. */
            if ( !astOK || match ) break;
         }
      }

/* Free the workspace arrays. */
      axes1 = astFree( axes1 );
      axes2 = astFree( axes2 );
      used = astFree( used );
   }

/* If an error occurred, free all allocated memory, annul the result
   Object pointers and clear all returned values. */
   if ( !astOK ) {
      *template_axes = astFree( *template_axes );
      *target_axes = astFree( *target_axes );
      *map = astAnnul( *map );
      *result = astAnnul( *result );
      match = 0;
   }

/* Return the result. */
   return match;
}

static void Norm( AstFrame *this_frame, double value[] ) {
/*
*  Name:
*     Norm

*  Purpose:
*     Normalise a set of CmpFrame coordinates.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void Norm( AstAxis *this, double value[] )

*  Class Membership:
*     CmpFrame member function (over-rides the astNorm method
*     inherited from the Frame class).

*  Description:
*     This function converts a set of CmpFrame coordinate values,
*     which might potentially be unsuitable for display to a user (for
*     instance, may lie outside the expected range of values) into a
*     set of acceptable alternative values suitable for display.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     value
*        An array of double, with one element for each CmpFrame axis.
*        This should contain the initial set of coordinate values,
*        which will be modified in place.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   const int *perm;              /* Axis permutation array */
   double *v;                    /* Pointer to permuted coordinates */
   int axis;                     /* Loop counter for axes */
   int naxes1;                   /* Number of axes in frame1 */
   int naxes;                    /* Number of axes in CmpFrame */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain a pointer to the CmpFrame's axis permutation array. */
   perm = astGetPerm( this );

/* Obtain the number of axes in the CmpFrame and in the first
   component Frame. */
   naxes = astGetNaxes( this );
   naxes1 = astGetNaxes( this->frame1 );

/* Allocate memory to hold the permuted coordinates. */
   v = astMalloc( sizeof( double ) * (size_t) naxes );
   if ( astOK ) {

/* Permute the coordinates using the CmpFrame's axis permutation array
   to put them into the order required internally (i.e. by the two
   component Frames). */
      for ( axis = 0; axis < naxes; axis++ ) v[ perm[ axis ] ] = value[ axis ];

/* Invoke the astNorm method of both component Frames, passing the
   relevant (permuted) coordinate values for normalisation. */
      astNorm( this->frame1, v );
      astNorm( this->frame2, v + naxes1 );

/* Copy the normalised values back into the original coordinate array,
   un-permuting them in the process. */
      for ( axis = 0; axis < naxes; axis++ ) value[ axis ] = v[ perm[ axis ] ];
   }

/* Free the memory used for the permuted coordinates. */
   v = astFree( v );
}

static void Offset( AstFrame *this_frame, const double point1[],
                    const double point2[], double offset, double point3[] ) {
/*
*  Name:
*     Offset

*  Purpose:
*     Calculate an offset along a geodesic curve.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void Offset( AstFrame *this,
*                  const double point1[], const double point2[],
*                  double offset, double point3[] )

*  Class Membership:
*     CmpFrame member function (over-rides the astOffset method
*     inherited from the Frame class).

*  Description:
*     This function finds the CmpFrame coordinate values of a point
*     which is offset a specified distance along the geodesic curve
*     between two other points.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     point1
*        An array of double, with one element for each CmpFrame axis.
*        This should contain the coordinates of the point marking the
*        start of the geodesic curve.
*     point2
*        An array of double, with one element for each CmpFrame axis.
*        This should contain the coordinates of the point marking the
*        end of the geodesic curve.
*     offset
*        The required offset from the first point along the geodesic
*        curve. If this is positive, it will be towards the second
*        point. If it is negative, it will be in the opposite
*        direction. This offset need not imply a position lying
*        between the two points given, as the curve will be
*        extrapolated if necessary.
*     point3
*        An array of double, with one element for each CmpFrame axis
*        in which the coordinates of the required point will be
*        returned.

*  Notes:
*     - The geodesic curve used by this function is the path of
*     shortest distance between two points, as defined by the
*     astDistance function.
*     - This function will return "bad" coordinate values (AST__BAD)
*     if any of the input coordinates has this value.
*     - "Bad" coordinate values will also be returned if the two
*     points supplied are coincident (or otherwise fail to uniquely
*     specify a geodesic curve) but the requested offset is non-zero.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   const int *perm;              /* Pointer to axis permutation array */
   double *p1;                   /* Permuted coordinates for point1 */
   double *p2;                   /* Permuted coordinates for point2 */
   double *p3;                   /* Permuted coordinates for point3 */
   double dist1;                 /* Distance between input points in frame1 */
   double dist2;                 /* Distance between input points in frame2 */
   double dist;                  /* Total distance between input points */
   double offset1;               /* Offset distance required in frame1 */
   double offset2;               /* Offset distance required in frame2 */
   int axis;                     /* Loop counter for axes */
   int bad;                      /* Set bad output coordinates? */
   int naxes1;                   /* Number of axes in frame1 */
   int naxes;                    /* Total number of axes in CmpFrame */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Obtain the number of axes in the CmpFrame. */
   naxes = astGetNaxes( this );

/* Obtain a pointer to the CmpFrame's axis permutation array. */
   perm = astGetPerm( this );

/* Allocate workspace. */
   p1 = astMalloc( sizeof( double ) * (size_t) naxes );
   p2 = astMalloc( sizeof( double ) * (size_t) naxes );
   p3 = astMalloc( sizeof( double ) * (size_t) naxes );
   
/* Initialise a flag to indicate whether "bad" coordinates should be
   returned. */
   bad = 0;

/* Check that all the coordinates of both input points are OK. If not,
   set the "bad" flag and quit checking. */
   if ( astOK ) {
      for ( axis = 0; axis < naxes; axis++ ) {
         if ( ( point1[ axis ] == AST__BAD ) ||
              ( point2[ axis ] == AST__BAD ) ) {
            bad = 1;
            break;

/* If the coordinates are OK, apply the axis permutation array to
   obtain them in the required order. */
         } else {
            p1[ perm[ axis ] ] = point1[ axis ];
            p2[ perm[ axis ] ] = point2[ axis ];
         }
      }
   }

/* If OK, obtain the number of axes in the first component Frame. */
   if ( astOK && !bad ) {
      naxes1 = astGetNaxes( this->frame1 );

/* Project the two input points into the two component Frames and
   determine the distance between the points in each Frame. */
      dist1 = astDistance( this->frame1, p1, p2 );
      dist2 = astDistance( this->frame2, p1 + naxes1, p2 + naxes1 );

/* Check that the returned distances are not bad. */
      if ( astOK ) bad = ( ( dist1 == AST__BAD ) || ( dist2 == AST__BAD ) );
   }

/* If OK, calculate the total distance between the two points. */
   if ( astOK && !bad ) {
      dist = sqrt( dist1 * dist1 + dist2 * dist2 );

/* If the points are co-incident, but "offset" is non-zero, then set
   the "bad" flag. */
      if ( dist == 0.0 ) {
         if ( offset != 0.0 ) {
            bad = 1;

/* Otherwise, set the offset distance required in each Frame to
   zero. */
         } else {
            offset1 = 0.0;
            offset2 = 0.0;
         }

/* If the points are not co-incident, divide the total offset required
   between each component Frame in such a way that the path being
   followed will pass through the second point. */
      } else {
         offset1 = offset * dist1 / dist;
         offset2 = offset * dist2 / dist;
      }
   }

/* If OK, apply the separate offsets to each component Frame. */
   if ( astOK && !bad ) {
      astOffset( this->frame1, p1, p2, offset1, p3 );
      astOffset( this->frame2, p1 + naxes1, p2 + naxes1, offset2,
                 p3 + naxes1 );

/* Copy the resulting coordinates into the output array "point3",
   permuting them back into the required order. */
      if ( astOK ) {
         for ( axis = 0; axis < naxes; axis++ ) {
            point3[ axis ] = p3[ perm[ axis ] ];

/* If any of the result coordinates is bad, set the "bad" flag and
   quit copying. */
            if ( point3[ axis ] == AST__BAD ) {
               bad = 1;
               break;
            }
         }
      }
   }

/* Free the workspace arrays. */
   p1 = astFree( p1 );
   p2 = astFree( p2 );
   p3 = astFree( p3 );

/* If no error has occurred, but bad coordinates must be returned,
   then set these in the output array. */
   if ( astOK && bad ) {
      for ( axis = 0; axis < naxes; axis++ ) point3[ axis ] = AST__BAD;
   }
}

static void PartitionSelection( int nselect, const int select[],
                                const int perm[], int naxes1, int naxes2,
                                int iframe[], int following ) {
/*
*  Name:
*     PartitionSelection

*  Purpose:
*     Partition a CmpFrame axis selection into two component Frame selections.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void PartitionSelection( int nselect, const int select[],
*                              const int perm[], int naxes1, int naxes2,
*                              int iframe[], int following )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function accepts an array containing the indices of axes
*     which are to be selected from a CmpFrame, and partitions these
*     indices to indicate which must be selected from each of the
*     CmpFrame's two component Frames.
*
*     This operation is trivial if all the axis indices supplied refer
*     to valid CmpFrame axes. However, if some of them do not (these
*     should generally be set to -1), this function assigns these
*     "extra" axes to one or other of the component Frames by
*     associating them with the axes selected immediately before (or
*     after). Allowance is made for the possibility that several
*     consecutive selected axes may be "extra" ones, or even that they
*     may all be. The CmpFrame's axis permutation array is also taken
*     into account.

*  Parameters:
*     nselect
*        The number of axes to be selected.
*     select
*        An array containing the (zero-based) indices of the CmpFrame
*        axes to be selected, or -1 where "extra" axes are required.
*     perm
*        The CmpFrame's axis permutation array.
*     naxes1
*        The number of axes in the CmpFrame's first component Frame.
*     naxes2
*        The number of axes in the CmpFrame's second component Frame.
*     iframe
*        An array with "nselect" elements in which to return a number
*        (either 1 or 2) to indicate to which component Frame (frame1
*        or frame2) each selected axis belongs.
*     following
*        If this is zero, "extra" axes will be associated with the
*        preceding normal selected axis which appears in the "select"
*        array (if any), otherwise they will be associated with the
*        following normal selected axis.
*/

/* Local Variables: */
   int end;                      /* Loop termination value */
   int ifr;                      /* Choice of Frame for next "extra" axis */
   int inc;                      /* Loop increment value */
   int iselect;                  /* Loop counter for axis selections */
   int naxes;                    /* Total number of CmpFrame axes */
   int start;                    /* Loop starting value */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain the total number of CmpFrame axes. */
   naxes = naxes1 + naxes2;

/* Loop through each axis selection and identify those which refer to
   valid CmpFrame axes. */
   for ( iselect = 0; iselect < nselect; iselect++ ) {
      if ( ( select[ iselect ] >= 0 ) && ( select[ iselect ] < naxes ) ) {

/* For these selections (only), enter a flag into the "iframe" array
   which indicates which component Frame the selected axis resides
   in. Permute each axis index before deciding this. */
         iframe[ iselect ] = 1 + ( perm[ select[ iselect ] ] >= naxes1 );
      }
   }

/* Set up a start, end, and increment value for looping through the
   array of axis selections forward (if "following" is 0) or backwards
   (otherwise). */
   start = following ? nselect - 1 : 0;
   end = following ? -1 : nselect;
   inc = following ? -1 : 1;

/* Set the default choice of component Frame. This will be used if
   there are no normal axis selections to guide the choice at all. */
   ifr = following ? 2 : 1;

/* Search for the first normal axis selection so that we can replace
   this default, if possible. (Here, "first" actually means "last" if
   "following" is set, because we will then be scanning the array of
   selections in reverse.) */
   for ( iselect = start; iselect != end; iselect += inc ) {

/* Identify normal axis selections and obtain the choice of component
   Frame for the first one found. The resulting value "ifr" will be
   used for initial (or final, if "following" is set) "extra"
   selections for which no earlier normal selection exists - see
   below. */
      if ( ( select[ iselect ] >= 0 ) && ( select[ iselect ] < naxes ) ) {
         ifr = iframe[ iselect ];
         break;
      }
   }

/* Loop through the selections again to allocate a choice of Frame to
   the "extra" selected axes. */
   for ( iselect = start; iselect != end; iselect += inc ) {

/* Remember the component Frame used by the most recently encountered
   normal axis selection. */
      if ( ( select[ iselect ] >= 0 ) && ( select[ iselect ] < naxes ) ) {
         ifr = iframe[ iselect ];

/* For "extra" axes, allocate the most recent Frame choice. The
   default choice (found above) will apply if no "most recent" choice
   has been encountered. */
      } else {
         iframe[ iselect ] = ifr;
      }
   }
}

static int PartMatch( AstCmpFrame *template, AstFrame *target,
                      int naxes1, const int axes1[],
                      int naxes2, const int axes2[],
                      int **template_axes, int **target_axes,
                      AstMapping **map, AstFrame **result ) {
/*
*  Name:
*     PartMatch

*  Purpose:
*     Match a CmpFrame template to partitioned target axes.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int PartMatch( AstCmpFrame *template, AstFrame *target,
*                    int naxes1, const int axes1[],
*                    int naxes2, const int axes2[],
*                    int **template_axes, int **target_axes,
*                    AstMapping **map, AstFrame **result )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function matches a "template" CmpFrame to a "target" Frame
*     and determines whether it is possible to convert coordinates
*     between them.  If it is, a Mapping that performs the
*     transformation is returned along with a new Frame that describes
*     the coordinate system that results when this Mapping is applied
*     to the "target" coordinate system. In addition, information is
*     returned to allow the axes in this "result" Frame to be
*     associated with the corresponding axes in the "target" Frame and
*     "template" CmpFrame from which they are derived.
*
*     To simplify the matching process for a CmpFrame template, this
*     function requires the caller to specify how the axes of the
*     target Frame should be partitioned between the two component
*     Frames of the template. The function attempts to find a match
*     using this axis partitioning only. In general, the way in which
*     the target axes must be partitioned is not known in advance, so
*     this function must be invoked several times with alternative
*     partitioning before a match will be found.

*  Parameters:
*     template
*        Pointer to the template CmpFrame. This describes the
*        coordinate system (or set of possible coordinate systems)
*        into which we wish to convert our coordinates.
*     target
*        Pointer to the target Frame. This describes the coordinate
*        system in which we already have coordinates.
*     naxes1
*        The number of target axes to be matched against the first
*        component Frame of the template CmpFrame.
*     axes1
*        An array with "naxes1" elements containing the (zero-based)
*        indices of the target axes to be matched against the first
*        component Frame. Order is not significant.
*     naxes2
*        The number of target axes to be matched against the second
*        component Frame of the template CmpFrame.
*     axes2
*        An array with "naxes2" elements containing the (zero-based)
*        indices of the target axes to be matched against the second
*        component Frame. Order is not significant.
*     template_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        template CmpFrame axis from which it is derived. If it is not
*        derived from any template axis, a value of -1 will be
*        returned instead.
*     target_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        target Frame axis from which it is derived. If it is not
*        derived from any target Frame axis, a value of -1 will be
*        returned instead.
*     map
*        Address of a location where a pointer to a new Mapping will
*        be returned if the requested coordinate conversion is
*        possible. If returned, the forward transformation of this
*        Mapping may be used to convert coordinates between the
*        "target" Frame and the "result" Frame (see below) and the
*        inverse transformation will convert in the opposite
*        direction.
*     result
*        Address of a location where a pointer to a new Frame will be
*        returned if the requested coordinate conversion is
*        possible. If returned, this Frame describes the coordinate
*        system that results from applying the returned Mapping
*        (above) to the "target" coordinate system. In general, this
*        Frame will combine attributes from (and will therefore be
*        more specific than) both the target Frame and the template
*        CmpFrame. In particular, when the template allows the
*        possibility of transformaing to any one of a set of
*        alternative coordinate systems, the "result" Frame will
*        indicate which of the alternatives was used.

*  Returned Value:
*     A non-zero value is returned if the requested coordinate
*     conversion is possible. Otherwise zero is returned (this will
*     not in itself result in an error condition).

*  Notes:
*     - The "axes1" and "axes2" arrays should not contain any axis
*     indices in common and should, taken together, list all the axes
*     of the target Frame.
*     - By default, the "result" Frame will have its number of axes
*     and axis order determined by the "template" CmpFrame. However,
*     if the PreserveAxes attribute of the template is non-zero, then
*     the axis count and axis order of the "target" Frame will be used
*     instead.
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstFrame *frame1;             /* Pointer to first sub-Frame from target */
   AstFrame *frame2;             /* Pointer to second sub-Frame from target */
   AstFrame *result1;            /* Result Frame pointer from first match */
   AstFrame *result2;            /* Result Frame pointer from second match */
   AstFrame *tmp_frame;          /* Temporary Frame pointer */
   AstMapping *junk_map;         /* Mapping pointer returned by astSubFrame */
   AstMapping *map1;             /* Mapping pointer from first match */
   AstMapping *map2;             /* Mapping pointer from second match */
   AstMapping *permmap;          /* Pointer to PermMap */
   AstMapping *tmp_map;          /* Temporary Mapping pointer */
   const int *perm;              /* Template axis permutation array pointer */
   int *inperm;                  /* Pointer to temporary permutation array */
   int *invperm;                 /* Inverse axis permutation array pointer */
   int *outperm;                 /* Pointer to temporary permutation array */
   int *pick;                    /* Pointer to array of axis selections */
   int *result_order;            /* Relative result axis order array pointer */
   int *result_perm;             /* Result axis permutation array pointer */
   int *target_assoc;            /* Target axis association array pointer */
   int *target_axes1;            /* Target axis associations from 1st match */
   int *target_axes2;            /* Target axis associations from 2nd match */
   int *template_assoc;          /* Template axis association array pointer */
   int *template_axes1;          /* Template axis associations, 1st match */
   int *template_axes2;          /* Template axis associations, 2nd match */
   int first;                    /* Axis in 1st component? */
   int full_axis;                /* Result Frame axis index, before sub-set */
   int match1;                   /* First match successful? */
   int match2;                   /* Second match successful? */
   int match;                    /* Both matches successful? (result) */
   int match_end1;               /* MatchEnd attribute for component 1 */
   int match_end2;               /* MatchEnd attribute for component 2 */
   int match_end;                /* MatchEnd attribute for template */
   int match_end_set;            /* Component MatchEnd attribute set? */
   int output_axis;              /* Output axis index */
   int part_result_axis;         /* Result Frame component axis index */
   int part_target_axis;         /* Target Frame component axis index */
   int part_template_axis;       /* Template CmpFrame component axis index */
   int permute_set;              /* Component Permute attribute set? */
   int permute_value;            /* Component Permute attribute value */
   int preserve_axes;            /* Template PreserveAxes attribute value */
   int preserve_axes_set;        /* Component PreserveAxes attribute set? */
   int ref_naxes;                /* Number of reference Frame axes */
   int result_axis;              /* Result Frame axis index */
   int result_naxes1;            /* Number of result Frame axes, component 1 */
   int result_naxes2;            /* Number of result Frame axes, component 2 */
   int result_naxes;             /* Total number of result Frame axes */
   int target_axis;              /* Target Frame axis index */
   int target_naxes;             /* Number of target Frame axes */
   int template_axis;            /* Template CmpFrame axis index */
   int template_naxes1;          /* Number of template axes, component 1 */
   int template_naxes;           /* Total number of template axes */

/* Initialise the returned values. */
   *template_axes = NULL;
   *target_axes = NULL;
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* Select the required sub-Frames from the target. */
/* ----------------------------------------------- */
/* We first create two sub-Frames (that can be matched against the two
   template component Frames) by selecting the two specified sets of
   axes from the target. This is done without overlaying any template
   attributes. Annul the Mappings produced by this process, as these
   are not needed. */
   (void) astSubFrame( target, NULL, naxes1, axes1, NULL, &junk_map, &frame1 );
   junk_map = astAnnul( junk_map );
   (void) astSubFrame( target, NULL, naxes2, axes2, NULL, &junk_map, &frame2 );
   junk_map = astAnnul( junk_map );

/* Match the sub-Frames with the template component Frames. */
/* -------------------------------------------------------- */
/* We now have two sub-Frames obtained from the target, and will
   attempt to match these with the component Frames contained within
   the template CmpFrame. */

/* Before using each template component Frame, see whether any of its
   attributes that control matching is "un-set". If so, over-ride it
   with the attribute value of the template CmpFrame as a whole. */
   match_end_set = astTestMatchEnd( template->frame1 );
   if ( !match_end_set ) {
      astSetMatchEnd( template->frame1, astGetMatchEnd( template ) );
   }
   preserve_axes_set = astTestPreserveAxes( template->frame1 );
   if ( !preserve_axes_set ) {
      astSetPreserveAxes( template->frame1, astGetPreserveAxes( template ) );
   }

/* We must also temporarily set the Permute attribute to 1 (this is
   normally the default, but might have been set otherwise). This is
   needed so that permutations of the target axes will be considered.
   Without this, the order in which the axes are presented is
   significant and we would have to test all the permutations. If the
   Permute attribute of the template CmpFrame as a whole is zero, then
   the resulting match may still have to be rejected, but this must be
   done at a higher level. */
   permute_set = astTestPermute( template->frame1 );
   if ( permute_set ) permute_value = astGetPermute( template->frame1 );
   astSetPermute( template->frame1, 1 );

/* Test for a match with the first template component Frame. */
   match1 = astMatch( template->frame1, frame1,
                      &template_axes1, &target_axes1, &map1, &result1 );

/* Clear the attribute values again afterwards if necessary. */
   if ( !match_end_set ) astClearMatchEnd( template->frame1 );
   if ( !preserve_axes_set ) astClearPreserveAxes( template->frame1 );

/* Also restore the original Permute attribute setting. */
   if ( permute_set ) {
      astSetPermute( template->frame1, permute_value );
   } else {
      astClearPermute( template->frame1 );
   }

/* Repeat the whole process for the second component Frame. */
   match_end_set = astTestMatchEnd( template->frame2 );
   if ( !match_end_set ) {
      astSetMatchEnd( template->frame2, astGetMatchEnd( template ) );
   }
   preserve_axes_set = astTestPreserveAxes( template->frame2 );
   if ( !preserve_axes_set ) {
      astSetPreserveAxes( template->frame2, astGetPreserveAxes( template ) );
   }
   permute_set = astTestPermute( template->frame2 );
   if ( permute_set ) permute_value = astGetPermute( template->frame2 );
   astSetPermute( template->frame2, 1 );

   match2 = astMatch( template->frame2, frame2,
                      &template_axes2, &target_axes2, &map2, &result2 );

   if ( !match_end_set ) astClearMatchEnd( template->frame2 );
   if ( !preserve_axes_set ) astClearPreserveAxes( template->frame2 );
   if ( permute_set ) {
      astSetPermute( template->frame2, permute_value );
   } else {
      astClearPermute( template->frame2 );
   }

/* See if both matches were successful. */
   if ( astOK && match1 && match2 ) {
      match = 1;

/* Obtain the number of target axes. */
      target_naxes = astGetNaxes( target );

/* Obtain the number of axes in each of the result Frames produced by
   the matching operation. */
      result_naxes1 = astGetNaxes( result1 );
      result_naxes2 = astGetNaxes( result2 );

/* Obtain the number of axes in the first template component Frame and
   in the template CmpFrame as a whole. */
      template_naxes1 = astGetNaxes( template->frame1 );
      template_naxes = astGetNaxes( template );

/* Obtain the value of the MatchEnd attribute for each of the
   template's component Frames and for the template CmpFrame as a
   whole. */
      match_end1 = astGetMatchEnd( template->frame1 );
      match_end2 = astGetMatchEnd( template->frame2 );
      match_end = astGetMatchEnd( template );

/* Obtain a pointer to the template CmpFrame's axis permutation
   array. Allocate space for a further array and fill it with the
   inverse of this axis permutation. */
      perm = astGetPerm( template );
      invperm = astMalloc( sizeof( int ) * (size_t) template_naxes );
      if ( astOK ) {
         for ( template_axis = 0; template_axis < template_naxes;
                                  template_axis++ ) {
            invperm[ perm[ template_axis ] ] = template_axis;
         }
      }

/* Generate template and target axis associations. */
/* ----------------------------------------------- */
/* We now construct two arrays which identify the axis associations
   between the result axes (in the order obtained from the matching
   process above) and the axes of the template and target. This
   involves tracing back through several steps. */

/* First calculate the total number of result axes and allocate memory
   for the association arrays. */
      result_naxes = result_naxes1 + result_naxes2;
      template_assoc = astMalloc( sizeof( int ) * (size_t) result_naxes );
      target_assoc = astMalloc( sizeof( int ) * (size_t) result_naxes );
      if ( astOK ) {

/* Produce associations for each result axis in turn. */
         for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {

/* Decide whether this result axis is contained in the first (or
   second) individual result Frame. */
            first = ( result_axis < result_naxes1 );

/* Obtain the index of the axis within the individual result Frame.
   This involves adjusting for the axis numbering offset of the second
   result Frame if necessary. */
            part_result_axis = first ? result_axis :
                                       result_axis - result_naxes1;

/* Find the template and target axis associations for this axis by
   looking them up in the association arrays returned from the
   matching process. This gives axis indices that apply to the
   individual template/target Frames supplied as input to the matching
   process. */
            part_template_axis = first ? template_axes1[ part_result_axis ] :
                                         template_axes2[ part_result_axis ];
            part_target_axis = first ? target_axes1[ part_result_axis ] :
                                       target_axes2[ part_result_axis ];

/* Check that the resulting template association identifies a valid
   template axis. */
            if ( part_template_axis != -1 ) {

/* If so, obtain the template axis index. This involves adjusting for
   the axis numbering offset of the second template component Frame
   (if necessary) and then applying the inverse template axis
   permutation to convert to the external template axis
   numbering. Store the result in the template association array. */
               template_assoc[ result_axis ] =
                  invperm[ first ? part_template_axis :
                                   part_template_axis + template_naxes1 ];

/* Indicate if there is no template axis association by storing an
   index of -1. */
            } else {
               template_assoc[ result_axis ] = -1;
            }

/* Similarly, check that the target association identifies a valid
   target axis. */
            if ( part_target_axis != -1 ) {

/* If so, obtain the target axis index. This simply involves using the
   axis selection arrays provided by the caller to look up which
   target axes were involved in the matching process. */
               target_assoc[ result_axis ] =
                  first ? axes1[ part_target_axis ] :
                          axes2[ part_target_axis ];

/* Indicate if there is no target axis association by storing an index
   of -1. */
            } else {
               target_assoc[ result_axis ] = -1;
            }
         }
      }

/* Free the inverse axis permutation array. */
      invperm = astFree( invperm );

/* Create the output Frame. */
/* ------------------------ */
/* Initialise. */
      result_order = NULL;
      result_perm = NULL;

/* Construct the basis of the final result Frame by combining the two
   individual result Frames (from the matching process) using a
   CmpFrame. */
      if ( astOK ) {
         *result = (AstFrame *) astCmpFrame( result1, result2, "" );

/* The next step is to permute the result Frame's axis order so that
   it corresponds with the axis order of the "reference Frame". The
   reference Frame is either the template or the target, depending on
   whether the template's PreserveAxes attribute is non-zero. Obtain
   the value of this attribute. */
         preserve_axes = astGetPreserveAxes( template );

/* Decide how many axes the reference Frame contains. */
         ref_naxes = preserve_axes ? target_naxes : template_naxes;

/* Make a copy of the axis association array that refers to the
   reference Frame. */
         result_order = astStore( NULL,
                                  preserve_axes ? target_assoc :
                                                  template_assoc,
                                  sizeof( int ) * (size_t) result_naxes );

/* The intention is to use this axis association array to permute the
   result axes into the same order as the reference Frame's axes. It
   is not that simple, however, because some of the axis associations
   may be null (i.e. result axes may exist that are not associated
   with reference axes) and they may also be incomplete (i.e. not
   every reference axis may be associated with a result axis).

   This prevents us from permuting the result axis order using this
   array directly, essentially because we haven't yet defined where
   any "extra" result axes (those with no association) should appear
   in the final axis order. */

/* To overcome this, we replace all the null (-1) entries in the
   "result_order" array with new values which define their position
   relative to the other entries. This also involves re-numbering
   other entries to avoid clashes. The new numbers assigned depend on
   the MatchEnd attribute for each of the template component Frames,
   so we handle the associations for each of these components
   separately. */
         AddExtraAxes( result_naxes, result_order,
                       0, result_naxes1 - 1, match_end1 );
         AddExtraAxes( result_naxes, result_order,
                       result_naxes1, result_naxes - 1, match_end2 );

/* There may now be some reference Frame axes which are not referenced
   in this array, so we renumber the entries starting at zero (but
   preserving their relative order) so that there are no missing
   values due to these. */
         RenumberAxes( result_naxes, result_order );

/* The resulting "result_order" array no longer describes the original
   reference Frame axis associations, but is now suitable for
   permuting the result axes into the required order. However, we
   require the inverse of this permutation, so allocate an array and
   fill it with the inverse. */
         result_perm = astMalloc( sizeof( int ) * (size_t) result_naxes );
         if ( astOK ) {
            for ( result_axis = 0; result_axis < result_naxes;
                                   result_axis++ ) {
               result_perm[ result_order[ result_axis ] ] = result_axis;
            }
         }

/* Apply the inverse permutation to the result CmpFrame to put its
   axes into the required order. */
         astPermAxes( *result, result_perm );

/* Check if the number of result Frame axes differs from the number of
   reference axes. This can arise if the PreserveAxes attribute of
   either template component Frame is set to a value that differs from
   that of the template CmpFrame as a whole. If this is the case, we
   must select a sub-set (or super-set) of the result axes, so that we
   end up with the same number of axes as the reference Frame. */
         if ( ref_naxes != result_naxes ) {

/* Allocate an array to hold the indices of the axes required. */
            pick = astMalloc( sizeof( int ) * (size_t) ref_naxes );
            if ( astOK ) {

/* Generate the axis indices, using the template CmpFrame's MatchEnd
   attribute to decide which ones to use. */
               for ( output_axis = 0; output_axis < ref_naxes;
                                      output_axis++ ) {
                  full_axis =
                     match_end ? output_axis + ( result_naxes - ref_naxes ) :
                                 output_axis;

/* If the index is valid (i.e. the required axis is available), store
   it. Otherwise, use an index of -1, which requests that new
   (default) axes be supplied where needed. */
                  if ( ( full_axis >= 0 ) && ( full_axis < result_naxes ) ) {
                     pick[ output_axis ] = full_axis;
                  } else {
                     pick[ output_axis ] = -1;
                  }
               }
            }

/* Pick the required axes from the result Frame and replace it with
   the new one. */
            tmp_frame = astPickAxes( *result, ref_naxes, pick, NULL );
            *result = astAnnul( *result );
            *result = tmp_frame;

/* Free the array of axis indices. */
            pick = astFree( pick );
         }
      }

/* Create output axis association arrays. */
/* -------------------------------------- */
/* We now construct the two arrays that are returned to identify which
   template and target axes (if any) are associated with each final
   result Frame axis. Allocate memory for these arrays. */
      if ( astOK ) {
         *target_axes = astMalloc( sizeof( int ) * (size_t) ref_naxes );
         *template_axes = astMalloc( sizeof( int ) * (size_t) ref_naxes );
         if ( astOK ) {

/* For each output axis, obtain the original result axis index (before
   any sub-set or super-set of the output axes was selected). */
            for ( output_axis = 0; output_axis < ref_naxes; output_axis++ ) {
               full_axis =
                  match_end ? output_axis + ( result_naxes - ref_naxes ) :
                              output_axis;

/* Derive the result axis index before the axes were permuted into
   their final order. */
               if ( ( full_axis >= 0 ) && ( full_axis < result_naxes ) ) {
                  result_axis = result_perm[ full_axis ];

/* Use this axis index and the axis association arrays generated
   earlier to obtain the required associations, and store these in the
   output arrays. */
                  ( *template_axes )[ output_axis ] =
                     template_assoc[ result_axis ];
                  ( *target_axes )[ output_axis ] =
                     target_assoc[ result_axis ];

/* Store a value of -1 if there is no association. */
               } else {
                  ( *template_axes )[ output_axis ] = -1;
                  ( *target_axes )[ output_axis ] = -1;
               }
            }
         }
      }

/* Free the original (un-permuted) axis association arrays. */
      template_assoc = astFree( template_assoc );
      target_assoc = astFree( target_assoc );

/* Create the output Mapping. */
/* -------------------------- */
/* Construct the basis of the final output Mapping by combining the
   Mappings produced by the individual matching processes in parallel,
   using a CmpMap. */
      *map = (AstMapping *) astCmpMap( map1, map2, 0, "" );

/* It is now necessary to prefix and suffix this CmpMap with two
   PermMaps, which correct the input and output axis order to
   correspond with the target and result Frame axes.

   At the target end, this reflects the partitioning of the target
   axes into two groups, as specified by the caller. At the result
   end, it reflects the axis permutation applied (above) to put the
   final result Frame axes into the required order, together with the
   selection of any sub-set or super-set of these axes. */

/* Allocate memory for permutation arrays to describe the prefix
   PermMap. */
      inperm = astMalloc( sizeof( int ) * (size_t) target_naxes );
      outperm = astMalloc( sizeof( int ) * (size_t) target_naxes );
      if ( astOK ) {

/* Consider the target axes in the order that they were supplied to
   the matching processes (i.e. the order that corresponds with the
   input coordinates of the CmpMap produced above). */
         for ( target_axis = 0; target_axis < target_naxes; target_axis++ ) {

/* Decide whether each axis belongs to the first (or second) selected
   group of target axes. */
            first = ( target_axis < naxes1 );

/* Obtain the index of the target axis within the group. This involves
   allowing for the numbering offset of the second group if
   necessary. */
            part_target_axis = first ? target_axis :
                                       target_axis - naxes1;

/* Obtain the original target axis index by looking up the axis in the
   appropriate axis selection array provided by the caller. */
            outperm[ target_axis ] = first ? axes1[ part_target_axis ] :
                                             axes2[ part_target_axis ];

/* Fill the "inperm" array with the inverse of this permutation. */
            inperm[ outperm[ target_axis ] ] = target_axis;
         }
      }

/* If the permutation is not null, use these permutation arrays to
   construct the required prefix PermMap. */
      if ( GoodPerm( target_naxes, inperm, target_naxes, outperm ) ) {
         permmap = (AstMapping *) astPermMap( target_naxes, inperm,
                                              target_naxes, outperm,
                                              NULL, "" );

/* Add the PermMap as a prefix to the result Mapping and then annul
   the original Mapping pointers. */
         tmp_map = (AstMapping *) astCmpMap( permmap, *map, 1, "" );
         (void) astAnnul( *map );
         *map = tmp_map;
         permmap = astAnnul( permmap );
      }

/* Free the permutation arrays. */
      inperm = astFree( inperm );
      outperm = astFree( outperm );

/* Allocate memory for permutation arrays to describe the suffix
   PermMap. */
      inperm = astMalloc( sizeof( int ) * (size_t) result_naxes );
      outperm = astMalloc( sizeof( int ) * (size_t) ref_naxes );
      if ( astOK ) {

/* Initialise the "inperm" array. */         
         for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {
            inperm[ result_axis ] = -1;
         }

/* For each output axis, obtain the index of the corresponding result
   axis before any sub-set or super-set was selected. */
         for ( output_axis = 0; output_axis < ref_naxes; output_axis++ ) {
            full_axis =
               match_end ? output_axis + ( result_naxes - ref_naxes ) :
                           output_axis;

/* Store the axis index before the result axes were permuted, and also
   construct the inverse permutation. */
            if ( ( full_axis >= 0 ) && ( full_axis < result_naxes ) ) {
               outperm[ output_axis ] = result_perm[ full_axis ];
               inperm[ outperm[ output_axis ] ] = output_axis;

/* Note which output axes do not exist in the result Frame
   (e.g. because a super-set was selected). */
            } else {
               outperm[ output_axis ] = -1;
            }
         }
      }

/* If the permutation is not null, use these permutation arrays to
   construct the required suffix PermMap. */
      if ( GoodPerm( target_naxes, inperm, target_naxes, outperm ) ) {
         permmap = (AstMapping *) astPermMap( result_naxes, inperm,
                                              ref_naxes, outperm,
                                              NULL, "" );

/* Add the PermMap as a suffix to the result Mapping and then annul
   the original Mapping pointers. */
         tmp_map = (AstMapping *) astCmpMap( *map, permmap, 1, "" );
         (void) astAnnul( *map );
         *map = tmp_map;
         permmap = astAnnul( permmap );
      }

/* Free the permutation arrays. */
      inperm = astFree( inperm );
      outperm = astFree( outperm );

/* Free the result axis permutation arrays. */
      result_order = astFree( result_order );
      result_perm = astFree( result_perm );
   }

/* If necessary, free the results of the first matching process. */
   if ( match1 ) {
      template_axes1 = astFree( template_axes1 );
      target_axes1 = astFree( target_axes1 );
      map1 = astAnnul( map1 );
      result1 = astAnnul( result1 );
   }

/* If necessary, free the results of the second matching process. */
   if ( match2 ) {
      template_axes2 = astFree( template_axes2 );
      target_axes2 = astFree( target_axes2 );
      map2 = astAnnul( map2 );
      result2 = astAnnul( result2 );
   }

/* Annul the pointers to the sub-Frames selected from the target. */
   frame1 = astAnnul( frame1 );
   frame2 = astAnnul( frame2 );

/* If an error occurred, free all allocated memory, annul the result
   Object pointers and clear all returned values. */
   if ( !astOK ) {
      *template_axes = astFree( *template_axes );
      *target_axes = astFree( *target_axes );
      *map = astAnnul( *map );;
      *result = astAnnul( *result );
      match = 0;
   }

/* Return the result. */
   return match;
}

static void PermAxes( AstFrame *this_frame, const int perm[] ) {
/*
*  Name:
*     PermAxes

*  Purpose:
*     Permute the order of a CmpFrame's axes.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void astPermAxes( AstFrame *this, const int perm[] )

*  Class Membership:
*     CmpFrame member function (over-rides the astPermAxes method
*     inherited from the Frame class).

*  Description:
*     This function permutes the order in which a CmpFrame's axes occur.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     perm
*        An array of int (with one element for each axis of the
*        CmpFrame) which lists the axes in their new order. Each
*        element of this array should be a (zero-based) axis index
*        identifying the axes according to their old (un-permuted)
*        order.

*  Notes:
*     - Only genuine permutations of the axis order are permitted, so
*     each axis must be referenced exactly once in the "perm" array.
*     - If more than one axis permutation is applied to a CmpFrame,
*     the effects are cumulative.

*  Implementation Notes:
*     - This function performs essentially the same operation as the
*     Frame member function which it over-rides. However, it operates
*     on a "perm" array held in the CmpFrame structure (rather than
*     the one in the parent Frame structure). This duplication of the
*     array is necessary because the one in the Frame structure is of
*     zero length, the number of axes in the Frame structure having
*     been set to zero to prevent unnecessary allocation of Axis
*     objects which are not needed by the CmpFrame.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   int *old;                     /* Pointer to copy of old permutation array */
   int axis;                     /* Loop counter for CmpFrame axes */
   int naxes;                    /* Number of CmpFrame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate the permutation array, to check that it describes a
   genuine permutation. */
   astCheckPerm( this, perm, "astPermAxes" );

/* Obtain the number of CmpFrame axes. */
   naxes = astGetNaxes( this );

/* Allocate memory and use it to store a copy of the old permutation
   array for the CmpFrame. */
   old = astStore( NULL, this->perm, sizeof( int ) * (size_t) naxes );

/* Apply the new axis permutation cumulatively to the old one and
   store the result in the CmpFrame. */
   if ( astOK ) {
      for ( axis = 0; axis < naxes; axis++ ) {
         this->perm[ axis ] = old[ perm[ axis ] ];
      }
   }

/* Free the temporary copy of the old array. */
   old = astFree( old );
}

static void PrimaryFrame( AstFrame *this_frame, int axis1,
                          AstFrame **frame, int *axis2 ) {
/*
*  Name:
*     PrimaryFrame

*  Purpose:
*     Uniquely identify a primary Frame and one of its axes.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void astPrimaryFrame( AstFrame *this, int axis1, AstFrame **frame,
*                           int *axis2 )

*  Class Membership:
*     CmpFrame member function (over-rides the protected
*     astPrimaryFrame method inherited from the Frame class).

*  Description:
*     This function returns information about the underlying (primary)
*     Frame corresponding to a specified CmpFrame axis.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis1
*        An axis index (zero-based) identifying the CmpFrame axis for
*        which information is required.
*     frame
*        Address of a location to receive a pointer to the underlying
*        (primary) Frame to which the requested axis belongs
*        (i.e. this will not be a compound Frame).
*     axis2
*        Pointer to an int which is to receive the (zero-based) axis
*        index within "frame" which identifies the axis being referred
*        to, using the axis order that applied when the primary Frame
*        was originally constructed (i.e. this function undoes all
*        subsequent axis pemutations and the effects of combining
*        Frames, in order to reveal the original underlying axis
*        order).

*  Notes:
*     - This protected method is provided so that class
*     implementations can distinguish the axes of Frames from one
*     another (e.g. can distinguish a longitude axis as being
*     different from a latitide axis) even after their order has been
*     permuted and they have been combined with axes from other
*     Frames.
*     - The reference count of the primary Frame will be incremented
*     by one to reflect the new pointer returned.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   int naxes1;                   /* Number of axes in frame1 */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis1 = astValidateAxis( this, axis1, "astPrimaryFrame" );

/* Obtain the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which Frame contains the axis and invoke its astPrimaryFrame
   method to obtain the required information. */
      if ( axis1 < naxes1 ) {
         astPrimaryFrame( this->frame1, axis1, frame, axis2 );
      } else {
         astPrimaryFrame( this->frame2, axis1 - naxes1, frame, axis2 );
      }
   }
}

static int QsortCmpAxes( const void *a, const void *b ) {
/*
*  Name:
*     QsortCmpAxes

*  Purpose:
*     Compare two axis indices for "qsort".

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int QsortCmpAxes( const void *a, const void *b )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This is a service function for the C RTL routine "qsort". It
*     takes the two values supplied and interprets them as integer
*     indices into the static "qsort_axes" array. It compares the
*     values of these two array elements and returns the result
*     required by "qsort".
*
*     This function is used when sorting an array of indices so that
*     they access the "qsort_axes" array in ascending order.

*  Parameters:
*     As required by "qsort".

*  Returned Value:
*     As required by "qsort".
*/

/* Local Variables. */
   int result;                   /* Result value to return */
   int val_a;                    /* First axis index */
   int val_b;                    /* Second axis index */

/* Convert the values passed by "qsort" into integer array indices and
   use these to access the "qsort_axes" array (this pointer to the
   array being assigned by the caller of "qsort"). Extract the two
   values being compared. */
   val_a = qsort_axes[ *( (const int *) a ) ];
   val_b = qsort_axes[ *( (const int *) b ) ];

/* Compare the two values as required by "qsort". */
   if ( val_a < val_b ) {
      result = -1;
   } else if ( val_a == val_b ) {
      result = 0;
   } else {
      result = 1;
   }

/* Return the result. */
   return result;
}

static void RenumberAxes( int naxes, int axes[] ) {
/*
*  Name:
*     RenumberAxes

*  Purpose:
*     Renumber axis indices to eliminate missing ones.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void RenumberAxes( int naxes, int axes[] )

*  Class Membership:
*     CmpFrame member function.

*  Description:
*     This function takes an array containing a list of (zero-based)
*     axis indices referring to the axes of a Frame, some of whose
*     axes may not be referenced. It renumbers the axis indices, to
*     eliminate any which are missing (i.e. not referenced), while
*     preserving the original order. It does this by replacing each
*     axis index by its rank (starting at zero) when the indices are
*     sorted into ascending order.

*  Parameters:
*     naxes
*        The number of axis indices present.
*     axes
*        An array, with "naxes" elements, containing the indices. This
*        is modified by this function to contain the new indices.
*/

/* Local Variables: */
   int *work;                    /* Pointer to workspace array */
   int i;                        /* Loop counter */

/* Check the global error status. */
   if ( !astOK ) return;

/* Allocate workspace. */
   work = astMalloc( sizeof( int ) * (size_t) naxes );
   if ( astOK ) {

/* Fill the workspace with indices which address the axis index values
   in their natural order. */
      for ( i = 0; i < naxes; i++ ) work[ i ] = i;

/* Make the "axes" values available to the C RTL function "qsort" via
   the static "qsort_axes" pointer. Then use "qsort" to permute the
   contents of "work" so that it addresses the axis indices in
   ascending order. */
      qsort_axes = axes;
      qsort( work, (size_t) naxes, sizeof( int ), QsortCmpAxes );

/* Use the result to replace each axis index by its rank when sorted
   into ascending order (starting with zero). */
      for ( i = 0; i < naxes; i++ ) axes[ work[ i ] ] = i;
   }

/* Free the workspace array. */
   work = astFree( work );
}

static void SetAxis( AstFrame *this_frame, int axis, AstAxis *newaxis ) {
/*
*  Name:
*     SetAxis

*  Purpose:
*     Set a new Axis for a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void astSetAxis( AstFrame *this, int axis, AstAxis *newaxis )

*  Class Membership:
*     CmpFrame member function (over-rides the astSetAxis method
*     inherited from the Frame class).

*  Description:
*     This function allows a new Axis object to be associated with one
*     of the axes of a CmpFrame, replacing the previous one. Each Axis
*     object contains a description of the quantity represented along
*     one of the CmpFrame's axes, so this function allows this
*     description to be exchanged for another one.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The index (zero-based) of the CmpFrame axis whose associated
*        Axis object is to be replaced.
*     newaxis
*        Pointer to the new Axis object.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int naxes1;                   /* Number of axes in frame1 */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astSetAxis" );

/* Determine the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which Frame contains the axis and invoke its astSetAxis
   method to set the new Axis. */
      if ( axis < naxes1 ) {
         astSetAxis( this->frame1, axis, newaxis );
      } else {
         astSetAxis( this->frame2, axis - naxes1, newaxis );
      }
   }
}

static void SetMaxAxes( AstFrame *this_frame, int maxaxes ) {
/*
*  Name:
*     SetMaxAxes

*  Purpose:
*     Set a value for the MaxAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void SetMaxAxes( AstFrame *this, int maxaxes )

*  Class Membership:
*     CmpFrame member function (over-rides the astSetMaxAxes method
*     inherited from the Frame class).

*  Description:
*     This function does nothing because the MaxAxes attribute for a
*     CmpFrame is determined by the Frames it contains and cannot be
*     altered independently.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     maxaxes
*        The new value to be set (ignored).

*  Returned Value:
*     void.
*/

/* Do nothing. */
}

static void SetMinAxes( AstFrame *this_frame, int minaxes ) {
/*
*  Name:
*     SetMinAxes

*  Purpose:
*     Set a value for the MinAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     void SetMinAxes( AstFrame *this, int minaxes )

*  Class Membership:
*     CmpFrame member function (over-rides the astSetMinAxes method
*     inherited from the Frame class).

*  Description:
*     This function does nothing because the MinAxes attribute for a
*     CmpFrame is determined by the Frames it contains and cannot be
*     altered independently.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     minaxes
*        The new value to be set (ignored).

*  Returned Value:
*     void.
*/

/* Do nothing. */
}

static int SubFrame( AstFrame *target_frame, AstFrame *template,
                     int result_naxes, const int *target_axes,
                     const int *template_axes, AstMapping **map,
                     AstFrame **result ) {
/*
*  Name:
*     SubFrame

*  Purpose:
*     Select axes from a CmpFrame and convert to the new coordinate system.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int SubFrame( AstFrame *target, AstFrame *template,
*                   int result_naxes, const int *target_axes,
*                   const int *template_axes, AstMapping **map,
*                   AstFrame **result )

*  Class Membership:
*     CmpFrame member function (over-rides the protected astSubFrame
*     method inherited from the Frame class).

*  Description:
*     This function selects a requested sub-set (or super-set) of the
*     axes from a "target" CmpFrame and creates a new Frame with
*     copies of the selected axes assembled in the requested order. It
*     then optionally overlays the attributes of a "template" Frame on
*     to the result. It returns both the resulting Frame and a Mapping
*     that describes how to convert between the coordinate systems
*     described by the target and result Frames. If necessary, this
*     Mapping takes account of any differences in the Frames'
*     attributes due to the influence of the template.

*  Parameters:
*     target
*        Pointer to the target CmpFrame, from which axes are to be selected.
*     template
*        Pointer to the template Frame, from which new attributes for
*        the result Frame are to be obtained. Optionally, this may be
*        NULL, in which case no overlaying of template attributes will
*        be performed.
*     result_naxes
*        Number of axes to be selected from the target Frame. This
*        number may be greater than or less than the number of axes in
*        this Frame (or equal).
*     target_axes
*        Pointer to an array of int with result_naxes elements, giving
*        a list of the (zero-based) axis indices of the axes to be
*        selected from the target CmpFrame. The order in which these
*        are given determines the order in which the axes appear in
*        the result Frame. If any of the values in this array is set
*        to -1, the corresponding result axis will not be derived from
*        the target Frame, but will be assigned default attributes
*        instead.
*     template_axes
*        Pointer to an array of int with result_naxes elements. This
*        should contain a list of the template axes (given as
*        zero-based axis indices) with which the axes of the result
*        Frame are to be associated. This array determines which axes
*        are used when overlaying axis-dependent attributes of the
*        template on to the result. If any element of this array is
*        set to -1, the corresponding result axis will not receive any
*        template attributes.
*
*        If the template argument is given as NULL, this array is not
*        used and a NULL pointer may also be supplied here.
*     map
*        Address of a location to receive a pointer to the returned
*        Mapping.  The forward transformation of this Mapping will
*        describe how to convert coordinates from the coordinate
*        system described by the target CmpFrame to that described by
*        the result Frame. The inverse transformation will convert in
*        the opposite direction.
*     result
*        Address of a location to receive a pointer to the result Frame.

*  Returned Value:
*     A non-zero value is returned if coordinate conversion is
*     possible between the target and the result Frame. Otherwise zero
*     is returned and *map and *result are returned as NULL (but this
*     will not in itself result in an error condition). In general,
*     coordinate conversion should always be possible if no template
*     Frame is supplied but may not always be possible otherwise.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.

*  Implementation Deficiencies:
*     - It is not clear that the method of handling "extra" axes is
*     the best one, nor is the method of setting the "following" flag
*     necessarily correct.  However, it is also not obvious that this
*     feature will ever be needed, so improvements have been left
*     until the requirement is clearer.
*/

/* Local Variables: */
   AstCmpFrame *target;          /* Pointer to target CmpFrame structure */
   AstFrame *sub_result1;        /* Pointer to result Frame for frame1 */
   AstFrame *sub_result2;        /* Pointer to result Frame for frame2 */
   AstMapping *permmap_pref;     /* Pointer to PermMap used as a prefix */
   AstMapping *permmap_suff;     /* Pointer to PermMap used as a suffix */
   AstMapping *sub_map1;         /* Pointer to Mapping from frame1 */
   AstMapping *sub_map2;         /* Pointer to Mapping from frame2 */
   AstMapping *sub_map;          /* Pointer to combined component Mappings */
   AstMapping *tmp_map;          /* Temporary Mapping pointer */
   const int *perm;              /* Pointer to axis permutation array */
   int *frame_choice;            /* Pointer to flag array for partitioning */
   int *inperm_pref;             /* Pointer to prefix permutation array */
   int *inperm_suff;             /* Pointer to suffix permutation array */
   int *outperm_pref;            /* Pointer to prefix permutation array */
   int *outperm_suff;            /* Pointer to suffix permutation array */
   int *target_axes1;            /* Pointer to frame1 axis selection array */
   int *target_axes2;            /* Pointer to frame2 axis selection array */
   int *template_axes1;          /* Pointer to frame1 template axis array */
   int *template_axes2;          /* Pointer to frame2 template axis array */
   int axis_p;                   /* Permuted axis index */
   int following;                /* Associate extra axis and following axis? */
   int i1;                       /* Count of axes obtained from frame1 */
   int i2;                       /* Count of axes obtained from frame2 */
   int match;                    /* Result value to return */
   int n1;                       /* Number of axes obtained from frame1 */
   int n2;                       /* Number of axes obtained from frame2 */
   int naxes1;                   /* Number of axes in frame1 */
   int naxes2;                   /* Number of axes in frame2 */
   int naxes;                    /* Number of axes in target */
   int result_axis;              /* Result axis index */
   int target_axis;              /* Target axis index */

/* Initialise the returned values. */
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* Obtain a pointer to the target CmpFrame structure. */
   target = (AstCmpFrame *) target_frame;

/* Obtain the number of axes in the target CmpFrame and in each of its
   component Frames. */
   naxes = astGetNaxes( target );
   naxes1 = astGetNaxes( target->frame1 );
   naxes2 = astGetNaxes( target->frame2 );

/* Obtain the axis permutation array for the target CmpFrame. */
   perm = astGetPerm( target );

/* Determine how any "extra" axes should be associated with existing
   axes (i.e. whether to associate with the preceding or following
   axis). */
   following = astGetMatchEnd( target );

/* Split selected axes into two groups. */
/* ------------------------------------ */
/* Allocate a workspace array to hold the choice of component Frame
   for each selected target axis. */
   frame_choice = astMalloc( sizeof( int ) * (size_t) result_naxes );

/* Obtain an array of flags indicating whether each selected target
   axis should be obtained from the first or second component
   Frame. */
   PartitionSelection( result_naxes, target_axes, perm, naxes1, naxes2,
                       frame_choice, following );

/* Allocate two arrays to hold the axis indices that refer to each of
   the component Frames. The maximum number of indices is given by
   "result_naxes" (if all the selected axes come from one component
   Frame alone). */
   target_axes1 = astMalloc( sizeof( int ) * (size_t) result_naxes );
   target_axes2 = astMalloc( sizeof( int ) * (size_t) result_naxes );

/* If a template Frame has been provided, allocate similar arrays to
   hold the indices of the two groups of template axes. */
   if ( template ) {
      template_axes1 = astMalloc( sizeof( int ) * (size_t) result_naxes );
      template_axes2 = astMalloc( sizeof( int ) * (size_t) result_naxes );
   }

/* Initialise the count of axes selected from each component Frame. */
   if ( astOK ) {
      n1 = n2 = 0;

/* Loop through each axis index to be selected from the CmpFrame. */
      for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {
         target_axis = target_axes[ result_axis ];

/* Determine if the index refers to a valid CmpFrame axis. If it does,
   then permute the index, otherwise set it to -1. */
         if ( ( target_axis >= 0 ) && ( target_axis < naxes ) ) {
            axis_p = perm[ target_axis ];
         } else {
            axis_p = -1;
         }

/* If the axis is to be selected from the first component Frame, store
   the index of the axis to be selected. Also store the associated
   template axis index (if any). */
         if ( frame_choice[ result_axis ] == 1 ) {
            target_axes1[ n1 ] = axis_p;
            if ( template ) {
               template_axes1[ n1 ] = template_axes[ result_axis ];
            }

/* Count the axes selected from the first component Frame. */
            n1++;

/* If the axis is to be selected from the second component Frame,
   store the index of the index to be selected (adjusting for the
   offset in axis numbering). Also store the associated template axis
   index (if any) and count the axes selected. */
         } else {
            target_axes2[ n2 ] = ( axis_p == -1 ) ? -1 : axis_p - naxes1;
            if ( template ) {
               template_axes2[ n2 ] = template_axes[ result_axis ];
            }
            n2++;
         }
      }
   }

/* Select from first component Frame only. */
/* --------------------------------------- */
/* If all the selected axes come from the first component Frame, use
   that Frame's astSubFrame method to select them (and overlay the
   template attributes if required). */
   if ( astOK ) {
      if ( n1 && !n2 ) {
         sub_map1 = NULL;
         match = astSubFrame( target->frame1, template, n1, target_axes1,
                              template_axes1, &sub_map1, result );

/* If this is successful, the "result" Frame will be ready to return
   and "sub_map1" will point at a Mapping that converts from the first
   component Frame to the "result" Frame. We must now modify this
   mapping to account for the CmpFrame's axis permutation array
   (i.e. make it refer back to the CmpFrame's original axis order). */
         if ( astOK && match ) {

/* To do this we must prefix the Mapping with a PermMap which converts
   between the target CmpFrame axes and those of the first component
   Frame. Allocate space for the permutation arrays required. */
            inperm_pref = astMalloc( sizeof( int ) * (size_t) naxes );
            outperm_pref = astMalloc( sizeof( int ) * (size_t) naxes1 );
            if ( astOK ) {

/* Permute each target axis index. */
               for ( target_axis = 0; target_axis < naxes; target_axis++ ) {
                  axis_p = perm[ target_axis ];

/* Set up arrays that describe this permutation and its inverse. */
                  if ( axis_p < naxes1 ) {
                     inperm_pref[ target_axis ] = axis_p;
                     outperm_pref[ axis_p ] = target_axis;

/* Note which target axes do not correspond with axes in the first
   component Frame and assign -1 (so the PermMap will assign "bad"
   coordinate values to these axes). */
                  } else {
                     inperm_pref[ target_axis ] = -1;
                  }
               }

/* Use these permutation arrays to construct the PermMap. Prefix this
   to the Mapping obtained earlier to give the final Mapping to be
   returned. */
               permmap_pref =
                  (AstMapping *) astPermMap( naxes, inperm_pref,
                                             naxes1, outperm_pref, NULL, "" );
               *map = (AstMapping *) astCmpMap( permmap_pref, sub_map1, 1, "" );

/* Annul the PermMap pointer. */
               permmap_pref = astAnnul( permmap_pref );
            }

/* Free the permutation arrays and annul the original Mapping pointer. */
            inperm_pref = astFree( inperm_pref );
            outperm_pref = astFree( outperm_pref );
            sub_map1 = astAnnul( sub_map1 );
         }

/* Select from second component Frame only. */
/* ---------------------------------------- */
/* If all the selected axes come from the second component Frame, use
   that Frame's astSubFrame method to select them (and overlay the
   template attributes if required). */
      } else if ( n2 && !n1 ) {
         sub_map2 = NULL;
         match = astSubFrame( target->frame2, template, n2, target_axes2,
                              template_axes2, &sub_map2, result );

/* If this is successful, the "result" Frame will be ready to return
   and "sub_map2" will point at a Mapping that converts from the second
   component Frame to the "result" Frame. We must now modify this
   mapping to account for the CmpFrame's axis permutation array
   (i.e. make it refer back to the CmpFrame's original axis order). */
         if ( astOK && match ) {

/* To do this we must prefix the Mapping with a PermMap which converts
   between the target CmpFrame axes and those of the second component
   Frame. Allocate space for the permutation arrays required. */
            inperm_pref = astMalloc( sizeof( int ) * (size_t) naxes );
            outperm_pref = astMalloc( sizeof( int ) * (size_t) naxes2 );
            if ( astOK ) {

/* Permute each target axis index. */
               for ( target_axis = 0; target_axis < naxes; target_axis++ ) {
                  axis_p = perm[ target_axis ];

/* Set up arrays that describe this permutation and its inverse,
   allowing for the shift in axis numbering for the second component
   Frame. */
                  if ( axis_p >= naxes1 ) {
                     inperm_pref[ target_axis ] = axis_p - naxes1;
                     outperm_pref[ axis_p - naxes1 ] = target_axis;

/* Note which target axes do not correspond with axes in the second
   component Frame and assign -1 (so the PermMap will assign "bad"
   coordinate values to these axes). */
                  } else {
                     inperm_pref[ target_axis ] = -1;
                  }
               }

/* Use these permutation arrays to construct the PermMap. Prefix this
   to the Mapping obtained earlier to give the final Mapping to be
   returned. */
               permmap_pref =
                  (AstMapping *) astPermMap( naxes, inperm_pref,
                                             naxes2, outperm_pref, NULL, "" );

               *map = (AstMapping *) astCmpMap( permmap_pref, sub_map2, 1, "" );

/* Annul the PermMap pointer. */
               permmap_pref = astAnnul( permmap_pref );
            }

/* Free the permutation arrays and annul the original Mapping pointer. */
            inperm_pref = astFree( inperm_pref );
            outperm_pref = astFree( outperm_pref );
            sub_map2 = astAnnul( sub_map2 );
         }

/* Select from both component Frames. */
/* ---------------------------------- */
/* If the selected axes come from both component Frames, then use both
   Frames' astSubFrame methods to select the required axes from each
   of them (and overlay the template attributes if required). */
      } else {
         sub_map1 = NULL;
         sub_map2 = NULL;
         sub_result1 = NULL;
         sub_result2 = NULL;
         match = astSubFrame( target->frame1, template, n1, target_axes1,
                              template_axes1, &sub_map1, &sub_result1 );
         if ( match ) {
            match = astSubFrame( target->frame2, template, n2, target_axes2,
                                 template_axes2, &sub_map2, &sub_result2 );
         }

/* If this is successful, the two "result" Frames will need to be
   combined together (in a CmpFrame) in order to produce the required
   result, and the two accompanying Mappings will also need to be
   applied in parallel (in a CmpMap). However, the axis order
   resulting from this will still not match that required.

   On the target side, this is because of the target's axis
   permutation array.  On the result side, it is because the result
   axes cannot be inter-mingled (as may be required) simply by joining
   the Frames and Mappings in parallel. The resulting CmpFrame axes
   will therefore need permuting into the required final order. */
         if ( astOK && match ) {

/* In addition, the Mappings will need to be both prefixed and
   suffixed with suitable PermMaps which re-order the axes. Allocate
   space for the permutation arrays required. */
            inperm_pref = astMalloc( sizeof( int ) * (size_t) naxes );
            outperm_pref = astMalloc( sizeof( int ) * (size_t) naxes );
            inperm_suff = astMalloc( sizeof( int ) * (size_t) result_naxes );
            outperm_suff = astMalloc( sizeof( int ) * (size_t) result_naxes );
            if ( astOK ) {

/* Set up permutation arrays to construct the prefix PermMap. This
   simply represents the target CmpFrame's axis permutation array and
   its inverse. */
               for ( target_axis = 0; target_axis < naxes; target_axis++ ) {
                  axis_p = perm[ target_axis ];
                  inperm_pref[ target_axis ] = axis_p;
                  outperm_pref[ axis_p ] = target_axis;
               }

/* Set up permutation arrays to construct the suffix PermMap. This
   represents the way the original axis selections were partitioned
   between the two component frames. */
               i1 = i2 = 0;
               for ( result_axis = 0; result_axis < result_naxes;
                                      result_axis++ ) {

/* For each result axis derived from the first component Frame, set up
   permutation array elements to link the output axis with the next
   component Frame axis. Count the number of component Frame axes
   used. */
                  if ( frame_choice[ result_axis ] == 1 ) {
                     inperm_suff[ i1 ] = result_axis;
                     outperm_suff[ result_axis ] = i1;
                     i1++;

/* Similarly link the axes derived from the second component Frame
   with the appropriate axes of that Frame. */
                  } else {
                     inperm_suff[ n1 + i2 ] = result_axis;
                     outperm_suff[ result_axis ] = n1 + i2;
                     i2++;
                  }
               }

/* Combine the Mappings supplied by the two component Frames in
   parallel. */
               sub_map = (AstMapping *) astCmpMap( sub_map1, sub_map2, 0, "" );

/* Create the PermMaps which are to be used as a prefix and a suffix. */
               permmap_pref =
                  (AstMapping *) astPermMap( naxes, inperm_pref,
                                             naxes, outperm_pref, NULL, "" );
               permmap_suff =
                  (AstMapping *) astPermMap( result_naxes, inperm_suff,
                                             result_naxes, outperm_suff,
                                             NULL, "" );

/* Add the prefix and suffix PermMaps. */
               tmp_map = (AstMapping *) astCmpMap( permmap_pref, sub_map,
                                                   1, "" );
               *map = (AstMapping *) astCmpMap( tmp_map, permmap_suff, 1, "" );

/* Annul the Mapping pointers that are no longer required. */
               sub_map = astAnnul( sub_map );
               permmap_pref = astAnnul( permmap_pref );
               permmap_suff = astAnnul( permmap_suff );
               tmp_map = astAnnul( tmp_map );

/* Create the result CmpFrame by combining the two component result
   Frames and permuting the resulting axes into the required order. */
               *result = (AstFrame *) astCmpFrame( sub_result1, sub_result2,
                                                   "" );
               astPermAxes( *result, outperm_suff );
            }

/* Free the temporary permutation arrays. */
            inperm_pref = astFree( inperm_pref );
            inperm_suff = astFree( inperm_suff );
            outperm_pref = astFree( outperm_pref );
            outperm_suff = astFree( outperm_suff );
         }

/* Annul the Mapping and Frame pointers obtained from each component
   Frame. */
         sub_map1 = astAnnul( sub_map1 );
         sub_map2 = astAnnul( sub_map2 );
         sub_result1 = astAnnul( sub_result1 );
         sub_result2 = astAnnul( sub_result2 );
      }
   }

/* Free the workspace used to store the choice of component Frame and the
   axis indices for each component Frame. */
   frame_choice = astFree( frame_choice );
   target_axes1 = astFree( target_axes1 );
   target_axes2 = astFree( target_axes2 );

/* If necessary, also free the memory used for the template axis
   indices. */
   if ( template ) {
      template_axes1 = astFree( template_axes1 );
      template_axes2 = astFree( template_axes2 );
   }

/* If an error occurred, clean up by annulling the result pointers and
   returning appropriate null values. */
   if ( !astOK ) {
      *map = astAnnul( *map );
      *result = astAnnul( *result );
      match = 0;
   }

/* Return the result. */
   return match;
}

static int TestMaxAxes( AstFrame *this_frame ) {
/*
*  Name:
*     TestMaxAxes

*  Purpose:
*     Test if a value has been set for the MaxAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int TestMaxAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astTestMaxAxes method
*     inherited from the Frame class).

*  Description:
*     This function returns a boolean value indicating if a value has
*     been set for the MaxAxes attribute of a CmpFrame. This will be 1
*     if a value has been set for the MaxAxes attribute of either of
*     its component Frames, otherwise it will be zero.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     1 if a MaxAxes value has been set for either component Frame,
*     otherwise zero.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int result;                   /* Result value to return */

/* Check the global error status. */
   if ( !astOK ) return 0;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* See if a MaxAxes attribute value is set for either component Frame. */
   result = astTestMaxAxes( this->frame1 ) || astTestMaxAxes( this->frame2 );

/* Return the result. */
   return result;
}

static int TestMinAxes( AstFrame *this_frame ) {
/*
*  Name:
*     TestMinAxes

*  Purpose:
*     Test if a value has been set for the MinAxes attribute of a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int TestMinAxes( AstFrame *this )

*  Class Membership:
*     CmpFrame member function (over-rides the astTestMinAxes method
*     inherited from the Frame class).

*  Description:
*     This function returns a boolean value indicating if a value has
*     been set for the MinAxes attribute of a CmpFrame. This will be 1
*     if a value has been set for the MinAxes attribute of either of
*     its component Frames, otherwise it will be zero.

*  Parameters:
*     this
*        Pointer to the CmpFrame.

*  Returned Value:
*     1 if a MinAxes value has been set for either component Frame,
*     otherwise zero.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   int result;                   /* Result value to return */

/* Check the global error status. */
   if ( !astOK ) return 0;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* See if a MinAxes attribute value is set for either component Frame. */
   result = astTestMinAxes( this->frame1 ) || astTestMinAxes( this->frame2 );

/* Return the result. */
   return result;
}

static int Unformat( AstFrame *this_frame, int axis, const char *string,
                     double *value ) {
/*
*  Name:
*     Unformat

*  Purpose:
*     Read a formatted coordinate value for a CmpFrame axis.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     int Unformat( AstFrame *this, int axis, const char *string,
*                   double *value )

*  Class Membership:
*     CmpFrame member function (over-rides the public astUnformat
*     method inherited from the Frame class).

*  Description:
*     This function reads a formatted coordinate value for a CmpFrame
*     axis (supplied as a string) and returns the equivalent numerical
*     value as a double. It also returns the number of characters read
*     from the string.

*  Parameters:
*     this
*        Pointer to the CmpFrame.
*     axis
*        The number of the CmpFrame axis for which the coordinate
*        value is to be read (axis numbering starts at zero for the
*        first axis).
*     string
*        Pointer to a constant null-terminated string containing the
*        formatted coordinate value.
*     value
*        Pointer to a double in which the coordinate value read will be
*        returned.

*  Returned Value:
*     The number of characters read from the string to obtain the
*     coordinate value.

*  Notes:
*     - Any white space at the beginning of the string will be
*     skipped, as also will any trailing white space following the
*     coordinate value read. The function's return value will reflect
*     this.
*     - A function value of zero (and no coordinate value) will be
*     returned, without error, if the string supplied does not contain
*     a suitably formatted value.
*     - The string "<bad>" is recognised as a special case and will
*     generate the value AST__BAD, without error. The test for this
*     string is case-insensitive and permits embedded white space.
*     - A function result of zero will be returned and no coordinate
*     value will be returned via the "value" pointer if this function
*     is invoked with the global error status set, or if it should
*     fail for any reason.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */
   AstFrame *frame;              /* Pointer to Frame containing axis */
   double coord;                 /* Coordinate value read */
   int naxes1;                   /* Number of axes in frame1 */
   int nc;                       /* Number of characters read */
   int set;                      /* Digits attribute set? */

/* Initialise. */
   nc = 0;

/* Check the global error status. */
   if ( !astOK ) return nc;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_frame;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astUnformat" );

/* Determine the number of axes in the first component Frame. */
   naxes1 = astGetNaxes( this->frame1 );
   if ( astOK ) {

/* Decide which component Frame contains the axis and adjust the axis
   index if necessary. */
      frame = ( axis < naxes1 ) ? this->frame1 : this->frame2;
      axis = ( axis < naxes1 ) ? axis : axis - naxes1;

/* Since the component Frame is "managed" by the enclosing CmpFrame,
   we next test if any Frame attributes which may affect the result
   are undefined (i.e. have not been explicitly set). If so, we
   over-ride them, giving them temporary values dictated by the
   CmpFrame. Only the Digits attribute is potentially relevant
   here. */
      set = astTestDigits( frame );
      if ( !set ) astSetDigits( frame, astGetDigits( this ) );

/* Invoke the Frame's astUnformat method to read the coordinate value. */
      nc = astUnformat( frame, axis, string, &coord );

/* Clear Frame attributes which were temporarily over-ridden. */
      if ( !set ) astClearDigits( frame );
   }

/* If an error occurred, clear the number of characters read. */
   if ( !astOK ) {
      nc = 0;

/* Otherwise, if characters were read, return the coordinate value. */
   } else if ( nc ) {
      *value = coord;
   }

/* Return the number of chracters read. */
   return nc;
}

/* Functions which access class attributes. */
/* ---------------------------------------- */
/* Implement member functions to access the attributes associated with
   the axes of a CmpFrame using the private macros defined for this
   purpose at the start of this file. */

/* Direction(axis). */
/* ---------------- */
MAKE_CLEAR(Direction)
MAKE_GET(Direction,int,0,0,0)
MAKE_SET(Direction,int)
MAKE_TEST(Direction)

/* Format(axis). */
/* ------------- */
MAKE_CLEAR(Format)
MAKE_GET(Format,const char *,NULL,0,NULL)
MAKE_SET(Format,const char *)
MAKE_TEST(Format)

/* Label(axis). */
/* ------------ */
MAKE_CLEAR(Label)

/* Over-ride the default axis labels produced by Frame class objects
   and substitute the axis numbering of the enclosing CmpFrame
   instead. */
static const char *label_class;
MAKE_GET(Label,const char *,NULL,( label_class = astGetClass( frame ),
                                   ( astOK && !strcmp( label_class,
                                                       "Frame" ) ) ),
         ( (void) sprintf( label_buff, "Axis %d", axis + 1 ), label_buff ))
MAKE_SET(Label,const char *)
MAKE_TEST(Label)

/* Symbol(axis). */
/* ------------- */
MAKE_CLEAR(Symbol)

/* Over-ride the default axis symbols produced by Frame class objects
   and substitute the axis numbering of the enclosing CmpFrame
   instead. */
static const char *symbol_class;
MAKE_GET(Symbol,const char *,NULL,( symbol_class = astGetClass( frame ),
                                    ( astOK && !strcmp( symbol_class,
                                                        "Frame" ) ) ),
         ( (void) sprintf( symbol_buff, "x%d", axis + 1 ), symbol_buff ))
MAKE_SET(Symbol,const char *)
MAKE_TEST(Symbol)

/* Unit(axis). */
/* ----------- */
MAKE_CLEAR(Unit)
MAKE_GET(Unit,const char *,NULL,0,NULL)
MAKE_SET(Unit,const char *)
MAKE_TEST(Unit)

/* Copy constructor. */
/* ----------------- */
static void Copy( const AstObject *objin, AstObject *objout ) {
/*
*  Name:
*     Copy

*  Purpose:
*     Copy constructor for CmpFrame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Copy( const AstObject *objin, AstObject *objout )

*  Description:
*     This function implements the copy constructor for CmpFrame objects.

*  Parameters:
*     objin
*        Pointer to the object to be copied.
*     objout
*        Pointer to the object being constructed.

*  Notes:
*     -  This constructor makes a deep copy.
*/

/* Local Variables: */
   AstCmpFrame *in;              /* Pointer to input CmpFrame */
   AstCmpFrame *out;             /* Pointer to output CmpFrame */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain pointers to the input and output CmpFrames. */
   in = (AstCmpFrame *) objin;
   out = (AstCmpFrame *) objout;

/* Copy the two component Frames. */
   out->frame1 = astCopy( in->frame1 );
   out->frame2 = astCopy( in->frame2 );

/* Determine the number of axes and copy the axis permutation
   array. */
   out->perm = astStore( NULL, in->perm, sizeof( int ) *
                         (size_t) GetNaxes( (AstFrame *) in ) );
}

/* Destructor. */
/* ----------- */
static void Delete( AstObject *obj ) {
/*
*  Name:
*     Delete

*  Purpose:
*     Destructor for CmpFrame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Delete( AstObject *obj )

*  Description:
*     This function implements the destructor for CmpFrame objects.

*  Parameters:
*     obj
*        Pointer to the object to be deleted.

*  Notes:
*     This function attempts to execute even if the global error
*     status is set.
*/

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to CmpFrame structure */

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) obj;

/* Annul the two component Frame pointers. */
   if ( this->frame1 ) this->frame1 = astAnnul( this->frame1 );
   if ( this->frame2 ) this->frame2 = astAnnul( this->frame2 );

/* Free the axis permutation array. */
   if ( this->perm ) this->perm = astFree( this->perm );
}

/* Dump function. */
/* -------------- */
static void Dump( AstObject *this_object, AstChannel *channel ) {
/*
*  Name:
*     Dump

*  Purpose:
*     Dump function for CmpFrame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Dump( AstObject *this, AstChannel *channel )

*  Description:
*     This function implements the Dump function which writes out data
*     for the CmpFrame class to an output Channel.

*  Parameters:
*     this
*        Pointer to the CmpFrame whose data are being written.
*     channel
*        Pointer to the Channel to which the data are being written.
*/

/* Local Constants: */
#define COMMENT_LEN 150          /* Maximum length of a comment string */
#define KEY_LEN 50               /* Maximum length of a keyword */

/* Local Variables: */
   AstCmpFrame *this;            /* Pointer to the CmpFrame structure */
   char comment[ COMMENT_LEN + 1 ]; /* Buffer for comment strings */
   char key[ KEY_LEN + 1 ];      /* Buffer for keywords */
   int axis;                     /* Loop counter for CmpFrame axes */
   int full;                     /* Full attribute value */
   int full_set;                 /* Full attribute set? */
   int ival;                     /* Integer value */
   int naxes;                    /* Number of CmpFrame axes */
   int set;                      /* Attribute value set? */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the CmpFrame structure. */
   this = (AstCmpFrame *) this_object;

/* Write out values representing the instance variables for the
   CmpFrame class.  Accompany these with appropriate comment strings,
   possibly depending on the values being written.*/

/* In the case of attributes, we first use the appropriate (private)
   Test...  member function to see if they are set. If so, we then use
   the (private) Get... function to obtain the value to be written
   out.

   For attributes which are not set, we use the astGet... method to
   obtain the value instead. This will supply a default value
   (possibly provided by a derived class which over-rides this method)
   which is more useful to a human reader as it corresponds to the
   actual default attribute value.  Since "set" will be zero, these
   values are for information only and will not be read back. */

/* Axis permutation array. */
/* ----------------------- */
/* Obtain the number of CmpFrame axes. */
   naxes = GetNaxes( (AstFrame *) this );

/* Write out the CmpFrame axis permutation array value for each axis,
   converting to 1-based axis numbering. */
   for ( axis = 0; axis < naxes; axis++ ) {
      set = ( this->perm[ axis ] != axis );
      ival = this->perm[ axis ] + 1;

/* Create a keyword and comment appropriate to the axis. */
      (void) sprintf( key, "Axp%d", axis + 1 );
      if ( set ) {
         (void) sprintf( comment,
                         "Axis %d permuted to use internal axis %d",
                         axis + 1, ival );
      } else {
         (void) sprintf( comment, "Axis %d not permuted", axis + 1 );
      }
      astWriteInt( channel, key, set, 0, ival, comment );
   }

/* Component Frames. */
/* ----------------- */
/* Temporarily set the Channel's Full attribute to -1 (unless it is +1
   to start with), remembering the original setting. This prevents any
   unnecessary "un-set" Frame values being output that would otherwise
   simply duplicate the CmpFrame's attributes which have already been
   written. "Set" Frame values are still written, however (and all
   values are written if Full is set to 1). */
   full_set = astTestFull( channel );
   full = astGetFull( channel );
   if ( full <= 0 ) astSetFull( channel, -1 );

/* Write out Object descriptions for the two component Frames. */
   astWriteObject( channel, "FrameA", 1, 1, this->frame1,
                   "First component Frame" );
   astWriteObject( channel, "FrameB", 1, 1, this->frame2,
                   "Second component Frame" );

/* Restore the Channel's original Full attribute setting. */
   if ( full_set ) {
      astSetFull( channel, full );
   } else {
      astClearFull( channel );
   }

/* Undefine macros local to this function. */
#undef COMMENT_LEN
#undef KEY_LEN
}

/* Standard class functions. */
/* ========================= */
/* Implement the astIsACmpFrame and astCheckCmpFrame functions using the macros
   defined for this purpose in the "object.h" header file. */
astMAKE_ISA(CmpFrame,Frame,check,&class_init)
astMAKE_CHECK(CmpFrame)

AstCmpFrame *astCmpFrame_( void *frame1_void, void *frame2_void,
                           const char *options, ... ) {
/*
*++
*  Name:
c     astCmpFrame
f     AST_CMPFRAME

*  Purpose:
*     Create a CmpFrame.

*  Type:
*     Public function.

*  Synopsis:
c     #include "cmpframe.h"
c     AstCmpFrame *astCmpFrame( AstFrame *frame1, AstFrame *frame2,
c                               const char *options, ... )
f     RESULT = AST_CMPFRAME( FRAME1, FRAME2, OPTIONS, STATUS )

*  Class Membership:
*     CmpFrame constructor.

*  Description:
*     This function creates a new CmpFrame and optionally initialises
*     its attributes.
*
*     A CmpFrame is a compound Frame which allows two component Frames
*     (of any class) to be merged together to form a more complex
*     Frame. The axes of the two component Frames then appear together
*     in the resulting CmpFrame (those of the first Frame, followed by
*     those of the second Frame).
*
*     Since a CmpFrame is itself a Frame, it can be used as a
*     component in forming further CmpFrames. Frames of arbitrary
*     complexity may be built from simple individual Frames in this
*     way.

*  Parameters:
c     frame1
f     FRAME1 = INTEGER (Given)
*        Pointer to the first component Frame.
c     frame2
f     FRAME2 = INTEGER (Given)
*        Pointer to the second component Frame.
c     options
f     OPTIONS = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated string containing an optional
c        comma-separated list of attribute assignments to be used for
c        initialising the new CmpFrame. The syntax used is identical to
c        that for the astSet function and may include "printf" format
c        specifiers identified by "%" symbols in the normal way.
f        A character string containing an optional comma-separated
f        list of attribute assignments to be used for initialising the
f        new CmpFrame. The syntax used is identical to that for the
f        AST_SET routine.
c     ...
c        If the "options" string contains "%" format specifiers, then
c        an optional list of additional arguments may follow it in
c        order to supply values to be substituted for these
c        specifiers. The rules for supplying these are identical to
c        those for the astSet function (and for the C "printf"
c        function).
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astCmpFrame()
f     AST_CMPFRAME = INTEGER
*        A pointer to the new CmpFrame.

*  Notes:
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.
*--

*  Implementation Notes:
*     - This function implements the basic CmpFrame constructor which
*     is available via the protected interface to the CmpFrame class.
*     A public interface is provided by the astCmpFrameId_ function.
*     - Because this function has a variable argument list, it is
*     invoked by a macro that evaluates to a function pointer (not a
*     function invocation) and no checking or casting of arguments is
*     performed before the function is invoked. Because of this, the
*     "frame1" and "frame2" parameters are of type (void *) and are
*     converted and validated within the function itself.
*/

/* Local Variables: */
   AstCmpFrame *new;             /* Pointer to new CmpFrame */
   AstFrame *frame1;             /* Pointer to first Frame structure */
   AstFrame *frame2;             /* Pointer to second Frame structure */
   va_list args;                 /* Variable argument list */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Obtain and validate pointers to the Frame structures provided. */
   frame1 = astCheckFrame( frame1_void );
   frame2 = astCheckFrame( frame2_void );
   if ( astOK ) {

/* Initialise the CmpFrame, allocating memory and initialising the
   virtual function table as well if necessary. */
      new = astInitCmpFrame( NULL, sizeof( AstCmpFrame ), !class_init,
                             &class_vtab, "CmpFrame", frame1, frame2 );

/* If successful, note that the virtual function table has been
   initialised. */
      if ( astOK ) {
         class_init = 1;

/* Obtain the variable argument list and pass it along with the
   options string to the astVSet method to initialise the new
   CmpFrame's attributes. */
         va_start( args, options );
         astVSet( new, options, args );
         va_end( args );

/* If an error occurred, clean up by deleting the new object. */
         if ( !astOK ) new = astDelete( new );
      }
   }

/* Return a pointer to the new CmpFrame. */
   return new;
}

AstCmpFrame *astCmpFrameId_( void *frame1_void, void *frame2_void,
                             const char *options, ... ) {
/*
*  Name:
*     astCmpFrameId_

*  Purpose:
*     Create a CmpFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "cmpframe.h"
*     AstCmpFrame *astCmpFrameId_( void *frame1_void, void *frame2_void,
*                                  const char *options, ... )

*  Class Membership:
*     CmpFrame constructor.

*  Description:
*     This function implements the external (public) interface to the
*     astCmpFrame constructor function. It returns an ID value
*     (instead of a true C pointer) to external users, and must be
*     provided because astCmpFrame_ has a variable argument list which
*     cannot be encapsulated in a macro (where this conversion would
*     otherwise occur). For the same reason, the "frame1" and "frame2"
*     parameters are of type (void *) and are converted and validated
*     within the function itself.
*
*     The variable argument list also prevents this function from
*     invoking astCmpFrame_ directly, so it must be a
*     re-implementation of it in all respects, except for the final
*     conversion of the result to an ID value.

*  Parameters:
*     As for astCmpFrame_.

*  Returned Value:
*     The ID value associated with the new CmpFrame.
*/

/* Local Variables: */
   AstCmpFrame *new;             /* Pointer to new CmpFrame */
   AstFrame *frame1;             /* Pointer to first Frame structure */
   AstFrame *frame2;             /* Pointer to second Frame structure */
   va_list args;                 /* Variable argument list */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Obtain the Frame pointers from the ID's supplied and validate the
   pointers to ensure they identify valid Frames. */
   frame1 = astCheckFrame( astMakePointer( frame1_void ) );
   frame2 = astCheckFrame( astMakePointer( frame2_void ) );
   if ( astOK ) {

/* Initialise the CmpFrame, allocating memory and initialising the
   virtual function table as well if necessary. */
      new = astInitCmpFrame( NULL, sizeof( AstCmpFrame ), !class_init,
                             &class_vtab, "CmpFrame", frame1, frame2 );

/* If successful, note that the virtual function table has been
   initialised. */
      if ( astOK ) {
         class_init = 1;

/* Obtain the variable argument list and pass it along with the
   options string to the astVSet method to initialise the new
   CmpFrame's attributes. */
         va_start( args, options );
         astVSet( new, options, args );
         va_end( args );

/* If an error occurred, clean up by deleting the new object. */
         if ( !astOK ) new = astDelete( new );
      }
   }

/* Return an ID value for the new CmpFrame. */
   return astMakeId( new );
}

AstCmpFrame *astInitCmpFrame_( void *mem, size_t size, int init,
                               AstCmpFrameVtab *vtab, const char *name,
                               AstFrame *frame1, AstFrame *frame2 ) {
/*
*+
*  Name:
*     astInitCmpFrame

*  Purpose:
*     Initialise a CmpFrame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "cmpframe.h"
*     AstCmpFrame *astInitCmpFrame( void *mem, size_t size, int init,
*                                   AstCmpFrameVtab *vtab, const char *name )

*  Class Membership:
*     CmpFrame initialiser.

*  Description:
*     This function is provided for use by class implementations to
*     initialise a new CmpFrame object. It allocates memory (if
*     necessary) to accommodate the CmpFrame plus any additional data
*     associated with the derived class.  It then initialises a
*     CmpFrame structure at the start of this memory. If the "init"
*     flag is set, it also initialises the contents of a virtual
*     function table for a CmpFrame at the start of the memory passed
*     via the "vtab" parameter.

*  Parameters:
*     mem
*        A pointer to the memory in which the CmpFrame is to be
*        created. This must be of sufficient size to accommodate the
*        CmpFrame data (sizeof(CmpFrame)) plus any data used by the
*        derived class. If a value of NULL is given, this function
*        will allocate the memory itself using the "size" parameter to
*        determine its size.
*     size
*        The amount of memory used by the CmpFrame (plus derived class
*        data).  This will be used to allocate memory if a value of
*        NULL is given for the "mem" parameter. This value is also
*        stored in the CmpFrame structure, so a valid value must be
*        supplied even if not required for allocating memory.
*     init
*        A logical flag indicating if the CmpFrame's virtual function
*        table is to be initialised. If this value is non-zero, the
*        virtual function table will be initialised by this function.
*     vtab
*        Pointer to the start of the virtual function table to be
*        associated with the new CmpFrame.
*     name
*        Pointer to a constant null-terminated character string which
*        contains the name of the class to which the new object
*        belongs (it is this pointer value that will subsequently be
*        returned by the Object astClass function).

*  Returned Value:
*     A pointer to the new CmpFrame.

*  Notes:
*     - A null pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Variables: */
   AstCmpFrame *new;             /* Pointer to new CmpFrame */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of CmpFrame axes */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Initialise a Frame structure (the parent class) as the first
   component within the CmpFrame structure, allocating memory if
   necessary. Set the number of Frame axes to zero, since all axis
   information is stored within the component Frames. */
   new = (AstCmpFrame *) astInitFrame( mem, size, init, (AstFrameVtab *) vtab,
                                       name, 0 );

/* If necessary, initialise the virtual function table. */
/* ---------------------------------------------------- */
   if ( init ) InitVtab( vtab );
   if ( astOK ) {

/* Initialise the CmpFrame data. */
/* ----------------------------- */
/* Clone the component Frame pointers. */
      new->frame1 = astClone( frame1 );
      new->frame2 = astClone( frame2 );

/* Determine the number of CmpFrame axes. */
      naxes = astGetNaxes( frame1 ) + astGetNaxes( frame2 );

/* Allocate memory to hold the axis permutation array and initialise
   this array. */
      new->perm = astMalloc( sizeof( int ) * (size_t) naxes );
      if ( astOK ) {
         for ( axis = 0; axis < naxes; axis++ ) new->perm[ axis ] = axis;
      }

/* If an error occurred, clean up by deleting the new object. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return a pointer to the new object. */
   return new;
}

AstCmpFrame *astLoadCmpFrame_( void *mem, size_t size, int init,
                               AstCmpFrameVtab *vtab, const char *name,
                               AstChannel *channel ) {
/*
*+
*  Name:
*     astLoadCmpFrame

*  Purpose:
*     Load a CmpFrame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "cmpframe.h"
*     AstCmpFrame *astLoadCmpFrame( void *mem, size_t size, int init,
*                                   AstCmpFrameVtab *vtab, const char *name,
*                                   AstChannel *channel )

*  Class Membership:
*     CmpFrame loader.

*  Description:
*     This function is provided to load a new CmpFrame using data read
*     from a Channel. It first loads the data used by the parent class
*     (which allocates memory if necessary) and then initialises a
*     CmpFrame structure in this memory, using data read from the
*     input Channel.
*
*     If the "init" flag is set, it also initialises the contents of a
*     virtual function table for a CmpFrame at the start of the memory
*     passed via the "vtab" parameter.

*  Parameters:
*     mem
*        A pointer to the memory into which the CmpFrame is to be
*        loaded.  This must be of sufficient size to accommodate the
*        CmpFrame data (sizeof(CmpFrame)) plus any data used by
*        derived classes. If a value of NULL is given, this function
*        will allocate the memory itself using the "size" parameter to
*        determine its size.
*     size
*        The amount of memory used by the CmpFrame (plus derived class
*        data).  This will be used to allocate memory if a value of
*        NULL is given for the "mem" parameter. This value is also
*        stored in the CmpFrame structure, so a valid value must be
*        supplied even if not required for allocating memory.
*
*        If the "vtab" parameter is NULL, the "size" value is ignored
*        and sizeof(AstCmpFrame) is used instead.
*     init
*        A boolean flag indicating if the CmpFrame's virtual function
*        table is to be initialised. If this value is non-zero, the
*        virtual function table will be initialised by this function.
*
*        If the "vtab" parameter is NULL, the "init" value is ignored
*        and the (static) virtual function table initialisation flag
*        for the CmpFrame class is used instead.
*     vtab
*        Pointer to the start of the virtual function table to be
*        associated with the new CmpFrame. If this is NULL, a pointer
*        to the (static) virtual function table for the CmpFrame class
*        is used instead.
*     name
*        Pointer to a constant null-terminated character string which
*        contains the name of the class to which the new object
*        belongs (it is this pointer value that will subsequently be
*        returned by the astGetClass method).
*
*        If the "vtab" parameter is NULL, the "name" value is ignored
*        and a pointer to the string "CmpFrame" is used instead.

*  Returned Value:
*     A pointer to the new CmpFrame.

*  Notes:
*     - A null pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Constants: */
#define KEY_LEN 50               /* Maximum length of a keyword */

/* Local Variables: */
   AstCmpFrame *new;             /* Pointer to the new CmpFrame */
   char key[ KEY_LEN + 1 ];      /* Buffer for keywords */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of CmpFrame axes */

/* Initialise. */
   new = NULL;

/* Check the global error status. */
   if ( !astOK ) return new;

/* If a NULL virtual function table has been supplied, then this is
   the first loader to be invoked for this CmpFrame. In this case the
   CmpFrame belongs to this class, so supply appropriate values to be
   passed to the parent class loader (and its parent, etc.). */
   if ( !vtab ) {
      size = sizeof( AstCmpFrame );
      init = !class_init;
      vtab = &class_vtab;
      name = "CmpFrame";
   }

/* Invoke the parent class loader to load data for all the ancestral
   classes of the current one, returning a pointer to the resulting
   partly-built CmpFrame. */
   new = astLoadFrame( mem, size, init, (AstFrameVtab *) vtab, name,
                       channel );

/* If required, initialise the part of the virtual function table used
   by this class. */
   if ( init ) InitVtab( vtab );

/* Note if we have successfully initialised the (static) virtual
   function table owned by this class (so that this is done only
   once). */
   if ( astOK ) {
      if ( ( vtab == &class_vtab ) && init ) class_init = 1;

/* Read input data. */
/* ================ */
/* Request the input Channel to read all the input data appropriate to
   this class into the internal "values list". */
      astReadClassData( channel, "CmpFrame" );

/* Now read each individual data item from this list and use it to
   initialise the appropriate instance variable(s) for this class. */

/* In the case of attributes, we first read the "raw" input value,
   supplying the "unset" value as the default. If a "set" value is
   obtained, we then use the appropriate (private) Set... member
   function to validate and set the value properly. */

/* Component Frames. */
/* ----------------- */
/* Read both component Frames, supplying a default 1-dimensional Frame
   if necessary. */
      new->frame1 = astReadObject( channel, "framea", NULL );
      if ( !new->frame1 ) new->frame1 = astFrame( 1, "" );

      new->frame2 = astReadObject( channel, "frameb", NULL );
      if ( !new->frame2 ) new->frame2 = astFrame( 1, "" );

/* Axis permutation array. */
/* ----------------------- */
/* Obtain the number of CmpFrame axes and allocate memory to hold the
   axis permutation array. */
      naxes = GetNaxes( (AstFrame *) new );
      new->perm = astMalloc( sizeof( int ) * (size_t) naxes );

/* If OK, loop to read the array value for each axis. */
      if ( astOK ) {
         for ( axis = 0; axis < naxes; axis++ ) {

/* Convert from 1-based to zero-based axis numbering at this
   point. The default is the "un-permuted" value. */
            sprintf( key, "axp%d", axis + 1 );
            new->perm[ axis ] = astReadInt( channel, key, axis + 1 ) - 1;

/* Quit looping if an error occurs. */
            if ( !astOK ) break;
         }
      }

/* If an error occurred, clean up by deleting the new CmpFrame. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return the new CmpFrame pointer. */
   return new;

/* Undefine macros local to this function. */
#undef KEY_LEN
}

/* Virtual function interfaces. */
/* ============================ */
/* These provide the external interface to the virtual functions defined by
   this class. Each simply checks the global error status and then locates and
   executes the appropriate member function, using the function pointer stored
   in the object's virtual function table (this pointer is located using the
   astMEMBER macro defined in "object.h").

   Note that the member function may not be the one defined here, as it may
   have been over-ridden by a derived class. However, it should still have the
   same interface. */
