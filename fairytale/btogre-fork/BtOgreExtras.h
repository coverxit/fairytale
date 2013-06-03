/*
 * =====================================================================================
 *
 *       Filename:  BtOgreExtras.h
 *
 *    Description:  Contains the Ogre Mesh to Bullet Shape converters.
 *
 *        Version:  1.0
 *        Created:  27/12/2008 01:45:56 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

// Modified by freshrat912@gmail.com (Akira Sakurai aka. TennenColl) to fit multithreading situations.

#ifndef _BtOgreShapes_H_
#define _BtOgreShapes_H_

#include <bullet/btBulletDynamicsCommon.h>
#include <Ogre/OgreVector3.h>
#include <Ogre/OgreQuaternion.h>

namespace BtOgre
{

typedef std::vector<Ogre::Vector3> Vector3Array;

//Converts from and to Bullet and Ogre stuff. Pretty self-explanatory.
class Convert
{
public:
	Convert() {};
	~Convert() {};

	static btQuaternion toBullet(const Ogre::Quaternion &q)
	{
		return btQuaternion(q.x, q.y, q.z, q.w);
	}
	static btVector3 toBullet(const Ogre::Vector3 &v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	static Ogre::Quaternion toOgre(const btQuaternion &q)
	{
		return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z());
	}
	static Ogre::Vector3 toOgre(const btVector3 &v)
	{
		return Ogre::Vector3(v.x(), v.y(), v.z());
	}
};

}

#endif
