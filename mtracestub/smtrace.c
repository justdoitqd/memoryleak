/* debugging hooks for `malloc'.
 * Simon Guo 2013/12
 * GPL
 */


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>

#define TRACE_BUFFER_SIZE 512

#include <sys/types.h>
#include <unistd.h>

// As a future enhancement:
//    Use thread local varaible to avoid thread interlocking while both are malloc/free()
#define GLIBC_FBT_DEPTH 16

// It was found libtten.so doesn't complain to x86 ABI: (Sometimes EBP is not used)
// So a "safety" offset is used to guard this, define it as one page at present
// It is dirty but no other good idea at present
// LincaseSH29_sam_1_1_1 SAM001:> ulimit -s
// 10240
// 10240 / 4 = 2^(8) * 10 = 2560
#define STACK_SAFE_OFFSET 2560
#define GLIBC_GEN_FBT(_fbt) \
do { \
   unsigned long* __ebp = 0; \
   unsigned long* __old_ebp = 0; \
   unsigned long __eip = 0; \
   int __i = 0; \
   long* __p = (long*)(_fbt); \
   for (; __i < GLIBC_FBT_DEPTH; __i++) \
   { \
          if (__i == 0) \
          { \
             __asm__ volatile ("movl $., %0" : "=r" (__eip)); \
             *__p++ = __eip; \
             continue;   \
          } \
          else if (__i == 1) \
          { \  
             __asm__ volatile ("mov %%ebp, %0" : "=r" (__ebp)); \
          } \
          else \
          { \
	    __old_ebp = __ebp; \
	    __ebp = (unsigned long*) (*(unsigned long*)__ebp); \
	    if ((__ebp <= __old_ebp) || ( __ebp > __old_ebp + STACK_SAFE_OFFSET)) \ 
	    { \
		    *__p = 0xdeadbeaf; \
		    break; \
	    } \
          } \
          if (__ebp == 0) \
                   break;  \
          *__p++ = *(__ebp + 1); \
   } \
   *__p = 0; \
} while(0)
	    
long g_glibc_fbt[GLIBC_FBT_DEPTH + 1];


static FILE *mallstream;
static char *malloc_trace_buffer;
static pthread_mutex_t lock;

#define __smtrace_unlock(_lock) \
	pthread_mutex_unlock(&(_lock))

#define __smtrace_lock(_lock) \
	pthread_mutex_lock(&(_lock))


/* File name and line number information, for callers that had
   the foresight to call through a macro.  */
char *_mtrace_file;

/* Old hook values.  */
//#define __ptr_t void *
# define __malloc_size_t size_t
static void (*tr_old_free_hook) (__ptr_t ptr, const __ptr_t);


//static void * (*tr_old_malloc_hook) (__malloc_size_t, const __ptr_t);
static __ptr_t (*tr_old_malloc_hook) (__malloc_size_t size, const __ptr_t);
static __ptr_t (*tr_old_realloc_hook) (__ptr_t ptr, __malloc_size_t size, const __ptr_t);
static __ptr_t (*tr_old_memalign_hook) (__malloc_size_t __alignment, __malloc_size_t __size, __const __ptr_t);

long g_glibc_fbt[GLIBC_FBT_DEPTH + 1];


static FILE *mallstream;
static char *malloc_trace_buffer;
static pthread_mutex_t lock;

#define __smtrace_unlock(_lock) \
	pthread_mutex_unlock(&(_lock))

#define __smtrace_lock(_lock) \
	pthread_mutex_lock(&(_lock))


/* File name and line number information, for callers that had
   the foresight to call through a macro.  */
char *_mtrace_file;

/* Old hook values.  */
# define __malloc_size_t size_t
static void (*tr_old_free_hook) (__ptr_t ptr, const __ptr_t);


static __ptr_t (*tr_old_malloc_hook) (__malloc_size_t size, const __ptr_t);
static __ptr_t (*tr_old_realloc_hook) (__ptr_t ptr, __malloc_size_t size, const __ptr_t);
static __ptr_t (*tr_old_memalign_hook) (__malloc_size_t __alignment, __malloc_size_t __size, __const __ptr_t);

void
tr_break ()
{
}

static void
tr_where (caller)
     const __ptr_t caller;
{
	static pid_t tr_pid = 0;
	if (tr_pid == 0)
	{
		tr_pid = getpid();
	}

	if (caller != NULL)
	{
		fprintf (mallstream, "@|%d| [%p] ", tr_pid, caller);

		// add more function trace:
		g_glibc_fbt[0] = 0;
		g_glibc_fbt[1] = 0;
		g_glibc_fbt[2] = 0;
		g_glibc_fbt[3] = 0;
		g_glibc_fbt[4] = 0;
		g_glibc_fbt[5] = 0;
		g_glibc_fbt[6] = 0;
		g_glibc_fbt[7] = 0;
		g_glibc_fbt[8] = 0;
		g_glibc_fbt[9] = 0;
		g_glibc_fbt[10] = 0;
		g_glibc_fbt[11] = 0;
		g_glibc_fbt[12] = 0;
		g_glibc_fbt[13] = 0;
		g_glibc_fbt[14] = 0;
		g_glibc_fbt[15] = 0;
		g_glibc_fbt[16] = 0;

		GLIBC_GEN_FBT(g_glibc_fbt);
		//The first 3 stack is in this so or glibc
		fprintf(mallstream, " |0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x| ",
				//g_glibc_fbt[0],
				//g_glibc_fbt[1],
				//g_glibc_fbt[2],
				g_glibc_fbt[3],
				g_glibc_fbt[4],
				g_glibc_fbt[5],
				g_glibc_fbt[6],
				g_glibc_fbt[7],
				g_glibc_fbt[8],
				g_glibc_fbt[9],
				g_glibc_fbt[10],
				g_glibc_fbt[11],
				g_glibc_fbt[12],
				g_glibc_fbt[13],
				g_glibc_fbt[14],
				g_glibc_fbt[15],
				g_glibc_fbt[16]);
	}
}

static void
tr_freehook (ptr, caller)
     __ptr_t ptr;
     const __ptr_t caller;
{
  if (ptr == NULL)
    return;

  __smtrace_lock(lock);
  tr_where (caller);
  /* Be sure to print it first.  */
  fprintf (mallstream, "- %p\n", ptr);

  __free_hook = tr_old_free_hook;
  if (tr_old_free_hook != NULL)
    (*tr_old_free_hook) (ptr, caller);
  else
    free (ptr);

  __free_hook = tr_freehook;
  __smtrace_unlock (lock);
}

static __ptr_t
tr_mallochook (size, caller)
     __malloc_size_t size;
     const __ptr_t caller;
{
  __ptr_t hdr;
  __smtrace_lock (lock);

  __malloc_hook = tr_old_malloc_hook;
  if (tr_old_malloc_hook != NULL)
    hdr = (__ptr_t) (*tr_old_malloc_hook) (size, caller);
  else
    hdr = (__ptr_t) malloc (size);
  __malloc_hook = tr_mallochook;

  tr_where (caller);
  /* We could be printing a NULL here; that's OK.  */
  fprintf (mallstream, "+ %p %#lx\n", hdr, (unsigned long int) size);

  __smtrace_unlock (lock);

  return hdr;
}

static __ptr_t
tr_reallochook (ptr, size, caller)
     __ptr_t ptr;
     __malloc_size_t size;
     const __ptr_t caller;
{
  __ptr_t hdr;

  __smtrace_lock (lock);

  __free_hook = tr_old_free_hook;
  __malloc_hook = tr_old_malloc_hook;
  __realloc_hook = tr_old_realloc_hook;
  if (tr_old_realloc_hook != NULL)
    hdr = (__ptr_t) (*tr_old_realloc_hook) (ptr, size, caller);
  else
    hdr = (__ptr_t) realloc (ptr, size);
  __free_hook = tr_freehook;
  __malloc_hook = tr_mallochook;
  __realloc_hook = tr_reallochook;

  tr_where (caller);
  if (hdr == NULL)
    /* Failed realloc.  */
    fprintf (mallstream, "! %p %#lx\n", ptr, (unsigned long int) size);
  else if (ptr == NULL)
    fprintf (mallstream, "+ %p %#lx\n", hdr, (unsigned long int) size);
  else
    {
      fprintf (mallstream, "< %p\n", ptr);
      fprintf (mallstream, "> %p %#lx\n", hdr, (unsigned long int) size);
    }

  __smtrace_unlock (lock);

  return hdr;
}

static __ptr_t
tr_memalignhook (alignment, size, caller)
     __malloc_size_t alignment, size;
     const __ptr_t caller;
{
  __ptr_t hdr;

  __smtrace_lock (lock);

  __memalign_hook = tr_old_memalign_hook;
  __malloc_hook = tr_old_malloc_hook;
  if (tr_old_memalign_hook != NULL)
    hdr = (__ptr_t) (*tr_old_memalign_hook) (alignment, size, caller);
  else
    hdr = (__ptr_t) memalign (alignment, size);
  __memalign_hook = tr_memalignhook;
  __malloc_hook = tr_mallochook;

  tr_where (caller);
  /* We could be printing a NULL here; that's OK.  */
  fprintf (mallstream, "+ %p %#lx\n", hdr, (unsigned long int) size);

  __smtrace_unlock (lock);

  return hdr;
}


static const char mallenv[]= "MALLOC_TRACE";


/* We enable tracing if either the environment variable MALLOC_TRACE
   is set, or if the variable mallwatch has been patched to an address
   that the debugging user wants us to stop on.  When patching mallwatch,
   don't forget to set a breakpoint on tr_break!  */

void
smtrace ()
{
  char *mallfile;

  /* Don't panic if we're called more than once.  */
  if (mallstream != NULL)
	  return;

  if (__malloc_hook == tr_mallochook)
  {
	  //have been under trace
	  return;
  }

  mallfile = getenv (mallenv);
  if (mallfile != NULL)
  {
	  char *mtb = malloc (TRACE_BUFFER_SIZE);
	  if (mtb == NULL)
		  return;

	  mallstream = fopen (mallfile != NULL ? mallfile : "/dev/null", "wc");
	  if (mallstream != NULL)
	  {
		  /* Make sure we close the file descriptor on exec.  */
		  int flags = fcntl (fileno (mallstream), F_GETFD, 0);
		  if (flags >= 0)
		  {
			  flags |= FD_CLOEXEC;
			  fcntl (fileno (mallstream), F_SETFD, flags);
		  }
		  /* Be sure it doesn't malloc its buffer!  */
		  malloc_trace_buffer = mtb;
		  setvbuf (mallstream, malloc_trace_buffer, _IOFBF, TRACE_BUFFER_SIZE);
		  fprintf (mallstream, "= Start\n");
		  tr_old_free_hook = __free_hook;
		  __free_hook = tr_freehook;
		  tr_old_malloc_hook = __malloc_hook;
		  __malloc_hook = tr_mallochook;
		  tr_old_realloc_hook = __realloc_hook;
		  __realloc_hook = tr_reallochook;
		  tr_old_memalign_hook = __memalign_hook;
		  __memalign_hook = tr_memalignhook;
	  }
	  else
		  free (mtb);
  }
}

void
smuntrace ()
{
  if (mallstream == NULL)
    return;

  if (__malloc_hook == tr_mallochook)
  {
	  fprintf (mallstream, "= End\n");
	  fclose (mallstream);
	  mallstream = NULL;
	  __free_hook = tr_old_free_hook;
	  __malloc_hook = tr_old_malloc_hook;
	  __realloc_hook = tr_old_realloc_hook;
	  __memalign_hook = tr_old_memalign_hook;
  }
}
