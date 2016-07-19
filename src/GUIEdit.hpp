#pragma once

#include <sstream>

#include <GUIElement.hpp>
#include <GUIText.hpp>

template<typename T = float>
class GUIEdit : public GUIElement
{
public:
	using Callback = std::function<void(void)>;
	
	GUIEdit(const std::string& label, T* value) :
		_value(value),
		_strValue(std::to_string(*_value)),
		_textLabel(label),
		_textValue(_strValue)
	{
		init();
	}
	
	GUIEdit(const std::string& label, T* value, Callback callback) :
		_value(value),
		_callback(callback),
		_strValue(std::to_string(*_value)),
		_textLabel(label),
		_textValue(_strValue)
	{
		init();
	}
	
	void updateAABB()
	{
		_aabb = _textLabel.getAABB() + (_textValue.getAABB() + _textValue.Position);
		_aabb.min -= glm::vec2{4.0};
		_aabb.max += glm::vec2{4.0};
	}

	virtual bool onClick(const glm::vec2& coords, int button) override
	{
		_active = true;
		return true;
	}
	
	virtual bool handleKey(int key, int scancode, int action, int mods) override
	{
		if(_active)
		{
			if(key == 257) // Return, GLFW_KEY_ENTER
			{
				if(_strValue == "") // Not valid, back to previous state.
				{
					_strValue = std::to_string(*_value);
				} else {
					*_value = from_string(_strValue);
					if(_callback)
						_callback();
				}
				_active = false;
			} else if(key == 259) { // Backspace, GLFW_KEY_BACKSPACE
				if(_strValue.size() > 0)
					_strValue.resize(_strValue.size() - 1);
			} else if(key == 256) { // Escape, GLFW_KEY_ESCAPE
				_strValue = std::to_string(*_value);
				_active = false;
			}
			
			_textValue.setText(_strValue);
			updateAABB();
			return true; // Always return true while we're active.
		}
		
		return false;
	}

	virtual bool handleTextInput(unsigned int unicode) override
	{
		if(_active)
		{
			/// @todo Check if char is valid for T
			_strValue += unicode;
			
			_textValue.setText(_strValue);
			updateAABB();
			return true;
		}
		
		return false;
	}
	
	void draw(const glm::vec2& resolution, const glm::vec2& position = glm::vec2(0.0)) override
	{
		auto p = c2p(position);
		
		if(_active)
		{
			drawAABB(resolution, p, glm::vec4(1.0, 1.0, 1.0, 0.25));
		}

		_textLabel.draw(resolution, p);
		_textValue.draw(resolution, p);
	}
	
	static T from_string(const std::string& str)
	{
		T ret;
		std::istringstream iss(str);
		iss >> ret;
		return ret;
	}

private:
	T*			_value;
	Callback	_callback;
	
	std::string	_strValue;
	
	GUIText		_textLabel;
	GUIText		_textValue;
	
	void init()
	{
		_textValue.Position = {_textLabel.getAABB().max.x, 0.0};
		updateAABB();
	}
};
