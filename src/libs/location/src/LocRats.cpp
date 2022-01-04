//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocRats
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LocRats.h"
#include "Entity.h"
#include "core.h"

//============================================================================================

LocRats::LocRats()
{
    num = sizeof(rat) / sizeof(LocRat);
}

LocRats::~LocRats()
{
}

//============================================================================================

// Initialization
bool LocRats::Init()
{
    // Location Pointer
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return false;
    // Execution
    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 100000);
    return true;
}

// Messages
uint64_t LocRats::ProcessMessage(MESSAGE &message)
{
    auto num = message.Long();
    if (num < 1)
        num = 1;
    if (num > sizeof(rat) / sizeof(LocRat))
        num = sizeof(rat) / sizeof(LocRat);
    // Location Pointer
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return 0;
    // Init rats
    for (int32_t i = 0; i < num; i++)
        rat[i].Init(location);
    return 1;
}

// Execution
void LocRats::Execute(uint32_t delta_time)
{
}

// Drawing
void LocRats::Realize(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f;
    for (int32_t i = 0; i < num; i++)
        rat[i].Update(dltTime);
}
