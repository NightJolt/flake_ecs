#include <flake/ecs/ecs.h>
#include <flake/macros.h>

namespace {
    fl::ecs::entity_t entity_selected = fl::ecs::nullentity;
    fl::ecs::entity_t next = fl::ecs::nullentity;
    fl::ecs::entity_id_t available = 0;
    fl::vec_t<fl::ecs::entity_t> entities;
}

namespace {
    auto generate_entity_uuid(fl::ecs::entity_id_t id, fl::ecs::entity_ver_t version) -> fl::ecs::entity_t {
        return ((fl::ecs::entity_t)id << SIZE_BITS(fl::ecs::entity_ver_t)) | version;
    }

    auto create_entity() -> fl::ecs::entity_t {
        return entities.emplace_back(generate_entity_uuid(entities.size(), 0));
    }

    auto recycle_entity() -> fl::ecs::entity_t {
        available--;

        fl::ecs::entity_t entity = next;

        std::swap(next, entities[fl::ecs::get_entity_id(next)]);

        return entity;
    }

    auto is_entity_recyclable(fl::ecs::entity_t entity) -> bool {
        return ~(fl::ecs::entity_ver_t)entity;
    }

    auto is_id_alive(fl::ecs::entity_id_t id) -> bool {
        return id < entities.size() && fl::ecs::get_entity_id(entities[id]) == id;
    }
}

auto fl::ecs::get_entity_id(entity_t entity) -> entity_id_t {
    return entity >> SIZE_BITS(entity_ver_t);
}

auto fl::ecs::get_entity_version(entity_t entity) -> entity_ver_t {
    return entity;
}

auto fl::ecs::destroy_entity(entity_t entity) -> void {
    if (!is_entity_alive(entity)) return;

    if (entity_selected == entity) entity_selected = nullentity;

    entity_id_t id = get_entity_id(entity);

    entity++;

    entities[id] = entity;

    if (is_entity_recyclable(entity)) {
        std::swap(next, entities[id]);

        available++;
    }
}

auto fl::ecs::is_entity_alive(entity_t entity) -> bool {
    return get_entity_id(entity) < entities.size() && entities[get_entity_id(entity)] == entity;
}

auto fl::ecs::new_entity() -> entity_t {
    if (available)
        return recycle_entity();
    else
        return create_entity();
}

auto fl::ecs::validate_entity(entity_t entity) -> entity_t {
    return is_entity_alive(entity) ? entity : nullentity;
}

auto fl::ecs::has_component(entity_t entity, component_id_t component_id) -> bool {
    if (!is_entity_alive(entity) || impl::sizes.size() <= component_id) return false;

    entity_id_t entity_id = get_entity_id(entity);
    entity_ver_t entity_v = get_entity_version(entity);

    auto& dense = impl::denses[component_id];
    auto& sparse = impl::sparses[component_id];
    
    if (sparse.size() <= entity_id) return false;

    auto dense_index = sparse[entity_id];

    if (impl::sizes[component_id] <= dense_index) return false;

    return get_entity_version(dense[dense_index]) == entity_v;
}
