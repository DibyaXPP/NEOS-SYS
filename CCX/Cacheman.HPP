/*
* PROJECT: NEOS System Kernel
* LICENSE: BSD Clause 2 / Apache License Version 2.0
* PURPOSE: Implements Cache Manager
* NT KERNEL: 5.2.3960
* SERVICE PACK: 3
* PLATFORM BUILD : XAlpha 0.1
* COPYRIGHT: Copyright 2023-2029 Dibymartanda Samanta <>
*/

typedef struct _EXCEPTION_POINTERS {
	PEXCEPTION_RECORD ExceptionRecord;
	PCONTEXT          ContextRecord;
} EXCEPTION_POINTERS, * XEXCEPTION_POINTERS;

#define EXCEPTION_HANDLER_SUCESS 1 
#define CACHE_PAGE_ERROR -1073741818
Class CacheManager
{
/* Exported by NTOSKRNL */
Public:
	long CcCopyReadExceptionFilter(IN  XEXCEPTION_POINTERS Xpointer , IN PNTSTATUS ExceptionCode)
    bool CcCanIWrite(IN PFILE_OBJECT FileObject, IN ulong BytesToWrite, bool wait, uchar Retrying)
	bool CcCopyRead(IN PFILE_OBJECT  FileObject, IN PLARGE_INTEGER FileOffset, IN ulong Length, IN bool Wait, OUT pvoid Buffer, out PIO_STATUS_BLOCK IoStatus)
    bool CcCopyWrite 
	bool CcDeferWrite 
/*All Internal Functions will Go here */
Private:
}
