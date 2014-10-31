#include "stdafx.h"

//http://cegui.org.uk/wiki/GLFW_to_CEGUI_Key_and_Mouse_Mapping
CEGUI::Key::Scan GlfwToCeguiKey(int glfwKey)
{
	switch(glfwKey)
	{
		case GLFW_KEY_UNKNOWN	: return CEGUI::Key::Unknown;
		case GLFW_KEY_ESCAPE	: return CEGUI::Key::Escape;
		case GLFW_KEY_F1	: return CEGUI::Key::F1;
		case GLFW_KEY_F2	: return CEGUI::Key::F2;
		case GLFW_KEY_F3	: return CEGUI::Key::F3;
		case GLFW_KEY_F4	: return CEGUI::Key::F4;
		case GLFW_KEY_F5	: return CEGUI::Key::F5;
		case GLFW_KEY_F6	: return CEGUI::Key::F6;
		case GLFW_KEY_F7	: return CEGUI::Key::F7;
		case GLFW_KEY_F8	: return CEGUI::Key::F8;
		case GLFW_KEY_F9	: return CEGUI::Key::F9;
		case GLFW_KEY_F10       : return CEGUI::Key::F10;
		case GLFW_KEY_F11       : return CEGUI::Key::F11;
		case GLFW_KEY_F12       : return CEGUI::Key::F12;
		case GLFW_KEY_F13       : return CEGUI::Key::F13;
		case GLFW_KEY_F14       : return CEGUI::Key::F14;
		case GLFW_KEY_F15       : return CEGUI::Key::F15;
		case GLFW_KEY_UP        : return CEGUI::Key::ArrowUp;
		case GLFW_KEY_DOWN      : return CEGUI::Key::ArrowDown;
		case GLFW_KEY_LEFT      : return CEGUI::Key::ArrowLeft;
		case GLFW_KEY_RIGHT     : return CEGUI::Key::ArrowRight;
		case GLFW_KEY_LEFT_SHIFT    : return CEGUI::Key::LeftShift;
		case GLFW_KEY_RIGHT_SHIFT    : return CEGUI::Key::RightShift;
		case GLFW_KEY_LEFT_CONTROL     : return CEGUI::Key::LeftControl;
		case GLFW_KEY_RIGHT_CONTROL    : return CEGUI::Key::RightControl;
		case GLFW_KEY_LEFT_ALT      : return CEGUI::Key::LeftAlt;
		case GLFW_KEY_RIGHT_ALT      : return CEGUI::Key::RightAlt;
		case GLFW_KEY_TAB       : return CEGUI::Key::Tab;
		case GLFW_KEY_ENTER     : return CEGUI::Key::Return;
		case GLFW_KEY_BACKSPACE : return CEGUI::Key::Backspace;
		case GLFW_KEY_INSERT    : return CEGUI::Key::Insert;
		case GLFW_KEY_DELETE       : return CEGUI::Key::Delete;
		case GLFW_KEY_PAGE_UP    : return CEGUI::Key::PageUp;
		case GLFW_KEY_PAGE_DOWN  : return CEGUI::Key::PageDown;
		case GLFW_KEY_HOME      : return CEGUI::Key::Home;
		case GLFW_KEY_END       : return CEGUI::Key::End;
		case GLFW_KEY_KP_ENTER	: return CEGUI::Key::NumpadEnter;
		default			: return CEGUI::Key::Unknown;
	}
}
 
CEGUI::MouseButton GlfwToCeguiButton(int glfwButton)
{
	switch(glfwButton)
	{
		case GLFW_MOUSE_BUTTON_LEFT	: return CEGUI::LeftButton;
		case GLFW_MOUSE_BUTTON_RIGHT	: return CEGUI::RightButton;
		case GLFW_MOUSE_BUTTON_MIDDLE	: return CEGUI::MiddleButton;
		default				: return CEGUI::NoButton;
	}
}

std::string glfwKeyToString(int glfwKey)
{
	std::string result = "";

	if(glfwKey > 32 && glfwKey <= 96)
	{
		result += (char)glfwKey;
	}
	else if(glfwKey >= 290 && glfwKey <= 301)
	{
		char buffer[50];
		sprintf(buffer,"F%d",glfwKey-289);
		result = std::string(buffer);
	}
	else
	{
		switch(glfwKey)
		{
		case 32:
			result = "Space";
			break;
		case 257:
			result = "Enter";
			break;
		case 259:
			result = "Delete";
			break;
		case 260:
			result = "Insert";
			break;
		case 261:
			result = "Delete";
			break;
		case 266:
			result = "PgUp";
			break;
		case 267:
			result = "PgDn";
			break;
		case 268:
			result = "Home";
			break;
		case 269:
			result = "End";
			break;
		case 340:
			result = "LShift";
			break;
		case 341:
			result = "LCtrl";
			break;
		case 342:
			result = "LAlt";
			break;
		case 346:
			result = "RAlt";
			break;
		case 344:
			result = "RShift";
			break;
		case 345:
			result = "RCtrl";
			break;
		}
	}

	return result;
}