/*
   Form1

   To create an API (Application Programming Interface) based Windows program using Microsoft's
   Software Development Kit (SDK), one must first fill out several of the more critical members
   of a WNDCLASS or WNDCLASSEX struct ...

   struct WNDCLASSEX
   {
     UINT    cbSize;
     UINT    style;
     WNDPROC lpfnWndProc;
     int     cbClsExtra;
     int     cbWndExtra;
     HANDLE  hInstance;
     HICON   hIcon;
     HCURSOR hCursor;
     HBRUSH  hbrBackground;
     LPCTSTR lpszMenuName;
     LPCTSTR lpszClassName;
     HICON   hIconSm;
   };

   ...and then pass this information to Windows by calling RegisterClass() or
   RegisterClassEx()...

   https://msdn.microsoft.com/en-us/library/windows/desktop/ms633587%28v=vs.85%29.aspx

   The three most important members of WNDCLASSEX that must be filled out properly and without
   which an application window can't be created are ...

   WNDCLASSEX::lpszClassName   // The null terminated textural name of the class, here "Form1"
   WNDCLASSEX::fnWndProc       // The address of the Window Procedure
   WNDCLASSEX::cbSize          // The size of the WNDCLASSEX object

   While the program can compile and run without filling out the WNDCLASSEX::hInstance member,
   I'd highly recommend that be filled out, because it could put you in a situation of
   undefined behavior.  Anyway, its provided for you as the 1st parameter of WinMain(), so
   there's no reason not to use it.

   After filling out these members of a WNDCLASSEX struct and calling RegisterClassEx() on that
   object, you can make a call to CreateWindow() or CreateWindowEx() to instantiate an
   instance of the Registered Class.  Note that the 1st parameter of CreateWindow() and the 2nd
   parameter of CreateWindowEx() is a pointer to the textural null terminated C String to which
   WNDCLASSEX::lpszClassName points - here "Form1".  The call to CreateWindow() or
   CreateWindowEx() represents a C based constructor call for an object.

   After calling CreateWindow() the remainder of the code in WinMain() must fall into a message
   loop - sometimes called a 'message pump,' which retrieves messages Windows the Operating
   System places in the program's 'message queue.'  This is a memory structure Windows creates
   for all programs where it places messages informing the program of user interactions with
   the program such as keyboard or mouse input, as well as system wide notifcations. The
   GetMessage() function retrieves the next message from the queue, and the DispatchMessage()
   function within the message pump causes Windows to dispatch (call) the Registered Window
   Procedure associated with the Window Handle of the message.

   The final piece of the puzzle is the Window Procedure itself, which is likely the most
   important concept in Windows Programming. Note that the WNDCLASSEX::lpfnWndProc member is
   typed as WNDPROC.  What that is is a function pointer like so...

   typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

   In x86 LRESULT is a typedef for a LONG and hence is 32 bits, while in x64 LRESULT is a 64
   bit entity.  The CALLBACK symbol is a typedef of __stdcall.  So if the following looks
   better to you, you could think of it as this, at least in x86 ...

   long (__stdcall* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

   In other words, the WNDCLASSEX::lpfnWndProc entity requires a pointer to a function that
   returns a long, uses standard call stack protocol, and has HWND, UINT, WPARAM, and LPARAM
   parameters. Don't let these confuse you.  Their usage will become clear, and I'll cover them
   in Form2 and Form3.  So just accept them for now, awaiting further clarification.  Note that
   our fnWndProc() function just below fullfills our specifications for a Window Procedure, and
   the address of that Window Procedure is assigned to WNDCLASSEX::fnWndProc in WinMain() as
   follows ...

   wc.lpfnWndProc = fnWndProc;

   Note that our fnWndProc() just below only does two things.  First, it tests the msg
   parameter of type unsigned int to see if it equals WM_DESTROY.  You can find WM_DESTROY
   defined in Windows.h as follows ...

   #define WM_DESTROY  0x0002

   So its equal to 2 (its actually in WinUser.h - #included by Windows.h).  Windows sends a
   WM_DESTROY message when you click the X in the Title Bar to close the program.  In that case
   the app calls PostQuitMessage(), which causes the message pump in WinMain() to fall through
   and end.  Then zero is returned in fnWndProc notifying Windows that the message has been
   handled, and to take no further action on it.  The second thing the program's Window
   Procedure does is pass every other message received in the Window Procedure not equal to
   WM_DESTROY to DefWindowProc() for default processing.  In other words, you are telling
   Windows that you aren't interested in that message and are taking no action on it, but that
   the Operating System can do whatever it must with it to make itself happy.  In general, when
   you handle a message by writing code, you make an early exit by returning zero.  Some
   messages do require other entities to be returned though.  Run the code below to make sure
   everything is working for you.
*/

// cl Form1.cpp /O1 /Os /GS- /link TCLib.lib user32.lib   //  4,096 Bytes UNICODE x64 Static Linked TCLib
// cl Form1.cpp /O1 /Os user32.lib                        // 93,184 Bytes UNICODE x64 Static Linked MS C Runtime
#ifndef UNICODE           // Microsoft's Visual Studio Defaults To Wide Character Unicode Strings So This Code
    #define UNICODE       // Would Build Correctly In That Visual Design Environment Without These Conditional
#endif                    // Compilation Directives Just Left.  However, If Built From The Command Line Or Some
#ifndef _UNICODE          // Other Code Editor, This Code Would Fail Without These Directives Because Of The
    #define _UNICODE      // Several Uses Of Wide Character String Literals In The Code Below Such As L"Form1".
#endif
#include <windows.h>

LRESULT CALLBACK fnWndProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
 if(msg==WM_DESTROY)       // This is the Window Procedure.  The concept of the
 {                         // Window Procedure is the most important concept to
    PostQuitMessage(0);    // grasp in C/C++ WinApi coding.  You never call this
    return 0;              // function with your code; rather, Windows calls it
 }                         // to inform code here of events occurring.  The events
                           // are reported here as messages.
 return (DefWindowProc(hwnd, msg, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIns, LPSTR lpszArgument, int iShow)
{
 MSG messages;    // The concept of the Window Class and its associated Window Procedure are
 WNDCLASS wc;     // the most important concepts in Windows Programming.

 wc.lpszClassName = L"Form1",    wc.lpfnWndProc = fnWndProc;  // The Class Name Will Be Form1 And The Symbol fnWndProc
 wc.hInstance     = hInstance,   wc.style         = 0;        // Will Be Resolved At Runtime To The Virtual Address Of
 wc.cbClsExtra    = 0,           wc.cbWndExtra    = 0;        // Form1's Window Procedure, Which Windows Will Call
 wc.hIcon         = NULL,        wc.hCursor       = NULL;     // Through This Address Rather Than Through It's Name.
 wc.lpszMenuName  = NULL,        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
 RegisterClass(&wc);                                          // Register The Window Class With Windows
 CreateWindow(L"Form1", L"Form1", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 200, 100, 325, 300, HWND_DESKTOP, 0, hInstance, 0);
 while (GetMessage(&messages, NULL, 0, 0))   // The message pump retrieves messages from the program's
 {                                           // message queue with GetMessage(), does some translation
    TranslateMessage(&messages);             // work in terms of character messages, then calls the
    DispatchMessage(&messages);              // Window Procedure associated with the HWND of the message
 }                                           // being processed.  Note that an app can have many Window
 
 return (int)messages.wParam;
}
