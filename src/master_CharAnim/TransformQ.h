/****************************************************************************
Copyright (C) 2010-2011 Alexandre Meyer

This file is part of  library.


Sime is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Sime is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sime.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef _TRANSFORMQ_H
#define _TRANSFORMQ_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <iostream>
#include <vec.h>


	/*! \brief
	 */
	class TransformQ
	{
	public:
		TransformQ() : T(0, 0, 0) {}
		TransformQ(const Vector& _T) : T(_T) {}

		//! constructor from axis and angle (degree) of rotation
		TransformQ(const Vector& axis_rotation, const float angle, const Vector& translation=Vector(0,0,0)) : Q(axis_rotation, angle), T(translation) {}

		TransformQ(const Quaternion& _Q, const Vector& _T = Vector(0, 0, 0)) : Q(_Q), T(_T) {}


		/*! \brief Tr x Tr */
		friend inline TransformQ operator*(const TransformQ& a, const TransformQ& b)
		{
			// TODO
			return TransformQ();
		}

		friend inline Vector 		operator*(const TransformQ& a, const Vector& v)
		{
			// TODO
			return Vector(0, 0, 0);
		}

		friend inline Point 		operator*(const TransformQ& a, const Point& v)
		{
			// TODO
			return Point(0, 0, 0);
		}


		inline static TransformQ slerp(const TransformQ& a, const TransformQ& b, float t)
		{
			// TODO
			return TransformQ();
		}

		void setIdentity()
		{
			T = Vector(0, 0, 0);
			Q.setAxisAngle(Vector(1, 0, 0), 0.f);
		}

		TransformQ getInverse() const
		{
			Quaternion NQ = Q.inverse();
			return TransformQ(NQ, NQ*Vector(-T.x, -T.y, -T.z));
		}

		void inverse()
		{
			*this = getInverse();
		}

		void setRotation(const Quaternion& _Q) { Q = _Q; }
		void setTranslation(const Vector& _T) { T = _T; }
		Quaternion& getRotation() { return Q; }
		Vector& getTranslation() { return T; }
		const Quaternion& getRotation() const { return Q; }
		const Vector& getTranslation() const { return T; }

		template<typename MAT>
		void getMatrix44(MAT& m) const
		{
			Q.getMatrix44(m);
			m[0][3] = T.x;
			m[1][3] = T.y;
			m[2][3] = T.z;
		}

		void getMatrix16(float m[16]) const
		{
			Q.getMatrix16(m);
			m[3] = T.x;
			m[7] = T.y;
			m[11] = T.z;
		}

		friend inline std::ostream& operator<<(std::ostream& o, const TransformQ& a)
		{
			o << "Q=" << a.Q << ", T=" << a.T;
			return o;
		}

	protected:
		Quaternion Q;
		Vector T;
	};



#endif

