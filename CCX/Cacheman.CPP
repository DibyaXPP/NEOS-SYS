/*
* PROJECT: NEOS System Kernel
* LICENSE: BSD Clause 2 / Apache License Version 2.0
* PURPOSE: Implements Cache Manager 
* NT KERNEL: 5.2.9360
* SERVICE PACK: 3
* PLATFORM BUILD : XAlpha 0.1
* COPYRIGHT: Copyright 2023-2029 Dibymartanda Samanta <>
*/


#include <ntoskrnl.hpp>
#define NDEBUG
#include <debug.hpp>
#include <cacheman.hpp> 


/*CcCanIWrite: MSDN Documentation Exist
* This function checks whether can a caller write to the System Cache 
*  FileObject : Pointer to the File Object in Cache 
*  BytesToWrite: Amounts of Byte to be written 
*  Wait: If Caller can wait 
*  Retrying: Check if caller is trying it for first time. False for First Try 
*  DEFERRED_WRITE _LIST_ENTRY Que is used to maintain stability such that Small request don't overwrite first one
*/

bool CacheManager::CcCanIWrite(in PFILE_OBJECT FileObject, in ULONG BytesToWrite, in bool Wait,in UCHAR  Retrying)
{
	PSECTION_OBJECT_POINTERS ObjectPointer; 
    /*Specific Data Structure Has been mentioned by NirSoft*/
    DEFERRED_WRITE  ListEntry;  
    if(wait)
    {

	/* Check if DEFERRED_WRITE List is empty */
	if ( IsListEmpty(&CcDeferredWrites) )
    {
      CcScheduleLazyWriteScan(true);
      KeReleaseQueuedSpinLock(LockQueueMasterLock, KeAcquireQueuedSpinLock(LockQueueMasterLock));
    }

	if(wait)
	{

	//https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntkeapi_x/kspin_lock_queue_number.htm
	/* Lazy Writer Scan  happens now */
	if ( IsListEmpty(&CcDeferredWrites) )
    {
      KIRQL  SpinLockBegin = KeAcquireQueuedSpinLock(LockQueueMasterLock); 
      CcScheduleLazyWriteScan(true);
      KeReleaseQueuedSpinLock(LockQueueMasterLock, SpinLockBegin);
    }

    /* Check If New entry */
    if (Retrying)
    {
        ExfInterlockedInsertHeadList(&CcDeferredWrites, &ListEntry, &CcDeferredWriteSpinLock);
    }
    else
    {
        ExfInterlockedInsertTailList(&CcDeferredWrites, &ListEntry, &CcDeferredWriteSpinLock);
    }
    do
        CcPostDeferredWrites();
    while (KeWaitForSingleObject(Object, Executive, 0, 0, &CcIdleDelay));

    return True;
    }
    

 return False;
    
}

bool CacheManager::CcScheduleLazyWriteScan (BOOLEAN  	NoDelay	) 
{
	return CcScheduleLazyWriteScanEx(NoDelay, 0);
}
bool CacheManager::CcScheduleLazyWriteScanEx (BOOLEAN  	NoDelay	, BOOLEAN LazyWriteEx_Teardowmn) 
{
	BOOLEAN Result;
	if(NoDelay)
	{
		LazyWriter.Active = 1;
		if(LazyWriteEx)
		{
			LazyWriter.PendingTeardown = 1;
		}
		result = KeSetTimer(&LazyWriter.Timer, CcNoDelay, &LazyWriter.Dpc);
	}
	else if (LazyWriter.Active)
	{
		result = KeSetTimer(&LazyWriter.Timer, CcIdleDelay, &LazyWriter.Dpc);
	}
	else
	{
		result = KeSetTimer(&LazyWriter.Timer, CcFirstDelay, &LazyWriter.Dpc);
	}
	return result;
}

/* This Function Set Event for Cachemanager Errors/Notifcation */
/* It receives parameter Event , which points to respective Error or Event for Cache Writer */

LONG NTAPI CcNotifyWriteBehind(int Event)
{
  LONG result; 
  
  if ( Event & 1 )
    result = KeSetEvent(&CcLowMemoryEvent, 0, 0);
  if ( Event & 2 )
    result = KeSetEvent(&CcPowerEvent, 0, 0);
  if ( Event & 4 )
    result = KeSetEvent(&CcPeriodicEvent, 0, 0);
  if ( Event & 8 )
    result = KeSetEvent(&CcWaitingForTeardownEvent, 0, 0);
  return result;
}




BOOLEAN  // done 
NTAPI
CcCanIWrite(
			IN	PFILE_OBJECT	FileObject,
			IN	ULONG			BytesToWrite,
			IN	BOOLEAN			Wait,
			IN	BOOLEAN			Retrying)
{
	UNIMPLEMENTED;
	return FALSE;
}

BOOLEAN NTAPI
CcCopyRead (IN PFILE_OBJECT FileObject,
	    IN PLARGE_INTEGER FileOffset,
	    IN ULONG Length,
	    IN BOOLEAN Wait,
	    OUT PVOID Buffer,
	    OUT PIO_STATUS_BLOCK IoStatus)
{
    UNIMPLEMENTED;
    return TRUE;
}

BOOLEAN NTAPI
CcCopyWrite (IN PFILE_OBJECT FileObject,
	     IN PLARGE_INTEGER FileOffset,
	     IN ULONG Length,
	     IN BOOLEAN Wait,
	     IN PVOID Buffer)
{
    UNIMPLEMENTED;
    return TRUE;
}

VOID
NTAPI
CcDeferWrite (
	IN	PFILE_OBJECT		FileObject,
	IN	PCC_POST_DEFERRED_WRITE	PostRoutine,
	IN	PVOID			Context1,
	IN	PVOID			Context2,
	IN	ULONG			BytesToWrite,
	IN	BOOLEAN			Retrying
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcFastCopyRead(
    IN  PFILE_OBJECT FileObject,
    IN  ULONG FileOffset,
    IN  ULONG Length,
    IN  ULONG PageCount,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcFastCopyWrite(
    IN  PFILE_OBJECT FileObject,
    IN  ULONG FileOffset,
    IN  ULONG Length,
    IN  PVOID Buffer)
{
	UNIMPLEMENTED;
}

NTSTATUS
NTAPI
CcWaitForCurrentLazyWriterActivity (
    VOID
    )
{
	UNIMPLEMENTED;
	return STATUS_NOT_IMPLEMENTED;
}

BOOLEAN
NTAPI
CcZeroData (IN PFILE_OBJECT     FileObject,
	    IN PLARGE_INTEGER   StartOffset,
	    IN PLARGE_INTEGER   EndOffset,
	    IN BOOLEAN          Wait)
{
    UNIMPLEMENTED;
    return TRUE;
}

VOID NTAPI
CcFlushCache(IN PSECTION_OBJECT_POINTERS SectionObjectPointers,
             IN PLARGE_INTEGER FileOffset OPTIONAL,
             IN ULONG Length,
             OUT PIO_STATUS_BLOCK IoStatus)
{
    UNIMPLEMENTED;
}

LARGE_INTEGER
NTAPI
CcGetDirtyPages (
	IN	PVOID			LogHandle,
	IN	PDIRTY_PAGE_ROUTINE	DirtyPageRoutine,
	IN	PVOID			Context1,
	IN	PVOID			Context2
	)
{
	LARGE_INTEGER i;
	UNIMPLEMENTED;
	i.QuadPart = 0;
	return i;
}

PFILE_OBJECT
NTAPI
CcGetFileObjectFromBcb (
	IN	PVOID	Bcb
	)
{
    UNIMPLEMENTED;
	return NULL;
}

LARGE_INTEGER
NTAPI
CcGetLsnForFileObject (
	IN	PFILE_OBJECT	FileObject,
	OUT	PLARGE_INTEGER	OldestLsn OPTIONAL
	)
{
	LARGE_INTEGER i;
	UNIMPLEMENTED;
	i.QuadPart = 0;
	return i;
}

VOID
NTAPI
CcInitializeCacheMap (
	IN	PFILE_OBJECT			FileObject,
	IN	PCC_FILE_SIZES			FileSizes,
	IN	BOOLEAN				PinAccess,
	IN	PCACHE_MANAGER_CALLBACKS	CallBacks,
	IN	PVOID				LazyWriterContext
	)
{
    UNIMPLEMENTED;
}

BOOLEAN
NTAPI
CcIsThereDirtyData (
	IN	PVPB	Vpb
	)
{
	UNIMPLEMENTED;
	return FALSE;
}

BOOLEAN
NTAPI
CcPurgeCacheSection (
	IN	PSECTION_OBJECT_POINTERS	SectionObjectPointer,
	IN	PLARGE_INTEGER			FileOffset OPTIONAL,
	IN	ULONG				Length,
	IN	BOOLEAN				UninitializeCacheMaps
	)
{
	UNIMPLEMENTED;
	return FALSE;
}


VOID NTAPI
CcSetFileSizes (IN PFILE_OBJECT FileObject,
		IN PCC_FILE_SIZES FileSizes)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcSetLogHandleForFile (
	IN	PFILE_OBJECT	FileObject,
	IN	PVOID		LogHandle,
	IN	PFLUSH_TO_LSN	FlushToLsnRoutine
	)
{
	UNIMPLEMENTED;
}

BOOLEAN
NTAPI
CcUninitializeCacheMap (
	IN	PFILE_OBJECT			FileObject,
	IN	PLARGE_INTEGER			TruncateSize OPTIONAL,
	IN	PCACHE_UNINITIALIZE_EVENT	UninitializeCompleteEvent OPTIONAL
	)
{
	UNIMPLEMENTED;
	return FALSE;
}

PFILE_OBJECT NTAPI
CcGetFileObjectFromSectionPtrs(IN PSECTION_OBJECT_POINTERS SectionObjectPointers)
{
    UNIMPLEMENTED;
    return NULL;
}

LARGE_INTEGER
NTAPI
CcGetFlushedValidData (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN BOOLEAN BcbListHeld
    )
{
	LARGE_INTEGER i;

	UNIMPLEMENTED;

	i.QuadPart = 0;
	return i;
}

PVOID
NTAPI
CcRemapBcb (
    IN PVOID Bcb
    )
{
    UNIMPLEMENTED;
    return 0;
}

VOID
NTAPI
CcScheduleReadAhead (
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcSetAdditionalCacheAttributes (
	IN	PFILE_OBJECT	FileObject,
	IN	BOOLEAN		DisableReadAhead,
	IN	BOOLEAN		DisableWriteBehind
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcSetBcbOwnerPointer (
	IN	PVOID	Bcb,
	IN	PVOID	Owner
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcSetDirtyPageThreshold (
	IN	PFILE_OBJECT	FileObject,
	IN	ULONG		DirtyPageThreshold
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcSetReadAheadGranularity (
	IN	PFILE_OBJECT	FileObject,
	IN	ULONG		Granularity
	)
{
	UNIMPLEMENTED;
}

BOOLEAN
NTAPI
CcMapData(
    IN PFILE_OBJECT FileObject,
	IN PLARGE_INTEGER FileOffset,
	IN ULONG Length,
	IN ULONG Flags,
	OUT PVOID *pBcb,
	OUT PVOID *pBuffer)
{
    UNIMPLEMENTED;
    return FALSE;
}

BOOLEAN
NTAPI
CcPinMappedData (
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length,
	IN	ULONG			Flags,
	OUT	PVOID			* Bcb
	)
{
    UNIMPLEMENTED;
    return TRUE;
}

BOOLEAN
NTAPI
CcPinRead (
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length,
	IN	ULONG			Flags,
	OUT	PVOID			* Bcb,
	OUT	PVOID			* Buffer
	)
{
    UNIMPLEMENTED;
    return FALSE;
}

BOOLEAN
NTAPI
CcPreparePinWrite (
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length,
	IN	BOOLEAN			Zero,
	IN	ULONG			Flags,
	OUT	PVOID			* Bcb,
	OUT	PVOID			* Buffer
	)
{
    UNIMPLEMENTED;
    return FALSE;
}

VOID NTAPI
CcSetDirtyPinnedData (IN PVOID Bcb,
		      IN PLARGE_INTEGER Lsn)
{
    UNIMPLEMENTED;
}


VOID NTAPI
CcUnpinData (IN PVOID Bcb)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcUnpinDataForThread (
	IN	PVOID			Bcb,
	IN	ERESOURCE_THREAD	ResourceThreadId
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcRepinBcb (
	IN	PVOID	Bcb
	)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcUnpinRepinnedBcb (
	IN	PVOID			Bcb,
	IN	BOOLEAN			WriteThrough,
	IN	PIO_STATUS_BLOCK	IoStatus
	)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcMdlRead(
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length,
	OUT	PMDL			* MdlChain,
	OUT	PIO_STATUS_BLOCK	IoStatus
	)
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcMdlReadComplete(IN PFILE_OBJECT FileObject,
                  IN PMDL MdlChain)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcMdlWriteComplete(IN PFILE_OBJECT FileObject,
                   IN PLARGE_INTEGER FileOffset,
                   IN PMDL MdlChain)
{
    UNIMPLEMENTED;
}

VOID
NTAPI
CcMdlWriteAbort (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain
    )
{
	UNIMPLEMENTED;
}

VOID
NTAPI
CcPrepareMdlWrite (
	IN	PFILE_OBJECT		FileObject,
	IN	PLARGE_INTEGER		FileOffset,
	IN	ULONG			Length,
	OUT	PMDL			* MdlChain,
	OUT	PIO_STATUS_BLOCK	IoStatus
	)
{
	UNIMPLEMENTED;
}

// Used internally




/* ExceptionCode ---> is mentioned in Cheched Build of Server 2003*/
/* Exception Record is Publically defined https://learn.microsoft.com/fr-fr/windows/win32/api/winnt/ns-winnt-exception_record */
