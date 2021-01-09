/*
 *    Copyright 2020, James Blades <jwkblades+git@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GUI.hpp"

#include "Utilities.hpp"

GUI::GUI(int argc, char** argv, const Configuration& cfg):
    mNotification(NULL),
    mConfig(cfg)
{
    if (!gtk_init_check(&argc, &argv))
    {
        return;
    }

    notify_init(mConfig.title);
    mNotification = notify_notification_new(mConfig.title, mConfig.description, mConfig.icon);

    for (int i = 0; i < mConfig.optIndex && i < mConfig.MAX_OPTIONS; ++i)
    {
        notify_notification_add_action(mNotification, mConfig.values[i], mConfig.options[i], close, NULL, NULL);
    }

    notify_notification_add_action(mNotification, "default", "OK", close, NULL, NULL);
    g_signal_connect(G_OBJECT(mNotification), "closed", defaultExit, mNotification);
    notify_notification_set_urgency(mNotification, NOTIFY_URGENCY_CRITICAL);
}

GUI::~GUI(void)
{
    notify_notification_close(mNotification, NULL);
    notify_uninit();
}

GUI::operator bool(void) const
{
    return mNotification != NULL;
}

void GUI::show(void)
{
    if (!*this)
    {
        return;
    }
    notify_notification_show(mNotification, NULL);
}

void GUI::main(void)
{
    if (!*this)
    {
        return;
    }
    gtk_main();
}

void GUI::quit(void)
{
    gtk_main_quit();
}


void GUI::closeSignal(void) const
{
    static const char* action = "default";
    close(mNotification, (char*)action, NULL);
}

void GUI::closeFirstOption(void) const
{
    if (mConfig.optIndex < 1)
    {
        closeSignal();
        return;
    }
    close(mNotification, (char*)mConfig.values[0], NULL);
}

void GUI::closeSecondOption(void) const
{
    if (mConfig.optIndex < 2)
    {
        closeFirstOption();
        return;
    }
    close(mNotification, (char*)mConfig.values[1], NULL);
}

void GUI::closeThirdOption(void) const
{
    if (mConfig.optIndex < 3)
    {
        closeSecondOption();
        return;
    }
    close(mNotification, (char*)mConfig.values[2], NULL);
}
