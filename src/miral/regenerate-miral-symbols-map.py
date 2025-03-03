#! /usr/bin/python3
"""This script processes the XML generated by "make doc" and produces summary information
on symbols that libmiral intends to make public.

To use: Go to your build folder and run "make regenerate-miral-symbols-map" """

from xml.dom import minidom
from sys import argv

HELPTEXT = __doc__
DEBUG = False

def _get_text(node):
    substrings = []
    for node in node.childNodes:
        if node.nodeType == node.TEXT_NODE:
            substrings.append(node.data)
        elif node.nodeType == node.ELEMENT_NODE:
            substrings.append(_get_text(node))
    return ''.join(substrings)

def _get_text_for_element(parent, tagname):
    substrings = []

    for node in parent.getElementsByTagName(tagname):
        substrings.append(_get_text(node))

    return ''.join(substrings)

def _get_file_location(node):
    for node in node.childNodes:
        if node.nodeType == node.ELEMENT_NODE and node.tagName == 'location':
            return node.attributes['file'].value
    if DEBUG:
        print('no location in:', node)
    return None

def _has_element(node, tagname):
    for node in node.childNodes:
        if node.nodeType == node.ELEMENT_NODE and node.tagName in tagname:
            return True
    return False

def _print_attribs(node, attribs):
    for attrib in attribs:
        print(' ', attrib, '=', node.attributes[attrib].value)

def _concat_text_from_tags(parent, tagnames):
    substrings = []

    for tag in tagnames:
        substrings.append(_get_text_for_element(parent, tag))

    return ''.join(substrings)

def _print_location(node):
    print(' ', 'location', '=', _get_file_location(node))

def _get_attribs(node):
    kind = node.attributes['kind'].value
    static = node.attributes['static'].value
    prot = node.attributes['prot'].value
    return kind, static, prot

COMPONENT_MAP = {}
SYMBOLS = {'public' : set(), 'private' : set()}

def _report(publish, symbol):
    symbol = symbol.replace('~', '?')

    if publish:
        SYMBOLS['public'].add(symbol)
    else:
        SYMBOLS['private'].add(symbol)

    if not DEBUG:
        return

    if publish:
        print('  PUBLISH: {}'.format(symbol))
    else:
        print('NOPUBLISH: {}'.format(symbol))

OLD_STANZAS = '''MIRAL_4.0 {
# WaylandExtensions::Context ctor and dtor symbols are inline
# but can be exported by unoptimized builds.
# This makes the "regenerate" script hide them:
#    miral::WaylandExtensions::Context::?Context*;
#    miral::WaylandExtensions::Context::Context*;
#    vtable?for?miral::WaylandExtensions::Context;
global:
  extern "C++" {
    miral::AddInitCallback::?AddInitCallback*;
    miral::AddInitCallback::AddInitCallback*;
    miral::AddInitCallback::operator*;
    miral::AppendEventFilter::AppendEventFilter*;
    miral::AppendEventFilter::operator*;
    miral::ApplicationAuthorizer::?ApplicationAuthorizer*;
    miral::ApplicationAuthorizer::ApplicationAuthorizer*;
    miral::ApplicationAuthorizer::operator*;
    miral::ApplicationCredentials::ApplicationCredentials*;
    miral::ApplicationCredentials::gid*;
    miral::ApplicationCredentials::pid*;
    miral::ApplicationCredentials::uid*;
    miral::ApplicationInfo::?ApplicationInfo*;
    miral::ApplicationInfo::ApplicationInfo*;
    miral::ApplicationInfo::application*;
    miral::ApplicationInfo::name*;
    miral::ApplicationInfo::operator*;
    miral::ApplicationInfo::userdata*;
    miral::ApplicationInfo::windows*;
    miral::BasicSetApplicationAuthorizer::?BasicSetApplicationAuthorizer*;
    miral::BasicSetApplicationAuthorizer::BasicSetApplicationAuthorizer*;
    miral::BasicSetApplicationAuthorizer::operator*;
    miral::BasicSetApplicationAuthorizer::the_application_authorizer*;
    miral::CanonicalWindowManagerPolicy::CanonicalWindowManagerPolicy*;
    miral::CanonicalWindowManagerPolicy::advise_focus_gained*;
    miral::CanonicalWindowManagerPolicy::confirm_inherited_move*;
    miral::CanonicalWindowManagerPolicy::confirm_placement_on_display*;
    miral::CanonicalWindowManagerPolicy::handle_modify_window*;
    miral::CanonicalWindowManagerPolicy::handle_raise_window*;
    miral::CanonicalWindowManagerPolicy::handle_window_ready*;
    miral::CanonicalWindowManagerPolicy::place_new_window*;
    miral::ConfigurationOption::?ConfigurationOption*;
    miral::ConfigurationOption::ConfigurationOption*;
    miral::ConfigurationOption::operator*;
    miral::CursorTheme::?CursorTheme*;
    miral::CursorTheme::CursorTheme*;
    miral::CursorTheme::operator*;
    miral::DisplayConfiguration::?DisplayConfiguration*;
    miral::DisplayConfiguration::DisplayConfiguration*;
    miral::DisplayConfiguration::add_output_attribute*;
    miral::DisplayConfiguration::layout_option*;
    miral::DisplayConfiguration::list_layouts*;
    miral::DisplayConfiguration::operator*;
    miral::DisplayConfiguration::select_layout*;
    miral::ExternalClientLauncher::?ExternalClientLauncher*;
    miral::ExternalClientLauncher::ExternalClientLauncher*;
    miral::ExternalClientLauncher::launch*;
    miral::ExternalClientLauncher::launch_using_x11*;
    miral::ExternalClientLauncher::operator*;
    miral::ExternalClientLauncher::snapcraft_launch*;
    miral::ExternalClientLauncher::split_command*;
    miral::FdHandle::?FdHandle*;
    miral::InternalClientLauncher::?InternalClientLauncher*;
    miral::InternalClientLauncher::InternalClientLauncher*;
    miral::InternalClientLauncher::launch*;
    miral::InternalClientLauncher::operator*;
    miral::Keymap::?Keymap*;
    miral::Keymap::Keymap*;
    miral::Keymap::operator*;
    miral::Keymap::set_keymap*;
    miral::MinimalWindowManager::?MinimalWindowManager*;
    miral::MinimalWindowManager::MinimalWindowManager*;
    miral::MinimalWindowManager::advise_delete_app*;
    miral::MinimalWindowManager::advise_focus_gained*;
    miral::MinimalWindowManager::advise_focus_lost*;
    miral::MinimalWindowManager::advise_new_app*;
    miral::MinimalWindowManager::begin_pointer_move*;
    miral::MinimalWindowManager::begin_pointer_resize*;
    miral::MinimalWindowManager::begin_touch_move*;
    miral::MinimalWindowManager::begin_touch_resize*;
    miral::MinimalWindowManager::confirm_inherited_move*;
    miral::MinimalWindowManager::confirm_placement_on_display*;
    miral::MinimalWindowManager::handle_keyboard_event*;
    miral::MinimalWindowManager::handle_modify_window*;
    miral::MinimalWindowManager::handle_pointer_event*;
    miral::MinimalWindowManager::handle_raise_window*;
    miral::MinimalWindowManager::handle_request_move*;
    miral::MinimalWindowManager::handle_request_resize*;
    miral::MinimalWindowManager::handle_touch_event*;
    miral::MinimalWindowManager::handle_window_ready*;
    miral::MinimalWindowManager::place_new_window*;
    miral::MirRunner::?MirRunner*;
    miral::MirRunner::MirRunner*;
    miral::MirRunner::add_start_callback*;
    miral::MirRunner::add_stop_callback*;
    miral::MirRunner::config_file*;
    miral::MirRunner::display_config_file*;
    miral::MirRunner::register_fd_handler*;
    miral::MirRunner::register_signal_handler*;
    miral::MirRunner::run_with*;
    miral::MirRunner::set_exception_handler*;
    miral::MirRunner::stop*;
    miral::MirRunner::wayland_display*;
    miral::MirRunner::x11_display*;
    miral::Output::?Output*;
    miral::Output::Output*;
    miral::Output::attribute*;
    miral::Output::attributes_map*;
    miral::Output::connected*;
    miral::Output::extents*;
    miral::Output::form_factor*;
    miral::Output::id*;
    miral::Output::is_same_output*;
    miral::Output::logical_group_id*;
    miral::Output::name*;
    miral::Output::operator*;
    miral::Output::orientation*;
    miral::Output::physical_size_mm*;
    miral::Output::pixel_format*;
    miral::Output::power_mode*;
    miral::Output::refresh_rate*;
    miral::Output::scale*;
    miral::Output::type*;
    miral::Output::used*;
    miral::Output::valid*;
    miral::PrependEventFilter::PrependEventFilter*;
    miral::PrependEventFilter::operator*;
    miral::PrintTo*;
    miral::SetCommandLineHandler::?SetCommandLineHandler*;
    miral::SetCommandLineHandler::SetCommandLineHandler*;
    miral::SetCommandLineHandler::operator*;
    miral::SetTerminator::?SetTerminator*;
    miral::SetTerminator::SetTerminator*;
    miral::SetTerminator::operator*;
    miral::SetWindowManagementPolicy::?SetWindowManagementPolicy*;
    miral::SetWindowManagementPolicy::SetWindowManagementPolicy*;
    miral::SetWindowManagementPolicy::operator*;
    miral::StartupInternalClient::?StartupInternalClient*;
    miral::StartupInternalClient::StartupInternalClient*;
    miral::StartupInternalClient::operator*;
    miral::WaylandExtensions::?WaylandExtensions*;
    miral::WaylandExtensions::Context::operator*;
    miral::WaylandExtensions::EnableInfo::app*;
    miral::WaylandExtensions::EnableInfo::name*;
    miral::WaylandExtensions::EnableInfo::user_preference*;
    miral::WaylandExtensions::WaylandExtensions*;
    miral::WaylandExtensions::add_extension*;
    miral::WaylandExtensions::add_extension_disabled_by_default*;
    miral::WaylandExtensions::all_supported*;
    miral::WaylandExtensions::conditionally_enable*;
    miral::WaylandExtensions::disable*;
    miral::WaylandExtensions::enable*;
    miral::WaylandExtensions::ext_session_lock_manager_v1*;
    miral::WaylandExtensions::operator*;
    miral::WaylandExtensions::recommended*;
    miral::WaylandExtensions::supported*;
    miral::WaylandExtensions::zwlr_foreign_toplevel_manager_v1*;
    miral::WaylandExtensions::zwlr_layer_shell_v1*;
    miral::WaylandExtensions::zwlr_screencopy_manager_v1*;
    miral::WaylandExtensions::zwlr_virtual_pointer_manager_v1*;
    miral::WaylandExtensions::zwp_input_method_manager_v2*;
    miral::WaylandExtensions::zwp_virtual_keyboard_manager_v1*;
    miral::WaylandExtensions::zxdg_output_manager_v1*;
    miral::Window::?Window*;
    miral::Window::Window*;
    miral::Window::application*;
    miral::Window::move_to*;
    miral::Window::operator*;
    miral::Window::resize*;
    miral::Window::size*;
    miral::Window::top_left*;
    miral::WindowInfo::?WindowInfo*;
    miral::WindowInfo::WindowInfo*;
    miral::WindowInfo::application_id*;
    miral::WindowInfo::attached_edges*;
    miral::WindowInfo::can_be_active*;
    miral::WindowInfo::can_morph_to*;
    miral::WindowInfo::children*;
    miral::WindowInfo::clip_area*;
    miral::WindowInfo::confine_pointer*;
    miral::WindowInfo::constrain_resize*;
    miral::WindowInfo::depth_layer*;
    miral::WindowInfo::exclusive_rect*;
    miral::WindowInfo::focus_mode*;
    miral::WindowInfo::has_output_id*;
    miral::WindowInfo::height_inc*;
    miral::WindowInfo::is_visible*;
    miral::WindowInfo::max_aspect*;
    miral::WindowInfo::max_height*;
    miral::WindowInfo::max_width*;
    miral::WindowInfo::min_aspect*;
    miral::WindowInfo::min_height*;
    miral::WindowInfo::min_width*;
    miral::WindowInfo::must_have_parent*;
    miral::WindowInfo::must_not_have_parent*;
    miral::WindowInfo::name*;
    miral::WindowInfo::needs_titlebar*;
    miral::WindowInfo::operator*;
    miral::WindowInfo::output_id*;
    miral::WindowInfo::parent*;
    miral::WindowInfo::preferred_orientation*;
    miral::WindowInfo::restore_rect*;
    miral::WindowInfo::shell_chrome*;
    miral::WindowInfo::state*;
    miral::WindowInfo::type*;
    miral::WindowInfo::userdata*;
    miral::WindowInfo::visible_on_lock_screen*;
    miral::WindowInfo::width_inc*;
    miral::WindowInfo::window*;
    miral::WindowManagementPolicy::?WindowManagementPolicy*;
    miral::WindowManagementPolicy::WindowManagementPolicy*;
    miral::WindowManagementPolicy::advise_adding_to_workspace*;
    miral::WindowManagementPolicy::advise_application_zone_create*;
    miral::WindowManagementPolicy::advise_application_zone_delete*;
    miral::WindowManagementPolicy::advise_application_zone_update*;
    miral::WindowManagementPolicy::advise_begin*;
    miral::WindowManagementPolicy::advise_delete_app*;
    miral::WindowManagementPolicy::advise_delete_window*;
    miral::WindowManagementPolicy::advise_end*;
    miral::WindowManagementPolicy::advise_focus_gained*;
    miral::WindowManagementPolicy::advise_focus_lost*;
    miral::WindowManagementPolicy::advise_move_to*;
    miral::WindowManagementPolicy::advise_new_app*;
    miral::WindowManagementPolicy::advise_new_window*;
    miral::WindowManagementPolicy::advise_output_create*;
    miral::WindowManagementPolicy::advise_output_delete*;
    miral::WindowManagementPolicy::advise_output_update*;
    miral::WindowManagementPolicy::advise_raise*;
    miral::WindowManagementPolicy::advise_removing_from_workspace*;
    miral::WindowManagementPolicy::advise_resize*;
    miral::WindowManagementPolicy::advise_state_change*;
    miral::WindowManagementPolicy::operator*;
    miral::WindowManagerOptions::WindowManagerOptions*;
    miral::WindowManagerOptions::operator*;
    miral::WindowManagerTools::?WindowManagerTools*;
    miral::WindowManagerTools::WindowManagerTools*;
    miral::WindowManagerTools::active_application_zone*;
    miral::WindowManagerTools::active_output*;
    miral::WindowManagerTools::active_window*;
    miral::WindowManagerTools::add_tree_to_workspace*;
    miral::WindowManagerTools::ask_client_to_close*;
    miral::WindowManagerTools::count_applications*;
    miral::WindowManagerTools::create_workspace*;
    miral::WindowManagerTools::drag_active_window*;
    miral::WindowManagerTools::drag_window*;
    miral::WindowManagerTools::find_application*;
    miral::WindowManagerTools::focus_next_application*;
    miral::WindowManagerTools::focus_next_within_application*;
    miral::WindowManagerTools::focus_prev_application*;
    miral::WindowManagerTools::focus_prev_within_application*;
    miral::WindowManagerTools::for_each_application*;
    miral::WindowManagerTools::for_each_window_in_workspace*;
    miral::WindowManagerTools::for_each_workspace_containing*;
    miral::WindowManagerTools::id_for_window*;
    miral::WindowManagerTools::info_for*;
    miral::WindowManagerTools::info_for_window_id*;
    miral::WindowManagerTools::invoke_under_lock*;
    miral::WindowManagerTools::modify_window*;
    miral::WindowManagerTools::move_workspace_content_to_workspace*;
    miral::WindowManagerTools::operator*;
    miral::WindowManagerTools::place_and_size_for_state*;
    miral::WindowManagerTools::raise_tree*;
    miral::WindowManagerTools::remove_tree_from_workspace*;
    miral::WindowManagerTools::select_active_window*;
    miral::WindowManagerTools::send_tree_to_back*;
    miral::WindowManagerTools::swap_tree_order*;
    miral::WindowManagerTools::window_at*;
    miral::WindowManagerTools::window_to_select_application*;
    miral::WindowSpecification::?WindowSpecification*;
    miral::WindowSpecification::WindowSpecification*;
    miral::WindowSpecification::application_id*;
    miral::WindowSpecification::attached_edges*;
    miral::WindowSpecification::aux_rect*;
    miral::WindowSpecification::aux_rect_placement_gravity*;
    miral::WindowSpecification::aux_rect_placement_offset*;
    miral::WindowSpecification::confine_pointer*;
    miral::WindowSpecification::depth_layer*;
    miral::WindowSpecification::exclusive_rect*;
    miral::WindowSpecification::focus_mode*;
    miral::WindowSpecification::height_inc*;
    miral::WindowSpecification::input_mode*;
    miral::WindowSpecification::input_shape*;
    miral::WindowSpecification::max_aspect*;
    miral::WindowSpecification::max_height*;
    miral::WindowSpecification::max_width*;
    miral::WindowSpecification::min_aspect*;
    miral::WindowSpecification::min_height*;
    miral::WindowSpecification::min_width*;
    miral::WindowSpecification::name*;
    miral::WindowSpecification::operator*;
    miral::WindowSpecification::output_id*;
    miral::WindowSpecification::parent*;
    miral::WindowSpecification::placement_hints*;
    miral::WindowSpecification::preferred_orientation*;
    miral::WindowSpecification::server_side_decorated*;
    miral::WindowSpecification::shell_chrome*;
    miral::WindowSpecification::size*;
    miral::WindowSpecification::state*;
    miral::WindowSpecification::top_left*;
    miral::WindowSpecification::type*;
    miral::WindowSpecification::userdata*;
    miral::WindowSpecification::visible_on_lock_screen*;
    miral::WindowSpecification::width_inc*;
    miral::WindowSpecification::window_placement_gravity*;
    miral::X11Support::?X11Support*;
    miral::X11Support::X11Support*;
    miral::X11Support::operator*;
    miral::Zone::?Zone*;
    miral::Zone::Zone*;
    miral::Zone::extents*;
    miral::Zone::id*;
    miral::Zone::is_same_zone*;
    miral::Zone::operator*;
    miral::application_for*;
    miral::apply_lifecycle_state_to*;
    miral::display_configuration_options*;
    miral::equivalent_display_area*;
    miral::kill*;
    miral::name_of*;
    miral::operator*;
    miral::pid_of*;
    miral::pre_init*;
    miral::socket_fd_of*;
    miral::toolkit::mir_event_get_input_event*;
    miral::toolkit::mir_event_get_type*;
    miral::toolkit::mir_input_event_get_event*;
    miral::toolkit::mir_input_event_get_event_time*;
    miral::toolkit::mir_input_event_get_keyboard_event*;
    miral::toolkit::mir_input_event_get_pointer_event*;
    miral::toolkit::mir_input_event_get_touch_event*;
    miral::toolkit::mir_input_event_get_type*;
    miral::toolkit::mir_input_event_has_cookie*;
    miral::toolkit::mir_keyboard_event_action*;
    miral::toolkit::mir_keyboard_event_input_event*;
    miral::toolkit::mir_keyboard_event_key_text*;
    miral::toolkit::mir_keyboard_event_keysym*;
    miral::toolkit::mir_keyboard_event_modifiers*;
    miral::toolkit::mir_keyboard_event_scan_code*;
    miral::toolkit::mir_pointer_event_action*;
    miral::toolkit::mir_pointer_event_axis_value*;
    miral::toolkit::mir_pointer_event_button_state*;
    miral::toolkit::mir_pointer_event_buttons*;
    miral::toolkit::mir_pointer_event_input_event*;
    miral::toolkit::mir_pointer_event_modifiers*;
    miral::toolkit::mir_touch_event_action*;
    miral::toolkit::mir_touch_event_axis_value*;
    miral::toolkit::mir_touch_event_id*;
    miral::toolkit::mir_touch_event_input_event*;
    miral::toolkit::mir_touch_event_modifiers*;
    miral::toolkit::mir_touch_event_point_count*;
    miral::toolkit::mir_touch_event_tooltype*;
    miral::window_for*;
    non-virtual?thunk?to?miral::ApplicationAuthorizer::?ApplicationAuthorizer*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::advise_focus_gained*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::confirm_inherited_move*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::confirm_placement_on_display*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::handle_modify_window*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::handle_raise_window*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::handle_window_ready*;
    non-virtual?thunk?to?miral::CanonicalWindowManagerPolicy::place_new_window*;
    non-virtual?thunk?to?miral::FdHandle::?FdHandle*;
    non-virtual?thunk?to?miral::MinimalWindowManager::advise_delete_app*;
    non-virtual?thunk?to?miral::MinimalWindowManager::advise_focus_gained*;
    non-virtual?thunk?to?miral::MinimalWindowManager::advise_focus_lost*;
    non-virtual?thunk?to?miral::MinimalWindowManager::advise_new_app*;
    non-virtual?thunk?to?miral::MinimalWindowManager::confirm_inherited_move*;
    non-virtual?thunk?to?miral::MinimalWindowManager::confirm_placement_on_display*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_keyboard_event*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_modify_window*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_pointer_event*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_raise_window*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_request_move*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_request_resize*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_touch_event*;
    non-virtual?thunk?to?miral::MinimalWindowManager::handle_window_ready*;
    non-virtual?thunk?to?miral::MinimalWindowManager::place_new_window*;
    non-virtual?thunk?to?miral::WaylandExtensions::Context::?Context*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::?WindowManagementPolicy*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_adding_to_workspace*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_application_zone_create*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_application_zone_delete*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_application_zone_update*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_begin*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_delete_app*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_delete_window*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_end*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_focus_gained*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_focus_lost*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_move_to*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_new_app*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_new_window*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_output_create*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_output_delete*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_output_update*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_raise*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_removing_from_workspace*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_resize*;
    non-virtual?thunk?to?miral::WindowManagementPolicy::advise_state_change*;
    typeinfo?for?miral::AddInitCallback;
    typeinfo?for?miral::AppendEventFilter;
    typeinfo?for?miral::ApplicationAuthorizer;
    typeinfo?for?miral::ApplicationCredentials;
    typeinfo?for?miral::ApplicationInfo;
    typeinfo?for?miral::BasicSetApplicationAuthorizer;
    typeinfo?for?miral::CanonicalWindowManagerPolicy;
    typeinfo?for?miral::ConfigurationOption;
    typeinfo?for?miral::CursorTheme;
    typeinfo?for?miral::DisplayConfiguration;
    typeinfo?for?miral::ExternalClientLauncher;
    typeinfo?for?miral::FdHandle;
    typeinfo?for?miral::InternalClientLauncher;
    typeinfo?for?miral::Keymap;
    typeinfo?for?miral::MinimalWindowManager;
    typeinfo?for?miral::MirRunner;
    typeinfo?for?miral::Output;
    typeinfo?for?miral::Output::PhysicalSizeMM;
    typeinfo?for?miral::PrependEventFilter;
    typeinfo?for?miral::SetCommandLineHandler;
    typeinfo?for?miral::SetTerminator;
    typeinfo?for?miral::SetWindowManagementPolicy;
    typeinfo?for?miral::StartupInternalClient;
    typeinfo?for?miral::WaylandExtensions;
    typeinfo?for?miral::WaylandExtensions::Builder;
    typeinfo?for?miral::WaylandExtensions::Context;
    typeinfo?for?miral::WaylandExtensions::EnableInfo;
    typeinfo?for?miral::Window;
    typeinfo?for?miral::WindowInfo;
    typeinfo?for?miral::WindowManagementPolicy;
    typeinfo?for?miral::WindowManagerOption;
    typeinfo?for?miral::WindowManagerOptions;
    typeinfo?for?miral::WindowManagerTools;
    typeinfo?for?miral::WindowSpecification;
    typeinfo?for?miral::WindowSpecification::AspectRatio;
    typeinfo?for?miral::X11Support;
    typeinfo?for?miral::Zone;
    vtable?for?miral::AddInitCallback;
    vtable?for?miral::AppendEventFilter;
    vtable?for?miral::ApplicationAuthorizer;
    vtable?for?miral::ApplicationCredentials;
    vtable?for?miral::ApplicationInfo;
    vtable?for?miral::BasicSetApplicationAuthorizer;
    vtable?for?miral::CanonicalWindowManagerPolicy;
    vtable?for?miral::ConfigurationOption;
    vtable?for?miral::CursorTheme;
    vtable?for?miral::DisplayConfiguration;
    vtable?for?miral::ExternalClientLauncher;
    vtable?for?miral::FdHandle;
    vtable?for?miral::InternalClientLauncher;
    vtable?for?miral::Keymap;
    vtable?for?miral::MinimalWindowManager;
    vtable?for?miral::MirRunner;
    vtable?for?miral::Output;
    vtable?for?miral::Output::PhysicalSizeMM;
    vtable?for?miral::PrependEventFilter;
    vtable?for?miral::SetCommandLineHandler;
    vtable?for?miral::SetTerminator;
    vtable?for?miral::SetWindowManagementPolicy;
    vtable?for?miral::StartupInternalClient;
    vtable?for?miral::WaylandExtensions;
    vtable?for?miral::WaylandExtensions::Builder;
    vtable?for?miral::WaylandExtensions::EnableInfo;
    vtable?for?miral::Window;
    vtable?for?miral::WindowInfo;
    vtable?for?miral::WindowManagementPolicy;
    vtable?for?miral::WindowManagerOption;
    vtable?for?miral::WindowManagerOptions;
    vtable?for?miral::WindowManagerTools;
    vtable?for?miral::WindowSpecification;
    vtable?for?miral::WindowSpecification::AspectRatio;
    vtable?for?miral::X11Support;
    vtable?for?miral::Zone;
  };

  local: *;
};

MIRAL_4.1 {
global:'''

END_NEW_STANZA = '''
} MIRAL_4.0;
'''

def _print_report():
    print(OLD_STANZAS)
    new_symbols = False;
    for symbol in sorted(SYMBOLS['public']):
        formatted_symbol = '    {};'.format(symbol)
        if formatted_symbol not in OLD_STANZAS and 'miral::' in formatted_symbol:
            if not new_symbols:
                new_symbols = True;
                print('  extern "C++" {')
            print(formatted_symbol)

    if new_symbols: print("  };")
    print(END_NEW_STANZA)

def _print_debug_info(node, attributes):
    if not DEBUG:
        return
    print()
    _print_attribs(node, attributes)
    _print_location(node)

def _parse_member_def(context_name, node, is_class):
    kind = node.attributes['kind'].value

    if (kind in ['enum', 'typedef']
        or _has_element(node, ['templateparamlist'])
        or kind in ['function'] and node.attributes['inline'].value == 'yes'):
        return

    name = _concat_text_from_tags(node, ['name'])

    if name in ['__attribute__']:
        if DEBUG:
            print('  ignoring doxygen mis-parsing:', _concat_text_from_tags(node, ['argsstring']))
        return

    if name.startswith('operator'):
        name = 'operator'

    if not context_name is None:
        symbol = context_name + '::' + name
    else:
        symbol = name

    is_function = kind == 'function'

    if is_function:
        _print_debug_info(node, ['kind', 'prot', 'static', 'virt'])
    else:
        _print_debug_info(node, ['kind', 'prot', 'static'])

    if DEBUG:
        print('  is_class:', is_class)

    publish = _should_publish(is_class, is_function, node)

    _report(publish, symbol + '*')

    if is_function and node.attributes['virt'].value == 'virtual':
        _report(publish, 'non-virtual?thunk?to?' + symbol + '*')


def _should_publish(is_class, is_function, node):
    (kind, static, prot) = _get_attribs(node)

    publish = True

    if publish:
        publish = kind != 'define'

    if publish and is_class:
        publish = is_function or static == 'yes'

    if publish and prot == 'private':
        if is_function:
            publish = node.attributes['virt'].value == 'virtual'
        else:
            publish = False

    if publish and _has_element(node, ['argsstring']):
        publish = not _get_text_for_element(node, 'argsstring').endswith('=0')

    return publish


def _parse_compound_defs(xmldoc):
    compounddefs = xmldoc.getElementsByTagName('compounddef')
    for node in compounddefs:
        kind = node.attributes['kind'].value

        if kind in ['page', 'file', 'example', 'union']:
            continue

        if kind in ['group']:
            for member in node.getElementsByTagName('memberdef'):
                _parse_member_def(None, member, False)
            continue

        if kind in ['namespace']:
            symbol = _concat_text_from_tags(node, ['compoundname'])
            for member in node.getElementsByTagName('memberdef'):
                _parse_member_def(symbol, member, False)
            continue

        filename = _get_file_location(node)

        if DEBUG:
            print('  from file:', filename)

        if ('/examples/' in filename or '/test/' in filename or '[generated]' in filename
            or '[STL]' in filename or _has_element(node, ['templateparamlist'])):
            continue

        symbol = _concat_text_from_tags(node, ['compoundname'])

        publish = True

        if publish:
            if kind in ['class', 'struct']:
                prot = node.attributes['prot'].value
                publish = prot != 'private'
                _print_debug_info(node, ['kind', 'prot'])
                _report(publish, 'vtable?for?' + symbol)
                _report(publish, 'typeinfo?for?' + symbol)

        if publish:
            for member in node.getElementsByTagName('memberdef'):
                _parse_member_def(symbol, member, kind in ['class', 'struct'])

if __name__ == "__main__":
    if len(argv) == 1 or '-h' in argv or '--help' in argv:
        print(HELPTEXT)
        exit()

    for arg in argv[1:]:
        try:
            if DEBUG:
                print('Processing:', arg)
            _parse_compound_defs(minidom.parse(arg))
        except Exception as error:
            print('Error:', arg, error)

    if DEBUG:
        print('Processing complete')

    _print_report()
