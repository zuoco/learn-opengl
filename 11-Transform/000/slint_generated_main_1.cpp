// This file is auto-generated
#include "/home/zcli/code/learn-opengl/11-Transform/000/main.h"

const slint::private_api::ItemTreeVTable MainWindow::static_vtable = { visit_children, get_item_ref, get_subtree_range, get_subtree, get_item_tree, parent_node, embed_component, subtree_index, layout_info, item_geometry, accessible_role, accessible_string_property, accessibility_action, supported_accessibility_actions, element_infos, window_adapter, slint::private_api::drop_in_place<MainWindow>, slint::private_api::dealloc };

auto MainWindow::init (const class SharedGlobals* globals,slint::cbindgen_private::ItemTreeWeak enclosing_component,uint32_t tree_index,uint32_t tree_index_of_first_child) -> void{
    auto self = this;
    self->self_weak = enclosing_component;
    self->globals = globals;
    this->tree_index_of_first_child = tree_index_of_first_child;
    self->tree_index = tree_index;
    self->root_1.background.set(slint::Brush(slint::Color::from_argb_encoded(+4.27819008e9)));
    self->root_1.height.set(600);
    self->root_1_image_2_preferred_height.set_binding([this]() {
                            [[maybe_unused]] auto self = this;
                            return slint::private_api::item_layout_info(SLINT_GET_ITEM_VTABLE(ImageItemVTable), const_cast<slint::cbindgen_private::ImageItem*>(&self->image_2), slint::cbindgen_private::Orientation::Vertical, &self->globals->window().window_handle(), self->self_weak.lock()->into_dyn(), self->tree_index_of_first_child + 1 - 1).preferred;
                        });
    self->root_1_image_2_preferred_width.set_binding([this]() {
                            [[maybe_unused]] auto self = this;
                            return slint::private_api::item_layout_info(SLINT_GET_ITEM_VTABLE(ImageItemVTable), const_cast<slint::cbindgen_private::ImageItem*>(&self->image_2), slint::cbindgen_private::Orientation::Horizontal, &self->globals->window().window_handle(), self->self_weak.lock()->into_dyn(), self->tree_index_of_first_child + 1 - 1).preferred;
                        });
    self->root_1_layoutinfo_h.set_binding([this]() {
                            [[maybe_unused]] auto self = this;
                            return (slint::private_api::item_layout_info(SLINT_GET_ITEM_VTABLE(WindowItemVTable), const_cast<slint::cbindgen_private::WindowItem*>(&self->root_1), slint::cbindgen_private::Orientation::Horizontal, &self->globals->window().window_handle(), self->self_weak.lock()->into_dyn(), self->tree_index) + [&](const auto &a_0, const auto &a_1, const auto &a_2, const auto &a_3, const auto &a_4, const auto &a_5){ slint::cbindgen_private::LayoutInfo o{}; o.max = a_0; o.max_percent = a_1; o.min = a_2; o.min_percent = a_3; o.preferred = a_4; o.stretch = a_5; return o; }(+3.4028234663852886e38, 100, 0, 0, self->root_1_image_2_preferred_width.get(), 0));
                        });
    self->root_1_layoutinfo_v.set_binding([this]() {
                            [[maybe_unused]] auto self = this;
                            return (slint::private_api::item_layout_info(SLINT_GET_ITEM_VTABLE(WindowItemVTable), const_cast<slint::cbindgen_private::WindowItem*>(&self->root_1), slint::cbindgen_private::Orientation::Vertical, &self->globals->window().window_handle(), self->self_weak.lock()->into_dyn(), self->tree_index) + [&](const auto &a_0, const auto &a_1, const auto &a_2, const auto &a_3, const auto &a_4, const auto &a_5){ slint::cbindgen_private::LayoutInfo o{}; o.max = a_0; o.max_percent = a_1; o.min = a_2; o.min_percent = a_3; o.preferred = a_4; o.stretch = a_5; return o; }(+3.4028234663852886e38, 100, 0, 0, self->root_1_image_2_preferred_height.get(), 0));
                        });
    self->root_1.title.set(slint::SharedString(u8"Soft Renderer - Transform"));
    self->root_1.width.set(800);
    self->image_2.height.set((1 * 600));
    self->image_2.source.set_binding([this]() {
                            [[maybe_unused]] auto self = this;
                            return self->root_1_frame_buffer.get();
                        });
    self->image_2.width.set((1 * 800));
    self->root_1.always_on_top.set_constant();
    self->root_1.background.set_constant();
    self->root_1.default_font_family.set_constant();
    self->root_1.default_font_size.set_constant();
    self->root_1.default_font_weight.set_constant();
    self->root_1.icon.set_constant();
    self->root_1.no_frame.set_constant();
    self->root_1.resize_border_width.set_constant();
    self->root_1.title.set_constant();
    self->image_2.colorize.set_constant();
    self->image_2.height.set_constant();
    self->image_2.image_fit.set_constant();
    self->image_2.image_rendering.set_constant();
    self->image_2.width.set_constant();
}

auto MainWindow::user_init () -> void{
    [[maybe_unused]] auto self = this;
}

auto MainWindow::layout_info (slint::cbindgen_private::Orientation o) const -> slint::cbindgen_private::LayoutInfo{
    [[maybe_unused]] auto self = this;
    return o == slint::cbindgen_private::Orientation::Horizontal ? [&]{ [[maybe_unused]] auto layout_info = self->root_1_layoutinfo_h.get();;return [&](const auto &a_0, const auto &a_1, const auto &a_2, const auto &a_3, const auto &a_4, const auto &a_5){ slint::cbindgen_private::LayoutInfo o{}; o.max = a_0; o.max_percent = a_1; o.min = a_2; o.min_percent = a_3; o.preferred = a_4; o.stretch = a_5; return o; }(800, layout_info.max_percent, 800, layout_info.min_percent, layout_info.preferred, layout_info.stretch); }() : [&]{ [[maybe_unused]] auto layout_info = self->root_1_layoutinfo_v.get();;return [&](const auto &a_0, const auto &a_1, const auto &a_2, const auto &a_3, const auto &a_4, const auto &a_5){ slint::cbindgen_private::LayoutInfo o{}; o.max = a_0; o.max_percent = a_1; o.min = a_2; o.min_percent = a_3; o.preferred = a_4; o.stretch = a_5; return o; }(600, layout_info.max_percent, 600, layout_info.min_percent, layout_info.preferred, layout_info.stretch); }();
}

auto MainWindow::item_geometry (uint32_t index) const -> slint::cbindgen_private::Rect{
    [[maybe_unused]] auto self = this;
    switch (index) {
        case 0: return slint::private_api::convert_anonymous_rect(std::make_tuple(float(600), float(800), float(0), float(0)));
        case 1: return slint::private_api::convert_anonymous_rect(std::make_tuple(float((1 * 600)), float((1 * 800)), float(0), float(0)));
    }
    return {};
}

auto MainWindow::accessible_role (uint32_t index) const -> slint::cbindgen_private::AccessibleRole{
    [[maybe_unused]] auto self = this;
    switch (index) {
        case 1: return slint::cbindgen_private::AccessibleRole::Image;
    }
    return {};
}

auto MainWindow::accessible_string_property (uint32_t index, slint::cbindgen_private::AccessibleStringProperty what) const -> std::optional<slint::SharedString>{
    [[maybe_unused]] auto self = this;
    switch ((index << 8) | uintptr_t(what)) {
    }
    return {};
}

auto MainWindow::accessibility_action (uint32_t index, const slint::cbindgen_private::AccessibilityAction &action) const -> void{
    [[maybe_unused]] auto self = this;
    switch ((index << 8) | uintptr_t(action.tag)) {
    }
    return ;
}

auto MainWindow::supported_accessibility_actions (uint32_t index) const -> uint32_t{
    [[maybe_unused]] auto self = this;
    switch (index) {
    }
    return {};
}

auto MainWindow::element_infos (uint32_t index) const -> std::optional<slint::SharedString>{
    [[maybe_unused]] auto self = this;
    switch (index) {
    }
    return {};
}

auto MainWindow::visit_children (slint::private_api::ItemTreeRef component, intptr_t index, slint::private_api::TraversalOrder order, slint::private_api::ItemVisitorRefMut visitor) -> uint64_t{
    static const auto dyn_visit = [] (const void *base,  [[maybe_unused]] slint::private_api::TraversalOrder order, [[maybe_unused]] slint::private_api::ItemVisitorRefMut visitor, [[maybe_unused]] uint32_t dyn_index) -> uint64_t {
        [[maybe_unused]] auto self = reinterpret_cast<const MainWindow*>(base);
        std::abort();
    };
    auto self_rc = reinterpret_cast<const MainWindow*>(component.instance)->self_weak.lock()->into_dyn();
    return slint::cbindgen_private::slint_visit_item_tree(&self_rc, get_item_tree(component) , index, order, visitor, dyn_visit);
}

auto MainWindow::get_item_ref (slint::private_api::ItemTreeRef component, uint32_t index) -> slint::private_api::ItemRef{
    return slint::private_api::get_item_ref(component, get_item_tree(component), item_array(), index);
}

auto MainWindow::get_subtree_range ([[maybe_unused]] slint::private_api::ItemTreeRef component, [[maybe_unused]] uint32_t dyn_index) -> slint::private_api::IndexRange{
        std::abort();
}

auto MainWindow::get_subtree ([[maybe_unused]] slint::private_api::ItemTreeRef component, [[maybe_unused]] uint32_t dyn_index, [[maybe_unused]] uintptr_t subtree_index, [[maybe_unused]] slint::private_api::ItemTreeWeak *result) -> void{
        std::abort();
}

auto MainWindow::get_item_tree (slint::private_api::ItemTreeRef) -> slint::cbindgen_private::Slice<slint::private_api::ItemTreeNode>{
    return item_tree();
}

auto MainWindow::parent_node ([[maybe_unused]] slint::private_api::ItemTreeRef component, [[maybe_unused]] slint::private_api::ItemWeak *result) -> void{
}

auto MainWindow::embed_component ([[maybe_unused]] slint::private_api::ItemTreeRef component, [[maybe_unused]] const slint::private_api::ItemTreeWeak *parent_component, [[maybe_unused]] const uint32_t parent_index) -> bool{
    return false; /* todo! */
}

auto MainWindow::subtree_index ([[maybe_unused]] slint::private_api::ItemTreeRef component) -> uintptr_t{
    return std::numeric_limits<uintptr_t>::max();
}

auto MainWindow::item_tree () -> slint::cbindgen_private::Slice<slint::private_api::ItemTreeNode>{
    static const slint::private_api::ItemTreeNode children[] {
        slint::private_api::make_item_node(1, 1, 0, 0, false), 
slint::private_api::make_item_node(0, 2, 0, 1, true) };
    return slint::private_api::make_slice(std::span(children));
}

auto MainWindow::item_array () -> const slint::private_api::ItemArray{
    static const slint::private_api::ItemArrayEntry items[] {
        { SLINT_GET_ITEM_VTABLE(WindowItemVTable),  offsetof(MainWindow, root_1) }, 
{ SLINT_GET_ITEM_VTABLE(ImageItemVTable),  offsetof(MainWindow, image_2) } };
    return slint::private_api::make_slice(std::span(items));
}

auto MainWindow::layout_info ([[maybe_unused]] slint::private_api::ItemTreeRef component, slint::cbindgen_private::Orientation o) -> slint::cbindgen_private::LayoutInfo{
    return reinterpret_cast<const MainWindow*>(component.instance)->layout_info(o);
}

auto MainWindow::item_geometry ([[maybe_unused]] slint::private_api::ItemTreeRef component, uint32_t index) -> slint::cbindgen_private::LogicalRect{
    return reinterpret_cast<const MainWindow*>(component.instance)->item_geometry(index);
}

auto MainWindow::accessible_role ([[maybe_unused]] slint::private_api::ItemTreeRef component, uint32_t index) -> slint::cbindgen_private::AccessibleRole{
    return reinterpret_cast<const MainWindow*>(component.instance)->accessible_role(index);
}

auto MainWindow::accessible_string_property ([[maybe_unused]] slint::private_api::ItemTreeRef component, uint32_t index, slint::cbindgen_private::AccessibleStringProperty what, slint::SharedString *result) -> bool{
    if (auto r = reinterpret_cast<const MainWindow*>(component.instance)->accessible_string_property(index, what)) { *result = *r; return true; } else { return false; }
}

auto MainWindow::accessibility_action ([[maybe_unused]] slint::private_api::ItemTreeRef component, uint32_t index, const slint::cbindgen_private::AccessibilityAction *action) -> void{
    reinterpret_cast<const MainWindow*>(component.instance)->accessibility_action(index, *action);
}

auto MainWindow::supported_accessibility_actions ([[maybe_unused]] slint::private_api::ItemTreeRef component, uint32_t index) -> uint32_t{
    return reinterpret_cast<const MainWindow*>(component.instance)->supported_accessibility_actions(index);
}

auto MainWindow::element_infos ([[maybe_unused]] slint::private_api::ItemTreeRef component, [[maybe_unused]] uint32_t index, [[maybe_unused]] slint::SharedString *result) -> bool{
    return false;
}

auto MainWindow::window_adapter (slint::private_api::ItemTreeRef component, [[maybe_unused]] bool do_create, slint::cbindgen_private::Option<slint::private_api::WindowAdapterRc>* result) -> void{
    *reinterpret_cast<slint::private_api::WindowAdapterRc*>(result) = reinterpret_cast<const MainWindow*>(component.instance)->globals->window().window_handle();
}

auto MainWindow::create () -> slint::ComponentHandle<MainWindow>{
    auto self_rc = vtable::VRc<slint::private_api::ItemTreeVTable, MainWindow>::make();
    auto self = const_cast<MainWindow *>(&*self_rc);
    self->self_weak = vtable::VWeak(self_rc).into_dyn();
    slint::cbindgen_private::slint_ensure_backend();
    self->globals = &self->m_globals;
    self->m_globals.root_weak = self->self_weak;
    slint::private_api::register_item_tree(&self_rc.into_dyn(), self->globals->m_window);
    self->init(self->globals, self->self_weak, 0, 1 );
    self->user_init();
    self->window();
    return slint::ComponentHandle<MainWindow>{ self_rc };
}

MainWindow::~MainWindow (){
    if (auto &window = globals->m_window) window->window_handle().unregister_item_tree(this, item_array());
}

auto MainWindow::invoke_close_requested () const -> void{
    slint::private_api::assert_main_thread();
    [[maybe_unused]] auto self = this;
    return self->root_1_close_requested.call();
}

auto MainWindow::get_frame_buffer () const -> slint::Image{
    slint::private_api::assert_main_thread();
    [[maybe_unused]] auto self = this;
    return self->root_1_frame_buffer.get();
}

auto MainWindow::set_frame_buffer (const slint::Image &value) const -> void{
    slint::private_api::assert_main_thread();
    [[maybe_unused]] auto self = this;
    self->root_1_frame_buffer.set(value);
}

auto MainWindow::show () -> void{
    window().show();
}

auto MainWindow::hide () -> void{
    window().hide();
}

auto MainWindow::window () const -> slint::Window&{
    return m_globals.window();
}

auto MainWindow::run () -> void{
    show();
    slint::run_event_loop();
    hide();
}
