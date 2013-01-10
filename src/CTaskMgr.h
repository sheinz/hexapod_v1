#ifndef __CTASKMGR_H__
#define __CTASKMGR_H__
//***********************************************************************
// FILE: CTaskMgr.h
// AUTHOR: Yuriy Skriyka
// DATE: 29-08-2010 
// DESCRIPTION: The class controls tasks.
//************************************************************************

#include <inttypes.h>


#define MAX_TASKS    16

class CTaskMgr;
extern CTaskMgr TaskMgr;


class ITask
{
public:

   virtual void init(void) {};

   virtual void onExecute(void) = 0;
};



class CTaskMgr
{
public:

   void init(void);

   void processTasks(void);


   // The method adds a task. The task is to be called with specified intervals in ms.
   void Add(ITask *pTask, uint32_t interval);

   // The method removes the task from the task list. The task will not be called any more.
   void Remove(ITask *pTask);

   // The method checks if the specified task is registered in the task manager
   bool isExist(const ITask *pTask);

private:

   struct TaskRecord
   {
      ITask *mpTask;          // the pointer to the task object. It is 0 if the record is free
      uint32_t mInterval;     
      uint32_t mLastTime;     // the time mark when the task was executed the last time
   };

   TaskRecord mTaskList[MAX_TASKS];    // the list of tasks


   inline void processSingleTask(TaskRecord *pTask, uint32_t currTime);
};

#endif
