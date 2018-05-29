
 -- vTaskDelay(const TickType_t xTicksToDelay)
  |
  | -- xAlreadyYielded = pdFALSE;
  |
  | -- if (xTicksToDelay > 0U)   // A delay time of zero just forces a reschedule
	 |
	 | -- confifASSERT(uxSchedulerSuspended == 0)	// uxSchedulerSuspended 要判断为0？
	 |
	 | -- vTaskSuspendAll() 	// ++uxSchedulerSuspended 要挂起调度器？
	    |
	    | -- prvAddCurrentTaskToDelayedList(xTicksToDelay, pdFALSE)
		   |
		   | -- const TickType_t xConstTickCount = xTickCount
		   |
		   | -- if (uxListRemove(&(pxCurrentTCB->xStateListItem)) == 0)    // (UBaseType_t)0
		      |
		      | -- portRESET_READY_PRIORITY(pxCurrentTCB->uxPriority, uxToReadyPriority)
		   |
		   | -- #if (INCLUDE_vTaskSuspended == 1)
		   |
	       | -- if (portMAX_DELAY ... xCanBlockIndefinitely)
		      | 
	          | -- vListInsertEnd(&xSuspendedTaskList, &(pxCurrentTCB->xStateListItem))  // InsertEnd
		   |
		   | -- else
		   |
	       | -- xTimeToWake = xConstTickCount + xTicksToWait
		      |
			  | -- if   {vListInsert(pxOverflowDelayedTaskList, &(pxCurrentTCB->xStateListItem))}
	          |
			  | -- else {vListInsert(pxDelayedTaskList, &())}  // Insert
				 |
				 | -- xNextTaskUnblockTime = xTimeToWake  // overflow 不需要设置 xNextTaskUnblockTime
	 |
     | --
  |
  | --  if (xAlreadyYielded == pdFALSE)
	 |
	 | -- portYIELD_WITHIN_API()		// portYIELD


 -- pxCurrentTCB->xGenericListItem 列表项的列表 [ready list] [blocked(delay) list] [suspended task list]
/* Add the task to the suspended task list instead of a delayed task list to ensure it is not woken by a【timing event】
   It will block indefinitely. */
/* Remove the task from the ready list before adding it to the blocked list as the same list item is used for both lists. */	 

 -- xGenericListItem.pvContainer在xDelayedTaskList1情况
  |
  | -- vTaskDelay
     |
     | -- pxCurrentTCB->xGenericListItem.pvContainer
        |
        | -- step①: List_t pxReadyTaskLists[configMAX_PRIORITIES]  remove from [ready list]
        | -- step②: List_t xDelayedTaskList1                       insert into [blocked(delay) list]
     |
     | -- pxCurrentTCB->xEventListItem //列表项成员没有变化 -- 不需要等待任何信号量
  |
  | -- wait semaphore && not portMAX_DELAY //待验证 2018-3-29 11:35:19
     |
     | -- pxCurrentTCB->xGenericListItem.pvContainer // like vTaskDelay --只是没有给加到SuspendTaskList, 在DelayedTaskList还有救
     |
     | -- pxCurrentTCB->xEventListItem.pvContainer // in Que->xTasksWaitingToReceive


//TaskName            Priority      State       Event Object
ETHPLC_NetIfRxTask       3         Blocked      0x10014b2c(TCB->xEventListItem.pvContainer   = Que->xTasksWaitingToReceive)
                                                          (TCB->xGenericListItem.pvContainer = xSuspendedTaskList(0x1000e228)) // portMAX_DELAY
 -- xQueueGenericReceive
  |
  | -- ...
  |
  | -- vTaskPlaceOnEventList(&(pxQueue->xTasksWaitingToReceive), xTicksToWait)  // only for queueSend && queueReceive
       |
       | -- vListInsert(&(pxQueue->xTasksWaitingToReceive), &(pxCurrentTCB->xEventListItem))  // xEventListItem sorted by priority
          |
          | -- pxCurrentTCB->xEventListItem.pvContainer = Que->xTasksWaitingToReceive // event list in que. warning it is a EventListItem
       |
       | -- uxListRemove(&(pxCurrentTCB->xGenericListItem))  // current task[xGenericListItem] remove from the ready list
       |
       | -- if (portMAX_DELAY == xTicksToWait)
          |
          | -- vListInsertEnd(&xSuspendedTaskList, &(pxCurrentTCB->xGenericListItem))  // event list has sorted, no need sorted in this, suspendedList like readyList
             |
             | -- pxCurrentTCB->xGenericListItem.pvContainer = &xSuspendedTaskList
       |
       | -- else  // warning!!!【xGenericListItem】in different List：①xSuspendedTaskList ②xDelayedTaskList
          |
          | -- xTimeToWake = xTickCount + xTicksToWait
          |
          | -- prvAddCurrentTaskToDelayedList(xTimeToWake)
             |
             | -- listSET_LIST_ITEM_VALUE(&(pxCurrentTCB->xGenericListItem), xTimeToWake) // GenericListItem.xItemValue = xTimeWake
             |
             | -- vListInsert(pxDelayedTaskList, &(pxCurrentTCB->xGenericListItem))  // delay list need sorted


   //xTicksToWait != portMAX_DELAY
   //pxCurrentTCB->xEventListItem.pvContainer   = Que->xTasksWaitingToReceive
   //pxCurrentTCB->xGenericListItem.pvContainer = xSuspendedTaskList or xDelayedTaskList
   
 -- vListInsert    用于需要排序的列表插入, 如eventList、timerList. 涉及排序速度较慢
 -- vListInsertEnd 用于不需要排序的列表插入, 如readyList、deleteList、suspendedList、pendingReadyList等。不涉及排序速度快
   
	 
/* Context switches are held pending while the scheduler is suspended.  Also,
interrupts must not manipulate the xStateListItem of a TCB, or any of the
lists the xStateListItem can be referenced from, if the scheduler is suspended.
If an interrupt needs to unblock a task while the scheduler is suspended then it
moves the task's event list item into the xPendingReadyList, ready for the
kernel to move the task from the pending ready list into the real ready list
when the scheduler is unsuspended.  The pending ready list itself can only be
accessed from a critical section. */
PRIVILEGED_DATA static volatile UBaseType_t uxSchedulerSuspended	= ( UBaseType_t ) pdFALSE;


// -- 看list源码的时候使用
static void prvResetNextTaskUnblockTime( void )
{
TCB_t *pxTCB;

	if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
	{
		/* The new current delayed list is empty.  Set xNextTaskUnblockTime to
		the maximum possible value so it is	extremely unlikely that the
		if( xTickCount >= xNextTaskUnblockTime ) test will pass until
		there is an item in the delayed list. */
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/* The new current delayed list is not empty, get the value of
		the item at the head of the delayed list.  This is the time at
		which the task at the head of the delayed list should be removed
		from the Blocked state. */
		( pxTCB ) = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
		xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE( &( ( pxTCB )->xStateListItem ) );
	}
}


// if( xTickCount >= xNextTaskUnblockTime ) test will pass next time through. 


/* It is time to remove the item from the Blocked state. */
( void ) uxListRemove( &( pxTCB->xStateListItem ) );

/* Is the task waiting on an event also?  If so remove
it from the event list. */
if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
{
	( void ) uxListRemove( &( pxTCB->xEventListItem ) );
}


if(TaskScheduler != pdFALSE)   // 很少用pdTRUE

if (xTickCount >= xNextTaskUnblockTime)














