#pragma once

#include "rack.hpp"
#include <cstring>
#include <functional>

using namespace rack;

using simd::float_4;
using simd::int32_4;

typedef std::function<int()> EnumFunc;
typedef std::function<void()> ToggleFunc;

template<typename TEnumFunc, typename TToggleFunc, class TModule, size_t num_labels>
struct NamedEnumToggle : SvgSwitch {
	TModule* module;
    std::vector<std::string> labels;
    std::string name;
    TEnumFunc enumFunc;
    TToggleFunc toggleFunc;
	ui::Tooltip* tooltip;

	void config(std::string name, std::vector<std::string> labels, bool momentary, TEnumFunc enumFunc, TToggleFunc toggleFunc, TModule* module) {
    	this->momentary = momentary;
        for (size_t i = 0; i < num_labels; i++) {
            this->labels.push_back(labels[i]);
        }
        this->name = name;
        this->enumFunc = enumFunc;
        this->toggleFunc = toggleFunc;
		this->module = module;
		this->tooltip = NULL;
	}

	void setTooltip(ui::Tooltip* tooltip) {
		if (this->tooltip) {
			this->tooltip->requestDelete();
			this->tooltip = NULL;
		}

		if (tooltip) {
			APP->scene->addChild(tooltip);
			this->tooltip = tooltip;
		}
	}

	void setTooltip() {
		std::string text;
		text = name + ": " + getLabel();
		ui::Tooltip* tooltip = new ui::Tooltip;
		tooltip->text = text;
		setTooltip(tooltip);
	}

	std::string getLabel() {
		return labels[enumFunc()];
	}

	void onEnter(const event::Enter& e) override {
		setTooltip();
	}

	void onLeave(const event::Leave& e) override {
		setTooltip(NULL);
	}

	void onButton(const event::Button& e) override {
		//ParamWidget::onButton(e);

        e.stopPropagating();
		if (!module) {
			return;
        }

		if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT || e.button == GLFW_MOUSE_BUTTON_RIGHT)) {
            toggleFunc();
			setTooltip();
			e.consume(this);
		}
	}
};

template <typename TModule, size_t num_labels>
struct GenericToggleDark : NamedEnumToggle<EnumFunc, ToggleFunc, TModule, num_labels> {
	GenericToggleDark() {
		SvgSwitch::addFrame(Svg::load(asset::system("res/ComponentLibrary/CKD6_0.svg")));
		SvgSwitch::addFrame(Svg::load(asset::system("res/ComponentLibrary/CKD6_1.svg")));
		SvgSwitch::shadow->opacity = 0.f;
	}
};