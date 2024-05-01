#pragma once

#include <flake/types.h>
#include <flake/std/types/function.h>
#include <flake/std/types/any.h>
#include <flake/iterator.h>
#include <flake/ecs/impl.h>

namespace fl::ecs {
    entity_id_t get_entity_id(entity_t);
    entity_ver_t get_entity_version(entity_t);

    bool is_entity_alive(entity_t);
    entity_t validate_entity(entity_t);

    entity_t new_entity();
    void destroy_entity(entity_t);
    
    bool has_component(entity_t, component_id_t);
}

namespace fl::ecs {
    template <class T>
    auto get_component_id() -> component_id_t {
        static component_id_t id = impl::next_component_id++;

        return id;
    }

    template <class T>
    auto component_exist() -> bool {
        const component_id_t component_id = get_component_id<T>();

        return impl::components.size() > component_id && impl::components[component_id].has_value();
    }

    template <class T>
    auto get_component_array() -> vec_t<T>& {
        return std::any_cast<fl::vec_t<T>&>(impl::components[get_component_id<T>()]);
    }

    template <class T>
    auto get_component(entity_t entity) -> T& {
        return get_component_array<T>()[impl::sparses[get_component_id<T>()][get_entity_id(entity)]];
    }

    template <class T>
    auto iterate_component() -> iterator_t<T> {
        const component_id_t component_id = get_component_id<T>();

        if (component_exist<T>()) {
            uint32_t count = impl::sizes[component_id];
            
            if (count > 0) {
                fl::vec_t<T>& vec = get_component_array<T>();

                return iterator_t<T>(&vec[0], &vec[0] + count);
            }
        }
        
        return iterator_t<T>();
    }

    template <class T>
    auto has_component(entity_t entity) -> bool {
        const component_id_t component_id = get_component_id<T>();

        return has_component(entity, component_id);
    }

    template <class T>
    auto get_entity(T& component) -> entity_t {
        const component_id_t component_id = get_component_id<T>();

        return impl::denses[component_id][&component - &get_component_array<T>()[0]];
    }

    template <class T, class... Args>
    auto add_component(entity_t entity, Args&&... args) -> T& {
        const component_id_t component_id = get_component_id<T>();

        if (impl::sizes.size() <= component_id) {
            auto new_size = component_id + 1;

            impl::denses.resize(new_size, dense_t());
            impl::sparses.resize(new_size, sparse_t());
            impl::components.resize(new_size);
            impl::sizes.resize(new_size, 0);
        }

        entity_id_t entity_id = get_entity_id(entity);
        entity_ver_t entity_v = get_entity_version(entity);

        if (!impl::components[component_id].has_value()) {
            impl::components[component_id] = fl::vec_t<T>();
        }

        auto& dense = impl::denses[component_id];
        auto& sparse = impl::sparses[component_id];
        auto& component_arr = get_component_array<T>();
        auto& size_p = impl::sizes[component_id];
        auto size = size_p;
        
        {
            if (dense.size() == size) {
                dense.emplace_back(entity);
            } else {
                dense[size] = entity;
            }

            size_p++;
        }

        {
            if (sparse.size() <= entity_id) {
                sparse.resize(entity_id + 1, nulladdr);
            }

            sparse[entity_id] = size;
        }

        {
            size_t arr_size = component_arr.size();

            if (arr_size == size) {
                component_arr.reserve(arr_size + 1);
                component_arr.emplace_back(std::forward<Args>(args)...);
            } else {
                component_arr[size] = T(std::forward<Args>(args)...);
            }

            if (impl::oncreate_callbacks.size() > component_id) {
                auto& callback_any = impl::oncreate_callbacks[component_id];

                if (callback_any.has_value()) {
                    auto& callback = std::any_cast<std::function<void(T&)>&>(callback_any);

                    callback(component_arr[size]);
                } 
            }
        }

        return component_arr[size];
    }

    template <class T>
    auto remove_component(entity_t entity) -> void {
        const component_id_t component_id = get_component_id<T>();
        
        auto size = --impl::sizes[component_id];
        
        entity_id_t entity_id = get_entity_id(entity);
        auto& sparse = impl::sparses[component_id];
        auto entity_ind = sparse[entity_id];

        auto& dense = impl::denses[component_id];
        auto& component_arr = get_component_array<T>();

        entity_ver_t entity_v = get_entity_version(entity);

        entity_t other = dense[size];
        entity_id_t other_id = get_entity_id(other);
        entity_ver_t other_v = get_entity_version(other);
        addr_t other_ind = sparse[other_id];

        if (entity_ind != size) {
            std::swap(dense[entity_ind], dense[other_ind]);

            sparse[entity_id] = nulladdr;
            sparse[other_id] = entity_ind;
        }

        {
            if (impl::ondestroy_callbacks.size() > component_id) {
                auto& callback_any = impl::ondestroy_callbacks[component_id];

                if (callback_any.has_value()) {
                    auto& callback = std::any_cast<std::function<void(T&)>&>(callback_any);

                    callback(component_arr[entity_ind]);
                } 
            }

            component_arr[entity_ind] = std::move(component_arr[other_ind]);
        }
    }

    template <class T>
    auto oncreate_callback(const std::function<void(T&)>& f) -> void {
        const component_id_t component_id = get_component_id<T>();

        if (impl::oncreate_callbacks.size() <= component_id) {
            impl::oncreate_callbacks.resize(component_id + 1);
        }

        impl::oncreate_callbacks[component_id] = f;
    }

    template <class T>
    auto ondestroy_callback(const std::function<void(T&)>& f) -> void {
        const component_id_t component_id = get_component_id<T>();

        if (impl::ondestroy_callbacks.size() <= component_id) {
            impl::ondestroy_callbacks.resize(component_id + 1);
        }

        impl::ondestroy_callbacks[component_id] = f;
    }
}
