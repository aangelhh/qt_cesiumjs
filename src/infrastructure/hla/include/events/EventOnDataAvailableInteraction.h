#ifndef EVENTS_ON_DATA_AVAILABLE_HPP
#define EVENTS_ON_DATA_AVAILABLE_HPP

#include <memory>

class EventOnDataAvailableInteraction {

public:
    
    template <typename T>
    void setEventOnDataAvailableInteraction(std::shared_ptr<T> userData) {
        m_UserData = std::static_pointer_cast<void>(userData);
    }

    std::shared_ptr<void> getEventOnDataAvailableInteraction() const {
        return m_UserData;
    }

    void setActionEnum(int actionEnum) {
        m_ActionEnum = actionEnum;
    }

    int getActionEnum() const {
        return m_ActionEnum;
    }

    void setInteractionEnum(int interactionEnum) {
        m_InteractionEnum = interactionEnum;
    }

    int getInteractionEnum() const {
        return m_InteractionEnum;
    }

    DevStudio::HlaFederateIdPtr* getFederateName() {
        return m_FederateName;
    }

    void setFederateName(DevStudio::HlaFederateIdPtr* val) {
        m_FederateName = val;
    }

private:
    std::shared_ptr<void> m_UserData;
    int m_ActionEnum;
    int m_InteractionEnum;
    DevStudio::HlaFederateIdPtr* m_FederateName;
};

#endif // EVENTS_ON_DATA_AVAILABLE_HPP
