#include <flake/ecs/impl.h>

fl::vec_t<fl::ecs::dense_t> fl::ecs::impl::denses = vec_t<dense_t> ();
fl::vec_t<fl::ecs::sparse_t> fl::ecs::impl::sparses = vec_t<sparse_t> ();
fl::vec_t<fl::any_t> fl::ecs::impl::components = vec_t<any_t> ();
fl::vec_t<uint32_t> fl::ecs::impl::sizes = vec_t<uint32_t> ();

fl::vec_t<fl::any_t> fl::ecs::impl::oncreate_callbacks = vec_t<any_t> ();
fl::vec_t<fl::any_t> fl::ecs::impl::ondestroy_callbacks = vec_t<any_t> ();

fl::ecs::component_id_t fl::ecs::impl::next_component_id = 0;