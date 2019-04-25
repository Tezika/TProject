#include "stdafx.h"
#include "PhysicsManager.h"
#include "PhysicsInfo.h"
#include "TList.h"
#include "Assert.h"
#include "GameObject.h"
#include "Point2D.h"
#include "SmartPtr.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "AABB.h"
#include <algorithm>
#include <limits>
#include "RenderInfo.h"
#include "RenderManager.h"
#include <ctime>

namespace Engine
{
	namespace Physics
	{
		bool PhysicsManager::Initialize()
		{
			m_pPhysicsInfos = new TList<PhysicsInfo>();
			assert( m_pPhysicsInfos );
			m_pCollisionPairs = new TList<CollisionPair>();
			assert( m_pCollisionPairs );
			DEBUG_PRINT_ENGINE( "The physics system initialized succuessfully!" );
			return true;
		}

		void PhysicsManager::Simulate( float i_dt )
		{
			CollisionPair * pEarliestCollisionPair = nullptr;
			float tLeft = 0;
			float tProcess = 0;
			// Gather the collision pairs in one pass.
			this->SimulateCollision( i_dt, m_pCollisionPairs );
			pEarliestCollisionPair = this->GetEarliestCollisionPair();

			if ( pEarliestCollisionPair == nullptr )
			{
				// If there is no collision, it'll simulate the movement directly
				this->SimulateMovement( i_dt );
			}
			else
			{
				// Assign timer's time equal to the frame time
				tLeft = i_dt;

				// Simualte the collision again based on that.
				while ( pEarliestCollisionPair != nullptr )
				{
					// Still curious about how to deal with the situation while the collision time < 0.
					if ( pEarliestCollisionPair->m_collisionTime <= 0.0f )
					{
						SmartPtr<GameObject> pGoA = pEarliestCollisionPair->m_pCollidables[0]->GetGameObject();
						SmartPtr<GameObject> pGoB = pEarliestCollisionPair->m_pCollidables[1]->GetGameObject();
						pGoA->SetVelocity( -pGoA->GetVelocity() );
						pGoB->SetVelocity( -pGoB->GetVelocity() );
						this->SimulateMovement( tLeft );
						break;
					}
					DEBUG_PRINT_ENGINE( "The left time is %f, the collision time is %f, and the process time is %f", tLeft, pEarliestCollisionPair->m_collisionTime, tProcess );
					// Subtract the process time
					tLeft -= tProcess;
					// Subtract the collision time
					tLeft -= pEarliestCollisionPair->m_collisionTime;
					if ( tLeft <= 0 )
					{
						// Run out of time and jump out the loop.
						break;
					}
					tProcess = (float) clock();
					// Simulate all objects' movment toward that time point.
					this->SimulateMovement( tLeft );
					// Resolve the collision
					this->ResolveCollision( pEarliestCollisionPair );
					// After resolving, it needs to remove the 'CollisionPair' from the link list.
					m_pCollisionPairs->Remove( pEarliestCollisionPair );
					pEarliestCollisionPair = this->GetEarliestCollisionPair();
					tProcess = ( (float) clock() - tProcess ) / 1000;
				}
			}
		}

		void PhysicsManager::SimulateMovement( float i_dt )
		{
			// Iterate every physics object in the moving object list
			Node<PhysicsInfo> * ptr = m_pPhysicsInfos->GetHead();
			Vector3 cachedAcceleration;
			Vector3 cachedVelocity;
			Vector3 cachedAverageVelocity;
			Vector3 cachedPosition;
			SmartPtr<GameObject> pCachedGo;

			// Simulate the position
			while ( ptr != nullptr )
			{
				PhysicsInfo * pInfo = ptr->GetData();
				pCachedGo = pInfo->GetGameObject();
				Vector3 drivingForce = pInfo->GetDrivingForce();
				assert( pCachedGo != nullptr );
				// Caculate the accerlation
				cachedAcceleration = drivingForce / pInfo->GetMass();
				// Use the acceraltion to update the velocity of current gameobject
				cachedVelocity = pCachedGo->GetVelocity();
				cachedVelocity += cachedAcceleration * i_dt;
				// caluate the drag
				float drag = pInfo->GetDragness() * ( cachedVelocity.Dot( cachedVelocity ) );
				// Apply the drag effect to current velocity
				cachedVelocity -= cachedVelocity.Normalize() * drag;
				// Update the velocity for go
				pCachedGo->SetVelocity( cachedVelocity + cachedAcceleration * i_dt );
				// Caluate the average velocity between this frame and last frame
				cachedAverageVelocity = ( cachedVelocity + pCachedGo->GetVelocity() ) / 2;
				// Update the position based on the average velocity
				cachedPosition = pCachedGo->GetPosition();
				pCachedGo->SetPosition( cachedPosition + cachedAverageVelocity * i_dt );
				ptr = ptr->GetNext();
			}
		}

		void  PhysicsManager::SimulateCollision( float i_dt, TList<CollisionPair> * i_pCollisionPairs )
		{
			CollisionPair * pEarliestCollisionPair = nullptr;
			i_pCollisionPairs->Clear( true );
			Node<PhysicsInfo> * ptr = m_pPhysicsInfos->GetHead();
			// Check the collision between two objects
			PhysicsInfo * pPhysicsA = nullptr;
			PhysicsInfo * pPhysicsB = nullptr;

			Node<PhysicsInfo> * ptr_1 = m_pPhysicsInfos->GetHead();
			SmartPtr<GameObject> pCachedGo;

			while ( ptr != nullptr )
			{
				pPhysicsA = ptr->GetData();

				// if the current object cannot be collided, then move to next one.
				if ( !pPhysicsA->GetCollidable() )
				{
					ptr = ptr->GetNext();
					continue;
				}
				while ( ptr_1 != nullptr )
				{
					if ( ptr == ptr_1 )
					{
						ptr_1 = ptr_1->GetNext();
						continue;
					}

					// if the current object cannot be collided, then move to next one.
					pPhysicsB = ptr_1->GetData();
					if ( !pPhysicsB->GetCollidable() )
					{
						ptr_1 = ptr_1->GetNext();
						continue;
					}

					float collisionTime;
					Vector3 collisionNormal = Vector3::Zero;

					// Check the collision between the A and B
					if ( this->IsCollision( pPhysicsA, pPhysicsB, i_dt, collisionTime, collisionNormal ) )
					{
						// Add a new collision pair into the list.
						m_pCollisionPairs->Insert( new CollisionPair( collisionTime, collisionNormal, pPhysicsA, pPhysicsB ) );
						pPhysicsA->SetIsCollision( true );
						pPhysicsB->SetIsCollision( true );

					}
					else
					{
						pPhysicsA->SetIsCollision( false );
						pPhysicsB->SetIsCollision( false );
					}
					ptr_1 = ptr_1->GetNext();
				}
				ptr = ptr->GetNext();
			}

			// Draw the debug visual information to indicate the collision.
#if defined(_DEBUG) && defined(_DrawDebugInfoWhileColliding)
			ptr = this->m_pPhysicsInfos->GetHead();
			while ( ptr != nullptr )
			{
				pPhysicsA = ptr->GetData();
				pCachedGo = pPhysicsA->GetGameObject();
				Render::RenderInfo * pRenderInfo = Render::RenderManager::GetInstance().GetRealRenderInfoByGameObject( pCachedGo );
				Render::RenderInfo * pDebugRenderInfo = Render::RenderManager::GetInstance().GetDebugRenderInfoByGameObject( pCachedGo );
				// If the object is in collision, it'll render the debug sprite
				if ( pPhysicsA->GetIsCollision() )
				{
					pRenderInfo->SetRenderable( false );
					pDebugRenderInfo->SetRenderable( true );
				}
				// If the object is not in collision, it'll render the normal sprite.
				else
				{
					pRenderInfo->SetRenderable( true );
					pDebugRenderInfo->SetRenderable( false );
				}
				ptr = ptr->GetNext();
			}
#endif
		}

		bool PhysicsManager::AddPhysicsObject( PhysicsInfo * i_pInfo )
		{
			assert( i_pInfo );
			return m_pPhysicsInfos->Insert( i_pInfo );
		}

		bool PhysicsManager::RemovePhysicsObject( GameObject * i_pGo )
		{
			assert( i_pGo );
			Node<PhysicsInfo> * ptr = m_pPhysicsInfos->GetHead();
			PhysicsInfo * removePhysicsInfo = nullptr;
			while ( ptr != nullptr )
			{
				removePhysicsInfo = ptr->GetData();
				if ( removePhysicsInfo->GetGameObject() == i_pGo )
				{
					assert( removePhysicsInfo );
					ptr = m_pPhysicsInfos->Remove( ptr );
					delete removePhysicsInfo;
				}
				else
				{
					ptr = ptr->GetNext();
				}
			}
			return true;
		}

		PhysicsInfo * PhysicsManager::GetInfoByGameObject( SmartPtr<GameObject> i_pGO )
		{
			assert( i_pGO );
			Node<PhysicsInfo> * ptr = m_pPhysicsInfos->GetHead();
			PhysicsInfo * pCurrentInfo = nullptr;
			while ( ptr != nullptr )
			{
				pCurrentInfo = ptr->GetData();
				if ( pCurrentInfo->GetGameObject() == i_pGO )
				{
					return pCurrentInfo;
				}
				ptr = ptr->GetNext();
			}
			return nullptr;
		}

		bool PhysicsManager::Destroy()
		{
			// Clear the collision pairs
			m_pCollisionPairs->Clear( true );
			delete m_pCollisionPairs;
			m_pCollisionPairs = nullptr;

			// Clean the physics objects
			m_pPhysicsInfos->Clear( true );
			delete m_pPhysicsInfos;
			m_pPhysicsInfos = nullptr;

			DEBUG_PRINT_ENGINE( "The physics system destoried succuessfully!" );
			return true;
		}

		bool PhysicsManager::IsCollision(
			PhysicsInfo * i_pPhysicsInfoA,
			PhysicsInfo * i_pPhysicsInfoB,
			float i_dt,
			float & i_collisionTime,
			Vector3 & i_collisionNormal
		)
		{
			// Initialize the min and max value for time;
			float tCloseLatest = -1;
			float tOpenEarilest = 100.0f;// However, this is a magic number :<.
			float bCollided = true;
			Vector3 collisionAxis = Vector3::Zero;

			// Check for the A projected onto B's in world.
			bCollided = this->CheckCollision( i_pPhysicsInfoA, i_pPhysicsInfoB, i_dt, tCloseLatest, tOpenEarilest, collisionAxis );
			if ( !bCollided )
			{
				return false;
			}

			// Swap A and B. Then do the check again.
			bCollided = true;
			bCollided = this->CheckCollision( i_pPhysicsInfoB, i_pPhysicsInfoA, i_dt, tCloseLatest, tOpenEarilest, collisionAxis );
			if ( !bCollided )
			{
				return false;
			}

			if ( tCloseLatest < tOpenEarilest )
			{
				i_collisionTime = tCloseLatest;
				// Still confused about the calculation about normal of collision axis.
				i_collisionNormal = -collisionAxis;
				return true;
			}
			return false;
		}

		bool PhysicsManager::CheckCollision(
			PhysicsInfo * pPhysicsInfoA,
			PhysicsInfo * pPhysicsInfoB,
			float tFrameEnd,
			float & tCloseLatest,
			float & tOpenEarilest,
			Vector3 & i_collisionAxis
		)
		{
			SmartPtr<GameObject> pGoA = pPhysicsInfoA->GetGameObject();
			assert( pGoA );
			AABB * pABB = pPhysicsInfoA->GetAABB();
			assert( pABB );

			SmartPtr<GameObject> pGoB = pPhysicsInfoB->GetGameObject();
			assert( pGoB );
			AABB * pBBB = pPhysicsInfoB->GetAABB();
			assert( pBBB );

			// Calculate the necessary matrixes
			Matrix4x4 mtx_AToWorld = pGoA->GetMatrixFromLocalToWorld();
			Matrix4x4 mtx_BToWorld = pGoB->GetMatrixFromLocalToWorld();

			// Calculate the B's collision axis in world
			// For X
			Vector4 CollisionAxisXInW = mtx_BToWorld * Vector4::UnitX;

			// Project the BoundingBox's center onto the collision axis in the world
			Vector4 ABBCenterInW = mtx_AToWorld * Vector4( pABB->center.m_x, pABB->center.m_y, 0, 1.0f );
			Vector4 BBBCenterInW = mtx_BToWorld * Vector4( pBBB->center.m_x, pBBB->center.m_y, 0, 1.0f );
			float ABBCenterOnXInW = ABBCenterInW.Dot( CollisionAxisXInW );
			float BBBCenterOnXInW = BBBCenterInW.Dot( CollisionAxisXInW );

			// Project the extends onto the collision axis in the world
			Vector4 ABBExtendsXInW = mtx_AToWorld * Vector4::UnitX * pABB->extends.m_x;
			Vector4 ABBExtendsYInW = mtx_AToWorld * Vector4::UnitY * pABB->extends.m_y;
			Vector4 BBBExtendsXInW = mtx_BToWorld * Vector4::UnitX * pBBB->extends.m_x;
			Vector4 BBBExtendsYInW = mtx_BToWorld * Vector4::UnitY * pBBB->extends.m_y;

			float AProjectedExtendsOntoX = fabs( ABBExtendsXInW.Dot( CollisionAxisXInW ) ) + fabs( ABBExtendsYInW.Dot( CollisionAxisXInW ) );
			float BProjectedExntedsOntoX = fabs( BBBExtendsXInW.Dot( CollisionAxisXInW ) ) + fabs( BBBExtendsYInW.Dot( CollisionAxisXInW ) );

			// Calculate the velocity onto the axis
			float AVelAlongXInW = Vector4( pGoA->GetVelocity(), 0.0f ).Dot( CollisionAxisXInW );
			float BVelAlongXInW = Vector4( pGoB->GetVelocity(), 0.0f ).Dot( CollisionAxisXInW );

			// Vector3 collision axis
			Vector3 vct3_collisionAxis = Vector3( CollisionAxisXInW.x, CollisionAxisXInW.y, CollisionAxisXInW.z );

			// Check for X axis
			bool bCollided = true;
			bCollided = this->CheckAxisCollision(
				AProjectedExtendsOntoX,
				BProjectedExntedsOntoX,
				BBBCenterOnXInW,
				ABBCenterOnXInW,
				AVelAlongXInW - BVelAlongXInW,
				tFrameEnd,
				tOpenEarilest,
				tCloseLatest,
				vct3_collisionAxis,
				i_collisionAxis
			);
			if ( !bCollided )
			{
				return false;
			}

			// For Y
			Vector4 CollisionAxisYInW = mtx_BToWorld * Vector4::UnitY;
			vct3_collisionAxis = Vector3( CollisionAxisYInW.x, CollisionAxisYInW.y, CollisionAxisYInW.z );

			// Recalculate bb's center onto axis
			float ABBCenterOnYInW = ABBCenterInW.Dot( CollisionAxisYInW );
			float BBBCenterOnYInW = BBBCenterInW.Dot( CollisionAxisYInW );

			// ReCalculate bbs' extends onto axis
			float AProjectedExtendsOntoY = fabs( ABBExtendsXInW.Dot( CollisionAxisYInW ) ) + fabs( ABBExtendsYInW.Dot( CollisionAxisYInW ) );
			float BProjectedExntedsOntoY = fabs( BBBExtendsXInW.Dot( CollisionAxisYInW ) ) + fabs( BBBExtendsYInW.Dot( CollisionAxisYInW ) );

			// Recalculate velocities
			float AVelAlongYInW = Vector4( pGoA->GetVelocity(), 0.0f ).Dot( CollisionAxisYInW );
			float BVelAlongYInW = Vector4( pGoB->GetVelocity(), 0.0f ).Dot( CollisionAxisYInW );

			// Check for Y axis
			bCollided = true;
			bCollided = this->CheckAxisCollision(
				AProjectedExtendsOntoY,
				BProjectedExntedsOntoY,
				BBBCenterOnYInW,
				ABBCenterOnYInW,
				AVelAlongYInW - BVelAlongYInW,
				tFrameEnd,
				tOpenEarilest,
				tCloseLatest,
				vct3_collisionAxis,
				i_collisionAxis
			);
			if ( !bCollided )
			{
				return false;
			}

			return true;
		}

		bool PhysicsManager::CheckAxisCollision(
			float aBBExtendsProjectedOntoAxis,
			float bBBExtendsOntoAxis,
			float bBBCenterOntoAxis,
			float aBBCenterOntoAxis,
			float velARelBOnAxis,
			float tFrameEnd,
			float & tOpenEarilest,
			float & tCloseLatest,
			const Vector3 & i_currentAxis,
			Vector3 & i_collisionAxis
		)
		{
			float bExtends = bBBExtendsOntoAxis + aBBExtendsProjectedOntoAxis;
			float bLeft = bBBCenterOntoAxis - bExtends;
			float bRight = bBBCenterOntoAxis + bExtends;

			float dClose = bLeft - aBBCenterOntoAxis;
			float dOpen = bRight - aBBCenterOntoAxis;

			float tOpen = 0;
			float tClose = 0;

			// When the realtive velocity projection is zero
			if ( velARelBOnAxis == 0.0f )
			{
				// If A's bb center outside the bleft ~ bRight, there is no collision. 
				if ( aBBCenterOntoAxis < bLeft || aBBCenterOntoAxis > bRight )
				{
					return false;
				}
			}
			else
			{
				tOpen = dOpen / velARelBOnAxis;
				tClose = dClose / velARelBOnAxis;
				// A move to B in the -x direction, and we just swap two value
				if ( tOpen < tClose )
				{
					std::swap( tOpen, tClose );
				}
				// Check the edge: If the close happens in the time point after this frame, there should be no collision
				if ( tClose > tFrameEnd )
				{
					return false;
				}
				// Check the edge: If the open's time less than zero, which means the gap should always be in the right, there is no collision too.
				if ( tOpen < 0 )
				{
					return false;
				}
				// While passing the edge check, update the earliest open and latest close time
				tOpenEarilest = tOpen < tOpenEarilest ? tOpen : tOpenEarilest;
				if ( tClose > tCloseLatest )
				{
					// Update the latest closing time
					tCloseLatest = tClose;
					// Update the collision axis
					i_collisionAxis = i_currentAxis;
				}
			}
			return true;
		}

		void PhysicsManager::RecalculateVelByMomentum( const PhysicsInfo * pPhysicsInfoA, const PhysicsInfo * pPhysicsInfoB, const Vector3 & velA_dir, const Vector3 & velB_dir )
		{
			SmartPtr<GameObject> pGoA = pPhysicsInfoA->GetGameObject();
			SmartPtr<GameObject> pGoB = pPhysicsInfoB->GetGameObject();
			float mass_A = pPhysicsInfoA->GetMass();
			float mass_B = pPhysicsInfoB->GetMass();
			float vel_A = pGoA->GetVelocity().Length();
			float vel_B = pGoB->GetVelocity().Length();
			assert( mass_A > 0 );
			assert( mass_B > 0 );

			float mag_velA = ( mass_A - mass_B ) / ( mass_A + mass_B ) * vel_A + 2 * mass_B / ( mass_A + mass_B ) * vel_B;
			float mag_velB = ( mass_B - mass_A ) / ( mass_A + mass_B ) * vel_B + 2 * mass_A / ( mass_A + mass_B ) * vel_A;

			pGoA->SetVelocity( mag_velA * velA_dir );
			pGoB->SetVelocity( mag_velB * velB_dir );
		}

		void PhysicsManager::ResolveCollision( const CollisionPair* pCollisionPair )
		{
			assert( pCollisionPair != nullptr );
			SmartPtr<GameObject> pGoA = pCollisionPair->m_pCollidables[0]->GetGameObject();
			SmartPtr<GameObject> pGoB = pCollisionPair->m_pCollidables[1]->GetGameObject();

			// Calculate the directions after reflection
			Vector3 dir_vel_A = pGoA->GetVelocity().Normalize().Reflect( pCollisionPair->m_collisionNormal );
			Vector3 dir_vel_B = pGoB->GetVelocity().Normalize().Reflect( pCollisionPair->m_collisionNormal );

			// Recalculate the two collidabes velocities based on the momentum
			this->RecalculateVelByMomentum( pCollisionPair->m_pCollidables[0], pCollisionPair->m_pCollidables[1], dir_vel_A, dir_vel_B );
		}

		CollisionPair * PhysicsManager::GetEarliestCollisionPair()
		{
			if ( m_pCollisionPairs->Length() == 0 )
			{
				return nullptr;
			}
			Node<CollisionPair> * pCachedCollisionPair = nullptr;
			Node<CollisionPair> * ptr = m_pCollisionPairs->GetHead();
			while ( ptr != nullptr )
			{
				if ( pCachedCollisionPair == nullptr || ptr->GetData()->m_collisionTime < pCachedCollisionPair->GetData()->m_collisionTime )
				{
					pCachedCollisionPair = ptr;
				}
				ptr = ptr->GetNext();
			}
			return pCachedCollisionPair->GetData();
		}
	}
}