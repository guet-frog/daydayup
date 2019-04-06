
#if List && ListItem

// -- mainly for task schedule
pxIndex的作用是对列表中的列表项进行遍历, 例如pxReadyTasksLists[0]的任务优先级是相同的
通过pxIndex来记录当前运行的是哪个任务[正在运行]
list.c listGET_OWNER_OF_NEXT_ENTRY //pxIndex move through every item contained in a list, don't return marker
//链表pxIndex指向列表结尾项 这样下次在更新是就会指向第一个列表项
pxList->pxIndex = (xListItem *)&( pxList->xListEnd );  // list init

// insert end not sort
 -- 在列表结尾处插入列表项, 这个插入不进行排序, 仅仅是插入到列表的结尾处
 -- 环形列表, 没有绝对的结尾
 -- pxIndex指向的就是结尾. 因为pxIndex指向【当前列表项】意味着: 再访问到此列表项需要转一圈才能实现
 	//pxNewListItem->pxNext = pxIndex;
	//pxNewListItem->pxPrevious = pxIndex->pxPrevious; // 插入到pxIndex的前面
 -- 任务加入到任务就绪列表一般使用此函数

 -- vListInsert    用于需要排序的列表插入, 如eventList、timerList. 涉及排序速度较慢
 -- vListInsertEnd 用于不需要排序的列表插入, 如readyList、deleteList、suspendedList、pendingReadyList等。不涉及排序速度快

------------------------------------------------------------------------------------------

	/* It is time to remove the item from the Blocked state. */
	uxListRemove(&(pxTCB->xStateListItem));	/* - note: xStatListItem */

	/* Is the task waiting on an event also?  If so remove it from the event list. */
	if(listLIST_ITEM_CONTAINER(&(pxTCB->xEventListItem)) != NULL)
	{
		uxListRemove(&(pxTCB->xEventListItem));	/* - note: xEventListItem */
	}

#endif /* List && ListItem */

#ifdef xGenericListItem && xEventListItem

 -- pxCurrentTCB->xGenericListItem.pvContainer		// note: not EventListItem.pvContainer
  |
  | -- ready list
  |
  | -- blocked(delay) list
  |
  | -- suspended task list

/* Add the task to the suspended task list instead of a delayed task list to ensure it is not woken by a [timing event], It will block indefinitely */
/* Remove the task from the ready list before adding it to the blocked list as the same list item is used for both lists */

	-- vTaskDelay
     |
     | -- pxCurrentTCB->xGenericListItem.pvContainer
        |
        | -- step①: List_t pxReadyTaskLists[configMAX_PRIORITIES]  remove from [ready list]
        | -- step②: List_t xDelayedTaskList1                       insert into [blocked(delay) list]
     |
     | -- pxCurrentTCB->xEventListItem //列表项成员没有变化 -- 不需要等待任何信号量

	-- wait semaphore && not portMAX_DELAY
     |
     | -- pxCurrentTCB->xGenericListItem.pvContainer // like vTaskDelay --只是没有给加到SuspendTaskList, 在DelayedTaskList还有救
     |
     | -- pxCurrentTCB->xEventListItem.pvContainer // in Que->xTasksWaitingToReceive

//TaskName            Priority      State       Event Object
ETHPLC_NetIfRxTask       3         Blocked      0x10014b2c(TCB->xEventListItem.pvContainer   = Que->xTasksWaitingToReceive)
                                                          (TCB->xGenericListItem.pvContainer = xSuspendedTaskList(0x1000e228)) // portMAX_DELAY

#endif /* xGenericListItem && xEventListItem */

#ifdef data struct
 -- v	 
 -- x    其他类型变量(如结构体)以x为前缀
 -- prv  文件内部函数以prv为前缀，private 
 -- ux   UBaseType_t // unsigned long
#endif /* data struct */

#ifdef context switch && vPortYield

任务切换场合：
①SysTick_Handler()
②执行系统调用：执行FreeRTOS系统提供的相关API，如：任务切换函数taskYIELD()
               FreeRTOS某些API也会调用taskYIELD()，这些API函数也会导致任务切换

时间片调度发生在systick的中断服务函数中
一个时间片的长度就是systick的中断周期，
void xPortSysTickHandler(void)
{
    if (pdFALSE != xTaskIncrementTick())
    {
        portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
    }
}

 -- vPortYield
  |
  | -- *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET

  #define queueYIELD_IF_USING_PREEMPTION()  portYIELD_WITHIN_API()
  #define portYIELD_WITHIN_API              portYIELD
  #define portYIELD()                       vPortYield()

#endif /* context switch && vPortYield */

#ifdef vTaskStartSchedule()

 -- vTaskStartSchedule()
  |
  | -- xTaskCreate() // Idle
  |
  | -- xTimerCreateTimerTask() // configUSE_TIMES == 1
  |
  | -- portDISABLE_INTERRUPTS() //to ensure a tick does not occur before or during the call to xPortStartScheduler()
  |
  | -- xNextTaskUnblockTime = portMAX_DELAY // important global variable init
  | -- xSchedulerRunning = pdTRUE
  | -- xTickCount = (TickType_t)0U
  |
  | -- xPortStartScheduler() // warning: this function will not return
     |
     | -- *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI  //PendSV && Systick lowest priority interrupt
     | -- *(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI
     |
     | -- prvSetupTimerInterrupt() // setup systick
     |
     | -- uxCriticalNesting = 0
     |
     | -- vPortStartFistTask() // in portasm.s; cm0没有使用到SVC中断来进行 start schedule
        |
        | -- // for get sp point, so the first item in pxCurrentTCB is the task [top] of stack

#endif /* vTaskStartSchedule */

#ifdef xTaskCreate()

 -- xTaskCreate()
  |
  | -- TCB_t * pxNewTCB;
  | -- StackType_t *pxTopOfStack;
  |
  | -- pxNewTCB = prvAllocateTCBAndStack(usStackDepth, puxStackBuffer)
	 |
	 | -- pxStack = pvPortMallocAligned(usStackDepth * sizeof(uint32_t));  // usStackDepth * 4
	 |
	 | -- pxNewTCB = (TCB_t *)pvPortMalloc(sizeof(TCB_t)) // pxNewTCB malloc later for safety, pxNewTCB'addr > pxStack'Top_addr
	 |
	 | -- pxNewTCB->pxStack = pxStack // bottom of stack in low Addr.    top of stack is first item in tcb
     |
  | -- pxTopOfStack = pxNewTCB->pxStack + (usStackDepth - (uint16_t)1) // note: uint32_t *
  |
  | -- prvInitialiseTCBVariables(pxNewTCB, pcName, uxPriority, xRegions, usStackDepth)
     |
     | -- listSET_LIST_ITEM_OWENER(&(pxTCB->xGenericListItem), pxTCB) // 相较于xEventListItem没有初始化xItemValue，仅set owner
     |                                                                // 插入readyList不需要进行排序， 插入到delayList通过xTimeTick
     | -- listSET_LIST_ITEM_VALUE(&(pxTCB->xEventListItem), configMAX_PRIORITIES - uxPriotity) // listItem在list中是升序排序，优先级越高的任务，listItemValue越小
     | -- listSET_LIST_ITEM_OWNER(&(pxTCB->xEventListItem), pxTCB)
  |
  | -- pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack, pxTaskCode, pvParameters) // initialize the tcb stack to look as if the task was already running
  |
  | -- *pxCreatedTask = (TaskHandle_t)pxNewTCB // taskHandle = pxCurrentTCB; stackTopPoint = *pxCurrentTCB
  |											   // pxCreatedTask是指针变量的地址
  | -- tackENTER_CRITICAL()
     |
     | -- uxCurrentNumberOfTasks++
     |
     | -- if (NULL == pxCurrentTCB) // no other tasks, or all other task are in suspended state
        |
        | -- pxCurrentTCB = pxNewTCB
        |
        | -- if (1 == uxCurrentNumberOfTasks) // this is the first task to be created so do the priliminary initialisation required
           |
           | -- prvInitialiseTaskLists()	// init xxxTaskList
     |
     | -- prvAddTaskToReadyList(pxNewTCB)	// set pvContainer
        |
        | -- taskRECORD_READY_PRIORITY(pxTCB->uxPriority)
        |
        | -- vListInsertEnd(&(pxReadyTasksLists[pxTCB->uxPriority]), &(pxTCB->xGenericListItem)) // insert end; pxTCB->xGenericListItem
  |
  | -- taskEXIT_CRITICAL()  // note: task schedule
  |
  | -- if (pdFALSE != xSchedulerRunning)
     |
     | -- if (pxCurrentTCB->uxPriority < uxPriority)
        |
        | -- taskYIELD_IF_USING_PREEMPTION() //  PendSV

#endif /* xTaskCreate */

#ifdef vTaskDelay()

 -- vTaskDelay(const TickType_t xTicksToDelay)
  |
  | -- if (xTicksToDelay > 0U)   // A delay time of zero just forces a reschedule
	 |
	 | -- vTaskSuspendAll()		// suspend all task ???
	    |
	    | -- prvAddCurrentTaskToDelayedList(xTicksToDelay, pdFALSE)
		   |
		   | -- uxListRemove(&(pxCurrentTCB->xStateListItem)) == 0
		   |
	       | -- if (portMAX_DELAY ... xCanBlockIndefinitely)	// delay infinite ???
		      | 
	          | -- vListInsertEnd(&xSuspendedTaskList, &(pxCurrentTCB->xStateListItem))  // InsertEnd
		   |
		   | -- else
		      |
	          | -- xTimeToWake = xConstTickCount + xTicksToWait
		      |
			  | -- if   {vListInsert(pxOverflowDelayedTaskList, &(pxCurrentTCB->xStateListItem))}
	          |
			  | -- else {vListInsert(pxDelayedTaskList, &(pxCurrentTCB->xStateListItem))}
				 |
				 | -- xNextTaskUnblockTime = xTimeToWake  // overflow 不需要设置 xNextTaskUnblockTime
	 |
     | -- xTaskResumeAll()
  |
  | --  if (xAlreadyYielded == pdFALSE)
	 |
	 | -- portYIELD_WITHIN_API()		// portYIELD

#endif /* vTaskDelay */

#ifdef xPortSysTickHandler()

 -- xPortSysTickHandler()
  |
  | -- if (xTaskIncrmentTick() != pdFALSE)
	 |
	 | -- *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET		/* - pend a context switch */

 -- xTaskIncrementTick
  |
  | -- if (uxSchedulerSuspended == pdFALSE)
     |
     | -- ++xTickCount  // increment the rtos tick 
     |
     | -- if (xTickCount == 0)
        |
        | -- taskSWITCH_DELAYED_LISTS()  // switch the delayed && overflowed delayed list if it wraps to 0
     |
     | -- if (xConstTickCount >= xNextTaskUnblockTime)
        |
        | -- pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList)
        |
        | -- uxListRemove(&(pxTCB->xGenericListItem))  // it's time to remove the item from the blocked state
        |
        | -- if (listLIST_ITEM_CONTAINER(&(pxTCB->xEventListItem)) != NULL) // is the task waiting on an event?
           |
           | -- uxListRemove(&(pxTCB->xEventListItem)) // if so remove it from the event list
        |
        | -- prvAddTaskToReadyList(pxTCB) // place the unblocked(delaying) task into the appropriate ready list
        |
        | -- if (pxTCB->uxPriority >= pxCurrentTCB->uxPriority)
           |
           | -- xSwitchRequired = pdTRUE
     |
     | -- #if ((configUSE_PREEMPTION == 1) && (configUSE_TIME_SLICING == 1)) // equal priority task will share processing time
        |
        | -- if (listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[pxCurrentTCB->uxPriority])) > (UBaseType_t)1)
           |
           | -- xSwitchRequired = pdTRUE
     |
     | -- return xSwitchRequired

#endif /* xPortSysTickHandler */

#if xQueueCreate()

 -- xQueueGenericCreate
  |
  | -- xQueueSizeInBytes = (size_t)(uxQueueLength * uxItemSize) + (size_t)1
  |
  | -- pxNewQueue = (Queue_t *)pvPortMalloc(sizeof(Queue_t) + xQueueSizeInBytes) // queue structure && storage area
  |
  | -- pxNewQueue->pcHead = ((int8_t *)pxNewQueue) + sizeof(Queue_t) //jump past the queue structure
  |
  | -- pxNewQueue->uxLength   = uxQueueLength
  | -- pxNewQueue->uxItemSize = uxItemSize
  |
  | -- xQueueGenericReset(pxNewQueue, pdTRUE)
     |
     | -- pxQueue->pcTail = pxQueue->pcHead + (pxQueue->uxLength * pxQueue->uxItemSize)
     | -- pxQueue->uxMessageWaiting = (UBaseType_t)0U // no message in queue
     | -- pxQueue->pcWriteTo = pxQueue->pcHead
     | -- pxQueue->u.pcReadFrom = pxQueue->pcHead + ((pxQueue->uxLength - (UBaseType_t)1U) * pxQueue->uxItemSize) // not the last
     | -- pxQueue->xRxLock = queueUNLOCKED
     | -- pxQueue->xTxLock = queueUNLOCKED
     |
     | -- if (xNewQueue == pdFALSE) // is a new queue ?
        |
        | -- if (listLIST_IS_EMPTY(&(pxQueue->xTaskWaitingToSend)) == pdFALSE)
           |
           | -- if (xTaskRemoveFromEventList(&(pxQueue->xTaskWaitingToSend)) == pdTRUE) // uxPriority > pxCurrentTCB->uxPriority
              |
              | -- queueYIELD_IF_USING_PREEMPTION()
     |
     | -- else
        |
        | -- vListInitialise(&(pxQueue->xTasksWaitingToSend))
        | -- vListInitialise(&(pxQueue->xTasksWaitingToReceive))
   |
   | -- xReturn = pxNewQueue // point Queue_t struct fist addr

#endif /* xQueueCreate */

#if xQueueGenericRececive()

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

#endif /* xQueueGenericReceive */

#ifdef xTaskRemoveFromEventList()

 -- xTaskRemoveFromEventList
  |
  | -- TCB_t *pxUnblockedTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(pxEventList) // get owner of head entry
  |
  | -- (void)uxListRemove(&(pxUnblockedTCB->xEventListItem))
     |
     | -- List_t * const pxList = (List_t *)pxItemToRemove->pvContainer
     |
     | -- pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious
     | -- pxItemToRemove->pxPrivious->pxNext = pxItemToRemove->pxNext
     |
     | -- if (pxList->pxIndex == pxItemToRemove)  // 注意pxIndex的合法性，如：指向刚刚被删除的列表项，指向xListEnd
        |
        | -- pxList->pxIndex = pxItemToRemove->pxPrevious // pxList->pxIndex point current item
     |
     | -- pxItemToRemove->pvContainer = NULL
     |
     | -- (pxList->uxNumberOfItems)-- // warning!!! ()
  |
  | -- if (uxSchedulerSuspended == pdFALSE)
     |
     | -- uxListRemove(&(pxUnblockedTCB->xGenericListItem)) // xGenericListItem
     |
     | -- prvAddTaskToReadyList(pxUnblockedTCB) // get xGenericListItem && uxPriority from TCB
  |
  | -- if (pxUnblockedTCB->uxPriority > pxCurrentTCB->uxPriority)
     |
     | -- xReturn = pdTRUE
     | -- xYieldPending = pdTRUE    // in case user is not using the "xHigherPriorityTaskWoken" param

#endif /* xTaskRemoveFromEventList */

#ifdef xSemaphore

 -- gBinarySemaphore=xSemaphoreCreateMutex();	// note: (1) create Queue_t Control Variable	\
														 (2) Queue_t->uxMessagesWaiting
 -- xSemaphoreGive(gMutexSemaphore)
  |
  | -- xQueueGenericSend()
     |
	 | -- if (pxQueue->uxMessagesWaiting < pxQueue->uxLength)	// not full
	    |
		| -- prvCopyDataToQueue()
		   |
		   | -- #if (configUSE_MUTEXS == 1)		// 表达式
		      |
			  | -- xTaskPriorityDisinherit(pxQueue->pxMutexHolder)	// note: pxMutexHolder need reset priority when release semaphore
			  | -- pxQueue->pxMutexHolder = NULL
		   |
		   | -- ++(pxQueue->uxMessagesWaiting)
		|
		| -- if (listLIST_IS_EMPTY(&( pxQueue->xTasksWaitingToReceive)) == pdFALSE)		// has task waiting semaphore
		   |
		   | -- xTaskRemoveFromEventList(&(pxQueue->xTasksWaitingToReceive))
	 |
	 | -- else		// full
	    |
		| -- return errQUEUE_FULL

 -- xSemaphoreTake(gMutexSemaphore, portMAX_DELAY)
  |
  | -- if (pxQueue->uxMessagesWaiting > 0)	// has semaphore
     |
     | -- --(pxQueue->uxMessagesWaiting)
	 |
	 | -- #if (configUSE_MUTEXES == 1)
		|
	    | -- pxQueue->pxMutexHolder = pvTaskIncrementMutexHeldCount()		// only mutex semaphore need
	 |
	 | -- if (listLIST_IS_EMPTY(&(pxQueue->xTasksWaitingToSend)))
		|
	    | -- xTaskRemoveFromEventList(&(pxQueue->xTasksWaitingToSend))		// pxQueue->xTasksWatingToSend
  |
  | -- else		// not has semaphore
     |
	 | -- vTaskSetTimeOutState(&xTimeOut)
  |
  | -- #if (configUSE_MUTEXES == 1)
	 |
	 | -- vTaskPriorityInherit(pxQueue->pxMutexHolder)	// inherit priority
	    |
		| -- if(mutexHolderTCB->uxPriority < pxCurrentTCB->uxPriority)
		   |
	       | -- mutexHolderTCB->uxPriority = pxCurrentTCB->uxPriority
  |
  | -- vTaskPlaceOnEventList(&(pxQueue->xTasksWaitingToReceive), xTicksToWait)

#endif




