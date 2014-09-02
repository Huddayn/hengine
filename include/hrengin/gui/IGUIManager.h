#ifndef _hrengin_IGUIManager_
#define _hrengin_IGUIManager_

#include <hrengin/common/Rect.h>

namespace hrengin {
namespace gui {
class IInputManager;
class IGUIElement;
class IGUIWindow;
class IGUITextBox;
class IGUIListBox;

class IGUIManager {
public:
	virtual ~IGUIManager() {};

	virtual void draw() = 0;

	virtual IInputManager* getInputManager() = 0;

	virtual void setFont(std::string path) = 0;

	virtual IGUIWindow* addWindow(Rect<i32> rect, bool isModal = false, 
		const wchar_t* title = 0, IGUIElement* parent = 0,
		i32 id = -1) = 0;

	virtual IGUITextBox* addTextBox(Rect<i32> rect, const wchar_t* text,
		bool border = true, IGUIElement* parent = 0, i32 id = -1) = 0;

	virtual IGUIListBox* addListBox(Rect<i32> rect, bool background = true,
		IGUIElement* parent = 0, i32 id = -1) = 0;
};

} // namespace gui
} // namespace hrengin

#endif //_hrengin_IGUIManager_
