#pragma once
#include <iostream>
namespace Engine
{
	class TString
	{
	public:
		TString();
		TString( const TString & );
		TString( const char * );
		~TString();

		char& operator[]( unsigned int );

		void operator = ( const TString & );
		void operator = ( const char * );
		void operator += ( const TString & );
		void operator += ( const char * );
		void SetBuffer( const char * );

		bool operator==( const TString & ) const;

		friend bool operator==( const TString &, const TString & );
		friend bool operator==( const TString &, const char * );
		friend std::ostream& operator<<( std::ostream &, const TString & );
		friend std::istream & operator>>( std::istream & is, TString & str );

		inline char * GetBuffer() const { return m_buffer; } 
		inline size_t Length() const { return m_size; }
		inline char * c_str() const { return m_buffer; }

	private:
		size_t m_size;
		char * m_buffer;
	};

};
