#include "definitions.h"
#include "Plugin.h"

//Called when the user hits the configure button in MPQDraft or FireGraft
BOOL WINAPI Plugin::Configure(HWND hParentWnd) {
	/*
	 *  You can't really change the version from
	 *  here with the current format.  However, you
	 *  can check the version and probably set any
	 *  globals.
	 */

	MessageBox(
		hParentWnd,
		PLUGIN_NAME " (ID: " STR(PLUGIN_ID) ")"
		"\nVisit http://gptp.googlecode.com/ for more information."
		"\nBuilt on " __DATE__ " " __TIME__
		"\n"
		"\n[Features]"
		"\n - Add your own text here"
		,
		PLUGIN_NAME,
		MB_TASKMODAL
		);

	return TRUE;
}

