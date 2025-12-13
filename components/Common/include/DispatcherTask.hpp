#pragma once

#include "ITask.hpp"
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <concepts>

class DispatcherTask
{
    public:

    enum TaskFrequency
    {
        TASK_FREQ_100MS = 0,
        TASK_FREQ_1S = 1,
    };

    struct TaskEntry
    {
        ITask* task;
        TaskFrequency freq;
    };

    DispatcherTask()
    {
        xTaskCreate(DispatcherTask::TaskHandle,
                    "DispatcherTask",
                    1024*8,
                    reinterpret_cast<void *>(this),
                    5,
                    nullptr);
    }

    void RegisterTask(ITask* task, TaskFrequency freq)
    {
        if(task == nullptr)
            return;
        
        m_entries.emplace_back(task,freq);
    }

    private:

    static void TaskHandle(void* pvParameter)
    {
        constexpr uint32_t SLOW_TASK_COUNTER = 10;

        DispatcherTask* instance = reinterpret_cast<DispatcherTask*>(pvParameter);

        TickType_t xLastWakeTime = xTaskGetTickCount();
        uint32_t tickCount = 0;

        while(1)
        {  
            bool slowTaskTrigger = false;

            if(tickCount >= SLOW_TASK_COUNTER)
                tickCount = 0;
            
            if(tickCount == 0)
                slowTaskTrigger = true;

            for(const auto& task: instance->m_entries)
            {
                if(task.freq == DispatcherTask::TaskFrequency::TASK_FREQ_100MS)
                {
                    task.task->Update();
                }
                else if(task.freq == DispatcherTask::TaskFrequency::TASK_FREQ_1S)
                {
                    if(slowTaskTrigger == false)
                        continue;

                    task.task->Update();
                }
            }

            tickCount++;
            xTaskDelayUntil(&xLastWakeTime, (100/portTICK_PERIOD_MS));
        }
    }

    std::vector<TaskEntry> m_entries;

};