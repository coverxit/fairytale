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

#include "pch.h"

#include <boost/uuid/uuid_generators.hpp>

#include "element.h"

namespace fairytale {

	typedef Ogre::Vector3 Vector3;
	typedef btScalar Scalar;

	class Element {
	protected:
		Element* _mParent;
		std::unordered_map<Element*, std::shared_ptr<Element>> _mChilds;
		std::unordered_map
		Vector3 _mPosition;

		void _addChild(Element* child);
		void _removeChild(Element* child);

	public:
		void setParent(Element* parent);
		Element* getParent() { return _mParent; }


		Element() : _mParent(nullptr) {}
		virtual ~Element() {}
	};

}
