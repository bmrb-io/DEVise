#ifndef _DEVISEKEY_H_
#define _DEVISEKEY_H_


class DeviseKey
{
  public:

    enum Mask {
	MODIFIER_MASK = 0xff000000,
	KEY_MASK = 0x00ffffff,
    };

    enum Modifier {
	SHIFT   = 0x01000000,
	CONTROL = 0x02000000,
	ALT     = 0x04000000,
    };

    enum Key {

	// all of the ascii keys

	// tty functions, map some to there ascii eqv.
	SPACE     = ' ',
	BACKSPACE = '\b',
	TAB       = '\t',
	LINEFEED  = '\n',
	RETURN    = '\r',
	ESCAPE    = 0x1b,
	DELETE    = 0x7f,

	CLEAR = 0x10001,
	PAUSE,
	SCROLL_LOCK,
	SYS_REQ,

	// cursor control & motion
	HOME = 0x20001,
	LEFT,
	UP,
	RIGHT,
	DOWN,
	PRIOR,
	PAGE_UP,
	NEXT,
	PAGE_DOWN,
	END,
	BEGIN,

	// misc functions
	SELECT = 0x30001,
	PRINT,
	EXECUTE,
	INSERT,
	UNDO,
	REDO,
	MENU,
	FIND,
	CANCEL,
	HELP,
	BREAK,
	MODE_SWITCH,
	SCRIPT_SWITCH,
	// NUM_LOCK, // not mapped

	// keypad functions
	KEYPAD_KEY = 0x40000,
	KP_SPACE,
	KP_TAB,
	KP_ENTER,
	KP_F1,
	KP_F2,
	KP_F3,
	KP_F4,
	KP_HOME,
	KP_LEFT,
	KP_UP,
	KP_RIGHT,
	KP_DOWN,
	KP_PRIOR,
	KP_PAGE_UP,
	KP_NEXT,
	KP_PAGE_DOWN,
	KP_END,
	KP_BEGIN,
	KP_INSERT,
	KP_DELETE,
	KP_EQUAL,
	KP_MULTIPLY,
	KP_ADD,
	KP_SEPARATOR,
	KP_SUBTRACT,
	KP_DECIMAL,
	KP_DIVIDE,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,

	// function keys
	FUNCTION_KEY = 0x50000,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	// mouse "keys" 
	// formed by or'ing button with action, eg MOUSE_CLICK|BUTTON_1
	BUTTON   = 0x60000,
	BUTTON_1 = 0x60100,
	BUTTON_2 = 0x60200,
	BUTTON_3 = 0x60400,

	MOUSE_SELECT_AREA = 0x60001,
	MOUSE_CLICK,
	MOUSE_DOUBLE_CLICK,
	MOUSE_PRESS,
    };

    static int Button(int i) { 
	if( i > 0 && i < 3 ) {
	    return BUTTON | (0x080<<i);
	}
	return 0;
    }

    static bool OnKeypad(int key) { return key & KEYPAD_KEY; }

    // note: IsFunctionKey(int key) conflicts with X-Windows macro
    // add'l note: I hate macros
    static bool IsFKey(int key) { return key & FUNCTION_KEY; }

    // extracts the modifier
    static int Modifier(int key) { return key & MODIFIER_MASK; }

    // removes any modifiers
    static int BaseKey(int key)  { return key & KEY_MASK; }
};


#endif // _DEVISEKEY_H_
