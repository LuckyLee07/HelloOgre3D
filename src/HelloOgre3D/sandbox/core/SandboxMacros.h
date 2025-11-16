#ifndef __SANDBOX_MACROS__
#define __SANDBOX_MACROS__

#define SAFE_DELETE(p)       { if(p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);	(p)=NULL; } }

#endif  // __SANDBOX_MACROS__
