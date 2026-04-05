#include <vector>
#include "DevStudio/datatypes/AggregateMissionEnum.h"
#include "DevStudio/datatypes/TaskModeEnum.h"
#include "DevStudio/datatypes/EntityIdentifierStruct.h"
#include "uuidUtils.h"

namespace SDL_RTI
{
    class TaskParams
    {
    public:
        TaskParams() :
            m_entity(),
            m_communicationNetworks(0),
            m_taskId(16),
            m_activity(DevStudio::AggregateMissionEnum::AggregateMissionEnum::OTHER),
            m_annotation(L""),
            m_mainTask(0),
            m_previousTask(0),
            m_nextTask(0),
            m_tasker(0),
            m_taskMode(DevStudio::TaskModeEnum::TaskModeEnum::NON_CONCURRENT_MODE)
        {

        }
        TaskParams(DevStudio::EntityIdentifierStruct entity) :
            m_entity(),
            m_communicationNetworks(0),
            m_taskId(16),
            m_activity(DevStudio::AggregateMissionEnum::AggregateMissionEnum::OTHER),
            m_annotation(L""),
            m_mainTask(0),
            m_previousTask(0),
            m_nextTask(0),
            m_tasker(0),
            m_taskMode(DevStudio::TaskModeEnum::TaskModeEnum::NON_CONCURRENT_MODE)
        {
            m_entity = uuidUtils::convert_entityIdentifier_to_uuid(entity);
        };
        TaskParams(DevStudio::EntityIdentifierStruct entity, short taskId, short step) :
            m_entity(),
            m_communicationNetworks(0),
            m_taskId(16),
            m_activity(DevStudio::AggregateMissionEnum::AggregateMissionEnum::OTHER),
            m_annotation(L""),
            m_mainTask(0),
            m_previousTask(0),
            m_nextTask(0),
            m_tasker(0),
            m_taskMode(DevStudio::TaskModeEnum::TaskModeEnum::NON_CONCURRENT_MODE)
        {
            m_entity = uuidUtils::convert_entityIdentifier_to_uuid(entity);
            m_taskId = uuidUtils::convert_task_to_uuid(taskId, step);
        };
        TaskParams(std::vector<char> entity, std::vector<std::vector<std::vector<char>>> communicationNetworks, std::vector<char> taskId, DevStudio::AggregateMissionEnum::AggregateMissionEnum activity, std::wstring annotation, std::vector<char> mainTask, std::vector<char> previousTask, std::vector<char> nextTask, std::vector<char> tasker, DevStudio::TaskModeEnum::TaskModeEnum taskMode) :
            m_entity(entity),
            m_communicationNetworks(communicationNetworks),
            m_taskId(taskId),
            m_activity(activity),
            m_annotation(annotation),
            m_mainTask(mainTask),
            m_previousTask(previousTask),
            m_nextTask(nextTask),
            m_tasker(tasker),
            m_taskMode(taskMode)
        {
        };

        TaskParams(const TaskParams& task) :
            m_entity(task.getEntity()),
            m_communicationNetworks(task.getCommunicationNetworks()),
            m_taskId(task.getTaskId()),
            m_activity(task.getActivity()),
            m_annotation(task.getAnnotation()),
            m_mainTask(task.getMainTask()),
            m_previousTask(task.getPreviousTask()),
            m_nextTask(task.getNextTask()),
            m_tasker(task.getTasker()),
            m_taskMode(task.getTaskMode())
        {
        };
        ~TaskParams() {};

        std::vector<std::vector<std::vector<char>>> getCommunicationNetworks() const { return m_communicationNetworks; }
        std::vector<char> getTaskId() const { return m_taskId; }
        DevStudio::AggregateMissionEnum::AggregateMissionEnum getActivity() const { return m_activity; }
        std::wstring getAnnotation() const { return m_annotation; }
        std::vector<char> getMainTask() const { return m_mainTask; }
        std::vector<char> getPreviousTask() const { return m_previousTask; }
        std::vector<char> getNextTask() const { return m_nextTask; }
        std::vector<char> getTasker() const { return m_tasker; }
        DevStudio::TaskModeEnum::TaskModeEnum getTaskMode() const { return m_taskMode; }
        std::vector<char> getEntity() const { return m_entity; }
        DevStudio::EntityIdentifierStruct getEntityIdentifier() const { return uuidUtils::convert_uuid_to_entityIdentifier(m_entity); }
        void getTask(short& taskId, short& step)
        {
            uuidUtils::convert_uuid_to_task(m_taskId, taskId, step);
        }
        void getMainTask(short& taskId, short& step)
        {
            uuidUtils::convert_uuid_to_task(m_mainTask, taskId, step);
        }
        void getPreviousStep(short& taskId, short& step)
        {
            uuidUtils::convert_uuid_to_task(m_previousTask, taskId, step);
        }
        void getNexTask(short& taskId, short& step)
        {
            uuidUtils::convert_uuid_to_task(m_nextTask, taskId, step);
        }

        void setTaskId(const std::vector<char>& taskId) { m_taskId = taskId; }
        void setCommunicationNetworks(std::vector<std::vector<std::vector<char>>>& communicationNetworks) { m_communicationNetworks = communicationNetworks; }
        void setActivity(DevStudio::AggregateMissionEnum::AggregateMissionEnum activity) { m_activity = activity; }
        void setAnnotation(const std::wstring& annotation) { m_annotation = annotation; }
        void setMainTask(const std::vector<char>& mainTask) { m_mainTask = mainTask; }
        void setPreviousTask(const std::vector<char>& previousTask) { m_previousTask = previousTask; }
        void setNextTask(const std::vector<char>& nextTask) { m_nextTask = nextTask; }
        void setTasker(const std::vector<char>& tasker) { m_tasker = tasker; }
        void setTaskMode(DevStudio::TaskModeEnum::TaskModeEnum taskMode) { m_taskMode = taskMode; }

    private:
        std::vector</* 16 */ char > m_entity;
        std::vector<std::vector<std::vector</* 16 */ char >>> m_communicationNetworks;
        std::vector</* 16 */ char > m_taskId;
        DevStudio::AggregateMissionEnum::AggregateMissionEnum m_activity;
        std::wstring m_annotation;
        std::vector</* 16 */ char > m_mainTask;
        std::vector</* 16 */ char > m_previousTask;
        std::vector</* 16 */ char > m_nextTask;
        std::vector</* 16 */ char > m_tasker;
        DevStudio::TaskModeEnum::TaskModeEnum m_taskMode;
    };
}