/*
 * Copyright 2012 TennenColl
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

#include "engine-pch.h"
#include "engine-application.h"
#include "engine-input.h"
#include "util-file.h"

#include <btBulletDynamicsCommon.h>

using namespace fairytale;
using namespace boost::python;

typedef void (*void_func_ptr_Application_const_ref_str)(Application*, const std::string&);
typedef void (*void_func_void)(void);

namespace fairytale
{
	class TestModule {};
}

void doBulletTest();

BOOST_PYTHON_MODULE(fairytale)
{
	class_<Application>("Application")
		.def("initOgre",						&Application::initOgre)
		.def("showConfigDialog",				&Application::showConfigDialog)
		.def("initWindow",						&Application::initWindow)
		.def("shutdown",						&Application::shutdown)
		.def("addResourceLocation",				&Application::addResourceLocation)
		.def("loadPlugin",						&Application::loadPlugin)
		.def("loadPluginsFromDirectory",		&Application::loadPluginsFromDirectory)
		;

	class_<TestModule>("Test")
		.def("bulletTest",						&doBulletTest)
		;
}

BOOST_PYTHON_MODULE(OIS)
{
	enum_<OIS::KeyCode>("KeyCode")
		.value("KC_UNASSIGNED",   OIS::KeyCode::KC_UNASSIGNED)
		.value("KC_ESCAPE",       OIS::KeyCode::KC_ESCAPE)
		.value("KC_1",            OIS::KeyCode::KC_1)
		.value("KC_2",            OIS::KeyCode::KC_2)
		.value("KC_3",            OIS::KeyCode::KC_3)
		.value("KC_4",            OIS::KeyCode::KC_4)
		.value("KC_5",            OIS::KeyCode::KC_5)
		.value("KC_6",            OIS::KeyCode::KC_6)
		.value("KC_7",            OIS::KeyCode::KC_7)
		.value("KC_8",            OIS::KeyCode::KC_8)
		.value("KC_9",            OIS::KeyCode::KC_9)
		.value("KC_0",            OIS::KeyCode::KC_0)
		.value("KC_MINUS",        OIS::KeyCode::KC_MINUS)
		.value("KC_EQUALS",       OIS::KeyCode::KC_EQUALS)
		.value("KC_BACK",         OIS::KeyCode::KC_BACK)
		.value("KC_TAB",          OIS::KeyCode::KC_TAB)
		.value("KC_Q",            OIS::KeyCode::KC_Q)
		.value("KC_W",            OIS::KeyCode::KC_W)
		.value("KC_E",            OIS::KeyCode::KC_E)
		.value("KC_R",            OIS::KeyCode::KC_R)
		.value("KC_T",            OIS::KeyCode::KC_T)
		.value("KC_Y",            OIS::KeyCode::KC_Y)
		.value("KC_U",            OIS::KeyCode::KC_U)
		.value("KC_I",            OIS::KeyCode::KC_I)
		.value("KC_O",            OIS::KeyCode::KC_O)
		.value("KC_P",            OIS::KeyCode::KC_P)
		.value("KC_LBRACKET",     OIS::KeyCode::KC_LBRACKET)
		.value("KC_RBRACKET",     OIS::KeyCode::KC_RBRACKET)
		.value("KC_RETURN",       OIS::KeyCode::KC_RETURN)
		.value("KC_LCONTROL",     OIS::KeyCode::KC_LCONTROL)
		.value("KC_A",            OIS::KeyCode::KC_A)
		.value("KC_S",            OIS::KeyCode::KC_S)
		.value("KC_D",            OIS::KeyCode::KC_D)
		.value("KC_F",            OIS::KeyCode::KC_F)
		.value("KC_G",            OIS::KeyCode::KC_G)
		.value("KC_H",            OIS::KeyCode::KC_H)
		.value("KC_J",            OIS::KeyCode::KC_J)
		.value("KC_K",            OIS::KeyCode::KC_K)
		.value("KC_L",            OIS::KeyCode::KC_L)
		.value("KC_SEMICOLON",    OIS::KeyCode::KC_SEMICOLON)
		.value("KC_APOSTROPHE",   OIS::KeyCode::KC_APOSTROPHE)
		.value("KC_GRAVE",        OIS::KeyCode::KC_GRAVE)
		.value("KC_LSHIFT",       OIS::KeyCode::KC_LSHIFT)
		.value("KC_BACKSLASH",    OIS::KeyCode::KC_BACKSLASH)
		.value("KC_Z",            OIS::KeyCode::KC_Z)
		.value("KC_X",            OIS::KeyCode::KC_X)
		.value("KC_C",            OIS::KeyCode::KC_C)
		.value("KC_V",            OIS::KeyCode::KC_V)
		.value("KC_B",            OIS::KeyCode::KC_B)
		.value("KC_N",            OIS::KeyCode::KC_N)
		.value("KC_M",            OIS::KeyCode::KC_M)
		.value("KC_COMMA",        OIS::KeyCode::KC_COMMA)
		.value("KC_PERIOD",       OIS::KeyCode::KC_PERIOD)
		.value("KC_SLASH",        OIS::KeyCode::KC_SLASH)
		.value("KC_RSHIFT",       OIS::KeyCode::KC_RSHIFT)
		.value("KC_MULTIPLY",     OIS::KeyCode::KC_MULTIPLY)
		.value("KC_LMENU",        OIS::KeyCode::KC_LMENU)
		.value("KC_SPACE",        OIS::KeyCode::KC_SPACE)
		.value("KC_CAPITAL",      OIS::KeyCode::KC_CAPITAL)
		.value("KC_F1",           OIS::KeyCode::KC_F1)
		.value("KC_F2",           OIS::KeyCode::KC_F2)
		.value("KC_F3",           OIS::KeyCode::KC_F3)
		.value("KC_F4",           OIS::KeyCode::KC_F4)
		.value("KC_F5",           OIS::KeyCode::KC_F5)
		.value("KC_F6",           OIS::KeyCode::KC_F6)
		.value("KC_F7",           OIS::KeyCode::KC_F7)
		.value("KC_F8",           OIS::KeyCode::KC_F8)
		.value("KC_F9",           OIS::KeyCode::KC_F9)
		.value("KC_F10",          OIS::KeyCode::KC_F10)
		.value("KC_NUMLOCK",      OIS::KeyCode::KC_NUMLOCK)
		.value("KC_SCROLL",       OIS::KeyCode::KC_SCROLL)
		.value("KC_NUMPAD7",      OIS::KeyCode::KC_NUMPAD7)
		.value("KC_NUMPAD8",      OIS::KeyCode::KC_NUMPAD8)
		.value("KC_NUMPAD9",      OIS::KeyCode::KC_NUMPAD9)
		.value("KC_SUBTRACT",     OIS::KeyCode::KC_SUBTRACT)
		.value("KC_NUMPAD4",      OIS::KeyCode::KC_NUMPAD4)
		.value("KC_NUMPAD5",      OIS::KeyCode::KC_NUMPAD5)
		.value("KC_NUMPAD6",      OIS::KeyCode::KC_NUMPAD6)
		.value("KC_ADD",          OIS::KeyCode::KC_ADD)
		.value("KC_NUMPAD1",      OIS::KeyCode::KC_NUMPAD1)
		.value("KC_NUMPAD2",      OIS::KeyCode::KC_NUMPAD2)
		.value("KC_NUMPAD3",      OIS::KeyCode::KC_NUMPAD3)
		.value("KC_NUMPAD0",      OIS::KeyCode::KC_NUMPAD0)
		.value("KC_DECIMAL",      OIS::KeyCode::KC_DECIMAL)
		.value("KC_OEM_102",      OIS::KeyCode::KC_OEM_102)
		.value("KC_F11",          OIS::KeyCode::KC_F11)
		.value("KC_F12",          OIS::KeyCode::KC_F12)
		.value("KC_F13",          OIS::KeyCode::KC_F13)
		.value("KC_F14",          OIS::KeyCode::KC_F14)
		.value("KC_F15",          OIS::KeyCode::KC_F15)
		.value("KC_KANA",         OIS::KeyCode::KC_KANA)
		.value("KC_ABNT_C1",      OIS::KeyCode::KC_ABNT_C1)
		.value("KC_CONVERT",      OIS::KeyCode::KC_CONVERT)
		.value("KC_NOCONVERT",    OIS::KeyCode::KC_NOCONVERT)
		.value("KC_YEN",          OIS::KeyCode::KC_YEN)
		.value("KC_ABNT_C2",      OIS::KeyCode::KC_ABNT_C2)
		.value("KC_NUMPADEQUALS", OIS::KeyCode::KC_NUMPADEQUALS)
		.value("KC_PREVTRACK",    OIS::KeyCode::KC_PREVTRACK)
		.value("KC_AT",           OIS::KeyCode::KC_AT)
		.value("KC_COLON",        OIS::KeyCode::KC_COLON)
		.value("KC_UNDERLINE",    OIS::KeyCode::KC_UNDERLINE)
		.value("KC_KANJI",        OIS::KeyCode::KC_KANJI)
		.value("KC_STOP",         OIS::KeyCode::KC_STOP)
		.value("KC_AX",           OIS::KeyCode::KC_AX)
		.value("KC_UNLABELED",    OIS::KeyCode::KC_UNLABELED)
		.value("KC_NEXTTRACK",    OIS::KeyCode::KC_NEXTTRACK)
		.value("KC_NUMPADENTER",  OIS::KeyCode::KC_NUMPADENTER)
		.value("KC_RCONTROL",     OIS::KeyCode::KC_RCONTROL)
		.value("KC_MUTE",         OIS::KeyCode::KC_MUTE)
		.value("KC_CALCULATOR",   OIS::KeyCode::KC_CALCULATOR)
		.value("KC_PLAYPAUSE",    OIS::KeyCode::KC_PLAYPAUSE)
		.value("KC_MEDIASTOP",    OIS::KeyCode::KC_MEDIASTOP)
		.value("KC_VOLUMEDOWN",   OIS::KeyCode::KC_VOLUMEDOWN)
		.value("KC_VOLUMEUP",     OIS::KeyCode::KC_VOLUMEUP)
		.value("KC_WEBHOME",      OIS::KeyCode::KC_WEBHOME)
		.value("KC_NUMPADCOMMA",  OIS::KeyCode::KC_NUMPADCOMMA)
		.value("KC_DIVIDE",       OIS::KeyCode::KC_DIVIDE)
		.value("KC_SYSRQ",        OIS::KeyCode::KC_SYSRQ)
		.value("KC_RMENU",        OIS::KeyCode::KC_RMENU)
		.value("KC_PAUSE",        OIS::KeyCode::KC_PAUSE)
		.value("KC_HOME",         OIS::KeyCode::KC_HOME)
		.value("KC_UP",           OIS::KeyCode::KC_UP)
		.value("KC_PGUP",         OIS::KeyCode::KC_PGUP)
		.value("KC_LEFT",         OIS::KeyCode::KC_LEFT)
		.value("KC_RIGHT",        OIS::KeyCode::KC_RIGHT)
		.value("KC_END",          OIS::KeyCode::KC_END)
		.value("KC_DOWN",         OIS::KeyCode::KC_DOWN)
		.value("KC_PGDOWN",       OIS::KeyCode::KC_PGDOWN)
		.value("KC_INSERT",       OIS::KeyCode::KC_INSERT)
		.value("KC_DELETE",       OIS::KeyCode::KC_DELETE)
		.value("KC_LWIN",         OIS::KeyCode::KC_LWIN)
		.value("KC_RWIN",         OIS::KeyCode::KC_RWIN)
		.value("KC_APPS",         OIS::KeyCode::KC_APPS)
		.value("KC_POWER",        OIS::KeyCode::KC_POWER)
		.value("KC_SLEEP",        OIS::KeyCode::KC_SLEEP)
		.value("KC_WAKE",         OIS::KeyCode::KC_WAKE)
		.value("KC_WEBSEARCH",    OIS::KeyCode::KC_WEBSEARCH)
		.value("KC_WEBFAVORITES", OIS::KeyCode::KC_WEBFAVORITES)
		.value("KC_WEBREFRESH",   OIS::KeyCode::KC_WEBREFRESH)
		.value("KC_WEBSTOP",      OIS::KeyCode::KC_WEBSTOP)
		.value("KC_WEBFORWARD",   OIS::KeyCode::KC_WEBFORWARD)
		.value("KC_WEBBACK",      OIS::KeyCode::KC_WEBBACK)
		.value("KC_MYCOMPUTER",   OIS::KeyCode::KC_MYCOMPUTER)
		.value("KC_MAIL",         OIS::KeyCode::KC_MAIL)
		.value("KC_MEDIASELECT",  OIS::KeyCode::KC_MEDIASELECT)
		;
}
