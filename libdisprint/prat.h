#include <stdlib.h>
#include <stdio.h>

/* Check pointer and...*/
/* evaluate to true/false */
#define CHKPTR(p) !p
/* abort */
#define CHKPTR_A(p) if(CHKPTR(p)) {fprintf(stderr, "nullptr at %p.\n", &p); abort();}
/* return -1 */
#define CHKPTR_R(p) if(CHKPTR(p)) {fprintf(stderr, "nullptr at %p.\n", &p); return -1;}
#define CHKPTR_Rm1(p) if(CHKPTR(p)) {fprintf(stderr, "nullptr at %p.\n", &p); return -1;}
#define CHKPTR_R1(p) if(CHKPTR(p)) {fprintf(stderr, "nullptr at %p.\n", &p); return 1;}
/* a pointer that should NEVER be null */
#define CHKPTR_N(p) if(CHKPTR(p)) {fprintf(stderr, "nullptr at %p.\nCan't happen.\n", &p); abort();}
