#pragma once
#include "Point2D.h"
#include "SmartPtr.h"

namespace Engine
{
	struct AABB;
	class GameObject;

	namespace Physics
	{
		class PhysicsInfo
		{
		public:
			static PhysicsInfo * Create( float i_mass, float i_dragness, SmartPtr<GameObject> i_pGo, AABB * i_pAABB );
			~PhysicsInfo();

			PhysicsInfo( const PhysicsInfo & ) = delete;
			void operator=( const PhysicsInfo & ) = delete;

			inline SmartPtr<GameObject> GetGameObject(){ return m_pGo; }

			inline const Point2D<float> & GetDrivingForce(){ return m_force; };
			inline void SetDrivingForce( const Point2D<float> & i_oForce ){ m_force = i_oForce; };

			inline float GetMass(){ return m_mass; };
			inline float GetDragness(){ return m_dragness; };

			inline AABB * GetAABB(){ return m_pAABB; }
			inline void SetAABB( AABB * i_pAABB ){ m_pAABB = i_pAABB; }

			void UpdateAABB();

		private:
			PhysicsInfo(){};
			PhysicsInfo( float i_mass, float i_drag, SmartPtr<GameObject> i_pGo, AABB * i_pAABB );
			SmartPtr<GameObject> m_pGo;
			Point2D<float> m_force;
			AABB * m_pAABB;
			float m_mass;
			float m_dragness;
		};
	}
}
