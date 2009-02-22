#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include "putty.h"

static char lngfile[MAX_PATH];
static char lang[32];
static char ofontname[128];
static HFONT hfont;
static char x[1024];
static WNDPROC orig_wndproc_button, orig_wndproc_static,
  orig_wndproc_systreeview32, orig_wndproc_edit, orig_wndproc_listbox;
static BOOL CALLBACK
defdlgproc (HWND a1, UINT a2, WPARAM a3, LPARAM a4)
{
  return 0;
}
struct prop
{
  WNDPROC oldproc;
  DLGPROC olddlgproc;
} defaultprop = { DefWindowProc, defdlgproc };
static char propstr[] = "l10n";
static int enabled;
static DLGPROC lastolddlgproc;

#define HOOK_TREEVIEW "xSysTreeView32"

static char *
aa (char *p)
{
  int i, n;

  n = strlen (p);
  for (i = 0 ; i < 1023 && *p ; i++)
    {
      if ((*p > 0 && *p < 32) || *p == '=' || *p == '%')
	{
	  x[i++] = '%';
	  x[i++] = "0123456789ABCDEF"[(*p>>4)&15];
	  x[i] = (*p++&15)["0123456789ABCDEF"];
	}
      else
	x[i] = *p++;
    }
  x[i] = '\0';
  return x;
}

static DWORD
getl10nstr (char *a, char *b, char *c, int n)
{
  DWORD r;

  r = GetPrivateProfileString (lang, aa (a), b, c, n, lngfile);
  for (a = c ; (*a = *c) ; a++, c++)
    {
      if (*c == '%')
	{
	  int d, e;

	  if (c[1] >= '0' && c[1] <= '9')
	    d = c[1] - '0';
	  else if (c[1] >= 'A' && c[1] <= 'F')
	    d = c[1] - 'A' + 10;
	  else
	    continue;
	  if (c[2] >= '0' && c[2] <= '9')
	    e = c[2] - '0';
	  else if (c[2] >= 'A' && c[2] <= 'F')
	    e = c[2] - 'A' + 10;
	  else
	    continue;
	  *a = d * 16 + e;
	  c += 2;
	}
    }
  return r;
}

static void
domenu (HMENU menu)
{
  int i, n;
  char a[256];
  MENUITEMINFO b;

  n = GetMenuItemCount (menu);
  for (i = 0 ; i < n ; i++)
    {
      b.cbSize = sizeof (MENUITEMINFO);
      b.fMask = MIIM_TYPE;
      b.dwTypeData = a;
      b.cch = 256;
      if (GetMenuItemInfo (menu, i, 1, &b))
	if (getl10nstr (a, "", a, 256))
	  SetMenuItemInfo (menu, i, 1, &b);
    }
}

static LRESULT CALLBACK
wndproc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;
  struct prop *p;

  p = GetProp (hwnd, propstr);
  if (!p)
    p = &defaultprop;
  proc = p->oldproc;
  switch (msg)
    {
    case WM_DESTROY:
      RemoveProp (hwnd, propstr);
      LocalFree ((HANDLE)p);
      SetWindowLong (hwnd, GWL_WNDPROC, (LONG)proc);
      break;
    case WM_INITMENUPOPUP:
      domenu ((HMENU)wparam);
      break;
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT
hook_setfont (WNDPROC proc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LOGFONT a;

  if (wparam && GetObject ((HGDIOBJ)wparam, sizeof (a), &a))
    {
      if (!stricmp (a.lfFaceName, ofontname))
	return CallWindowProc (proc, hwnd, msg, (WPARAM)hfont, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT
hook_create (WNDPROC proc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  char a[256];
  LRESULT r;

  r = CallWindowProc (proc, hwnd, msg, (WPARAM)hfont, lparam);
  if (GetWindowText (hwnd, a, 256))
    if (getl10nstr (a, "", a, 256))
      SetWindowText (hwnd, a);
  return r;
}

static LRESULT
hook_tvm_insertitem (WNDPROC proc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  TVINSERTSTRUCT *p;
  char *q;
  char a[256];
  LRESULT r;

  p = (TVINSERTSTRUCT *)lparam;
  q = p->item.pszText;
  if (getl10nstr (q, "", a, 256))
    p->item.pszText = a;
  r = CallWindowProc (proc, hwnd, msg, wparam, lparam);
  p->item.pszText = q;
  return r;
}

static LRESULT
hook_tvm_getitem (WNDPROC proc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  TVITEM *p;
  char *q;
  char a[256];
  int l;
  LRESULT r;

  p = (TVITEM *)lparam;
  q = p->pszText;
  l = p->cchTextMax;
  p->pszText = a;
  p->cchTextMax = 256;
  r = CallWindowProc (proc, hwnd, msg, wparam, lparam);
  p->pszText = q;
  p->cchTextMax = l;
  getl10nstr (a, a, a, 256);
  strncpy (q, a, l);
  return r;
}

static LRESULT CALLBACK
wndproc_button (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;

  proc = orig_wndproc_button;
  switch (msg)
    {
    case WM_SETFONT: return hook_setfont (proc, hwnd, msg, wparam, lparam);
    case WM_CREATE: return hook_create (proc, hwnd, msg, wparam, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT CALLBACK
wndproc_static (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;

  proc = orig_wndproc_static;
  switch (msg)
    {
    case WM_SETFONT: return hook_setfont (proc, hwnd, msg, wparam, lparam);
    case WM_CREATE: return hook_create (proc, hwnd, msg, wparam, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT CALLBACK
wndproc_systreeview32 (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;

  proc = orig_wndproc_systreeview32;
  switch (msg)
    {
    case WM_SETFONT: return hook_setfont (proc, hwnd, msg, wparam, lparam);
    case TVM_INSERTITEM: return hook_tvm_insertitem (proc, hwnd, msg, wparam, lparam);
    case TVM_GETITEM: return hook_tvm_getitem (proc, hwnd, msg, wparam, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT CALLBACK
wndproc_edit (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;

  proc = orig_wndproc_edit;
  switch (msg)
    {
    case WM_SETFONT: return hook_setfont (proc, hwnd, msg, wparam, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

static LRESULT CALLBACK
wndproc_listbox (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WNDPROC proc;

  proc = orig_wndproc_listbox;
  switch (msg)
    {
    case WM_SETFONT: return hook_setfont (proc, hwnd, msg, wparam, lparam);
    }
  return CallWindowProc (proc, hwnd, msg, wparam, lparam);
}

void
l10n (HINSTANCE hinst)
{
  int i;
  char fontname[128];
  char a[128];
  int fontsize;
  struct
  {
    char *classname;
    char *newclassname;
    WNDPROC *orig_wndproc;
    WNDPROC new_wndproc;
  } b[] =
    {
      { "Button", "xButton", &orig_wndproc_button, wndproc_button },
      { "Static", "xStatic", &orig_wndproc_static, wndproc_static },
      { WC_TREEVIEW, HOOK_TREEVIEW, &orig_wndproc_systreeview32, wndproc_systreeview32 },
      { "Edit", "xEdit", &orig_wndproc_edit, wndproc_edit },
      { "ListBox", "xListBox", &orig_wndproc_listbox, wndproc_listbox },
      { NULL }
    };

  enabled = 0;
  GetModuleFileName (0, lngfile, MAX_PATH);
  for (i = strlen (lngfile) ; i >= 0 ; i--)
    if (lngfile[i] == '.')
      break;
  if (i >= 0)
    {
      strcpy (&lngfile[i + 1], "lng");
      if (GetPrivateProfileString ("Default", "Language", "", lang, 32,
				   lngfile))
	{
	  enabled = 1;
	  GetPrivateProfileString (lang, "_FONTNAME_", "System", fontname, 128,
				   lngfile);
	  GetPrivateProfileString (lang, "_OFONTNAME_", "MS Sans Serif",
				   ofontname, 128, lngfile);
	  GetPrivateProfileString (lang, "_FONTSIZE_", "10", a, 128, lngfile);
	  fontsize = atoi (a);
	  hfont = CreateFont (fontsize, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
			      0, 0, 0, 0, fontname);
	  for (i = 0; b[i].classname != NULL; i++)
	    {
	      WNDCLASS wndclass;

	      GetClassInfo (hinst, b[i].classname, &wndclass);
	      wndclass.hInstance = hinst;
	      wndclass.lpszClassName = b[i].newclassname;
	      *b[i].orig_wndproc = wndclass.lpfnWndProc;
	      wndclass.lpfnWndProc = b[i].new_wndproc;
	      RegisterClass (&wndclass);
	    }
	}
    }
}

static HWND
aftercreate (HWND r)
{
  char classname[256];
  char windowname[256];
  DWORD style;
  HWND parent;
  struct prop b, *qq;
  char buf[256];
  enum { TYPE_OFF, TYPE_MAIN, TYPE_OTHER } type;

  if (!r)
    return r;
  GetClassName (r, classname, 256);
  GetWindowText (r, windowname, 256);
  style = GetWindowLong (r, GWL_STYLE);
  parent = GetParent (r);
  type = TYPE_OFF;
  if (!(style & WS_CHILD))
    {
      if (!strcmp (classname, "PuTTY"))
	type = TYPE_MAIN;
      else if (getl10nstr (windowname, "", buf, 256))
	type = TYPE_OTHER;
    }
  if (type == TYPE_OFF)
    return r;
  if (type == TYPE_OTHER)
    {
      if (GetWindowText (r, buf, 256))
	if (getl10nstr (buf, "", buf, 256))
	  SetWindowText (r, buf);
    }
  if (type == TYPE_MAIN)
    do
      {
	qq = (struct prop *)LocalAlloc (0, sizeof (b));
	if (!qq)
	  break;
	if (!SetProp (r, propstr, (HANDLE)qq))
	  {
	    LocalFree ((HANDLE)qq);
	    break;
	  }
	*qq = b;
	qq->oldproc = (WNDPROC)SetWindowLong (r, GWL_WNDPROC,
					      (LONG)wndproc);
      }
    while (0);
  return r;
}

static void
aftercreate2 (HWND hwnd)
{
  HWND a;
  LOGFONT l;
  char buf[256];

  a = GetWindow (hwnd, GW_CHILD);
  while (a)
    {
      aftercreate2 (a);
      if (GetWindowText (a, buf, 256))
	if (getl10nstr (buf, "", buf, 256))
	  SetWindowText (a, buf);
      if (GetObject ((HGDIOBJ)SendMessage (a, WM_GETFONT, 0, 0),
		     sizeof (l), &l))
	{
	  if (!stricmp (l.lfFaceName, ofontname))
	    SendMessage (a, WM_SETFONT, (WPARAM)hfont,
			 MAKELPARAM (TRUE, 0));
	}
      a = GetWindow (a, GW_HWNDNEXT);
    }
}

static BOOL CALLBACK
dlgproc (HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  DLGPROC proc;
  struct prop *p;

  if (message == WM_INITDIALOG)
    {
      aftercreate (hwnd);
      aftercreate2 (hwnd);
      p = (struct prop *)LocalAlloc (0, sizeof (*p));
      if (p && !SetProp (hwnd, propstr, (HANDLE)p))
	LocalFree ((HANDLE)p);
      else
	*p = defaultprop;
    }
  p = GetProp (hwnd, propstr);
  if (!p)
    p = &defaultprop;
  if (message == WM_INITDIALOG)
    p->olddlgproc = lastolddlgproc;
  proc = p->olddlgproc;
  return proc (hwnd, message, wparam, lparam);
}

#undef MessageBoxA
int
xMessageBoxA (HWND a1, LPCSTR a2, LPCSTR a3, UINT a4)
{
  char *b2 = (char *)a2, *b3 = (char *)a3;
  char c2[256], c3[256];

  if (!enabled)
    return MessageBoxA (a1, a2, a3, a4);
  if (getl10nstr (b2, "", c2, 256))
    b2 = c2;
  if (getl10nstr (b3, "", c3, 256))
    b3 = c3;
  return MessageBoxA (a1, b2, b3, a4);
}

#undef CreateWindowExA
HWND
xCreateWindowExA (DWORD a1, LPCSTR a2, LPCSTR a3, DWORD a4, int a5, int a6,
		 int a7, int a8, HWND a9, HMENU a10, HINSTANCE a11, LPVOID a12)
{
  HWND r;

#ifdef L10N_WNDPROC_HOOK
  if (enabled && IsBadStringPtr (a2, 100) == FALSE)
    {
      if (stricmp (a2, WC_TREEVIEW) == 0) a2 = HOOK_TREEVIEW;
      else if (stricmp (a2, "Button") == 0) a2 = "xButton";
      else if (stricmp (a2, "Static") == 0) a2 = "xStatic";
      else if (stricmp (a2, "Edit") == 0) a2 = "xEdit";
      else if (stricmp (a2, "ListBox") == 0) a2 = "xListBox";
    }
#endif
  r = CreateWindowExA (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
  if (!enabled)
    return r;
  return aftercreate (r);
}

#undef DialogBoxParamA
int
xDialogBoxParamA (HINSTANCE a1, LPCSTR a2, HWND a3, DLGPROC a4, LPARAM a5)
{
#ifdef L10N_WNDPROC_HOOK
  if (!enabled)
#endif
    return DialogBoxParamA (a1, a2, a3, a4, a5);
  lastolddlgproc = a4;
  return DialogBoxParamA (a1, a2, a3, dlgproc, a5);
}

#undef CreateDialogParamA
HWND
xCreateDialogParamA (HINSTANCE a1, LPCSTR a2, HWND a3, DLGPROC a4, LPARAM a5)
{
  HWND r;

  r = CreateDialogParamA (a1, a2, a3, a4, a5);
#ifdef L10N_WNDPROC_HOOK
  if (enabled)
    {
      aftercreate (r);
      aftercreate2 (r);
    }
#endif
  return r;
}

char *
l10n_dupstr (char *s)
{
  char *a = s;
  char b[256];

  if (enabled && getl10nstr (s, "", b, 256))
    a = b;
  return dupstr (a);
}

void
l10n_created_window (HWND hwnd)
{
  if (enabled)
    aftercreate2 (hwnd);
}

HFONT
l10n_getfont (HFONT f)
{
  LOGFONT l;

  if (enabled && GetObject ((HGDIOBJ)f, sizeof (l), &l))
    {
      if (!stricmp (l.lfFaceName, ofontname))
	return hfont;
    }
  return f;
}
