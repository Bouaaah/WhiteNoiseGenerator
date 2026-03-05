#include <JUCEHeader.h>
#include "Controller.hpp"
#include "View.hpp"
#include "Model.hpp"


class WhiteNoisGeneratorApplication : public JUCEApplication
{
    public:
        const String getApplicationName() override
        {
            return String(APP_NAME); // Define in CMake
        }

        const String getApplicationVersion() override
        {
            return String(APP_VERSION); // Define in CMake
        }

        void initialise(const String &commandLineParameters)
        {
            model = std::make_unique<Model>();
            view = std::make_unique<View>();
            controller= std::make_unique<Controller>(*view,*model);
        }
        
        void shutdown()
        {
            model.reset();
            view.reset();
            controller.reset();
        }

    private:
        std::unique_ptr<Model> model;
        std::unique_ptr<View> view;
        std::unique_ptr<Controller> controller;

};

START_JUCE_APPLICATION(WhiteNoisGeneratorApplication)
