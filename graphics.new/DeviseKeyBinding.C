#include "DeviseKeyBinding.h"

#define BIND(A,B) (A)

DeviseKeyBinding::Binding DeviseKeyBinding::_default_binding[] = {

    { DeviseKey::MOUSE_SELECT_AREA|BUTTON_1,  DeviseAction::ZOOM_AREA_X },
    { DeviseKey::MOUSE_SELECT_AREA|BUTTON_3,  DeviseAction::ZOOM_AREA_XY },
    { DeviseKey::MOUSE_CLICK|BUTTON_2,        DeviseAction::QUERY_RECORD },

    { DeviseKey::KP_7, DeviseAction::ZOOM_IN_X },
    { DeviseKey::KP_9, DeviseAction::ZOOM_OUT_X },
    { DeviseKey::KP_1, DeviseAction::ZOOM_IN_Y },
    { DeviseKey::KP_3, DeviseAction::ZOOM_OUT_Y },
    {            'i',  DeviseAction::ZOOM_IN_Z },
    {            'I',  DeviseAction::ZOOM_IN_Z },
    {            'o',  DeviseAction::ZOOM_OUT_Z },
    {            'O',  DeviseAction::ZOOM_OUT_Z },
    { DeviseKey::KP_5, DeviseAction::ZOOM_MAX_XYZ },

    { 'w', DeviseAction::TOGGLE_SOLID_3D },
    { 'W', DeviseAction::TOGGLE_SOLID_3D },
    { 'v', DeviseAction::TOGGLE_VALUES },
    { 'V', DeviseAction::TOGGLE_VALUES },
    { 'z', DeviseAction::TOGGLE_ZOOM_3D },
    { 'Z', DeviseAction::TOGGLE_ZOOM_3D },
    { 'c', DeviseAction::TOGGLE_CONNECTORS },
    { 'C', DeviseAction::TOGGLE_CONNECTORS },
    { 's', DeviseAction::TOGGLE_SYMBOLS },
    { 'S', DeviseAction::TOGGLE_SYMBOLS },
    { 'h', DeviseAction::SET_HISTOGRAM_WIDTH_TO_FILTER_RANGE },
    { 'H', DeviseAction::SET_HISTOGRAM_WIDTH_TO_FILTER_RANGE },

    {           '>',    DeviseAction::PAN_LEFT },
    {           '.',    DeviseAction::PAN_LEFT },
    { DeviseKey::KP_6,  DeviseAction::PAN_LEFT },
    { DeviseKey::LEFT,  DeviseAction::PAN_LEFT },
    {           '<',    DeviseAction::PAN_RIGHT },
    {           ',',    DeviseAction::PAN_RIGHT },
    { DeviseKey::KP_4,  DeviseAction::PAN_RIGHT },
    { DeviseKey::RIGHT, DeviseAction::PAN_RIGHT },
    { DeviseKey::KP_8,  DeviseAction::PAN_UP },
    { DeviseKey::DOWN,  DeviseAction::PAN_UP },
    { DeviseKey::KP_2,  DeviseAction::PAN_DOWN },
    { DeviseKey::UP,    DeviseAction::PAN_DOWN },

    { 'f', DeviseAction::TOGGLE_FOCUS_POINT },
    { 'F', DeviseAction::TOGGLE_FOCUS_POINT },
#if 0
    { 'p', DeviseAction::TOGGLE_PERSPECTIVE },
    { 'P', DeviseAction::TOGGLE_PERSPECTIVE },
#endif

    { DeviseKey::KP_PLUS,  DeviseAction::INCREASE_PIXEL_SIZE },
    { DeviseKey::KP_MINUS, DeviseAction::DECREASE_PIXEL_SIZE },

    { 0, 0			// must last entry!
};



DeviseKeyBinding::DeviseKeyBinding()
: _key_to_fn(MAX_BINDINGS)
{
    // set default key bindings
    for(int i = 0 ; _default_binding[i].key != 0 ; i++) {
	if( _key_binding.insert(_default_binding[i].key,
				_default_binding[i].action) ) {
	    DO_ASSERT(0, "error inserting default key binding!");
	}
    }
}


int DeviseKeyBinding::Bind(int key, int action)
{
    int old_action = Unbind(key);
    DO_ASSERT(_key_binding.insert(key, action) == 0, 
	      "couldn't add key binding");
    return old_action;
}


int DeviseKeyBinding::Unbind(int key)
{
    int old_action = 0;
    if( _key_binding.lookup(key, old_action) ) {
	old_action = DeviseAction::NO_ACTION;
    } else {
	DO_ASSERT(_key_binding.remove(key) == 0, 
		  "couldn't remove key binding");
    }
    return old_action;
}

