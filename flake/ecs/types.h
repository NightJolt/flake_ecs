#pragma once

#include <flake/types.h>
#include <flake/std/types/vector.h>

namespace fl::ecs {
    typedef uint64_t entity_t;
    typedef uint32_t entity_id_t;
    typedef uint32_t entity_ver_t;
    typedef uint8_t component_id_t;
    typedef uint32_t addr_t;

    typedef vec_t<entity_t> dense_t;
    typedef vec_t<addr_t> sparse_t;
}
