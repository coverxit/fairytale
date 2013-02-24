/*
 * Copyright 2012-2013 TennenColl
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

#ifdef _DEBUG

#pragma comment(lib, "OgreMain_d.lib")
#pragma comment(lib, "OIS_d.lib")
#pragma comment(lib, "OgreOverlay_d.lib")
#pragma comment(lib, "SkyX_d.lib")
#pragma comment(lib, "BtOgre_d.lib")

#else

#pragma comment(lib, "OgreMain.lib")
#pragma comment(lib, "OIS.lib")
#pragma comment(lib, "sqstdlib.lib")
#pragma comment(lib, "squirrel.lib")
#pragma comment(lib, "OgreOverlay.lib")
#pragma comment(lib, "SkyX.lib")
#pragma	comment(lib, "BtOgre.lib")
#pragma	comment(lib, "bulletdynamics.lib")
#pragma	comment(lib, "bulletcollision.lib")
#pragma	comment(lib, "linearmath.lib")
#pragma	comment(lib, "OgreVolume.lib")

#endif
