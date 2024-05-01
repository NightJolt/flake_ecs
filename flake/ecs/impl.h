#include <flake/ecs/types.h>
#include <flake/std/types/any.h>
#include <flake/std/types/vector.h>

namespace fl::ecs::impl {
    extern vec_t<dense_t> denses;
    extern vec_t<sparse_t> sparses;
    extern vec_t<any_t> components;
    extern vec_t<uint32_t> sizes;

    extern vec_t<any_t> oncreate_callbacks;
    extern vec_t<any_t> ondestroy_callbacks;

    extern component_id_t next_component_id;
}
