#include <iostream>
#include <cstdlib>
using namespace std;

#include <libnotify/notify.h>

void cb(void)
{
    cout << "Callback called!" << endl;
	NotifyNotification * Hello = notify_notification_new ("This is a test", "A secondary notification triggered by the first.", "");
	notify_notification_show (Hello, NULL);
	g_object_unref(G_OBJECT(Hello));
}

int main()
{
	notify_init ("Hello world!");
	NotifyNotification * Hello = notify_notification_new ("Hello world", "This is an example notification.", "dialog-information");
    notify_notification_add_action (Hello, "cb", "Callback", NOTIFY_ACTION_CALLBACK(cb), NULL, NULL);
	notify_notification_show (Hello, NULL);
	g_object_unref(G_OBJECT(Hello));

	notify_uninit();
	return 0;
}

//#include <libnotifymm.h>
//
//int main()
//{
//	Notify::init("Hello world!");
//	Notify::Notification Hello("Hello world", "This is an example notification.", "dialog-information");
//	Hello.show();
//	return 0;
//}
