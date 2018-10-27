========================================================================================================
// -- 2018-1-29 10:35:03
// -- List && ListItem
========================================================================================================
// -- mainly for task schedule
pxIndex的作用是对列表中的列表项进行遍历, 例如pxReadyTasksLists[0]的任务优先级是相同的
通过pxIndex来记录当前运行的是哪个任务【正在运行】
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
========================================================================================================
// -- 2018-4-17 16:55:21
// -- xGenericListItem && xEventListItem
========================================================================================================

========================================================================================================

========================================================================================================
 -- v	 
 -- x    其他类型变量(如结构体)以x为前缀
 -- prv  文件内部函数以prv为前缀，private 
 -- ux   UBaseType_t // unsigned long
========================================================================================================

========================================================================================================
// -- 2018-1-29 11:15:22
// -- vTaskStartSchedule()
========================================================================================================
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

========================================================================================================
// -- 2018-1-29 12:14:34
// -- xTaskCreate()
========================================================================================================
/*!
 *param[in]：TaskFunction_t      pxTaskCode;    // typedef void (*TaskFunction_t)(void *);
 *param[in]: const char * const  pcName;
 *param[in]: const uint16_t      usStackDepth;  // length = usStackDepth * 4
 *param[in]: void * const        pvParameters;
 *param[in]: UBaseType_t         uxPriority;    // typedef unsigned long    UBaseType_t;
 *param[in]: TaskHandle_t *const pxCreatedTask; // typedef TaskHandle_t     void *;
 */
========================================================================================================
 -- xTaskCreate()
  |
  | -- TCB_t * pxNewTCB;
  | -- StackType_t *pxTopOfStack;
  |
  | -- pxNewTCB = prvAllocateTCBAndStack(usStackDepth, puxStackBuffer) // puxStackBuffer == NULL for static ram
     |                                                                 // return pxNewTCB
     | -- if (portSTACK_GROWTH < 0)
        |
        | -- pxStack = pvPortMallocAligned(usStackDepth * sizeof(uint32_t));  // usStackDepth * 4
        |
        | -- pxNewTCB = (TCB_t *)pvPortMalloc(sizeof(TCB_t)) // pxNewTCB malloc later for safety because: pxNewTCB'addr > pxStack'Top_addr
           |
           | -- pxNewTCB->pxStack = pxStack // bottom of stack in low Addr.    top of stack is first item in tcb
  |
  | -- if (portSTACK_GROWTH < 0)
     |
     | -- pxTopOfStack = pxNewTCB->pxStack + (usStackDepth - (uint16_t)1) //line71对应 StackType_t *pxStack(warning type!); (uint16_t)1(偏移4byte); top of stack in high addr
  |
  | -- prvInitialiseTCBVariables(pxNewTCB, pcName, uxPriority, xRegions, usStackDepth) // null = xRegions
     |
     | -- listSET_LIST_ITEM_OWENER(&(pxTCB->xGenericListItem), pxTCB) // 相较于xEventListItem没有初始化xItemValue，仅set owner
     |                                                                // 插入readyList不需要进行排序， 插入到delayList通过xTimeTick
     | -- listSET_LIST_ITEM_VALUE(&(pxTCB->xEventListItem), configMAX_PRIORITIES - uxPriotity) // listItem在list中是升序排序，优先级越高的任务，listItemValue越小
     | -- listSET_LIST_ITEM_OWNER(&(pxTCB->xEventListItem), pxTCB)
  |
  | -- pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack, pxTaskCode, pvParameters) // line77对应 initialize the tcb stack to look as if the task was already running
  |
  | -- if (NULL != pxCreatedTask) // pxCreatedTask = &taskHandler
     |
     | -- *pxCreatedTask = (TaskHandle_t)pxNewTCB //TaskHandle = &pxCurrentTCB; stackTopPoint = *pxCurrentTCB
  |												  //taskHandle = pxCurrentTCB
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
           | -- prvInitialiseTaskLists()
              |
              | -- for (uxPriority=0; uxPriority<configMAX_PRIORITIES; uxPriority++)
                 |
                 | -- vListInitialise(&(pxReadyTasksLists[uxPriority]))
              |
              | -- vListInitialise(&xDelayedTaskList1)
              | -- vListInitialise(&xDelayedTaskList2)
              | -- vListInitialise(&xPendingReadyList)
              | -- vListInitialise(&xTasksWaitingTermination)
              | -- vListInitialise(&xSuspendedTaskList)
              |
              | -- pxDelayedTaskList = &xDelayedTaskList1
              | -- pxOverflowDelayedTaskList = &xDelayedTaskList2
    |
    | -- else
       |
       | -- if (pdFALSE == xSchedulerRunning)
          |
          | -- if (pxCurrentTCB->uxPriority <= uxPriority)
             |
             | -- pxCurrentTCB = pxNewTCB // if the scheduler is not already running.  if running?
    |
    | -- uxTaskNunber++ // uxCurrentNumberOfTasks ?
    |
    | -- prvAddTaskToReadyList(pxNewTCB)
       |
       | -- taskRECORD_READY_PRIORITY(pxTCB->uxPriority)
       |
       | -- vListInsertEnd(&(pxReadyTasksLists[pxTCB->uxPriority]), &(pxTCB->xGenericListItem)) // insert end; pxTCB->xGenericListItem
  | 
  | -- taskEXIT_CRITICAL()  // EXIT 不退出后面的就不能进行任务调度
  |
  | -- if (pdFALSE != xSchedulerRunning)
     |
     | -- if (pxCurrentTCB->uxPriority < uxPriority)
        |
        | -- taskYIELD_IF_USING_PREEMPTION() // 立即触发一次PendSV
  |
  | -- xReturn = pdPASS

========================================================================================================
// -- 2018-3-15 11:14:28
// -- xQueueCreate
========================================================================================================
/*!
 *param[in]: const UBaseType_t uxQueueLength
 *param[in]: const UBaseType_t uxItemSize
 *param[in]: const uint8_t     ucQueueType
*/
========================================================================================================
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
  
========================================================================================================
 // -- 2018-3-15 15:21:54
 // -- xTaskRemoveFromEventList
========================================================================================================
/*!
 *param[in]: const List_t * const pxEventList
*/
========================================================================================================
 -- xTaskRemoveFromEventList
  |
  | -- TCB_t *pxUnblockedTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(pxEventList) // get listItem's owner
  |
  | -- uxListRemove(&(pxUnblockedTCB->xEventListItem))
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

========================================================================================================
 // -- 2018-3-16 17:51:44
 // -- context switch
========================================================================================================
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

========================================================================================================
 // -- 2018-3-29 15:55:29
 // -- xTaskIncrementTick
========================================================================================================
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
        | -- uxListRemove(&(pxTCB->xGenericListItem))  // it's time to remove the item from the delaying state
        |
        | -- if (listLIST_ITEM_CONTAINER(&(pxTCB->xEventListItem)) != NULL) // is the task waiting on an event?
           |
           | -- uxListRemove(&(pxTCB->xEventListItem)) // if so remove it from the event list
        |
        | -- prvAddTaskToReadyList(pxTCB) // place the unblocked(delaying) task into the appropriate ready list
        |
        | -- if (pxTCB->uxPriority >= pxCurrentTCB->uxPriority) // context switch should only be performed
           |                                        // if the unblocked task has a priority that is equal to
           | -- xSwitchRequired = pdTRUE            // or higher than the currently executing task
     |
     | -- #if ((configUSE_PREEMPTION == 1) && (configUSE_TIME_SLICING == 1)) // equal priority task will share processing time
        |
        | -- if (listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[pxCurrentTCB->uxPriority])) > (UBaseType_t)1)
           |
           | -- xSwitchRequired = pdTRUE
     |
     | -- return xSwitchRequired

========================================================================================================
 // -- 2018-3-15 12:11:31
 // -- vPortYield
========================================================================================================
 -- vPortYield
  |
  | -- *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET

  #define queueYIELD_IF_USING_PREEMPTION()  portYIELD_WITHIN_API()
  #define portYIELD_WITHIN_API              portYIELD
  #define portYIELD()                       vPortYield()
  
  
========================================================================================================
========================================================================================================
========================================================================================================
========================================================================================================
========================================================================================================
========================================================================================================
========================================================================================================

























































 