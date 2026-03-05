#pragma once

#include "View.hpp"
#include "Model.hpp"

class Controller : public ActionListener, public ChangeListener
{
public:
    Controller(View &v, Model &m);
    void actionListenerCallback(const String &message) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
private:
    String audioManagerToString(AudioDeviceManager* audiomanager);
    View& view;
    Model& model;
};