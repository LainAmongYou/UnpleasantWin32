/* =================================== */
/* Basic Win32 Dialog Tutorial by Lain */
/* =================================== */

/* ofc */
#include <Windows.h>

/* I like true and false more than TRUE and FALSE */
#include <stdbool.h>

/* resource.h is automatically created when you have a resource file, you can
 * just include it right away like this.
 *
 * it'll automatically make the IDs of all resources within the resource file
 * be visible. Then you can use those iDs with MAKEINTRESOURCE() with functions
 * like CreateDialog/DialogBox/LoadIcon/LoadBitmap/etc along with the HINSTANCE
 of the module and and you can then load those resources automatically. */
#include "resource.h"

#define YOOK_HYPE L"YukieHype"
#define YOOK_WOAH L"YukieWoah"

static void CenterWindow(HWND hwndMain)
{
	RECT rect;
	GetWindowRect(hwndMain, &rect);

	int rectCX = rect.right - rect.left;
	int rectCY = rect.bottom - rect.top;
	int parentCX = GetSystemMetrics(SM_CXFULLSCREEN);
	int parentCY = GetSystemMetrics(SM_CYFULLSCREEN);

	int x = (parentCX - rectCX) / 2;
	int y = (parentCY - rectCY) / 2;

	SetWindowPos(hwndMain, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static void ChangedSelection(HWND hwndMain, const wchar_t *name)
{
	HBITMAP hbmp = NULL;

	if (wcscmp(name, YOOK_HYPE) == 0) {
		hbmp = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_YUKIE_HYPE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	} else if (wcscmp(name, YOOK_WOAH) == 0) {
		hbmp = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_YUKIE_WOAH), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	} else {
		/* shouldn't get here */
		return;
	}

	HWND hwndImage = GetDlgItem(hwndMain, IDC_IMAGE);
	HBITMAP prev = (HBITMAP)SendMessage(hwndImage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmp);

	if (prev)
		DeleteObject(prev);
}

/* For windows and dialogs, you need to have a "window procedure" which is
 * called every time the window receives a message. Dialogs use DialogProc, and
 * normal windows use WindowProc.
 *
 * Dialog procedures (DialogProc) return TRUE if you process a message,
 * otherwise make sure to return FALSE.
 *
 * Normal window procedures (WindowProc) return an LRESULT value which varies
 * depending on the message; when you're not processing a message, pass the
 * parameters to DefWindowProc and return with its return value. */
static INT_PTR WINAPI YookMainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG: {
		CenterWindow(hwnd);

		/* WM_INITDIALOG is sent to the dialog's procedure after the
		 * window is created to tell the window to do any
		 * initialization */

		/* GetDlgItem() gets the handle of a window by its ID */
		HWND emojiList = GetDlgItem(hwnd, IDC_EMOJIS);

		/* SendMessage calls the target window's procedure with the
		 * specified message. (blocks).
		 *
		 * CB_* messages are used for combo boxes. Similarly there are
		 * LB_* for list boxes, WM_* for generic * messages, etc */
		SendMessage(emojiList, CB_ADDSTRING, 0, (LPARAM)YOOK_HYPE);
		SendMessage(emojiList, CB_ADDSTRING, 0, (LPARAM)YOOK_WOAH);

		/* PostMessage posts a message to the message queue to call the
		 * window's procedure later with the specified
		 * message/parameters. (does not block) */
		PostMessage(emojiList, CB_SETCURSEL, 0, 0);

		/* Ok, Apparently the above doesn't call WM_COMMAND. I think I
		 * hate this API even more */
		PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_EMOJIS, CBN_SELCHANGE), (LPARAM)emojiList);
		return true;
	}

	case WM_COMMAND: {
		/* WM_COMMAND is sent to the window's procedure to indicate
		 * that a control has been activated/pressed. The ID of the
		 * control is indicated by the wParam. */

		HWND ctrl = (HWND)lParam;
		WORD id   = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if (id == IDC_EXIT) {
			EndDialog(hwnd, 0);
		} else if (id == IDC_EMOJIS) {
			if (code == CBN_SELCHANGE) {
				/* Oh god getting the string.. this is a bit painful */
				int idx = (int)SendMessage(ctrl, CB_GETCURSEL, 0, 0);
				int len = (int)SendMessage(ctrl, CB_GETLBTEXTLEN, idx, 0);

				wchar_t *str = malloc((len + 1) * sizeof(wchar_t));
				SendMessage(ctrl, CB_GETLBTEXT, idx, (LPARAM)str);
				ChangedSelection(hwnd, str);
				free(str);
			}
		}
		return true;
	}

	case WM_CLOSE: {
		EndDialog(hwnd, 0);
	}
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hinstMain, HINSTANCE hprev, LPSTR cmdline, int showcmd)
{
	DialogBox(hinstMain, MAKEINTRESOURCE(IDD_SILLY_YUKE_WINDOW), NULL, YookMainWindowProc);
	return 0;
}
