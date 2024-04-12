#include "plugin.hpp"
#include <buttplugclient.h>

template <class TModule>
struct PlugDisplay : TransparentWidget {
    TModule* module;

    const NVGcolor off_white = nvgRGBA(0xff, 0xff, 0xeb, 0xff);
    const NVGcolor grey = nvgRGBA(0x10, 0x10, 0x10, 0xff);

    Rect b;

    std::string fontPath;

    PlugDisplay() : 
            fontPath(asset::plugin(pluginInstance, "res/mplus-1m-light.ttf")) {}

    void setBBox() {
        b = Rect(Vec(0, 0), box.size);
    }

    void drawDeviceInfo(const DrawArgs& args, std::shared_ptr<Font> font) {
        //Vec t_box = b.size.minus(Vec(0,20));
        Vec mid = Vec(5., 10.);
        Vec p = mid;

        nvgBeginPath(args.vg);
        nvgFillColor(args.vg, off_white);
        nvgFontSize(args.vg, 13);
        nvgFontFaceId(args.vg, font->handle);
        const char* text = module->deviceString.c_str();
        nvgTextBox(args.vg, p.x, p.y, b.size.x, text, NULL);
        nvgFill(args.vg);

    }

    void draw(const DrawArgs& args) override {
        // get font each draw, fix for Rack in VST context
        std::shared_ptr<Font> font = APP->window->loadFont(fontPath);

        if (!module)
            return;

        nvgSave(args.vg);
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, b.pos.x, b.pos.y, b.size.x, b.size.y, 10.0);
            nvgFillColor(args.vg, grey);
            nvgFill(args.vg);
            nvgClosePath(args.vg);
        nvgRestore(args.vg);

        nvgSave(args.vg);
            drawDeviceInfo(args, font);
        nvgRestore(args.vg);

        Widget::draw(args);
    }
};


struct Plug : Module {
    enum ParamIds {
        STRENGTH_PARAM,
        SYNC_BUTTON_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };


    static void callbackFunction(const mhl::Messages msg) {
        if (msg.messageType == mhl::MessageTypes::DeviceList) {
            //cout << "Device List callback" << endl;
        }
        if (msg.messageType == mhl::MessageTypes::DeviceAdded) {
            //cout << "Device Added callback" << endl;
        }
        if (msg.messageType == mhl::MessageTypes::ServerInfo) {
            //cout << "Server Info callback" << endl;
        }
        if (msg.messageType == mhl::MessageTypes::DeviceRemoved) {
            //cout << "Device Removed callback" << endl;
        }
        if (msg.messageType == mhl::MessageTypes::SensorReading) {
            //cout << "Sensor Reading callback" << endl;
        }
    }

    std::string url = "ws://127.0.0.1";
    Client client{url, 12345, "test.txt"};

    dsp::ClockDivider pollingDivider;
    dsp::ClockDivider syncDivider;
    dsp::ClockDivider sensorDivider;

    std::string deviceString = "";

    bool scanning = true;

    Plug() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(STRENGTH_PARAM, 0.f, 2.f, 1.f, "Strength");
        pollingDivider.setDivision(3000); // Around 30fps
        syncDivider.setDivision(100000); // Around 0.5fps
        sensorDivider.setDivision(100000); // Around 0.5fps
        client.connect(callbackFunction);
        client.requestDeviceList();
        //client.startScan();
        setSync();
	}

    void setSync() {
        scanning = true;
    }

	void process(const ProcessArgs& args) override {
        /*if (sensorDivider.process()) {
            client.getSensors();
        }*/
        if (scanning) {
            if (syncDivider.getClock() == 0) {
                client.startScan();
                client.requestDeviceList();
            }
            if (syncDivider.process()) {
                client.stopScan();
                scanning = false;
            }
        } else {
            syncDivider.reset();
            if (pollingDivider.process()) {
                int channels = std::max(1, inputs[INPUT].getChannels());
                float v = math::rescale(inputs[INPUT].getVoltage(0), 0.f, 10.f, 0.f, 1.f);
                float strength = params[STRENGTH_PARAM].getValue();

                std::vector<DeviceClass> myDevices = client.getDevices();
                int inputIdx = 0;
                deviceString = "";
                for (auto device : myDevices) {
                    deviceString += std::to_string(device.deviceID) + ": " + device.deviceName + "\n";
                    deviceString += "Sensors:\n";
                    for (auto& s: device.sensorTypes) {
                        deviceString += s + "\n";
                    }
                    if (channels == 1) {
                        client.sendScalar(device, math::clamp(strength * v, 0.f, 1.f));
                    } else {
                        client.sendScalar(device, math::clamp(strength * math::rescale(inputs[INPUT].getVoltage(inputIdx), 0.f, 10.f, 0.f, 1.f), 0.f, 1.f));
                        inputIdx++;
                    }
                }
            }
        }
    }
};


struct PluggedWidget : ModuleWidget {
    PluggedWidget(Plug* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Plug.svg")));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.08, 40)), module, Plug::STRENGTH_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 20)), module, Plug::INPUT));

        {
            auto button = createParamCentered<GenericToggleDark<Plug, 2>>(mm2px(Vec(5.08, 60)), module, Plug::SYNC_BUTTON_PARAM);
            button->config(
                "Sync",
                std::vector<std::string>{"Waiting", "Syncing..."},
                true, 
                [=] () -> int { return module->scanning ? 1 : 0; }, 
                [=] () -> void { module->setSync(); }, 
                module
            );
            addParam(button);
        }

        auto display = new PlugDisplay<Plug>();
        display->module = module;
        display->box.pos = mm2px(Vec(10., 9.0));
        display->box.size = mm2px(Vec(60., 90.));
        display->setBBox();
        addChild(display);
    }
};


Model* modelPlug = createModel<Plug, PluggedWidget>("Futex-Plug");