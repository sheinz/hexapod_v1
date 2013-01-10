//***********************************************************************
// FILE: CTaskMgr.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 29-08-2010 
// DESCRIPTION: The class controls tasks.
//************************************************************************

#include "CTaskMgr.h"
#include "CTimer.h"

CTaskMgr TaskMgr;

// ----------------------------------------------------------------------------

void CTaskMgr::init(void)
{
   for (uint8_t i = 0; i < MAX_TASKS; i++)
   {
      mTaskList[i].mpTask = 0;  // clear all task records
   }  
}

// ----------------------------------------------------------------------------

void CTaskMgr::processSingleTask(TaskRecord *pTask, uint32_t currTime)
{
   uint32_t threshold = pTask->mLastTime + pTask->mInterval;

   if (threshold < pTask->mLastTime)
   {  // there was overflow of the threshold
      if ( (currTime > pTask->mLastTime) ||
           (currTime < threshold) )
         return;
   }
   else
   {
      if ( currTime < (pTask->mLastTime + pTask->mInterval) )
         return;
   }

   pTask->mLastTime = currTime;

   pTask->mpTask->onExecute();
}

// ----------------------------------------------------------------------------

void CTaskMgr::processTasks(void)
{
   uint32_t currTime = Timer.getFullTimer();    // do not call it all the time

   for (uint8_t i = 0; i <MAX_TASKS; i++)
   {
      if (mTaskList[i].mpTask != 0)
      {
         processSingleTask(&mTaskList[i], currTime);
      }
   }
}

// ----------------------------------------------------------------------------

void CTaskMgr::Add(ITask *pTask, uint32_t interval)
{
   for (uint8_t i = 0; i < MAX_TASKS; i++)
   {
      if (mTaskList[i].mpTask == 0)  // if it's free
      {
         mTaskList[i].mpTask = pTask;
         mTaskList[i].mInterval = interval;
         mTaskList[i].mLastTime = Timer.getFullTimer();
         break;
      }
   }
   // We do not handle errors. There is might be a problem.
}

// ----------------------------------------------------------------------------

void CTaskMgr::Remove(ITask *pTask)
{
   for (uint8_t i = 0; i < MAX_TASKS; i++)
   {
      if (mTaskList[i].mpTask == pTask)
      {
         mTaskList[i].mpTask = 0;  // set it free
         break;
      }
   }
}

// ----------------------------------------------------------------------------

bool CTaskMgr::isExist(const ITask *pTask)
{
   for (uint8_t i = 0; i < MAX_TASKS; i++)
   {
      if (mTaskList[i].mpTask == pTask)
      {
         return true;
      }
   }
   return false;
}


