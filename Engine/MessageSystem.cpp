#include "stdafx.h"
#include "MessageSystem.h"
#include "TString.h"
#include "MultiCastDelegate.h"


namespace Engine
{
	namespace Messaging
	{
		void MessageSystem::RegisterMessageDelegate( const HashedString& i_message, Delegate<void *> & i_delegate )
		{
			std::map<HashedString, MultiCastDelegate<void *>>::iterator itr;
			itr = m_pMessage2Delegates->find( i_message );
			if ( itr == m_pMessage2Delegates->end() )
			{
					MultiCastDelegate<void*> newMultiCastDelegate;
					newMultiCastDelegate.AddDelegate( i_delegate );
					m_pMessage2Delegates->insert( std::make_pair( i_message, newMultiCastDelegate ) );
			}
			else
			{
				itr->second.AddDelegate( i_delegate );
			}
		}

		void MessageSystem::DeregisterMessageDelegate( const HashedString & i_message, Delegate<void *> & i_delegate )
		{
			std::map<HashedString, MultiCastDelegate<void *>>::iterator itr;
			itr = m_pMessage2Delegates->find( i_message );
			if ( itr == m_pMessage2Delegates->end() )
			{
				return;
			}
			else
			{
				itr->second.RemoveDelegate( i_delegate );
			}
		}

		void MessageSystem::SendMessageW( const HashedString & i_message, void * i_pInfo )
		{
			std::map<HashedString, MultiCastDelegate<void *>>::iterator itr;
			itr = m_pMessage2Delegates->find( i_message );
			if ( itr != m_pMessage2Delegates->end() )
			{
				itr->second.ExecuteIfBound( i_pInfo );
			}
		}

		bool MessageSystem::Initialize()
		{
			m_pMessage2Delegates = new std::map<HashedString, MultiCastDelegate<void *>>();
			assert( m_pMessage2Delegates != nullptr );
			return true;
		}

		bool MessageSystem::Destroy()
		{
			m_pMessage2Delegates->clear();
			delete m_pMessage2Delegates;
			m_pMessage2Delegates = nullptr;
			return true;
		}
	}
}