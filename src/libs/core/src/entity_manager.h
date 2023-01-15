#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

#include "plf_stack.h"

#include "entity.h"
#include "entity_container_cache.h"

class EntityManager final
{
  public:
    hash_t GetClassCode(entid_t id) const;
    entptr_t GetEntityPointer(entid_t id) const;
    entity_container_cref GetEntityIds(layer_type_t type) const;
    entity_container_cref GetEntityIds(const char *name) const;
    entity_container_cref GetEntityIds(uint32_t hash) const;
    entity_container_cref GetEntityIds(layer_index_t index) const;
    entid_t GetEntityId(const char *name) const;
    bool IsEntityValid(entid_t id) const;
    layer_type_t GetLayerType(layer_index_t index) const;
    bool IsLayerFrozen(layer_index_t index) const;

    void AddToLayer(layer_index_t layer_idx, entid_t entity_id, priority_t priority); // no bounds check
    void RemoveFromLayer(layer_index_t layer_idx, entid_t entity_id);                 // no bounds check
    entid_t CreateEntity(const char *name, ATTRIBUTES *attr = nullptr);
    void EraseEntity(entid_t id);
    void EraseAll();
    void SetLayerType(layer_index_t index, layer_type_t type);
    void SetLayerFrozen(layer_index_t index, bool freeze);
    void NewLifecycle();
    void ForEachEntity(const std::function<void(entptr_t)> &f);

  private:
    constexpr static size_t kMaxLayerNum = sizeof(uint32_t) * 8;

    using entid_index_t = uint32_t;
    using entid_stamp_t = uint32_t;
    using entity_index_t = uint32_t;

    enum EntityState : uint8_t
    {
        kNotExists = 0,
        kExists = 1,
        kValid = 2
    };

    struct EntityInternalData
    {
        using layer_mask_t = uint32_t;

        layer_mask_t mask;
        priority_t priorities[kMaxLayerNum];
        EntityState state;

        entptr_t ptr;
        entid_t id;
        hash_t hash;
    };

    struct Layer
    {
        std::vector<priority_t> priorities;
        std::vector<entid_t> entity_ids;

        layer_type_t type;
        bool frozen;
    };

    static bool EntIdxValid(size_t idx);
    static entid_t CalculateEntityId(size_t idx);

    size_t GetEntityDataIdx(entid_t id) const;
    entid_t GetEntityId(uint32_t hash) const;

    void AddToLayer(layer_index_t index, EntityInternalData &data, priority_t priority); // no bounds check
    void RemoveFromLayer(layer_index_t index, EntityInternalData &data);                 // no bounds check
    void EraseAndFree(EntityInternalData &data);
    void MarkDeleted(EntityInternalData &data);
    entid_t InsertEntity(entptr_t ptr, hash_t hash);

    mutable EntityContainerCache cache_;

    // array of layers
    std::array<Layer, kMaxLayerNum> layers_{};

    // array of all entities
    std::vector<EntityInternalData> entities_;

    // stack of erased entities
    plf::stack<entity_index_t> freeIndices_;

    // stack of entities for deferred delete
    plf::stack<entity_index_t> deletedIndices_;
};
